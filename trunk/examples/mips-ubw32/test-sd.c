/*
 * Testing SD card.
 * Two SD/MMC cards on SPI1.
 * Signals:
 *	D0  - SDO1
 *	D10 - SCK1
 *	C4  - SDI1
 *	A9  - /CS0
 *	A10 - /CS1
 *	G7  - CD0
 *	G6  - WE0
 *	G9  - CD1
 *	G8  - WE1
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>

#define CTL(c)		((c) & 037)

ARRAY (stack_console, 1000);
unsigned char data [512];

#define SDADDR		((struct sdreg*) &SPI1CON)
#define SLOW		250
#define FAST		20000
#define SECTSIZE	512

#define PIN_CS0		9	/* port A9 */
#define PIN_CS1		10	/* port A10 */
#define PIN_CD0		7	/* port G7 */
#define PIN_WE0		6	/* port G6 */
#define PIN_CD1		9	/* port G9 */
#define PIN_WE1		8	/* port G8 */

/*
 * Definitions for MMC/SDC commands.
 */
#define CMD_GO_IDLE		0		/* CMD0 */
#define CMD_SEND_OP_MMC		1		/* CMD1 (MMC) */
#define	CMD_SEND_OP_SDC		41              /* ACMD41 (SDC) */
#define CMD_SEND_IF		8
#define CMD_SEND_CSD		9
#define CMD_SEND_CID		10
#define CMD_STOP		12
#define CMD_STATUS_SDC 		(0x80+13)	/* ACMD13 (SDC) */
#define CMD_SET_BLEN		16
#define CMD_READ_SINGLE		17
#define CMD_READ_MULTIPLE	18
#define CMD_SET_BCOUNT		23		/* (MMC) */
#define	CMD_SET_WBECNT		(0x80+23)	/* ACMD23 (SDC) */
#define CMD_WRITE_SINGLE	24
#define CMD_WRITE_MULTIPLE	25
#define CMD_APP			55              /* CMD55 */
#define CMD_READ_OCR		58

/*
 * SPI registers.
 */
struct sdreg {
	volatile unsigned con;		/* Control */
        volatile unsigned conclr;
        volatile unsigned conset;
        volatile unsigned coninv;
        volatile unsigned stat;		/* Status */
        volatile unsigned statclr;
        volatile unsigned statset;
        volatile unsigned statinv;
        volatile unsigned buf;		/* Transmit and receive buffer */
        volatile unsigned unused1;
        volatile unsigned unused2;
        volatile unsigned unused3;
        volatile unsigned brg;		/* Baud rate generator */
        volatile unsigned brgclr;
        volatile unsigned brgset;
        volatile unsigned brginv;
};

/*
 * Send one byte of data and receive one back at the same time.
 */
static unsigned
spi_io (byte)
	unsigned byte;
{
	register struct	sdreg *reg = SDADDR;
        register int count;

	reg->buf = (unsigned char) byte;
	for (count=0; count<1000; count++)
                if (reg->stat & PIC32_SPISTAT_SPIRBF)
                        return (unsigned char) reg->buf;
        return 0xFF;
}

static inline void
spi_select (unit, on)
	int unit, on;
{
	if (on) {
		PORTACLR = (unit == 0) ? 1 << PIN_CS0 : 1 << PIN_CS1;
	} else {
                PORTASET = (unit == 0) ? 1 << PIN_CS0 : 1 << PIN_CS1;

                /* Need additional SPI clocks after deselect */
                spi_io (0xFF);
	}
}

/*
 * SD card connector detection switch.
 * Returns nonzero if the card is present.
 */
static int
card_detect (unit)
	int unit;
{
	if (unit == 0) {
		return ! (PORTG & (1 << PIN_CD0));
	} else {
		return ! (PORTG & (1 << PIN_CD1));
	}
}

/*
 * SD card write protect detection switch.
 * Returns nonzero if the card is writable.
 */
static int
card_writable (unit)
	int unit;
{
	if (unit == 0) {
		return ! (PORTG & (1 << PIN_WE0));
	} else {
		return ! (PORTG & (1 << PIN_WE1));
	}
}

/*
 * Send a command and address to SD media.
 * Return response:
 *   FF - timeout
 *   00 - command accepted
 *   01 - command received, card in idle state
 *
 * Other codes:
 *   bit 0 = Idle state
 *   bit 1 = Erase Reset
 *   bit 2 = Illegal command
 *   bit 3 = Communication CRC error
 *   bit 4 = Erase sequence error
 *   bit 5 = Address error
 *   bit 6 = Parameter error
 *   bit 7 = Always 0
 */
static int
card_cmd (cmd, addr)
	unsigned cmd, addr;
{
	int i, reply;
#if 0
	/* Fetch pending data. */
	for (i=0; i<1000; i++) {
		reply = spi_io (0xFF);
		if (reply == 0xFF)
			break;
	}
#endif
	/* Send a comand packet (6 bytes). */
	spi_io (cmd | 0x40);
	spi_io (addr >> 24);
	spi_io (addr >> 16);
	spi_io (addr >> 8);
	spi_io (addr);

	/* Send cmd checksum for CMD_GO_IDLE.
         * For all other commands, CRC is ignored. */
        if (cmd == CMD_GO_IDLE)
                spi_io (0x95);
        else
                spi_io (0xFF);

	/* Wait for a response. */
	for (i=0; i<200; i++) {
		reply = spi_io (0xFF);
		if (! (reply & 0x80))
		        break;
	}
	return reply;
}

/*
 * Initialize a card.
 * Return nonzero if successful.
 */
static int
card_init (unit)
	int unit;
{
	int i, reply;

	/* Unselect the card. */
	spi_select (unit, 0);

	/* Send 80 clock cycles for start up. */
	for (i=0; i<10; i++)
		spi_io (0xFF);

	/* Select the card and send a single GO_IDLE command. */
	spi_select (unit, 1);
	reply = card_cmd (CMD_GO_IDLE, 0);
	spi_select (unit, 0);
	if (reply != 1) {
		/* It must return Idle. */
		return 0;
	}

	/* Send repeatedly SEND_OP until Idle terminates. */
	for (i=0; ; i++) {
		spi_select (unit, 1);
		card_cmd (CMD_APP, 0);
		reply = card_cmd (CMD_SEND_OP_SDC, 0);
#if 0
                /* Fetch pending data. */
                int k;
                for (k=0; k<1000; k++) {
                        spi_io (0xFF);
                }
#endif
		spi_select (unit, 0);
//debug_printf ("card_init: SEND_OP reply = %d\n", reply);
		if (reply == 0)
			break;
		if (i >= 10000) {
			/* Init timed out. */
//debug_printf ("card_init: SEND_OP timed out, reply = %d\n", reply);
			return 0;
		}
	}
	return 1;
}

/*
 * Get number of sectors on the disk.
 * Return nonzero if successful.
 */
int
card_size (unit, nbytes)
	int unit;
	unsigned *nbytes;
{
	unsigned char csd [16];
	unsigned csize, n;
	int reply, i;

	spi_select (unit, 1);
	reply = card_cmd (CMD_SEND_CSD, 0);
	if (reply != 0) {
		/* Command rejected. */
		spi_select (unit, 0);
		return 0;
	}
	/* Wait for a response. */
	for (i=0; ; i++) {
		if (i >= 25000) {
			/* Command timed out. */
			spi_select (unit, 0);
			return 0;
		}
		reply = spi_io (0xFF);
		if (reply == 0xFE)
			break;
	}

	/* Read data. */
	for (i=0; i<sizeof(csd); i++) {
		csd [i] = spi_io (0xFF);
	}
	/* Ignore CRC. */
	spi_io (0xFF);
	spi_io (0xFF);

	/* Disable the card. */
	spi_select (unit, 0);

	if ((csd[0] >> 6) == 1) {
		/* SDC ver 2.00 */
		csize = csd[9] + (csd[8] << 8) + 1;
		*nbytes = csize << 10;
	} else {
		/* SDC ver 1.XX or MMC. */
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + (csd[7] << 2) + ((csd[6] & 3) << 10) + 1;
		*nbytes = csize << (n - 9);
	}
	return 1;
}

/*
 * Read a block of data.
 * Return nonzero if successful.
 */
int
card_read (unit, offset, data, bcount)
	int unit;
	unsigned offset, bcount;
	char *data;
{
	int reply, i;
#if 1
	debug_printf ("sd%d: read offset %u, length %u bytes, addr %p\n",
		unit, offset, bcount, data);
#endif
again:
	/* Send READ command. */
	spi_select (unit, 1);
	reply = card_cmd (CMD_READ_SINGLE, offset);
	if (reply != 0) {
		/* Command rejected. */
debug_printf ("card_read: bad READ_SINGLE reply = %d, offset = %08x\n", reply, offset);
		spi_select (unit, 0);
		return 0;
	}

	/* Wait for a response. */
	for (i=0; ; i++) {
		if (i >= 25000) {
			/* Command timed out. */
debug_printf ("card_read: READ_SINGLE timed out, reply = %d\n", reply);
			spi_select (unit, 0);
			return 0;
		}
		reply = spi_io (0xFF);
		if (reply == 0xFE)
			break;
if (reply != 0xFF) debug_printf ("card_read: READ_SINGLE reply = %d\n", reply);
	}

	/* Read data. */
        i = (bcount < SECTSIZE) ? bcount : SECTSIZE;
	while (i-- > 0)
		*data++ = spi_io (0xFF);

	/* Ignore CRC. */
        /* spi_io (0xFF); */
        /* spi_io (0xFF); */

	/* Disable the card. */
	spi_select (unit, 0);

        if (bcount > SECTSIZE) {
                bcount -= SECTSIZE;
                offset += SECTSIZE;
                goto again;
        }
	return 1;
}

/*
 * Write a block of data.
 * Return nonzero if successful.
 */
int
card_write (unit, offset, data, bcount)
	int unit;
	unsigned offset, bcount;
	char *data;
{
	unsigned reply, i;
#if 1
	debug_printf ("sd%d: write offset %u, length %u bytes, addr %p\n",
		unit, offset, bcount, data);
#endif
again:
	/* Check Write Protect. */
	if (! card_writable (unit))
		return 0;

	/* Send WRITE command. */
	spi_select (unit, 1);
	reply = card_cmd (CMD_WRITE_SINGLE, offset);
	if (reply != 0) {
		/* Command rejected. */
		spi_select (unit, 0);
		return 0;
	}

	/* Send data. */
	spi_io (0xFE);
	for (i=0; i<SECTSIZE; i++)
		spi_io (*data++);

	/* Send dummy CRC. */
	spi_io (0xFF);
	spi_io (0xFF);

	/* Check if data accepted. */
	reply = spi_io (0xFF);
	if ((reply & 0x1f) != 0x05) {
		/* Data rejected. */
		spi_select (unit, 0);
		return 0;
	}

	/* Wait for write completion. */
	for (i=0; ; i++) {
		if (i >= 250000) {
			/* Write timed out. */
			spi_select (unit, 0);
			return 0;
		}
		reply = spi_io (0xFF);
		if (reply != 0)
			break;
	}

	/* Disable the card. */
	spi_select (unit, 0);

        if (bcount > SECTSIZE) {
                bcount -= SECTSIZE;
                offset += SECTSIZE;
                goto again;
        }
	return 1;
}

/*
 * Enter 16-bit integer value, 5 digits.
 */
unsigned short get_short (unsigned short init_val)
{
	small_uint_t i, first_touch, cmd;
	unsigned long val;

	printf (&debug, "%d", init_val);
	if      (init_val > 9999) i = 5;
	else if (init_val > 999)  i = 4;
	else if (init_val > 99)   i = 3;
	else if (init_val > 9)    i = 2;
	else if (init_val > 0)    i = 1;
	else {
		i = 0;
		putchar (&debug, '\b');
	}
	first_touch = 1;
	val = init_val;
	for (;;) {
		cmd = getchar (&debug);
		if (cmd >= '0' && cmd <= '9') {
			if (first_touch) {
				first_touch = 0;
				while (i-- > 0)
					puts (&debug, "\b \b");
				i = 0;
				val = 0;
			}
			if (i == 5) {
err:				putchar (&debug, 7);
				continue;
			}
			val = val * 10 + cmd - '0';
			if (val > 0xffff) {
				val /= 10;
				goto err;
			}
			putchar (&debug, cmd);
			++i;
			continue;
		}
		first_touch = 0;
		if (cmd == CTL('[') || cmd == '[')
			continue;
		if (cmd == CTL('C'))
			return init_val;
		if (cmd == '\n' || cmd == '\r')
			return val;
		if (cmd == CTL('H') || cmd == 0177) {
			if (i == 0)
				goto err;
			printf (&debug, "\b \b");
			--i;
			val /= 10;
			continue;
		}
		goto err;
	}
}

void fill_data (unsigned byte0, unsigned byte1)
{
        unsigned char *p = data;
        unsigned i;

        for (i=0; i<SECTSIZE; i+=2) {
                *p++ = byte0;
                *p++ = byte1;
        }
}

int check_data (unsigned byte0, unsigned byte1)
{
        unsigned char *p = data;
        unsigned i;

        for (i=0; i<SECTSIZE; i+=2) {
                if (*p != byte0) {
                        printf (&debug, "Offset %u written %08X read %08X\n",
                                i, byte0, *p);
                        return 0;
                }
                p++;
                if (*p != byte1) {
                        printf (&debug, "Offset %u written %08X read %08X\n",
                                i, byte0, *p);
                        return 0;
                }
                p++;
        }
        return 1;
}

void test_sectors (unsigned first, unsigned last)
{
        unsigned i;

        for (i=first; i<last; i++) {
                fill_data (0x55^i, 0xaa^i);
                if (! card_write (0, i*SECTSIZE, data, SECTSIZE)) {
                        printf (&debug, "Sector %u: write error.\n", i);
                        break;
                }
                if (! card_read (0, i*SECTSIZE, data, SECTSIZE)) {
                        printf (&debug, "Sector %u: read error.\n", i);
                        break;
                }
                if (! check_data (0x55^i, 0xaa^i)) {
                        printf (&debug, "Sector %u: data error.\n", i);
                        break;
                }
        }
}

void print_data (unsigned char *buf, unsigned nbytes)
{
        unsigned i;

        printf (&debug, "%02x", *buf++);
        for (i=1; i<nbytes; i++) {
                if ((i & 15) == 0)
                        printf (&debug, "\n%02x", *buf++);
                else
                        printf (&debug, "-%02x", *buf++);
        }
        printf (&debug, "\n");
}

void menu ()
{
	small_uint_t cmd;

	printf (&debug, "\nSD card: %s\n", ! card_detect(0) ? "Not inserted" :
                card_writable(0) ? "Writable" : "Write protected");

	printf (&debug, "\n  1. Initialize a card");
	printf (&debug, "\n  2. Get card size");
	printf (&debug, "\n  3. Read sector #0");
	printf (&debug, "\n  4. Read sector #1");
	printf (&debug, "\n  5. Read sector #99");
	printf (&debug, "\n  6. Write sector #0");
	printf (&debug, "\n  7. Write-read sectors #0...99");
	puts (&debug, "\n\n");
	for (;;) {
		/* Ввод команды. */
		puts (&debug, "Command: ");
		while (peekchar (&debug) < 0)
			mdelay (5);
		cmd = getchar (&debug);
		putchar (&debug, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
                        if (card_init (0)) {
                                printf (&debug, "Card initialized successfully.\n");
                                SDADDR->stat = 0;
                                SDADDR->brg = (KHZ / FAST + 1) / 2 - 1;
                                SDADDR->con = PIC32_SPICON_MSTEN | PIC32_SPICON_CKE |
                                        PIC32_SPICON_ON;
                                printf (&debug, "Fast speed: %d bits/sec\n",
                                        (KHZ / (SDADDR->brg + 1) + 1) / 2);
                        }
			break;
		}
		if (cmd == '2') {
		        unsigned nbytes;
                        if (card_size (0, &nbytes))
                                printf (&debug, "%u bytes, %u kbytes, %u Mbytes\n",
                                        nbytes, nbytes/1024, nbytes/1024/1024);
			break;
		}
		if (cmd == '3') {
                        if (card_read (0, 0, data, SECTSIZE))
                                print_data (data, SECTSIZE);
			break;
		}
		if (cmd == '4') {
                        if (card_read (0, SECTSIZE, data, SECTSIZE))
                                print_data (data, SECTSIZE);
			break;
		}
		if (cmd == '5') {
                        if (card_read (0, 99*SECTSIZE, data, SECTSIZE))
                                print_data (data, SECTSIZE);
			break;
		}
		if (cmd == '6') {
                        if (card_write (0, 0, data, SECTSIZE))
                                printf (&debug, "Data written successfully.\n");
			break;
		}
		if (cmd == '7') {
                        test_sectors (0, 100);
			break;
		}
		if (cmd == CTL('T')) {
			/* Список задач uOS. */
			task_print (&debug, 0);
			task_print (&debug, (task_t*) stack_console);
			putchar (&debug, '\n');
			continue;
		}
	}
}

void console_task (void *data)
{
	for (;;)
		menu ();
}

void uos_init (void)
{
        /* Initialize hardware. */
        spi_select (0, 0);		// initially keep the SD card disabled
        TRISACLR = 1 << PIN_CS0;	// make Card select an output pin
        TRISACLR = 1 << PIN_CS1;

        /* Slow speed: max 40 kbit/sec. */
        SDADDR->stat = 0;
        SDADDR->brg = (KHZ / SLOW + 1) / 2 - 1;
        SDADDR->con = PIC32_SPICON_MSTEN | PIC32_SPICON_CKE |
                PIC32_SPICON_ON;

	printf (&debug, "\nTesting SD card.\n");
	printf (&debug, "Slow speed: %d bits/sec\n",
                (KHZ / (SDADDR->brg + 1) + 1) / 2);

	task_create (console_task, 0, "console", 10,
		stack_console, sizeof (stack_console));
}
