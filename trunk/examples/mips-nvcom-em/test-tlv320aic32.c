/*
 * Testing I2C.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <kernel/uos.h>
#include <elvees/i2c.h>

#define I2C_SPEED       100		/* in KBits/s */

#define I2C_READ_OP	1
#define SLAVE_ADDR	0x30

#define MFBSP_CHANNEL   3


#define WAVE_FORMAT_PCM 0x0001

typedef struct
{
    uint8_t     ckID[4];
    uint32_t    cksize;
} chunk_hdr_t;

typedef struct __attribute__((packed))
{
    uint16_t    wFormatTag;
    uint16_t    nChannels;
    uint32_t    nSamplesPerSec;
    uint32_t    nAvgBytesPerSec;
    uint16_t    nBlockAlign;
    uint16_t    wBitsPerSample;
} wave_fmt_t;


ARRAY (task_space, 0x400);	/* Memory for task stack */

static int wave_fmt_valid = 0;
static wave_fmt_t wave_fmt;

elvees_i2c_t i2c;

static inline void tlv320_write_reg(uint8_t addr, uint8_t val)
{
    miic_t *c = (miic_t *)&i2c;
    uint8_t tx_mes[] = {addr, val};
    miic_transaction(c, SLAVE_ADDR, tx_mes, 2, 0, 0);
}

static inline uint8_t tlv320_read_reg(uint8_t addr)
{
    uint8_t value = 0;
    miic_t *c = (miic_t *)&i2c;
    miic_transaction(c, SLAVE_ADDR, &addr, 1, &value, 1);
    return value;
}

void tlv320_init()
{
    tlv320_write_reg(0x03, 0x91);
    tlv320_write_reg(0x04, 0x24);
    tlv320_write_reg(0x05, 0x04);
    tlv320_write_reg(0x06, 0xF0);
    tlv320_write_reg(0x07, 0x8A);
    tlv320_write_reg(0x0F, 0x20);
    tlv320_write_reg(0x10, 0x20);
    tlv320_write_reg(0x13, 0x00);
    tlv320_write_reg(0x16, 0x00);
    tlv320_write_reg(0x19, 0x00);
    tlv320_write_reg(0x20, 0x18);
    tlv320_write_reg(0x21, 0x18);
    tlv320_write_reg(0x2B, 0xAF);
    tlv320_write_reg(0x2C, 0xAF);
    tlv320_write_reg(0x2D, 0x2F);
    tlv320_write_reg(0x2E, 0x2F);
    tlv320_write_reg(0x2F, 0xAF);
    tlv320_write_reg(0x33, 0x0C);
    tlv320_write_reg(0x34, 0x2F);
    tlv320_write_reg(0x35, 0x2F);
    tlv320_write_reg(0x36, 0xAF);
    tlv320_write_reg(0x3A, 0x0C);
    tlv320_write_reg(0x3E, 0x2F);
    tlv320_write_reg(0x3F, 0x2F);
    tlv320_write_reg(0x40, 0xAF);
    tlv320_write_reg(0x41, 0x0C);
    tlv320_write_reg(0x45, 0x2F);
    tlv320_write_reg(0x46, 0x2F);
    tlv320_write_reg(0x47, 0xAF);
    tlv320_write_reg(0x48, 0x0C);
    tlv320_write_reg(0x49, 0x2F);
    tlv320_write_reg(0x4A, 0x2F);
    tlv320_write_reg(0x4B, 0xAF);
    tlv320_write_reg(0x4C, 0x2F);
    tlv320_write_reg(0x4D, 0x2F);
    tlv320_write_reg(0x4E, 0xAF);
    tlv320_write_reg(0x4F, 0x08);
    tlv320_write_reg(0x50, 0x2F);
    tlv320_write_reg(0x51, 0x2F);
    tlv320_write_reg(0x52, 0xAF);
    tlv320_write_reg(0x56, 0x08);
    tlv320_write_reg(0x5A, 0x2F);
    tlv320_write_reg(0x5B, 0x2F);
    tlv320_write_reg(0x5C, 0xAF);
    tlv320_write_reg(0x5D, 0x08);
    
    tlv320_write_reg(0x25, 0xC0);
    tlv320_write_reg(0x33, 0x0D);
    tlv320_write_reg(0x41, 0x0D);
    tlv320_write_reg(0x56, 0x09);
    tlv320_write_reg(0x5D, 0x09);
    tlv320_write_reg(0x2B, 0x2F);
    tlv320_write_reg(0x2C, 0x2F);
}


/*
 * Map virtual address to physical address in FM mode.
 */
static unsigned
virt_to_phys (unsigned virtaddr)
{
	switch (virtaddr >> 28 & 0xE) {
	default:  return virtaddr + 0x40000000;		/* kuseg */
	case 0x8: return virtaddr - 0x80000000;		/* kseg0 */
	case 0xA: return virtaddr - 0xA0000000;		/* kseg1 */
	case 0xC: return virtaddr;			/* kseg2 */
	case 0xE: return virtaddr;			/* kseg3 */
	}
}

void init_i2s(int port)
{
    MC_CLKEN |= MC_CLKEN_MFBSP;

    MC_MFBSP_CSR(port) = MC_MFBSP_SPI_I2S_EN;

    MC_MFBSP_DIR(port) = MC_MFBSP_RCLK_DIR | MC_MFBSP_TCLK_DIR | 
        MC_MFBSP_TCS_DIR | MC_MFBSP_RCS_DIR | MC_MFBSP_TD_DIR;

	MC_MFBSP_RCTR(port) = 0;    // Пока не используем приёмник

    MC_MFBSP_TCTR_RATE(port) = 
        MC_MFBSP_TCLK_RATE(KHZ / (2 * 32 * wave_fmt.nSamplesPerSec / 1000) - 1) |
        MC_MFBSP_TCS_RATE(15);

	MC_MFBSP_TCTR(port) = MC_MFBSP_TNEG | MC_MFBSP_TDEL |
        MC_MFBSP_TWORDCNT(0) | MC_MFBSP_TCSNEG | MC_MFBSP_TMBF |
        MC_MFBSP_TWORDLEN(15) | MC_MFBSP_TPACK | MC_MFBSP_TSWAP |
        MC_MFBSP_TCS_CONT | MC_MFBSP_TCLK_CONT;

    MC_MFBSP_TSTART(port) = 1;
}

void tx_dma(int port, void *buf, int size)
{
    int sz;
    unsigned addr = (unsigned) buf;
    do {
        sz = (size < 0x80000) ? size : 0x80000;
        MC_IR_MFBSP_TX(port) = virt_to_phys (addr);
        MC_CSR_MFBSP_TX(port) = MC_DMA_CSR_WN(0) | 
            MC_DMA_CSR_WCX(sz / 8 - 1) | MC_DMA_CSR_RUN;
        while (MC_RUN_MFBSP_TX(port) & 1);
        size -= sz;
        addr += sz;
    } while (size > 0);
}

void do_play(void *snd_data, unsigned size)
{
    if (wave_fmt.wFormatTag != WAVE_FORMAT_PCM) {
        debug_printf ("Chunk with unsupported format: 0x%04X\n",
            wave_fmt.wFormatTag);
        return;
    }

    if (wave_fmt.nChannels != 2) {
        debug_printf ("Unsupported number of channels: %d\n",
            wave_fmt.nChannels);
        debug_printf ("Currently supported 16-bit stereo only\n");
        return;
    }

    if (wave_fmt.nBlockAlign/wave_fmt.nChannels != 2) {
        debug_printf ("Unsupported number of bits: %d\n",
            wave_fmt.nBlockAlign/wave_fmt.nChannels * 8);
        debug_printf ("Currently supported 16-bit stereo only\n");
        return;
    }

    init_i2s(MFBSP_CHANNEL);
    tlv320_init();

    debug_printf("Playing wave, size = %d... ", size);
    tx_dma(MFBSP_CHANNEL, snd_data, size);
    debug_printf("done\n");
}

void *parse_next_chunk(chunk_hdr_t *hdr)
{
    unsigned char id[5] = {0, 0, 0, 0, 0};
    static int n = 0;

    memcpy(id, hdr->ckID, 4);
    debug_printf("Chunk #%d: \"%s\"\n", n, id);
    n++;

    if (memcmp(hdr->ckID, "fmt ", 4) == 0) {
        memcpy(&wave_fmt, hdr + 1, sizeof(wave_fmt_t));
        wave_fmt_valid = 1;
    } else if (memcmp(hdr->ckID, "data", 4) == 0) {
        if (wave_fmt_valid)
            do_play(hdr + 1, hdr->cksize);
        else
            debug_printf ("Chunk \"data\", but format is unknown yet\n");
    
    } else {
        debug_printf ("Unknown chunk ID: %s, passing by...\n", id);
    }

    return (uint8_t *) (hdr + 1) + hdr->cksize;
}

void play_wave(void *file)
{
    unsigned char *filestart = file;
    unsigned char *p = file;
    chunk_hdr_t *hdr = (chunk_hdr_t *) p;
    unsigned filesize; 

    if (memcmp(hdr->ckID, "RIFF", 4) != 0) {
        debug_printf ("Not a RIFF file!\n");
        return;
    }
    filesize = hdr->cksize;

    p += sizeof(chunk_hdr_t);
    if (memcmp(p, "WAVE", 4) != 0) {
        debug_printf ("Not a WAVE file!\n");
        return;
    }

    p += 4;
    while (p - filestart < filesize) {
        p = parse_next_chunk((chunk_hdr_t *) p);
    }
}





void task (void *arg)
{
    unsigned char *p = (unsigned char *) 0xa0000000;  /* Wave file */
    
    elvees_i2c_init(&i2c, I2C_SPEED);
    /*
    int i;
    for (i = 0; i < 0x70; ++i)
        debug_printf("Register %2X: %02X\n", i, tlv320_read_reg(i));
    debug_printf("Switching register page\n");
    tlv320_write_reg(0, 1);
    for (i = 0; i < 25; ++i)
        debug_printf("Register %2X: %02X\n", i, tlv320_read_reg(i));
    */
    
    for (;;)
        play_wave(p);
}

void uos_init(void)
{
	debug_printf ("\n\nTesting TLV320AIC32...\n");

	//unsigned char *p = (unsigned char *) 0xa0000000;  /* Wave file */

    //for (;;)
    //    play_wave(p);
    
    /*
    i2c_init();
    int i;
    for (i = 0; i < 25; ++i)
        debug_printf("Register %2d: %02X\n", i, tlv320_read_reg(i));
    for (;;);
    */
    
    task_create (task, "task", "task", 1, task_space, sizeof (task_space));
}
