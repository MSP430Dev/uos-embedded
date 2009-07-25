#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>

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

static mutex_t lock;

/*
 * Write a byte to NVRAM.
 */
void
nvram_write_byte (unsigned addr, unsigned char c)
{
/*	assert (__arch_intr_is_enabled_now ());*/
	mutex_lock (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		mutex_wait (&lock);

	EEAR = addr;
	EEDR = c;

	asm volatile (
	       "cli \n"
	"	sbi %0, %1 \n"
	"	sbi %0, %2 \n"
	"	sei"
	 : /* no outputs */
	 : "I" (_SFR_IO_ADDR (EECR)), "I" (EEMWE), "I" (EEWE));

	mutex_unlock (&lock);
}

/*
 * Read a byte from NVRAM.
 */
unsigned char
nvram_read_byte (unsigned addr)
{
	unsigned char c;

	mutex_lock (&lock);

	/* Wait until EEWE becomes zero. */
	while (testb (EEWE, EECR))
		mutex_wait (&lock);

	EEAR = addr;
	setb (EERE, EECR);
	c = EEDR;

	mutex_unlock (&lock);
	return c;
}

void
nvram_init ()
{
	/* Associate the interrupt. */
	mutex_lock_irq (&lock, NVRAM_IRQ, 0, 0);
	mutex_unlock (&lock);
}
