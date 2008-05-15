/*
 * Interface to 25c128 serial EEPROM.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "s3c4530/gpio.h"
#include "timer/timer.h"
#include "nvram/nvram.h"

static lock_t lock;

#define CMD_WREN	0x06	/* enable write operations */
#define CMD_WRDI	0x04	/* disable write operations */
#define CMD_RDSR	0x05	/* read status register */
#define CMD_WRSR	0x01	/* write status register */
#define CMD_READ	0x03	/* read data from memory */
#define CMD_WRITE	0x02	/* write data to memory */

#define STATUS_NOTREADY	0x01	/* busy with write operation */
#define STATUS_WEL	0x02	/* write enable state */
#define STATUS_BP0	0x04	/* block 0 protect */
#define STATUS_BP1	0x08	/* block 1 protect */
#define STATUS_WPEN	0x80	/* write protect pin enable */

#define PIN_SI		9	/* P9 - output */
#define PIN_SCK		10	/* P10 - output */
#define PIN_NCS		11	/* P11 - output */

// LY:	для решения конфликта и ликвидации частых ошибок
//	номер пина вынесен во внешнюю переменную nvram_pin_so,
//	которая должна быть определена в клиентском коде.
extern const uint_t nvram_so_pin;
//	Раньше было так:
//	#if 0
//	#	define PIN_SO	22	/* P22 - input, так на мостах ethernet */
//	#else
//	#	define PIN_SO	12	/* P12 - input: так на RMC2, на других может быть иначе */
//	#endif

static void
chip_select (bool_t select)
{
	/* Keep /CS low for at least 1000 nsec. */
	gpio_set (PIN_NCS, 0);
	gpio_set (PIN_NCS, 0);
	gpio_set (PIN_NCS, 0);
	gpio_set (PIN_NCS, 0);
	gpio_set (PIN_NCS, 0);
	gpio_set (PIN_NCS, 0);

	if (! select) {
		/* Keep /CS high for at least 1000 nsec. */
		gpio_set (PIN_NCS, 1);
		gpio_set (PIN_NCS, 1);
		gpio_set (PIN_NCS, 1);
		gpio_set (PIN_NCS, 1);
		gpio_set (PIN_NCS, 1);
		gpio_set (PIN_NCS, 1);
	}
}

static void
send_byte (uint_t val)
{
	int i;

	/* Send 8 bit data */
	for (i = 7; i >= 0; --i) {
		gpio_set (PIN_SI, (val >> i) & 1);
		gpio_set (PIN_SCK, 1);
		gpio_set (PIN_SCK, 1);	/* tWH = 250nsec */
		gpio_set (PIN_SCK, 0);
		gpio_set (PIN_SCK, 0);	/* tWL = 250nsec */
	}
}

static uint_t
get_byte (bool_t last)
{
	int_t i;
	uint_t val;

	/* Receive 8 bit data */
	val = 0;
	for (i = 7; i >= 0; --i) {
		val |= gpio_get (nvram_so_pin) << i;
		if (last && i == 0)	/* Skip last clock */
			break;
		gpio_set (PIN_SCK, 1);
		gpio_set (PIN_SCK, 1);	/* tWH = 250nsec */
		gpio_set (PIN_SCK, 0);
		gpio_set (PIN_SCK, 0);	/* tWL = 250nsec */
	}
	return val;
}

static uint_t
read_status (void)
{
	uint_t val;

	chip_select (1);
	send_byte (CMD_RDSR);
	val = get_byte (1);
	chip_select (0);
	return val;
}

static void
write_status (uint_t val, timer_t *timer)
{
	while (read_status () != val) {

		/* Write enable - set WEL. */
		chip_select (1);
		send_byte (CMD_WREN);
		chip_select (0);

		chip_select (1);
		send_byte (CMD_WRSR);
		send_byte (val);
		chip_select (0);

		watchdog_alive ();
		/* Need a 10 msec pause here. */
		timer_delay (timer, 10);
	}
}

/*
 * Write a byte to NVRAM.
 */
void
__arch_nvram_write_byte (unsigned addr, unsigned char val)
{
	lock_take (&lock);

	chip_select (1);
	send_byte (CMD_WREN);
	chip_select (0);

	chip_select (1);
	send_byte (CMD_WRITE);
	send_byte (addr >> 8);
	send_byte (addr);
	send_byte (val);
	chip_select (0);

	/* Wait for write to complete. */
	while (read_status() & STATUS_NOTREADY)
		continue;

	lock_release (&lock);
}

/*
 * Read a byte from NVRAM.
 */
unsigned char
__arch_nvram_read_byte (unsigned addr)
{
	unsigned val;

	lock_take (&lock);

	chip_select (1);
	send_byte (CMD_READ);
	send_byte (addr >> 8);
	send_byte (addr);
	val = get_byte (1);
	chip_select (0);

	lock_release (&lock);
	return val;
}

void
nvram_unprotect (nvram_t *v, timer_t *timer)
{
	lock_take (&v->lock);
	lock_take (&lock);

	/* Clear WPEN, BP0, BP1. */
	write_status (0, timer);

	lock_release (&lock);
	lock_release (&v->lock);
}

void
nvram_protect (nvram_t *v, timer_t *timer)
{
	lock_take (&v->lock);
	lock_take (&lock);

	/* Set BP0, BP1. */
	write_status (STATUS_BP0 | STATUS_BP1, timer);

	lock_release (&lock);
	lock_release (&v->lock);
}

void
__arch_nvram_init (nvram_t *v)
{
	lock_init (&lock);
	lock_take (&lock);

	/* Set pins to initial state. */
	gpio_set (PIN_NCS, 1);
	gpio_set (PIN_SI, 0);
	gpio_set (PIN_SCK, 0);

	gpio_config (PIN_NCS, 1);	/* P11 - output */
	gpio_config (PIN_SI, 1);	/* P9 - output */
	gpio_config (PIN_SCK, 1);	/* P10 - output */
	gpio_config (nvram_so_pin, 0);	/* P22 - input */

	/* Dummy transaction just to activate device. */
	read_status ();
	lock_release (&lock);
}
