#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/eeprom.h>

#ifdef __AVR_ATmega2561__
#	define NVRAM_IRQ		29	/* EEPROM write complete */
#endif
#if defined (__AVR_ATmega128__) || \
    defined (__AVR_ATmega103__) ||\
    defined (__AVR_ATmega168__)
#	define NVRAM_IRQ		21	/* EEPROM write complete */
#endif
#ifdef __AVR_ATmega161__
#	define NVRAM_IRQ		20	/* EEPROM write complete */
#endif

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

	EEAR = addr;
	EEDR = c;

	asm volatile (
	       "cli \n"
	"	sbi %0, %1 \n"
	"	sbi %0, %2 \n"
	"	sei"
	 : /* no outputs */
	 : "I" (_SFR_IO_ADDR (EECR)), "I" (EEMWE), "I" (EEWE));

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

	EEAR = addr;
	setb (EERE, EECR);
	c = EEDR;

	lock_release (&lock);
	return c;
}

void
eeprom_init ()
{
	/* Associate the interrupt. */
	lock_take_irq (&lock, NVRAM_IRQ, 0, 0);
	lock_release (&lock);
}
