#include <runtime/lib.h>
#include <kernel/uos.h>
#include <flash/m25pxx.h>

#define M25PXX_CMD_WREN         0x06
#define M25PXX_CMD_WRDI         0x04
#define M25PXX_CMD_RDID         0x9F
#define M25PXX_CMD_RDSR         0x05
#define M25PXX_CMD_WRSR         0x01
#define M25PXX_CMD_READ         0x03
#define M25PXX_CMD_FAST_READ    0x0B
#define M25PXX_CMD_PP           0x02
#define M25PXX_CMD_SE           0xD8
#define M25PXX_CMD_BE           0xC7
#define M25PXX_CMD_DP           0xB9
#define M25PXX_CMD_RES          0xAB

#define M25PXX_STATUS_WIP       (1 << 0)
#define M25PXX_STATUS_WEL       (1 << 1)
#define M25PXX_STATUS_BP0       (1 << 2)
#define M25PXX_STATUS_BP1       (1 << 3)
#define M25PXX_STATUS_BP2       (1 << 4)
#define M25PXX_STATUS_SRWD      (1 << 7)

static int m25pxx_connect(flashif_t *flash)
{
    m25pxx_t *m = (m25pxx_t *) flash;
    mutex_lock(&flash->lock);

    m->databuf[0] = M25PXX_CMD_RDID;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    spim_trx(m->spi, &m->msg);

    m->msg.tx_data = 0;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 3;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    spim_trx(m->spi, &m->msg);

    if (m->databuf[0] != 0x20 || m->databuf[1] != 0x20) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }

    unsigned size = 1 << m->databuf[2];
    flash->nb_sectors = size >> 16;
    flash->nb_pages_in_sector = 256;
    flash->page_size = size / flash->nb_sectors / 
        flash->nb_pages_in_sector;
    flash->min_address = 0;

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int enable_write(m25pxx_t *m)
{
    m->databuf[0] = M25PXX_CMD_WREN;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) == SPI_ERR_OK)
        return FLASH_ERR_OK;
    else return FLASH_ERR_IO;
}

static int read_status(m25pxx_t *m, uint8_t *status)
{
    m->databuf[0] = M25PXX_CMD_RDSR;
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

    *status = m->databuf[0];

    return FLASH_ERR_OK;
}

static int m25pxx_erase_all(flashif_t *flash)
{
    int res;
    uint8_t status;
    m25pxx_t *m = (m25pxx_t *) flash;
    mutex_lock(&flash->lock);

    res = enable_write(m);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }

    m->databuf[0] = M25PXX_CMD_BE;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    while (1) {
        res = read_status(m, &status);
        if (res != FLASH_ERR_OK) {
            mutex_unlock(&flash->lock);
            return res;
        }

        if (! (status & M25PXX_STATUS_WIP)) break;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int erase_sector(flashif_t *flash, unsigned sector_num)
{
    int res;
    uint8_t status;
    m25pxx_t *m = (m25pxx_t *) flash;

    res = enable_write(m);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }

    uint32_t address = sector_num * flash_sector_size(flash);
    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = M25PXX_CMD_SE;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    while (1) {
        res = read_status(m, &status);
        if (res != FLASH_ERR_OK) {
            mutex_unlock(&flash->lock);
            return res;
        }

        if (! (status & M25PXX_STATUS_WIP)) break;
    }

    return FLASH_ERR_OK;
}

static int m25pxx_erase_sectors(flashif_t *flash, unsigned sector_num,
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
    m25pxx_t *m = (m25pxx_t *) flash;

    res = enable_write(m);
    if (res != FLASH_ERR_OK) return res;

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = M25PXX_CMD_PP;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
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
        res = read_status(m, &status);
        if (res != FLASH_ERR_OK) return res;
        if (! (status & M25PXX_STATUS_WIP)) break;
    }
    
    return FLASH_ERR_OK;
}

static int read_one_page(flashif_t *flash, unsigned address, 
                            void *data, unsigned size)
{
    m25pxx_t *m = (m25pxx_t *) flash;

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = M25PXX_CMD_READ;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
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

static int m25pxx_write(flashif_t *flash, unsigned page_num, 
                        void *data, unsigned size)
{
    return cyclic_func(flash, page_num * flash_page_size(flash),
        data, size, write_one_page);
}

static int m25pxx_read(flashif_t *flash, unsigned page_num, 
                        void *data, unsigned size)
{
    return cyclic_func(flash, page_num * flash_page_size(flash),
        data, size, read_one_page);
}

void m25pxx_init(m25pxx_t *m, spimif_t *s, unsigned freq, unsigned mode)
{
    m->spi = s;
    flashif_t *f = &m->flashif;

    f->connect = m25pxx_connect;
    f->erase_all = m25pxx_erase_all;
    f->erase_sectors = m25pxx_erase_sectors;
    f->write = m25pxx_write;
    f->read = m25pxx_read;

    m->msg.freq = freq;
    m->msg.mode = (mode & 0xFF0F) | SPI_MODE_NB_BITS(8);
}
