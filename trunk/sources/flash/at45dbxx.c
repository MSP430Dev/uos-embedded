#include <runtime/lib.h>
#include <kernel/uos.h>
#include <flash/at45dbxx.h>

static int read_status(at45dbxx_t *m, uint8_t *status)
{
    m->databuf[0] = AT45_CMD_STATUS_REG_READ;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 2;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    *status = m->databuf[1];

    if (*status & 0x3C != 0x1C)
        return FLASH_ERR_IO;

    return FLASH_ERR_OK;
}

static int at45dbxx_connect(flashif_t *flash)
{
    at45dbxx_t *m = (at45dbxx_t *) flash;
    mutex_lock(&flash->lock);

    m->databuf[0] = AT45_CMD_MANUF_AND_DEVICE_ID_READ;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 3;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;

    if ((m->databuf[1] != 0x1F) || ((m->databuf[2] & 0xE0) != 0x20)) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }

    uint8_t status;
    int res = read_status(m, &status);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }

    if (status & AT45_STATUS_PAGE_SIZE) {
        flash->size = 32768 << (m->databuf[2] & 0x1F);
        flash->nb_sectors = flash->size / 2048;
    } else {
        flash->size = 33792 << (m->databuf[2] & 0x1F);
        flash->nb_sectors = flash->size / 2112;
    }
    flash->nb_pages_in_sector = 8;
    flash->min_address = 0;

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int at45dbxx_erase_all(flashif_t *flash)
{
    int res;
    uint8_t status;
    at45dbxx_t *m = (at45dbxx_t *) flash;
    mutex_lock(&flash->lock);

    m->databuf[0] = 0xC7;
    m->databuf[1] = 0x94;
    m->databuf[2] = 0x80;
    m->databuf[3] = 0x9A;
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
        if (status & AT45_STATUS_RDY) break;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int at45dbxx_erase_block(flashif_t *flash, unsigned address)
{
    int res;
    uint8_t status;
    at45dbxx_t *m = (at45dbxx_t *) flash;
    mutex_lock(&flash->lock);

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = AT45_CMD_BLOCK_ERASE;
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

        if (status & AT45_STATUS_RDY) break;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int at45dbxx_program_page(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    if (size > 264)
        return FLASH_ERR_INVAL_SIZE;

    int res;
    uint8_t status;
    at45dbxx_t *m = (at45dbxx_t *) flash;
    mutex_lock(&flash->lock);

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = AT45_CMD_BUFFER_1_WRITE;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    m->msg.tx_data = data;
    m->msg.rx_data = 0;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    m->databuf[0] = AT45_CMD_BUFFER_1_PROGRAM_WITHOUT_ERASE;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
    //m->msg.mode &= ~SPI_MODE_CS_HOLD;
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

        if (status & AT45_STATUS_RDY) break;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static unsigned at45dbxx_page_address(flashif_t *flash, unsigned page_num)
{
    if (flash_page_size(flash) == 264)
        return page_num << 9;
    else return page_num << 8;
}

static unsigned at45dbxx_sector_address(flashif_t *flash, unsigned sector_num)
{
    if (flash_page_size(flash) == 264)
        return sector_num << 12;
    else return sector_num << 11;
}

static int at45dbxx_read(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    at45dbxx_t *m = (at45dbxx_t *) flash;
    mutex_lock(&flash->lock);

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = AT45_CMD_CONT_ARRAY_READ_HIGH_FREQ;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->databuf[4] = 0;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 5;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    m->msg.tx_data = 0;
    m->msg.rx_data = data;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

void at45dbxx_init(at45dbxx_t *m, spimif_t *s, unsigned freq, unsigned mode)
{
    m->spi = s;
    flashif_t *f = &m->flashif;

    f->connect = at45dbxx_connect;
    f->erase_all = at45dbxx_erase_all;
    f->erase_sector = at45dbxx_erase_block;
    f->program_page = at45dbxx_program_page;
    f->read = at45dbxx_read;
    f->page_address = at45dbxx_page_address;
    f->sector_address = at45dbxx_sector_address;

    m->msg.freq = freq;
    m->msg.mode = (mode & 0xFF07) | SPI_MODE_NB_BITS(8);
}
