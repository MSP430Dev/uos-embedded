/*
 * Testing I2C.
 */
#include <runtime/lib.h>
#include <stream/stream.h>

#define I2C_SPEED       10		/* in KBits/s */

#define I2C_READ_OP	1
#define SLAVE_ADDR	0x90

void uos_init (void)
{
	debug_printf ("\n\nTesting I2C...\n");
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	MC_I2C_CTR = MC_I2C_PRST;
	MC_I2C_CTR = MC_I2C_EN;
	MC_I2C_PRER = KHZ / (5 * I2C_SPEED) - 1;

        /* Set address pointer register to select temperature */
	debug_printf ("Setting address pointer register up\n");
	MC_I2C_TXR = SLAVE_ADDR;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STA;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_TXR = 0;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STO;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	/* Read 2-byte temperature value */
	debug_printf ("Reading temperature value\n");
	MC_I2C_TXR = SLAVE_ADDR | I2C_READ_OP;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STA;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_CR = MC_I2C_RCV;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

        int8_t hi_byte = MC_I2C_RXR;
	debug_printf ("Read: %02X\n", hi_byte);
    
	MC_I2C_CR = MC_I2C_RCV | MC_I2C_NACK | MC_I2C_STO;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

        uint8_t low_byte = MC_I2C_RXR;
	debug_printf ("Read: %02X\n", low_byte);

        int16_t temp = ((hi_byte << 8) | low_byte) >> 6;
        debug_printf ("temp = %04X, approx. %d\n", temp, hi_byte);
}
