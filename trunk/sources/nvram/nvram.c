#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>
#include <watchdog/watchdog.h>

static inline bool_t __nvram_is_valid_addr (nvram_t *v, unsigned addr)
{
	return nvram_is_owned (v) && v->begin <= addr && v->limit > addr;
}

void nvram_write_byte (nvram_t *v, unsigned char c)
{
	assert (__nvram_is_valid_addr (v, v->__addr));
	if (__nvram_is_valid_addr (v, v->__addr)) {
		__arch_nvram_write_byte (v->__addr, c);
		v->crc = crc32_vak_byte (v->crc, c);
		v->__addr++;
		watchdog_alive ();
	}
}

unsigned char nvram_read_byte (nvram_t *v)
{
	unsigned char c = (unsigned char) -1;

	assert (__nvram_is_valid_addr (v, v->__addr));
	if (__nvram_is_valid_addr (v, v->__addr)) {
		c = __arch_nvram_read_byte (v->__addr);
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
nvram_write_word (nvram_t *v, unsigned short val)
{
	nvram_write_byte (v, (unsigned char) val);
	nvram_write_byte (v, (unsigned char) (val >> 8));
}

/*
 * Read a short from NVRAM (lsb first).
 */
unsigned
nvram_read_word (nvram_t *v)
{
	unsigned val = nvram_read_byte (v);
	val += ((unsigned) nvram_read_byte (v)) << 8;
	return val;
}

/*
 * Write a long to NVRAM (lsb first).
 */
void
nvram_write_dword (nvram_t *v, unsigned long val)
{
	nvram_write_word (v, (unsigned short) val);
	nvram_write_word (v, (unsigned short) (val >> 16));
}

/*
 * Read a long from NVRAM (lsb first).
 */
unsigned long
nvram_read_dword (nvram_t *v)
{
	unsigned long val = nvram_read_word (v);
	val += ((unsigned long) nvram_read_word (v)) << 16;
	return val;
}

int_t
nvram_begin_read (nvram_t *v, unsigned sign)
{
	unsigned nvram_sign, len;
	int_t reason;

	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	assert (v->__addr == NVRAM_BAD_ADDRESS);
	v->end = v->begin;
	v->sign = 0;

	v->__addr = v->begin;
	if (nvram_read_word (v) != 0) {
		/* LY: в этом поле будет храниться смещение следующей
		       (второй) копии данных. */
		reason = NVRAM_DIFFERENT;
		goto ballout;
	}

	nvram_sign = nvram_read_word (v);
	/* debug_printf ("begin-read: nvram_sign = 0x%02X->0x%02X, soft_sign = 0x%02X\n",
		nvram_sign, nvram_sign ^ 0xDEAD, sign); */
	if (nvram_sign == 0 || nvram_sign == 0xFFFF) {
		/* LY: в NVRAM ничего нет. */
		reason = NVRAM_EMPTY;
		goto ballout;
	}

	nvram_sign ^= 0xDEAD;
	if (nvram_sign == 0) {
		/* LY: предыдущая транзакция чтения или записи
		       не была завершена, т.е. содержание NVRAM вызвало
		       фатальный сбой при чтении конфигурации, либо
		       процесс записи не был закончен.
		*/
		reason = NVRAM_DEAD;
		goto ballout;
	}

	if (! nvram_is_compatible (nvram_sign, sign)) {
		/* LY: NVRAM записан другой, несовместимой, версией софта. */
		reason = NVRAM_DIFFERENT;
		goto ballout;
	}

	/* LY: считываем и проверяем длину. */
	len = nvram_read_word (v);
	/* debug_printf ("begin-read: nvram_len = %d, begin = %d, end = %d\n",
		len, v->begin, v->begin + len); */
	if (! __nvram_is_valid_addr (v, len + v->begin)
	|| len + v->begin < v->begin + NVRAM_HEADER_SIZE) {
		reason =  NVRAM_INVALID;
		goto ballout;
	}
	/* LY: чтобы не читать NVRAM два раза, проверим CRC не сейчас, а в конце чтения. */

	/* LY: сохраняем сигнатуру для проверки CRC. */
	v->sign = nvram_sign;

#if NVRAM_READ_TAINT
	/* LY: отмечаем начало процесса чтения. */
	v->__addr = v->begin + 2; nvram_write_word (v, 0xDEAD);
#endif

	/* LY: пропускаем заголовок. */
	v->__addr = v->begin + NVRAM_HEADER_SIZE;

	/* LY: инициализируем CRC32 = 2^32 / PI. */
	v->crc = 0x517CC1B7;
	return NVRAM_OK;

ballout:
	v->__addr = NVRAM_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

int_t
nvram_finalize_read (nvram_t *v)
{
	unsigned len, nvram_len;
	unsigned long crc, nvram_crc;
	int_t reason = NVRAM_INVALID;

	assert (nvram_is_owned (v) && v->__addr != NVRAM_BAD_ADDRESS);
	if (! nvram_is_owned (v) || v->__addr == NVRAM_BAD_ADDRESS)
		return NVRAM_INVALID;

	v->end = v->begin;
	if (v->__addr >= v->limit) {
		reason = NVRAM_EOF;
		goto ballout;
	}

	/* LY: добавляем считаную ранее сигнатуру к контрольной сумме. */
	v->crc = crc32_vak_byte (v->crc, (unsigned char) v->sign);
	v->crc = crc32_vak_byte (v->crc, (unsigned char) (v->sign >> 8));

	/* LY: проверям  длину, пропуская при чтении
	       смещение второй копии и сигнатуру. */
	len = v->__addr - v->begin;
	v->__addr = v->begin + 4; nvram_len = nvram_read_word (v);
	/* debug_printf ("finalize-read: nvram_len = %d, soft_len = %d\n", nvram_len, len); */
	if (nvram_len != len)
		goto ballout;

	/* LY: контрольная сумма для данных, сигнатуры и длины. */
	crc = v->crc; nvram_crc = nvram_read_dword (v);
	/* debug_printf ("finalize-read: nvram_crc = 0x%08lX, soft_crc = 0x%08lX\n", nvram_crc, crc); */
	if (nvram_crc != crc)
		goto ballout;

#if NVRAM_READ_TAINT
	/* LY: отмечаем завершение процесса чтения. */
	v->__addr = v->begin + 2; nvram_write_word (v, v->sign ^ 0xDEAD);
#endif
	v->end = v->begin + len;
	reason = NVRAM_OK;

ballout:
	v->__addr = NVRAM_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

void
nvram_begin_write (nvram_t *v)
{
	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	assert (v->__addr == NVRAM_BAD_ADDRESS);

	v->end = v->begin;
	/* LY: смещение второй копии данных. */
	v->__addr = v->begin; nvram_write_word (v, 0);
#if NVRAM_WRITE_TAINT
	/* LY: отмечаем начало процесса записи. */
	nvram_write_word (v, 0xDEAD);
#endif

	/* LY: пропускаем заголовок. */
	v->__addr = v->begin + NVRAM_HEADER_SIZE;

	/* LY: инициализируем CRC32 = 2^32 / PI. */
	v->crc = 0x517CC1B7;
}

int_t
nvram_finalize_write (nvram_t *v, unsigned sign)
{
	unsigned len;
	int_t reason;

	assert (nvram_is_owned (v) && v->__addr != NVRAM_BAD_ADDRESS);
	if (! nvram_is_owned (v) || v->__addr == NVRAM_BAD_ADDRESS)
		return NVRAM_INVALID;

	v->end = v->begin;
	if (v->__addr >= v->limit) {
		reason = NVRAM_EOF;
		goto ballout;
	}

	v->sign = sign;
	/* LY: добавляем сигнатуру к контрольной сумме. */
	v->crc = crc32_vak_byte (v->crc, (unsigned char) v->sign);
	v->crc = crc32_vak_byte (v->crc, (unsigned char) (v->sign >> 8));

	/* LY: записываем длину и CRC. */
	len = v->__addr - v->begin;
	v->__addr = v->begin + 4; nvram_write_word (v, len);
	/* debug_printf ("finalize-write: nvram_len = %d, nvram_crc = 0x%08lX\n",
		len, v->crc); */
	nvram_write_dword (v, v->crc);

	/* LY: отмечаем завершение процесса записи. */
	v->__addr = v->begin + 2; nvram_write_word (v, v->sign ^ 0xDEAD);
	v->end = v->begin + len;
	reason = NVRAM_OK;

ballout:
	v->__addr = NVRAM_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

void nvram_init (nvram_t *v, unsigned region_begin, unsigned region_end)
{
	assert (region_end > region_begin && region_end - region_begin > 255);
	assert (region_end <= NVRAM_BAD_ADDRESS);

	lock_init (&v->lock);
	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	v->begin = region_begin;
	v->limit = region_end;
	v->__addr = NVRAM_BAD_ADDRESS;
	v->end = v->begin;
	__arch_nvram_init (v);
	assert (! nvram_is_valid_addr (v, NVRAM_BAD_ADDRESS));
	assert (! nvram_is_valid_addr (v, 0));
	assert (! __nvram_is_valid_addr (v, NVRAM_BAD_ADDRESS));
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
}
