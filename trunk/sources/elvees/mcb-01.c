/*
 * Чтение/запись памяти и регистров MBA осуществляется с помощью обычного обращения
 * по адресу, чтение/запись регистров SWIC, PMSC и адресного окна PCI должна
 * осущестляться только с помощью функций mcb_read_reg/mcb_write_reg.
 * Для разъяснений см. Руководство пользователя на микросхему 1892ХД1Я, п. 5.3.
 */
#include <runtime/lib.h>
#include <elvees/mcb-01.h>

#define TIMEOUT	100000

unsigned
mcb_read_reg (unsigned addr)
{
	unsigned count;
	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
//	if (! count)
//		debug_printf ("mcb_read_reg: timeout before writing %08X to BDR\n",
//			addr);

	MCB_MBA_BDR = addr;

	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count) {
//		debug_printf ("mcb_read_reg: timeout after writing %08X to BDR\n",
//			addr);
		return ~0;
	}
	return MCB_MBA_BDR;
}

void
mcb_write_reg (unsigned addr, unsigned value)
{
	volatile unsigned *modif_addr = (unsigned*) (addr | 0xae000000);
	unsigned count;
	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count)
		debug_printf ("mcb_write_reg: timeout before writing %08X to %08X\n",
			value, modif_addr);

	*modif_addr = value;

	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count)
		debug_printf ("mcb_write_reg: timeout after writing %08X to %08X\n",
			value, modif_addr);
}
