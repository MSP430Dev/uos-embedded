#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>
#include <kernel/arch.h>

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
__arch_nvram_write_byte (unsigned addr, unsigned char c)
{
	assert (__arch_intr_is_enabled_now ());
	lock_take (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		lock_wait (&lock);

	outw (addr, EEAR);
	outb (c, EEDR);
	__arch_cli ();
	setb_const (EEMWE, EECR);
	setb_const (EEWE, EECR);
	__arch_sti ();

	lock_release (&lock);
}

/*
 * Read a byte from NVRAM.
 */
unsigned char
__arch_nvram_read_byte (unsigned addr)
{
	unsigned char c;

	lock_take (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		lock_wait (&lock);

	outw (addr, EEAR);
	setb_const (EERE, EECR);
	c = inb (EEDR);

	lock_release (&lock);
	return c;
}

void
__arch_nvram_init (nvram_t *v)
{
	/* Associate the interrupt. */
	lock_init (&lock);
	lock_take_irq (&lock, NVRAM_IRQ, 0, 0);
	lock_release (&lock);
}
