#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>
/*#include <kernel/arch.h>*/

#ifdef __AVR_ATmega2561__
#     define NVRAM_IRQ		29	/* EEPROM write complete */
#else
#     define NVRAM_IRQ		21	/* EEPROM write complete */
#endif  /*__AVR_ATmega2561__*/

static lock_t lock;

/*
 * Write a byte to NVRAM.
 */
void
eeprom_write_byte (unsigned addr, unsigned char c)
{
/*	assert (__arch_intr_is_enabled_now ());*/
	lock_take (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		lock_wait (&lock);

	outw (addr, EEAR);
	outb (c, EEDR);
	asm volatile ("cli");
	setb (EEMWE, EECR);
	setb (EEWE, EECR);
	asm volatile ("sei");

	lock_release (&lock);
}

/*
 * Read a byte from NVRAM.
 */
unsigned char
eeprom_read_byte (unsigned addr)
{
	unsigned char c;

	lock_take (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		lock_wait (&lock);

	outw (addr, EEAR);
	setb (EERE, EECR);
	c = inb (EEDR);

	lock_release (&lock);
	return c;
}

void
eeprom_init (nvram_t *v)
{
	/* Associate the interrupt. */
	lock_take_irq (&lock, NVRAM_IRQ, 0, 0);
	lock_release (&lock);
}
