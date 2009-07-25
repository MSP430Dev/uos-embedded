#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>

/*
  LY: Начинает транзакцию частичного чтения NVRAM.
*/
small_int_t nvdata_begin_load (nvdata_t *v, unsigned addr /* relative */)
{
	small_int_t reason;

	/* debug_printf ("begin-load: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X, addr=0x%04X\n",
		v->__addr, v->begin, v->end, addr); */
	mutex_lock (&v->lock);
#ifndef NDEBUG
	v->owner = task_current;
#endif

	assert (v->__addr == NVDATA_BAD_ADDRESS
		&& v->end != v->begin);
	if (v->__addr != NVDATA_BAD_ADDRESS
	|| v->end == v->begin) {
		/* LY: nvram еще небыл прочитан или записан. */
		reason = NVDATA_INVALID;
		goto ballout;
	}

	assert (nvdata_is_valid_addr (v, addr));
	if (! nvdata_is_valid_addr (v, addr)) {
		/* LY: начальный адрес вне допустимого диапазона. */
		reason = NVDATA_EOF;
		goto ballout;
	}

#if NVDATA_READ_TAINT
	/* Отмечаем начало процесса чтения. */
	v->__addr = v->begin + 2; nvdata_write_word (v, 0xDEAD);
#endif

	/* Переходим на указанный адрес. */
	v->__addr = addr + v->begin;
	return NVDATA_OK;

ballout:
#ifndef NDEBUG
	v->owner = 0;
#endif
	mutex_unlock (&v->lock);
	return reason;
}

/*
  LY: Завершает транзакцию частичного чтения NVRAM.
*/
small_int_t nvdata_finalize_load (nvdata_t *v)
{
	small_int_t reason;

	/* debug_printf ("finalize-update: v->__addr=0x%04X, v->begin=0x%04X, v->end=0x%04X\n",
		v->__addr, v->begin, v->end); */
	assert (v->__addr != NVDATA_BAD_ADDRESS
		&& nvdata_is_owned (v)
		&& v->end != v->begin);
	if (v->__addr == NVDATA_BAD_ADDRESS || v->end == v->begin
	|| ! nvdata_is_owned (v))
		return NVDATA_INVALID;

	assert (v->__addr >= v->begin + NVDATA_HEADER_SIZE && v->__addr <= v->end);
	if (v->__addr < v->begin + NVDATA_HEADER_SIZE || v->__addr > v->end) {
		/* LY: фатальная ощибка, вышли за конец данных. */
		reason = NVDATA_EOF;
		goto ballout;
	}

#if NVDATA_READ_TAINT
	/* LY: отмечаем завершение процесса чтения. */
	v->__addr = v->begin + 2; nvdata_write_word (v, v->sign ^ 0xDEAD);
#endif
	reason = NVDATA_OK;

ballout:
#ifndef NDEBUG
	v->owner = 0;
#endif
	v->__addr = NVDATA_BAD_ADDRESS;
	mutex_unlock (&v->lock);
	return reason;
}
