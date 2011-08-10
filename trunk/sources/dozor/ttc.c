#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include "ttc.h"
#include "ttc-reg.h"


void ttc_init (ttc_t *ttc, int over_spi, int ttc_num, spi_t *spi)
{
	ttc->led_state = 0;
	ttc->over_spi = over_spi;
	ttc->ttc_num = ttc_num;
	ttc->spi = spi;
}

unsigned short ttc_read16 (ttc_t *ttc, unsigned short addr)
{
	unsigned short received;

	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		ttc->spi_command [0] = addr;
		spi_output_block (ttc->spi, ttc->spi_command, 2);
		spi_input_wait (ttc->spi, &received);
		spi_input_wait (ttc->spi, &received);
	} else {
		received = *((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1)));
	}
	mutex_unlock (&ttc->lock);
	return received;
}

unsigned ttc_read32 (ttc_t *ttc, unsigned short addr)
{
	unsigned received;

	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		ttc->spi_command [0] = addr;
		spi_output_block (ttc->spi, ttc->spi_command, 3);
		spi_input_wait (ttc->spi, (unsigned short *) &received);
		spi_input_wait (ttc->spi, (unsigned short *) &received);
		spi_input_wait (ttc->spi, ((unsigned short *) &received) + 1);
	} else {
		received = *((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1)));
		received |= *((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1) + 4)) << 16;
	}
	mutex_unlock (&ttc->lock);
	return received;
}

void ttc_read_array (ttc_t *ttc, unsigned short addr, void *buf, int size)
{
	unsigned short received;
	int rd = 0;
	int i, sz, spi_block_count;
	unsigned short *p = (unsigned short *) buf;

	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		while (rd < size) {
			sz = size - rd;
			if (sz > 14) sz = 14;
			spi_block_count = (sz + 1) / 2 + 1; /* Размер посылки по SPI - один 16-битный адрес и данные */
			ttc->spi_command [0] = addr + rd;
			spi_output_block (ttc->spi, ttc->spi_command, spi_block_count);
			spi_input_wait (ttc->spi, &received);
			for (i = 0; i < spi_block_count - 1; ++i) {
				spi_input_wait (ttc->spi, p + (rd >> 1) + i);
			}
			rd += sz;
		}
	} else {
		i = 0;
		while (size > 0) {
			p [i++] = *((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1)));
			addr += 2;
			size -= 2;
		}

	}
	mutex_unlock (&ttc->lock);
}

void ttc_write16 (ttc_t *ttc, unsigned short addr, unsigned short data)
{
	unsigned short received;

	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		ttc->spi_command [0] = addr | 1;
		ttc->spi_command [1] = data;
		spi_output_block (ttc->spi, ttc->spi_command, 2);
		spi_input_wait (ttc->spi, &received);
		spi_input_wait (ttc->spi, &received);
	} else {
		*((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1))) = data;
	}
	mutex_unlock (&ttc->lock);
}

void ttc_write32 (ttc_t *ttc, unsigned short addr, unsigned data)
{
	unsigned short received;

	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		ttc->spi_command [0] = addr | 1;
		memcpy (ttc->spi_command + 1, &data, 4);
		spi_output_block (ttc->spi, ttc->spi_command, 3);
		spi_input_wait (ttc->spi, &received);
		spi_input_wait (ttc->spi, &received);
		spi_input_wait (ttc->spi, &received);
	} else {
		*((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1))) = data;
		*((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1) + 4)) = data >> 16;
	}
	mutex_unlock (&ttc->lock);
}

void ttc_write_array (ttc_t *ttc, unsigned short addr, const void *buf, int size)
{
	unsigned short received;
	int written = 0;
	int i, sz, spi_block_count;
	unsigned short *pbuf = (unsigned short *) buf;
	
	mutex_lock (&ttc->lock);
	if (ttc->over_spi) {
		while (written < size) {
			sz = size - written;
			if (sz > 14) sz = 14;
			spi_block_count = ((sz + 1) >> 1) + 1; /* Размер посылки по SPI - один 16-битный адрес и данные */
			ttc->spi_command [0] = (addr + written) | 1;
			memcpy (ttc->spi_command + 1, (char *) buf + written, sz);
			spi_output_block (ttc->spi, ttc->spi_command, spi_block_count);
			for (i = 0; i < spi_block_count; ++i)
				spi_input_wait (ttc->spi, &received);
			written += sz;
		}
	} else {
		i = 0;
		while (size > 0) {
			*((volatile unsigned short *) (TTC_ADDR_BASE(ttc->ttc_num) + (addr << 1))) = pbuf [i++];
			addr += 2;
			size -= 2;
		}
	}
	mutex_unlock (&ttc->lock);
}

/*
 * Извлечение 32-битного значения из контроллера TTP.
 */
int ttc_get_data32 (ttc_t *ttc, uint32_t *result,
	unsigned addr0, unsigned addr_status0,
	unsigned addr1, unsigned addr_status1)
{
	uint32_t s;

	/* Анализируем статусы обеих копий и выбираем правильное значение. */
	s = ttc_read32 (ttc, addr_status0);
	if (s & TTC_RSR_RDN) {
		*result = ttc_read32 (ttc, addr0);
		return 1;
	}
	s = ttc_read32 (ttc, addr_status1);
	if (s & TTC_RSR_RDN) {
		*result = ttc_read32 (ttc, addr1);
		return 1;
	}
	return 0;
}

/*
 * Ожидание стартового пакета в течение одного цикла.
 * Возврат 1, если пакет получен.
 */
int ttc_wait_start_packet (ttc_t *ttc, unsigned addr_status0, unsigned addr_status1,
	int *cluster_mode, int *start_node,
	uint32_t *cluster_time, uint32_t *local_time)
{
	uint32_t params;

	ttc_write16 (ttc, TTC_GSR, TTC_GSR_CCL);
	for (;;) {
		int s;

		s = ttc_read32 (ttc, addr_status0);
debug_printf ("<status0=%04x ", s);
		if (s) {
			if (s == TTC_RSR_STRT) {
				/* Принят стартовый пакет с шины 0. */
				params = ttc_read32 (ttc, addr_status0 + 4);
				*start_node = params >> 24;
				*cluster_mode = (uint8_t) (params >> 16);
				*cluster_time = ttc_read32 (ttc, addr_status0 + 8);
				*local_time = ttc_read32 (ttc, addr_status0 + 12);
				return 1;
			}
			ttc_write32 (ttc, addr_status0, 0);
		}
		s = ttc_read32 (ttc, addr_status1);
debug_printf ("status1=%04x ", s);
		if (s) {
			if (s == TTC_RSR_STRT) {
				/* Принят стартовый пакет с шины 1. */
				params = ttc_read32 (ttc, addr_status1 + 4);
				*start_node = params >> 24;
				*cluster_mode = (uint8_t) (params >> 16);
				*cluster_time = ttc_read32 (ttc, addr_status1 + 8);
				*local_time = ttc_read32 (ttc, addr_status1 + 12);
				return 1;
			}
			ttc_write32 (ttc, addr_status1, 0);
		}
		s = ttc_read32 (ttc, TTC_RSPC(0));
debug_printf ("rspc0=%04x ", s);
		s = ttc_read32 (ttc, TTC_RSPC(1));
debug_printf ("rspc1=%04x ", s);
		s = ttc_read16 (ttc, TTC_GSR);
debug_printf ("GSR=%04x>\n", s);
		if (s & TTC_GSR_CCL) {
			/* Закончился цикл кластера. */
			return 0;
		}
	}
}

/*
 * Самотестирование.
 * Возврашает 0 при ошибке.
 */
int ttc_self_check (ttc_t *ttc)
{
	unsigned errors, i, rval;

	debug_printf ("Check TTP registers... ");
	errors = 0;

	/* Пишем и читаем 16-битный регистр IER. */
	for (i=0; i<16; ++i) {
		ttc_write16 (ttc, TTC_IER, 1 << i);
		rval = ttc_read16 (ttc, TTC_IER);
		if (rval != 1 << i) {
			debug_printf ("IER register error: written %04x read %04x\n",
				1 << i, rval);
			++errors;
		}
	}
	/* Пишем и читаем 32-битный регистр SID. */
	for (i=0; i<32; ++i) {
		ttc_write32 (ttc, TTC_SID, 1 << i);
		rval = ttc_read32 (ttc, TTC_SID);
		if (rval != 1 << i) {
			debug_printf ("SID register error: written %08x read %08x\n",
				1 << i, rval);
			++errors;
		}
	}
	if (errors)
		return 0;

#if 0
	/* Расписываем и проверяем память MEDL. */
	debug_printf ("schedule memory... ");
	for (addr=(unsigned)TTC_MEDL; addr<(unsigned)TTC_MEDL+medl_bytes; addr+=4) {
		*(volatile unsigned*) addr = addr;
	}
	for (addr=(unsigned)TTC_MEDL; addr<(unsigned)TTC_MEDL+medl_bytes; addr+=4) {
		rval = *(volatile unsigned*) addr;
		if (rval != addr) {
			debug_printf ("Address %08x written %08x read %08x\n",
				addr, addr, rval);
			++errors;
		}
	}
	if (errors)
		return 0;

	/* Расписываем и проверяем память данных TTP. */
	debug_printf ("data memory... ");
	for (addr=(unsigned)TTC_RAM; addr<(unsigned)TTC_RAM+data_bytes; addr+=4) {
		*(volatile unsigned*) addr = addr;
	}
	for (addr=(unsigned)TTC_RAM; addr<(unsigned)TTC_RAM+data_bytes; addr+=4) {
		rval = *(volatile unsigned*) addr;
		if (rval != addr) {
			debug_printf ("Address %08x written %08x read %08x\n",
				addr, addr, rval);
			++errors;
		}
	}
	if (errors)
		return 0;
#endif

	debug_printf (" Done\n");
	return 1;
}

void ttc_led_on (ttc_t *ttc, int led_num)
{
	ttc->led_state |= 1 << led_num;
	ttc_write16 (ttc, TTC_LEDR, ttc->led_state);
}
void ttc_led_off (ttc_t *ttc, int led_num)
{
	ttc->led_state &= ~(1 << led_num);
	ttc_write16 (ttc, TTC_LEDR, ttc->led_state);
}

void ttc_led_sw (ttc_t *ttc, int led_num)
{
	ttc->led_state ^= 1 << led_num;
	ttc_write16 (ttc, TTC_LEDR, ttc->led_state);
}

int ttc_get_led (ttc_t *ttc, int led_num)
{
	return (ttc->led_state >> led_num) & 1;
}


