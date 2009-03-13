#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>
#include <nvram/nvram.h>
#include <watchdog/watchdog.h>

static inline bool_t __nvdata_is_valid_addr (nvdata_t *v, unsigned addr)
{
	return nvdata_is_owned (v) && v->begin <= addr && v->limit > addr;
}

void nvdata_write_byte (nvdata_t *v, unsigned char c)
{
	assert (__nvdata_is_valid_addr (v, v->__addr));
	if (__nvdata_is_valid_addr (v, v->__addr)) {
		nvram_write_byte (v->__addr, c);
		v->crc = crc32_vak_byte (v->crc, c);
		v->__addr++;
		watchdog_alive ();
	}
}

unsigned char nvdata_read_byte (nvdata_t *v)
{
	unsigned char c = (unsigned char) -1;

	assert (__nvdata_is_valid_addr (v, v->__addr));
	if (__nvdata_is_valid_addr (v, v->__addr)) {
		c = nvram_read_byte (v->__addr);
		v->crc = crc32_vak_byte (v->crc, c);
		v->__addr++;
		watchdog_alive ();
	}
	return c;
}

/*
 * Write a short to NVRAM (lsb first).
 */
void
nvdata_write_word (nvdata_t *v, unsigned short val)
{
	nvdata_write_byte (v, (unsigned char) val);
	nvdata_write_byte (v, (unsigned char) (val >> 8));
}

/*
 * Read a short from NVRAM (lsb first).
 */
unsigned
nvdata_read_word (nvdata_t *v)
{
	unsigned val = nvdata_read_byte (v);
	val += ((unsigned) nvdata_read_byte (v)) << 8;
	return val;
}

/*
 * Write a long to NVRAM (lsb first).
 */
void
nvdata_write_dword (nvdata_t *v, unsigned long val)
{
	nvdata_write_word (v, (unsigned short) val);
	nvdata_write_word (v, (unsigned short) (val >> 16));
}

/*
 * Read a long from NVRAM (lsb first).
 */
unsigned long
nvdata_read_dword (nvdata_t *v)
{
	unsigned long val = nvdata_read_word (v);
	val += ((unsigned long) nvdata_read_word (v)) << 16;
	return val;
}

small_int_t
nvdata_begin_read (nvdata_t *v, unsigned sign)
{
	unsigned nvdata_sign, len;
	small_int_t reason;

	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	assert (v->__addr == NVDATA_BAD_ADDRESS);
	v->end = v->begin;
	v->sign = 0;

	v->__addr = v->begin;
	if (nvdata_read_word (v) != 0) {
		/* LY: в этом поле будет храниться смещение следующей
		       (второй) копии данных. */
		reason = NVDATA_DIFFERENT;
		goto ballout;
	}

	nvdata_sign = nvdata_read_word (v);
	/* debug_printf ("begin-read: nvdata_sign = 0x%02X->0x%02X, soft_sign = 0x%02X\n",
		nvdata_sign, nvdata_sign ^ 0xDEAD, sign); */
	if (nvdata_sign == 0 || nvdata_sign == 0xFFFF) {
		/* LY: в NVRAM ничего нет. */
		reason = NVDATA_EMPTY;
		goto ballout;
	}

	nvdata_sign ^= 0xDEAD;
	if (nvdata_sign == 0) {
		/* LY: предыдущая транзакция чтения или записи
		       не была завершена, т.е. содержание NVRAM вызвало
		       фатальный сбой при чтении конфигурации, либо
		       процесс записи не был закончен.
		*/
		reason = NVDATA_DEAD;
		goto ballout;
	}

	if (! nvdata_is_compatible (nvdata_sign, sign)) {
		/* LY: NVRAM записан другой, несовместимой, версией софта. */
		reason = NVDATA_DIFFERENT;
		goto ballout;
	}

	/* LY: считываем и проверяем длину. */
	len = nvdata_read_word (v);
	/* debug_printf ("begin-read: nvdata_len = %d, begin = %d, end = %d\n",
		len, v->begin, v->begin + len); */
	if (! __nvdata_is_valid_addr (v, len + v->begin)
	|| len + v->begin < v->begin + NVDATA_HEADER_SIZE) {
		reason =  NVDATA_INVALID;
		goto ballout;
	}
	/* LY: чтобы не читать NVRAM два раза, проверим CRC не сейчас, а в конце чтения. */

	/* LY: сохраняем сигнатуру для проверки CRC. */
	v->sign = nvdata_sign;

#if NVDATA_READ_TAINT
	/* LY: отмечаем начало процесса чтения. */
	v->__addr = v->begin + 2; nvdata_write_word (v, 0xDEAD);
#endif

	/* LY: пропускаем заголовок. */
	v->__addr = v->begin + NVDATA_HEADER_SIZE;

	/* LY: инициализируем CRC32 = 2^32 / PI. */
	v->crc = 0x517CC1B7;
	return NVDATA_OK;

ballout:
	v->__addr = NVDATA_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

small_int_t
nvdata_finalize_read (nvdata_t *v)
{
	unsigned len, nvdata_len;
	unsigned long crc, nvdata_crc;
	small_int_t reason = NVDATA_INVALID;

	assert (nvdata_is_owned (v) && v->__addr != NVDATA_BAD_ADDRESS);
	if (! nvdata_is_owned (v) || v->__addr == NVDATA_BAD_ADDRESS)
		return NVDATA_INVALID;

	v->end = v->begin;
	if (v->__addr >= v->limit) {
		reason = NVDATA_EOF;
		goto ballout;
	}

	/* LY: добавляем считаную ранее сигнатуру к контрольной сумме. */
	v->crc = crc32_vak_byte (v->crc, (unsigned char) v->sign);
	v->crc = crc32_vak_byte (v->crc, (unsigned char) (v->sign >> 8));

	/* LY: проверям  длину, пропуская при чтении
	       смещение второй копии и сигнатуру. */
	len = v->__addr - v->begin;
	v->__addr = v->begin + 4; nvdata_len = nvdata_read_word (v);
	/* debug_printf ("finalize-read: nvdata_len = %d, soft_len = %d\n", nvdata_len, len); */
	if (nvdata_len != len)
		goto ballout;

	/* LY: контрольная сумма для данных, сигнатуры и длины. */
	crc = v->crc; nvdata_crc = nvdata_read_dword (v);
	/* debug_printf ("finalize-read: nvdata_crc = 0x%08lX, soft_crc = 0x%08lX\n", nvdata_crc, crc); */
	if (nvdata_crc != crc)
		goto ballout;

#if NVDATA_READ_TAINT
	/* LY: отмечаем завершение процесса чтения. */
	v->__addr = v->begin + 2; nvdata_write_word (v, v->sign ^ 0xDEAD);
#endif
	v->end = v->begin + len;
	reason = NVDATA_OK;

ballout:
	v->__addr = NVDATA_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

void
nvdata_begin_write (nvdata_t *v)
{
	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	assert (v->__addr == NVDATA_BAD_ADDRESS);

	v->end = v->begin;
	/* LY: смещение второй копии данных. */
	v->__addr = v->begin; nvdata_write_word (v, 0);
#if NVDATA_WRITE_TAINT
	/* LY: отмечаем начало процесса записи. */
	nvdata_write_word (v, 0xDEAD);
#endif

	/* LY: пропускаем заголовок. */
	v->__addr = v->begin + NVDATA_HEADER_SIZE;

	/* LY: инициализируем CRC32 = 2^32 / PI. */
	v->crc = 0x517CC1B7;
}

small_int_t
nvdata_finalize_write (nvdata_t *v, unsigned sign)
{
	unsigned len;
	small_int_t reason;

	assert (nvdata_is_owned (v) && v->__addr != NVDATA_BAD_ADDRESS);
	if (! nvdata_is_owned (v) || v->__addr == NVDATA_BAD_ADDRESS)
		return NVDATA_INVALID;

	v->end = v->begin;
	if (v->__addr >= v->limit) {
		reason = NVDATA_EOF;
		goto ballout;
	}

	v->sign = sign;
	/* LY: добавляем сигнатуру к контрольной сумме. */
	v->crc = crc32_vak_byte (v->crc, (unsigned char) v->sign);
	v->crc = crc32_vak_byte (v->crc, (unsigned char) (v->sign >> 8));

	/* LY: записываем длину и CRC. */
	len = v->__addr - v->begin;
	v->__addr = v->begin + 4; nvdata_write_word (v, len);
	/* debug_printf ("finalize-write: nvdata_len = %d, nvdata_crc = 0x%08lX\n",
		len, v->crc); */
	nvdata_write_dword (v, v->crc);

	/* LY: отмечаем завершение процесса записи. */
	v->__addr = v->begin + 2; nvdata_write_word (v, v->sign ^ 0xDEAD);
	v->end = v->begin + len;
	reason = NVDATA_OK;

ballout:
	v->__addr = NVDATA_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

void nvdata_init (nvdata_t *v, unsigned region_begin, unsigned region_end)
{
	assert (region_end > region_begin && region_end - region_begin > 255);
	assert (region_end <= NVDATA_BAD_ADDRESS);

	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	v->begin = region_begin;
	v->limit = region_end;
	v->__addr = NVDATA_BAD_ADDRESS;
	v->end = v->begin;
	nvram_init ();
	assert (! nvdata_is_valid_addr (v, NVDATA_BAD_ADDRESS));
	assert (! nvdata_is_valid_addr (v, 0));
	assert (! __nvdata_is_valid_addr (v, NVDATA_BAD_ADDRESS));
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
}
