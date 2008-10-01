#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

/*
  LY: Начинает транзакцию частичного обновления NVRAM.
*/
small_int_t nvram_begin_update (nvram_t *v, unsigned addr /* relative */)
{
	small_int_t reason;

	/* debug_printf ("begin-update: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X, addr=0x%04X\n",
		v->__addr, v->begin, v->end, addr); */
	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif
	assert (v->__addr == NVRAM_BAD_ADDRESS && v->end != v->begin);
	if (v->__addr != NVRAM_BAD_ADDRESS || v->end == v->begin) {
		/* LY: nvram еще небыл прочитан или записан. */
		reason = NVRAM_INVALID;
		goto ballout;
	}

	assert (nvram_is_valid_addr (v, addr));
	if (! nvram_is_valid_addr (v, addr)) {
		/* LY: начальный адрес вне допустимого диапазона. */
		reason = NVRAM_EOF;
		goto ballout;
	}

#if NVRAM_WRITE_TAINT
	/* Отмечаем начало процесса обновления. */
	v->__addr = v->begin + 2; nvram_write_word (v, 0xDEAD);
#endif

	/* Пропускаем заголовок. */
	v->__addr = v->begin + NVRAM_HEADER_SIZE;

	/* LY: инициализируем CRC32 = 2^32 / PI. */
	v->crc = 0x517CC1B7;

	/* LY: читаем байты до указанного адреса для пересчета CRC. */
	while (v->__addr < addr + v->begin)
		nvram_read_byte (v);
	return NVRAM_OK;

ballout:
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

/*
 * Завершает транзакцию частичного обновления NVRAM.
 */
small_int_t nvram_finalize_update (nvram_t *v)
{
	small_int_t reason;

	/* debug_printf ("finalize-update: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X\n",
		v->__addr, v->begin, v->end); */
	assert (v->__addr != NVRAM_BAD_ADDRESS
		&& v->end != v->begin
		&& nvram_is_owned (v));
	if (v->__addr == NVRAM_BAD_ADDRESS || v->end == v->begin
	|| ! nvram_is_owned (v))
		return NVRAM_INVALID;

	assert (v->__addr >= v->begin + NVRAM_HEADER_SIZE && v->__addr <= v->end);
	if (v->__addr < v->begin + NVRAM_HEADER_SIZE || v->__addr > v->end) {
		/* LY: фатальная ощибка, вышли за конец данных. */
		reason = NVRAM_EOF;
		goto ballout;
	}

	/* LY: читаем байты до конца для пересчета CRC. */
	while (v->__addr < v->end)
		nvram_read_byte (v);

	/* LY: Добавляем к CRC сигнатуру и длину. */
	v->crc = crc32_vak_byte (v->crc, (unsigned char) v->sign);
	v->crc = crc32_vak_byte (v->crc, (unsigned char) (v->sign >> 8));
	v->__addr = v->begin + 4; nvram_read_word (v);

	/* LY: обновляем CRC. */
	nvram_write_dword (v, v->crc);

#if NVRAM_WRITE_TAINT
	/* LY: отмечаем завершение процесса обновления. */
	v->__addr = v->begin + 2; nvram_write_word (v, v->sign ^ 0xDEAD);
#endif
	reason = NVRAM_OK;

ballout:
	v->__addr = NVRAM_BAD_ADDRESS;
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}
