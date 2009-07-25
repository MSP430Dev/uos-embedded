/*
 * Interface to 25c128 serial EEPROM.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <s3c4530/gpio.h>
#include <timer/timer.h>
#include <nvram/nvram.h>

static mutex_t lock;

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

/*
 * Номер пина SO отличается на разных моделях.
 * Он вынесен во внешний макрос NVRAM_PIN_SO и должен быть
 * установлен в файле конфигурации target.cfg, например:
 *	CFLAGS += -DNVRAM_PIN_SO=12 # Для RMC2
 *
 * Раньше было так:
 * #if 0
 * #	define PIN_SO	22	/ * P22 - input, так на мостах ethernet * /
 * #else
 * #	define PIN_SO	12	/ * P12 - input: так на RMC2, на других может быть иначе * /
 * #endif
 */

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
send_byte (small_uint_t val)
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

static small_uint_t
get_byte (bool_t last)
{
	small_int_t i;
	small_uint_t val;

	/* Receive 8 bit data */
	val = 0;
	for (i = 7; i >= 0; --i) {
		val |= gpio_get (NVRAM_PIN_SO) << i;
		if (last && i == 0)	/* Skip last clock */
			break;
		gpio_set (PIN_SCK, 1);
		gpio_set (PIN_SCK, 1);	/* tWH = 250nsec */
		gpio_set (PIN_SCK, 0);
		gpio_set (PIN_SCK, 0);	/* tWL = 250nsec */
	}
	return val;
}

static small_uint_t
read_status (void)
{
	small_uint_t val;

	chip_select (1);
	send_byte (CMD_RDSR);
	val = get_byte (1);
	chip_select (0);
	return val;
}

static void
write_status (small_uint_t val, timer_t *timer)
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
nvram_write_byte (unsigned addr, unsigned char val)
{
	mutex_lock (&lock);

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

	mutex_unlock (&lock);
}

/*
 * Read a byte from NVRAM.
 */
unsigned char
nvram_read_byte (unsigned addr)
{
	unsigned val;

	mutex_lock (&lock);

	chip_select (1);
	send_byte (CMD_READ);
	send_byte (addr >> 8);
	send_byte (addr);
	val = get_byte (1);
	chip_select (0);

	mutex_unlock (&lock);
	return val;
}

void
nvram_unprotect (timer_t *timer)
{
	mutex_lock (&lock);

	/* Clear WPEN, BP0, BP1. */
	write_status (0, timer);

	mutex_unlock (&lock);
}

void
nvram_protect (timer_t *timer)
{
	mutex_lock (&lock);

	/* Set BP0, BP1. */
	write_status (STATUS_BP0 | STATUS_BP1, timer);

	mutex_unlock (&lock);
}

void
nvram_init ()
{
	mutex_lock (&lock);

	/* Set pins to initial state. */
	gpio_set (PIN_NCS, 1);
	gpio_set (PIN_SI, 0);
	gpio_set (PIN_SCK, 0);

	gpio_config (PIN_NCS, 1);	/* P11 - output */
	gpio_config (PIN_SI, 1);	/* P9 - output */
	gpio_config (PIN_SCK, 1);	/* P10 - output */
	gpio_config (NVRAM_PIN_SO, 0);	/* P22 - input */

	/* Dummy transaction just to activate device. */
	read_status ();
	mutex_unlock (&lock);
}
