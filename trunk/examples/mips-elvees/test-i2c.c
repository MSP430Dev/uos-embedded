/*
 * Testing I2C.
 */
#include <runtime/lib.h>
#include <stream/stream.h>

#define I2C_SPEED       100		/* in KBits/s */

#define I2C_READ_OP	1
#define SLAVE_ADDR	0xA0

#define BYTE_TO_WRITE	0x5A

void uos_init (void)
{
	debug_printf ("\n\nTesting I2C...\n");
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	MC_I2C_CTR = MC_I2C_PRST;
	MC_I2C_CTR = MC_I2C_EN;
	MC_I2C_PRER = KHZ / (5 * I2C_SPEED) - 1;

	debug_printf ("Writing %02X at address 0x000\n", BYTE_TO_WRITE);
	MC_I2C_TXR = SLAVE_ADDR;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STA;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_TXR = 0;
	MC_I2C_CR = MC_I2C_SND;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_TXR = BYTE_TO_WRITE;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STO;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	/* Reading the byte back */
	debug_printf ("Reading the byte back\n");
	MC_I2C_TXR = SLAVE_ADDR;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STA;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_TXR = 0;
	MC_I2C_CR = MC_I2C_SND;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);
	
	MC_I2C_TXR = SLAVE_ADDR | I2C_READ_OP;
	MC_I2C_CR = MC_I2C_SND | MC_I2C_STA;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	MC_I2C_CR = MC_I2C_RCV | MC_I2C_NACK | MC_I2C_STO;
	//udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	debug_printf ("Read: %02X\n", MC_I2C_RXR);

	/*
	MC_I2C_CR = MC_I2C_RCV | MC_I2C_STO;
	udelay (1);
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
	debug_printf ("SR = %02X, CR = %02X\n", MC_I2C_SR, MC_I2C_CR);

	debug_printf ("Read: %02X\n", MC_I2C_RXR);
	*/

}
