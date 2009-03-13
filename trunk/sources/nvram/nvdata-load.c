#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

/*
  LY: Начинает транзакцию частичного чтения NVRAM.
*/
small_int_t nvram_begin_load (nvram_t *v, unsigned addr /* relative */)
{
	small_int_t reason;

	/* debug_printf ("begin-load: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X, addr=0x%04X\n",
		v->__addr, v->begin, v->end, addr); */
	lock_take (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif

	assert (v->__addr == NVRAM_BAD_ADDRESS
		&& v->end != v->begin);
	if (v->__addr != NVRAM_BAD_ADDRESS
	|| v->end == v->begin) {
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

#if NVRAM_READ_TAINT
	/* Отмечаем начало процесса чтения. */
	v->__addr = v->begin + 2; nvram_write_word (v, 0xDEAD);
#endif

	/* Переходим на указанный адрес. */
	v->__addr = addr + v->begin;
	return NVRAM_OK;

ballout:
#ifndef NDEBUG
	v->owner = 0;
#endif
	lock_release (&v->lock);
	return reason;
}

/*
  LY: Завершает транзакцию частичного чтения NVRAM.
*/
small_int_t nvram_finalize_load (nvram_t *v)
{
	small_int_t reason;

	/* debug_printf ("finalize-update: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X\n",
		v->__addr, v->begin, v->end); */
	assert (v->__addr != NVRAM_BAD_ADDRESS
		&& nvram_is_owned (v)
		&& v->end != v->begin);
	if (v->__addr == NVRAM_BAD_ADDRESS || v->end == v->begin
	|| ! nvram_is_owned (v))
		return NVRAM_INVALID;

	assert (v->__addr >= v->begin + NVRAM_HEADER_SIZE && v->__addr <= v->end);
	if (v->__addr < v->begin + NVRAM_HEADER_SIZE || v->__addr > v->end) {
		/* LY: фатальная ощибка, вышли за конец данных. */
		reason = NVRAM_EOF;
		goto ballout;
	}

#if NVRAM_READ_TAINT
	/* LY: отмечаем завершение процесса чтения. */
	v->__addr = v->begin + 2; nvram_write_word (v, v->sign ^ 0xDEAD);
#endif
	reason = NVRAM_OK;

ballout:
#ifndef NDEBUG
	v->owner = 0;
#endif
	v->__addr = NVRAM_BAD_ADDRESS;
	lock_release (&v->lock);
	return reason;
}
