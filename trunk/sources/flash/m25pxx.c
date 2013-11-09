#include <runtime/lib.h>
#include <kernel/uos.h>
#include <flash/m25pxx.h>

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

    flash->size = 1 << m->databuf[2];
    flash->nb_sectors = flash->size >> 16;
    flash->nb_pages_in_sector = 256;
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
    else return FLASH_ERR_OP_FAILED;
}

static int read_status(m25pxx_t *m, uint8_t *status)
{
    m->databuf[0] = M25PXX_CMD_RDSR;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 1;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_OP_FAILED;

    m->msg.tx_data = 0;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 1;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_OP_FAILED;

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
        return FLASH_ERR_OP_FAILED;
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

static int m25pxx_erase_sector(flashif_t *flash, unsigned address)
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
        return FLASH_ERR_OP_FAILED;
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

static int m25pxx_program_page(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    if (size > 256)
        return FLASH_ERR_INVAL_SIZE;

    int res;
    uint8_t status;
    m25pxx_t *m = (m25pxx_t *) flash;
    mutex_lock(&flash->lock);

    res = enable_write(m);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = M25PXX_CMD_PP;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_OP_FAILED;
    }

    m->msg.tx_data = data;
    m->msg.rx_data = 0;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_OP_FAILED;
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

static int m25pxx_read(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    m25pxx_t *m = (m25pxx_t *) flash;
    mutex_lock(&flash->lock);

    uint8_t *p = (uint8_t *) &address;
    m->databuf[0] = M25PXX_CMD_READ;
    m->databuf[1] = p[2];
    m->databuf[2] = p[1];
    m->databuf[3] = p[0];
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = 0;
    m->msg.word_count = 4;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_OP_FAILED;
    }

    m->msg.tx_data = 0;
    m->msg.rx_data = data;
    m->msg.word_count = size;
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_OP_FAILED;
    }

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int m25pxx_power_down(flashif_t *flash)
{
    return FLASH_ERR_OK;
}

static int m25pxx_wake_up(flashif_t *flash)
{
    return FLASH_ERR_OK;
}

void m25pxx_init(m25pxx_t *m, spimif_t *s, unsigned freq, unsigned mode)
{
    m->spi = s;
    flashif_t *f = &m->flashif;

    f->connect = m25pxx_connect;
    f->erase_all = m25pxx_erase_all;
    f->erase_sector = m25pxx_erase_sector;
    f->program_page = m25pxx_program_page;
    f->read = m25pxx_read;
    f->power_down = m25pxx_power_down;
    f->wake_up = m25pxx_wake_up;

    m->msg.freq = freq;
    m->msg.mode = (mode & 0xFF07) | SPI_MODE_NB_BITS(8);
}
