/*
 * Драйвер для SPI-Flash микросхем фирмы Spansion. Проверялся на
 * S25FL256SAIF00 (S25FL256SAGMFI000).
 * 
 * Драйвер поддерживает только микросхемы с 64-килобайтными секторами,
 * хотя несложно его доработать, чтобы была поддержка 256-килобайтных
 * секторов.
 * 4-килобайтные сектора программно объединены в 64-килобайтные сектора,
 * поскольку интерфейс flash-interface не поддерживает микросхемы
 * с разными размерами секторов.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <flash/s25fl.h>

#define CMD_WRR                 0x01    // Write Register (Status-1, Configuration-1) 
#define CMD_PP                  0x02    // Page Program (3- or 4-byte address) 
#define CMD_READ                0x03    // Read (3- or 4-byte address) 
#define CMD_WRDI                0x04    // Write Disable 
#define CMD_RDSR1               0x05    // Read Status Register-1 
#define CMD_WREN                0x06    // Write Enable 
#define CMD_RDSR2               0x07    // Read Status Register-2 
#define CMD_FAST_READ           0x0B    // Fast Read (3- or 4-byte address) 
#define CMD_4FAST_READ          0x0C    // Fast Read (4-byte address) 
#define CMD_DDRFR               0x0D    // DDR Fast Read (3- or 4-byte address) 
#define CMD_4DDRFR              0x0E    // DDR Fast Read (4-byte address) 
#define CMD_4PP                 0x12    // Page Program (4-byte address) 
#define CMD_4READ               0x13    // Read (4-byte address) 
#define CMD_ABRD                0x14    // Read (4-byte address) 
#define CMD_ABWR                0x15    // AutoBoot Register Write 
#define CMD_BRRD                0x16    // Bank Register Read 
#define CMD_BRWR                0x17    // Bank Register Write 
#define CMD_P4E                 0x20    // Parameter 4 kB-sector Erase (3- or 4-byte address) 
#define CMD_4P4E                0x21    // Parameter 4 kB-sector Erase (4-byte address) 
#define CMD_ASPRD               0x2B    // ASP Read 
#define CMD_ASPP                0x2F    // ASP Program 
#define CMD_CLSR                0x30    // Clear Status Register - Erase/Program Fail Reset 
#define CMD_QPP                 0x32    // Quad Page Program (3- or 4-byte address) 
#define CMD_4QPP                0x34    // Quad Page Program (4-byte address) 
#define CMD_RDCR                0x35    // Read Configuration Register-1 
#define CMD_QPP2                0x38    // Quad Page Program (3- or 4-byte address) 
#define CMD_DOR                 0x3B    // Read Dual Out (3- or 4-byte address) 
#define CMD_4DOR                0x3C    // Read Dual Out (4-byte address) 
#define CMD_DLPRD               0x41    // Data Learning Pattern Read 
#define CMD_OTPP                0x42    // OTP Program
#define CMD_PNVDLR              0x43    // Program NV Data Learning Register 
#define CMD_WVDLR               0x4A    // Write Volatile Data Learning Register 
#define CMD_OTPR                0x4B    // OTP Read 
#define CMD_BE                  0x60    // Bulk Erase
#define CMD_QOR                 0x6B    // Read Quad Out (3- or 4-byte address) 
#define CMD_4QOR                0x6C    // Read Quad Out (4-byte address) 
#define CMD_ERSP                0x75    // Erase Suspend 
#define CMD_ERRS                0x7A    // Erase Resume 
#define CMD_PGSR                0x85    // Program Suspend 
#define CMD_PGRS                0x8A    // Program Resume 
#define CMD_READ_ID             0x90    // Read Electronic Manufacturer Signature 
#define CMD_RDID                0x9F    // Read ID (JEDEC Manufacturer ID and JEDEC CFI) 
#define CMD_MPM                 0xA3    // Reserved for Multi-I/O-High Perf Mode (MPM) 
#define CMD_PLBWR               0xA6    // PPB Lock Bit Write 
#define CMD_PLBRD               0xA7    // PPB Lock Bit Read 
#define CMD_RES                 0xAB    // Read Electronic Signature 
#define CMD_BRAC                0xB9    // Bank Register Access  (legacy)
#define CMD_DIOR                0xBB    // Dual I/O Read (3- or 4-byte address) 
#define CMD_4DIOR               0xBC    // Dual I/O Read (4-byte address) 
#define CMD_DDRDIOR             0xBD    // DDR Dual I/O Read (3- or 4-byte address) 
#define CMD_4DDRDIOR            0xBE    // DDR Dual I/O Read (4-byte address) 
#define CMD_BE2                 0xC7    // Bulk Erase (alternate command) 
#define CMD_SE                  0xD8    // Erase 64 kB or 256 kB (3- or 4-byte address) 
#define CMD_4SE                 0xDC    // Erase 64 kB or 256 kB (4-byte address) 
#define CMD_DYBRD               0xE0    // DYB Read
#define CMD_DYBWR               0xE1    // DYB Write
#define CMD_PPBRD               0xE2    // PPB Read
#define CMD_PPBP                0xE3    // PPB Program
#define CMD_PPBE                0xE4    // PPB Erase
#define CMD_PASSRD              0xE7    // Password Read 
#define CMD_PASSP               0xE8    // Password Program 
#define CMD_PASSU               0xE9    // Password Unlock 
#define CMD_QIOR                0xEB    // Quad I/O Read (3- or 4-byte address) 
#define CMD_4QIOR               0xEC    // Quad I/O Read (4-byte address) 
#define CMD_DDRQIOR             0xED    // DDR Quad I/O Read (3- or 4-byte address) 
#define CMD_4DDRQIOR            0xEE    // DDR Quad I/O Read (4-byte address) 
#define CMD_RESET               0xF0    // Software Reset
#define CMD_MBR                 0xFF    // Mode Bit Reset

// Status Register 1 (SR1)
#define SR1_WIP                 (1 << 0) // Write in Progress 
#define SR1_WEL                 (1 << 1) // Write Enable Latch 
#define SR1_BP0                 (1 << 2) // Block Protection 
#define SR1_BP1                 (1 << 3) // Block Protection 
#define SR1_BP2                 (1 << 4) // Block Protection 
#define SR1_E_ERR               (1 << 5) // Erase Error Occurred 
#define SR1_P_ERR               (1 << 6) // Programming Error Occurred 
#define SR1_SRWD                (1 << 7) // Status Register Write Disable 

// Configuration Register 1 (CR1)
#define CR1_FREEZE              (1 << 0) // Lock current state of BP2-0 bits in Status 
                                         // Register, TBPROT and TBPARM in 
                                         // Configuration Register, and OTP
                                         // regions 
#define CR1_QUAD                (1 << 1) // Puts the device into 
                                         // Quad I/O operation
#define CR1_TBPARM              (1 << 2) // Configures Parameter Sectors location 
#define CR1_BPNV                (1 << 3) // Configures BP2-0 in Status Register 
#define CR1_TBPROT              (1 << 5) // Configures Start of Block Protection 
#define CR1_LC0                 (1 << 6) // Latency Code 
#define CR1_LC1                 (1 << 7) // Latency Code 

// Status Register 2 (SR2)
#define SR2_PS                  (1 << 0) // Program Suspend 
#define SR2_ES                  (1 << 1) // Erase Suspend 

// AutoBoot Register
#define AB_ABE                  (1 << 0)   // AutoBoot Enable
#define AB_ABSD(n)              ((n) << 1) // AutoBoot Start Delay 
#define AB_ABSA(n)              ((n) << 9) // AutoBoot Start Address 

// Bank Address Register
#define BAR_BA24                (1 << 0)   // Bank Address (A24)
#define BAR_EXTADD              (1 << 7)   // Extended Address Enable

// ASP Register (ASPR)
#define ASPR_PSTMLB             (1 << 1) // Persistent Protection Mode Lock Bit 
#define ASPR_PWDMLB             (1 << 2) // Password Protection Mode Lock Bit 


static int s25fl_connect(flashif_t *flash)
{
    s25fl_t *m = (s25fl_t *) flash;
    mutex_lock(&flash->lock);
    
    m->databuf[0] = CMD_RDID;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    spim_trx(m->spi, &m->msg);

    m->msg.tx_data = 0;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 5;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    spim_trx(m->spi, &m->msg);
    
    // Из CFI определяем ID производителя
    if (m->databuf[0] != 0x01) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }
    
    // Из CFI определяем размер
    unsigned size;
    if (m->databuf[1] == 0x20 && m->databuf[2] == 0x18)
        size = 16*1024*1024;
    else if (m->databuf[1] == 0x02 && m->databuf[2] == 0x19)
        size = 32*1024*1024;
    else {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }
    
    // Из CFI определяем размер сектора (количество секторов)
    if (m->databuf[4] == 0x00) {
        flash->nb_sectors = size / (256*1024);
        flash->page_size = 512;
        flash->nb_pages_in_sector = 512;
    } else if (m->databuf[4] == 0x01) {
        flash->nb_sectors = size / (64*1024);
        flash->page_size = 256;
        flash->nb_pages_in_sector = 256;
    } else {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }
    flash->min_address = 0;
    
    m->databuf[0] = CMD_BRWR;
    m->databuf[1] = BAR_EXTADD;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 2;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int enable_write(s25fl_t *m)
{
    m->databuf[0] = CMD_WREN;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) == SPI_ERR_OK)
        return FLASH_ERR_OK;
    else return FLASH_ERR_IO;
}

static int read_sr1(s25fl_t *m, uint8_t *sr1)
{
    m->databuf[0] = CMD_RDSR1;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    m->msg.tx_data = 0;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    *sr1 = m->databuf[0];

    return FLASH_ERR_OK;
}

static int s25fl_erase_all(flashif_t *flash)
{
    int res;
    uint8_t status;
    s25fl_t *m = (s25fl_t *) flash;
    mutex_lock(&flash->lock);
    
    res = enable_write(m);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }
    
    m->databuf[0] = CMD_BE;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    while (1) {
        res = read_sr1(m, &status);
        if (res != FLASH_ERR_OK) {
            mutex_unlock(&flash->lock);
            return res;
        }
        if (! (status & SR1_WIP)) break;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int erase_sector(flashif_t *flash, unsigned sector_num)
{
    int res;
    uint8_t status;
    s25fl_t *m = (s25fl_t *) flash;

    res = enable_write(m);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }

    uint32_t address = sector_num * flash_sector_size(flash);
    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = CMD_SE;
    m->databuf[1] = p[3];
    m->databuf[2] = p[2];
    m->databuf[3] = p[1];
    m->databuf[4] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 5;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    while (1) {
        res = read_sr1(m, &status);
        if (res != FLASH_ERR_OK) {
            mutex_unlock(&flash->lock);
            return res;
        }

        if (! (status & SR1_WIP)) break;
    }

    return FLASH_ERR_OK;
}

static int s25fl_erase_sectors(flashif_t *flash, unsigned sector_num,
    unsigned nb_sectors)
{
    int res;
    int i;
    mutex_lock(&flash->lock);
    for (i = 0; i < nb_sectors; ++i) {
        res = erase_sector(flash, sector_num + i);
        if (res != FLASH_ERR_OK) return res;
    }
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int write_one_page(flashif_t *flash, unsigned address, 
                            void *data, unsigned size)
{
    int res;
    uint8_t status;
    s25fl_t *m = (s25fl_t *) flash;

    res = enable_write(m);
    if (res != FLASH_ERR_OK) return res;

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = CMD_PP;
    m->databuf[1] = p[3];
    m->databuf[2] = p[2];
    m->databuf[3] = p[1];
    m->databuf[4] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 5;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    m->msg.tx_data = data;
    m->msg.rx_data = 0;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    while (1) {
        res = read_sr1(m, &status);
        if (res != FLASH_ERR_OK) return res;
        if (! (status & SR1_WIP)) break;
    }
    
    return FLASH_ERR_OK;
}

static int read_one_page(flashif_t *flash, unsigned address, 
                            void *data, unsigned size)
{
    s25fl_t *m = (s25fl_t *) flash;

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = CMD_READ;
    m->databuf[1] = p[3];
    m->databuf[2] = p[2];
    m->databuf[3] = p[1];
    m->databuf[4] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 5;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    m->msg.tx_data = 0;
    m->msg.rx_data = data;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    return FLASH_ERR_OK;
}

typedef int (* io_func)(flashif_t *flash, unsigned address, 
                        void *data, unsigned size);
                
static int cyclic_func(flashif_t *flash, unsigned address, 
                        void *data, unsigned size, io_func func)
{
    int res;
    unsigned cur_size = size;
    uint8_t *cur_data = data;
    
    mutex_lock(&flash->lock);
    
    while (size > 0) {
        cur_size = (size < flash_page_size(flash)) ?
            size : flash_page_size(flash);
        res = func(flash, address, cur_data, cur_size);
        if (res != FLASH_ERR_OK) {
            mutex_unlock(&flash->lock);
            return res;
        }
        size -= cur_size;
        address += cur_size;
        cur_data += cur_size;
    }
    
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int s25fl_write(flashif_t *flash, unsigned page_num, 
                        void *data, unsigned size)
{
    return cyclic_func(flash, page_num * flash_page_size(flash),
        data, size, write_one_page);
}

static int s25fl_read(flashif_t *flash, unsigned page_num, 
                        void *data, unsigned size)
{
    return cyclic_func(flash, page_num * flash_page_size(flash),
        data, size, read_one_page);
}

void s25fl_init(s25fl_t *m, spimif_t *s, unsigned freq, unsigned mode)
{
    m->spi = s;
    flashif_t *f = &m->flashif;

    f->connect = s25fl_connect;
    f->erase_all = s25fl_erase_all;
    f->erase_sectors = s25fl_erase_sectors;
    f->write = s25fl_write;
    f->read = s25fl_read;

    m->msg.freq = freq;
    m->msg.mode = (mode & 0xFF0F) | SPI_MODE_NB_BITS(8);
}
