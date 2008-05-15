/*
 * Intel 8253 programmable counter / timer chip.
 */
#define I8253_CNTR0	0x040		/* timer 0 counter port */
#define I8253_CNTR1	0x041		/* timer 1 counter port */
#define I8253_CNTR2	0x042		/* timer 2 counter port */
#define I8253_MODE	0x043		/* timer mode port */

#define I8253_MODE_SEL0		0x00	/* select counter 0 */
#define I8253_MODE_SEL1		0x40	/* select counter 1 */
#define I8253_MODE_SEL2		0x80	/* select counter 2 */
#define I8253_MODE_INTTC	0x00	/* mode 0, intr on terminal cnt */
#define I8253_MODE_ONESHOT	0x02	/* mode 1, one shot */
#define I8253_MODE_RATEGEN	0x04	/* mode 2, rate generator */
#define I8253_MODE_SQWAVE	0x06	/* mode 3, square wave */
#define I8253_MODE_SWSTROBE	0x08	/* mode 4, s/w triggered strobe */
#define I8253_MODE_HWSTROBE	0x0a	/* mode 5, h/w triggered strobe */
#define I8253_MODE_LATCH	0x00	/* latch counter for reading */
#define I8253_MODE_LSB		0x10	/* r/w counter LSB */
#define I8253_MODE_MSB		0x20	/* r/w counter MSB */
#define I8253_MODE_16BIT	0x30	/* r/w counter 16 bits, LSB first */
#define I8253_MODE_BCD		0x01	/* count in BCD */
