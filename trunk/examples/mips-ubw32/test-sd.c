/*
 * Testing SD card.
 *
 * These cards are known to work:
 * 1) NCP SD 256Mb       - type 1, 249856 kbytes,  244 Mbytes
 * 2) Patriot SD 2Gb     - type 2, 1902592 kbytes, 1858 Mbytes
 * 3) Wintec microSD 2Gb - type 2, 1969152 kbytes, 1923 Mbytes
 * 4) Transcend SDHC 4Gb - type 3, 3905536 kbytes, 3814 Mbytes
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>

/*
 * Signals for SPI1:
 *	D0  - SDO1
 *	D10 - SCK1
 *	C4  - SDI1
 */
#define SD_PORT     SPI1CON

#define SD_CS0_PORT TRISA       /* A9 - /CS0 */
#define SD_CS0_PIN  9
#define SD_CS1_PORT TRISA       /* A10 - /CS1 */
#define SD_CS1_PIN  10

/*
 * Port i/o access, relative to TRIS base.
 */
#define TRIS_CLR(p)     (&p)[1]
#define TRIS_SET(p)     (&p)[2]
#define TRIS_INV(p)     (&p)[3]
#define PORT_CLR(p)     (&p)[5]
#define PORT_SET(p)     (&p)[6]
#define PORT_INV(p)     (&p)[7]

#define CTL(c)		((c) & 037)

#define SDADDR          ((struct sdreg*) &SD_PORT)
#define SLOW		250
#define FAST		14000
#define SECTSIZE	512

ARRAY (stack_console, 1000);
unsigned char data [SECTSIZE * 2];
int sd_type[2];                 /* Card type */

/*
 * Definitions for MMC/SDC commands.
 */
#define CMD_GO_IDLE		0	/* CMD0 */
#define CMD_SEND_OP_MMC		1	/* CMD1 (MMC) */
#define CMD_SEND_IF_COND	8
#define CMD_SEND_CSD		9
#define CMD_SEND_CID		10
#define CMD_STOP		12
#define CMD_STATUS_SDC 		13      /* ACMD13 (SDC) */
#define CMD_SET_BLEN		16
#define CMD_READ_SINGLE		17
#define CMD_READ_MULTIPLE	18
#define CMD_SET_BCOUNT		23	/* (MMC) */
#define	CMD_SET_WBECNT		23      /* ACMD23 (SDC) */
#define CMD_WRITE_SINGLE	24
#define CMD_WRITE_MULTIPLE	25
#define	CMD_SEND_OP_SDC		41      /* ACMD41 (SDC) */
#define CMD_APP			55      /* CMD55 */
#define CMD_READ_OCR		58

#define DATA_START_BLOCK        0xFE    /* start data token for read or write single block */
#define STOP_TRAN_TOKEN         0xFD    /* stop token for write multiple blocks */
#define WRITE_MULTIPLE_TOKEN    0xFC    /* start data token for write multiple blocks */

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
static inline unsigned
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
        switch (unit) {
        case 0:
                if (on)
                        PORT_CLR(SD_CS0_PORT) = 1 << SD_CS0_PIN;
                else
                        PORT_SET(SD_CS0_PORT) = 1 << SD_CS0_PIN;
                break;
#ifdef SD_CS1_PORT
        case 1:
                if (on)
                        PORT_CLR(SD_CS1_PORT) = 1 << SD_CS1_PIN;
                else
                        PORT_SET(SD_CS1_PORT) = 1 << SD_CS1_PIN;
                break;
#endif
        }
        if (! on) {
                /* Need additional SPI clocks after deselect */
                spi_io (0xFF);
        }
}

/*
 * Wait while busy, up to 300 msec.
 */
static void
spi_wait_ready ()
{
        int i;

	for (i=0; i<100000; i++)
                if (spi_io (0xFF) == 0xFF)
                        break;
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

        /* Wait while busy, up to 300 msec. */
        spi_wait_ready ();

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
        else if (cmd == CMD_SEND_IF_COND)
                spi_io (0x87);
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
        unsigned char response[4];

	/* Unselect the card. */
	spi_select (unit, 0);
        sd_type[unit] = 0;

	/* Send 80 clock cycles for start up. */
	for (i=0; i<10; i++)
		spi_io (0xFF);

	/* Select the card and send a single GO_IDLE command. */
	spi_select (unit, 1);
	reply = card_cmd (CMD_GO_IDLE, 0);
	spi_select (unit, 0);
	if (reply != 1) {
		/* It must return Idle. */
                debug_printf ("No card inserted, reply = %02x\n", reply);
		return 0;
	}

        /* Check SD version. */
	spi_select (unit, 1);
        reply = card_cmd (CMD_SEND_IF_COND, 0x1AA);
        if (reply & 4) {
                /* Illegal command: card type 1. */
                spi_select (unit, 0);
                sd_type[unit] = 1;
        } else {
                response[0] = spi_io (0xFF);
                response[1] = spi_io (0xFF);
                response[2] = spi_io (0xFF);
                response[3] = spi_io (0xFF);
                spi_select (unit, 0);
                if (response[3] != 0xAA) {
                        debug_printf ("sd%d: cannot detect card type, response=%02x-%02x-%02x-%02x\n",
                                unit, response[0], response[1], response[2], response[3]);
                        return 0;
                }
                sd_type[unit] = 2;
        }

	/* Send repeatedly SEND_OP until Idle terminates. */
	for (i=0; ; i++) {
		spi_select (unit, 1);
		card_cmd (CMD_APP, 0);
		reply = card_cmd (CMD_SEND_OP_SDC,
                        sd_type[unit] == 2 ? 0x40000000 : 0);
		spi_select (unit, 0);
		if (reply == 0)
			break;
		if (i >= 10000) {
			/* Init timed out. */
			return 0;
		}
	}

        /* If SD2 read OCR register to check for SDHC card. */
        if (sd_type[unit] == 2) {
		spi_select (unit, 1);
                reply = card_cmd (CMD_READ_OCR, 0);
                if (reply != 0) {
                        spi_select (unit, 0);
                        debug_printf ("sd%d: READ_OCR failed, reply=%02x\n",
                                unit, reply);
                        return 0;
                }
                response[0] = spi_io (0xFF);
                response[1] = spi_io (0xFF);
                response[2] = spi_io (0xFF);
                response[3] = spi_io (0xFF);
                spi_select (unit, 0);
                if ((response[0] & 0xC0) == 0xC0) {
                        sd_type[unit] = 3;
                }
        }
	return 1;
}

/*
 * Get number of sectors on the disk.
 * Return nonzero if successful.
 */
int
card_size (unit, nblocks)
	int unit;
	unsigned *nblocks;
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
		if (reply == DATA_START_BLOCK)
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
		*nblocks = csize << 10;
	} else {
		/* SDC ver 1.XX or MMC. */
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + (csd[7] << 2) + ((csd[6] & 3) << 10) + 1;
		*nblocks = csize << (n - 9);
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
again:
	/* Send READ command. */
	spi_select (unit, 1);
	reply = card_cmd (CMD_READ_SINGLE, sd_type[unit] == 3 ? offset>>9 : offset);
	if (reply != 0) {
		/* Command rejected. */
		spi_select (unit, 0);
		return 0;
	}

	/* Wait for a response. */
	for (i=0; ; i++) {
		if (i >= 250000) {
			/* Command timed out. */
			spi_select (unit, 0);
			return 0;
		}
		reply = spi_io (0xFF);
		if (reply == DATA_START_BLOCK)
			break;
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

	/* Send pre-erase count. */
	spi_select (unit, 1);
        card_cmd (CMD_APP, 0);
	reply = card_cmd (CMD_SET_WBECNT,
                (bcount + SECTSIZE - 1) / SECTSIZE);
	if (reply != 0) {
		/* Command rejected. */
		spi_select (unit, 0);
		return 0;
	}

	/* Send write-multiple command. */
	reply = card_cmd (CMD_WRITE_MULTIPLE,
                sd_type[unit] == 3 ? offset>>9 : offset);
	if (reply != 0) {
		/* Command rejected. */
		spi_select (unit, 0);
		return 0;
	}
	spi_select (unit, 0);
again:
        /* Select, wait while busy. */
	spi_select (unit, 1);
        spi_wait_ready ();

	/* Send data. */
	spi_io (WRITE_MULTIPLE_TOKEN);
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
	spi_select (unit, 0);

        if (bcount > SECTSIZE) {
                bcount -= SECTSIZE;
                offset += SECTSIZE;
                goto again;
        }

        /* End a write multiple blocks sequence. */
	spi_select (unit, 1);
        spi_wait_ready ();
	spi_io (STOP_TRAN_TOKEN);
        spi_wait_ready ();
	spi_select (unit, 0);
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

void fill_sector (unsigned char *p, unsigned byte0, unsigned byte1)
{
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
        unsigned i, r0, r1, w0, w1, kbytes;

        printf (&debug, "Testing sectors %u-%u\n", first, last);
        printf (&debug, "Write...");
        w0 = mips_read_c0_register (C0_COUNT);
        for (i=first; i<last; i+=2) {
                fill_sector (data, 0x55^i, 0xaa^i);
                fill_sector (data+SECTSIZE, 0x55^(i+1), 0xaa^(i+1));
                if (! card_write (0, i*SECTSIZE, data, SECTSIZE*2)) {
                        printf (&debug, "Sector %u: write error.\n", i);
                        break;
                }
        }
        w1 = mips_read_c0_register (C0_COUNT);
        printf (&debug, " done\n");
        printf (&debug, "Verify...");
        r0 = mips_read_c0_register (C0_COUNT);
        for (i=first; i<last; i++) {
                if (! card_read (0, i*SECTSIZE, data, SECTSIZE)) {
                        printf (&debug, "Sector %u: read error.\n", i);
                        break;
                }
                if (! check_data (0x55^i, 0xaa^i)) {
                        printf (&debug, "Sector %u: data error.\n", i);
                        break;
                }
        }
        r1 = mips_read_c0_register (C0_COUNT);
        printf (&debug, " done\n");

        kbytes = (last - first + 1) * SECTSIZE / 1024;
        printf (&debug, "Write: %u kbytes/sec\n", kbytes * KHZ/2 * 1000 / (w1 - w0));
        printf (&debug, " Read: %u kbytes/sec\n", kbytes * KHZ/2 * 1000 / (r1 - r0));
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

	printf (&debug, "\n  1. Initialize a card");
	printf (&debug, "\n  2. Get card size");
	printf (&debug, "\n  3. Read sector #0");
	printf (&debug, "\n  4. Read sector #1");
	printf (&debug, "\n  5. Read sector #99");
	printf (&debug, "\n  6. Write sector #0");
	printf (&debug, "\n  7. Write-read sectors #0...200");
	puts (&debug, "\n\n");
	for (;;) {
		/* Ввод команды. */
		puts (&debug, "Command: ");
		while (peekchar (&debug) < 0)
			mdelay (5);
		cmd = getchar (&debug);
                printf (&debug, "\r\33[K");

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
                        if (card_init (0)) {
                                printf (&debug, "Card initialized successfully.\n");
                                SDADDR->stat = 0;
                                SDADDR->brg = (KHZ / FAST + 1) / 2 - 1;
                                SDADDR->con = PIC32_SPICON_MSTEN | PIC32_SPICON_CKE |
                                        PIC32_SPICON_ON;
                                printf (&debug, "Fast speed: %d kbit/sec\n",
                                        (KHZ / (SDADDR->brg + 1) + 1) / 2);
                        }
			break;
		}
		if (cmd == '2') {
		        unsigned nblocks;
                        if (card_size (0, &nblocks))
                                printf (&debug, "%u blocks, %u kbytes, %u Mbytes\n",
                                        nblocks, nblocks/2, nblocks/2/1024);
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
                        test_sectors (0, 200);
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

        // make Card select an output pin
        TRIS_CLR(SD_CS0_PORT) = 1 << SD_CS0_PIN;
#ifdef SD_CS1_PORT
        TRIS_CLR(SD_CS1_PORT) = 1 << SD_CS1_PIN;
#endif
        /* Slow speed: max 40 kbit/sec. */
        SDADDR->stat = 0;
        SDADDR->brg = (KHZ / SLOW + 1) / 2 - 1;
        SDADDR->con = PIC32_SPICON_MSTEN | PIC32_SPICON_CKE |
                PIC32_SPICON_ON;

	printf (&debug, "\nTesting SD card.\n");
	printf (&debug, "Slow speed: %d kbit/sec\n",
                (KHZ / (SDADDR->brg + 1) + 1) / 2);

	task_create (console_task, 0, "console", 10,
		stack_console, sizeof (stack_console));
}
