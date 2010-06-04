/*
 * Чтение/запись памяти и регистров MBA осуществляется с помощью обычного обращения
 * по адресу, чтение/запись регистров SWIC, PMSC и адресного окна PCI должна
 * осущестляться только с помощью функций mcb_read_reg/mcb_write_reg.
 * Для разъяснений см. Руководство пользователя на микросхему 1892ХД1Я, п. 5.3.
 */
#include <elvees/mcb-01.h>

unsigned
mcb_read_reg (unsigned addr)
{
	while (MCB_MBA_BUSY)
		continue;
	MCB_MBA_BDR = addr;
	while (MCB_MBA_BUSY)
		continue;
	return MCB_MBA_BDR;
}

void
mcb_write_reg (unsigned addr, unsigned value)
{
	volatile unsigned *modif_addr = (unsigned*) (addr | 0xae000000);
	while (MCB_MBA_BUSY)
		continue;
	*modif_addr = value;
}
