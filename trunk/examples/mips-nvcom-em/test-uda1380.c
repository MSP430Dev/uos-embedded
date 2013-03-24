/*
 * Testing I2C.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include "uda1380.h"

#define I2C_SPEED       100		/* in KBits/s */

#define I2C_READ_OP	1
#define SLAVE_ADDR	0x30

#define BYTE_TO_WRITE	0x5A

#define MFBSP_CHANNEL   2


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

static int wave_fmt_valid = 0;
static wave_fmt_t wave_fmt;


void i2c_init()
{
	MC_I2C_CTR = MC_I2C_PRST;
	MC_I2C_CTR = MC_I2C_EN;
	MC_I2C_PRER = KHZ / (5 * I2C_SPEED) - 1;
}

void i2c_write(uint8_t data, uint8_t flags)
{
	MC_I2C_TXR = data;
	MC_I2C_CR = MC_I2C_SND | flags;
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
    mdelay(1);
}

uint8_t i2c_read(uint8_t flags)
{
    MC_I2C_CR = MC_I2C_RCV | flags;
	while (MC_I2C_SR & MC_I2C_TIP);
	if (MC_I2C_SR & MC_I2C_AL)
		debug_printf ("Arbitration lost\n");
    mdelay(1);
    return MC_I2C_RXR;
}

void uda1380_write_reg(uint8_t addr, uint16_t val)
{
    i2c_write(SLAVE_ADDR, MC_I2C_STA);
    i2c_write(addr, 0);
    i2c_write(val >> 8, 0);
    i2c_write(val & 0xFF, MC_I2C_NACK | MC_I2C_STO);
}

uint16_t uda1380_read_reg(uint8_t addr)
{
    uint16_t value = 0;
    i2c_write(SLAVE_ADDR, MC_I2C_STA);
    i2c_write(addr, 0);
    i2c_write(SLAVE_ADDR | I2C_READ_OP, MC_I2C_STA);
    value = i2c_read(0);
    value <<= 8;
    value |= i2c_read(MC_I2C_NACK | MC_I2C_STO);
    return value;
}

void uda1380_init()
{
    uint8_t pll;
    uint16_t value;

    i2c_init();

    uda1380_write_reg(UDA1380_RESET, 0); // UDA1380 software reset

    if (wave_fmt.nSamplesPerSec < 12500) pll = 0;
    else if (wave_fmt.nSamplesPerSec < 25000) pll = 1;
    else if (wave_fmt.nSamplesPerSec < 50000) pll = 2;
    else pll = 3;

    uda1380_write_reg(UDA1380_CLK, R00_PLL(pll) | R00_DAC_CLK | 
        R00_ADC_CLK |R00_EN_INT | R00_EN_DAC | R00_EN_DEC | R00_EN_DAC);

    uda1380_write_reg(UDA1380_PM, R02_PON_PLL | R02_PON_HP |
        R02_PON_DAC | R02_PON_BIAS);

    value = uda1380_read_reg(UDA1380_DEEMP);
    uda1380_write_reg(UDA1380_DEEMP, value & ~R13_MTM);
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
        MC_MFBSP_TCS_CONT;

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

    uda1380_init();
    init_i2s(MFBSP_CHANNEL);

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


extern void _etext();
extern unsigned __data_start, _edata;

void uos_init(void)
{
	debug_printf ("\n\nTesting UDA1380...\n");
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	unsigned char *p = (unsigned char *)&_etext;  /* Flash image pointer */
	p += (&_edata - &__data_start) << 2;

    for (;;)
        play_wave(p);
}
