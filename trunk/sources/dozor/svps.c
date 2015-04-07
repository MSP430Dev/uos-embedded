#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include "svp.h"

void svps_init (svps_t *svp, spimif_t *spim, unsigned freq_hz)
{
    svp->svp_gen.over_spi = 1;
    svp->svp_gen.irq = -1;
	svp->svp_gen.int_handler = 0;
    svp->spi = spim;
    svp->msg.freq = freq_hz;
    svp->msg.mode = SPI_MODE_CPHA | SPI_MODE_NB_BITS(16);
}

uint16_t svps_read16 (svps_t *svp, uint16_t addr)
{
    uint16_t tx_buf[2];
    uint16_t rx_buf[2];
    
    tx_buf[0] = addr;
    svp->msg.tx_data = tx_buf;
    svp->msg.rx_data = rx_buf;
    svp->msg.word_count = 2;
    spim_trx(svp->spi, &svp->msg);
    return rx_buf[1];
}

uint32_t svps_read32 (svps_t *svp, uint16_t addr)
{
    uint16_t tx_buf[3];
    uint16_t rx_buf[3];

    tx_buf[0] = addr;
    svp->msg.tx_data = tx_buf;
    svp->msg.rx_data = rx_buf;
    svp->msg.word_count = 3;
    spim_trx(svp->spi, &svp->msg);

    unsigned result;
    uint16_t *p = (uint16_t *) &result;
    p[0] = rx_buf[1];
    p[1] = rx_buf[2];
    return result;
}

void svps_read_array (svps_t *svp, uint16_t addr, void *buf, int size)
{
    unsigned cur_mode = svp->msg.mode;

    // Передаём адрес
    svp->msg.rx_data = 0;
    svp->msg.tx_data = &addr;
    svp->msg.word_count = 1;
    svp->msg.mode = cur_mode | SPI_MODE_CS_HOLD;
    spim_trx(svp->spi, &svp->msg);

    // Принимаем данные
    svp->msg.rx_data = buf;
    svp->msg.tx_data = 0;
    svp->msg.word_count = (size + 1) >> 1;
    svp->msg.mode = cur_mode;
    spim_trx(svp->spi, &svp->msg);
}

void svps_write16 (svps_t *svp, uint16_t addr, uint16_t data)
{
    uint16_t tx_buf[2];
    tx_buf[0] = addr | 1;
    tx_buf[1] = data;

    svp->msg.tx_data = tx_buf;
    svp->msg.rx_data = 0;
    svp->msg.word_count = 2;
    spim_trx(svp->spi, &svp->msg);
}

void svps_write32 (svps_t *svp, uint16_t addr, uint32_t data)
{
    uint16_t tx_buf[3];
    uint16_t *p = (uint16_t *)&data;
    tx_buf[0] = addr | 1;
    tx_buf[1] = p[0];
    tx_buf[2] = p[1];

    svp->msg.tx_data = tx_buf;
    svp->msg.rx_data = 0;
    svp->msg.word_count = 3;
    spim_trx(svp->spi, &svp->msg);
}

void svps_write_array (svps_t *svp, uint16_t addr, const void *buf, int size)
{
    unsigned cur_mode = svp->msg.mode;

    // Передаём адрес
    addr |= 1;
    svp->msg.rx_data = 0;
    svp->msg.tx_data = &addr;
    svp->msg.word_count = 1;
    svp->msg.mode = cur_mode | SPI_MODE_CS_HOLD;
    spim_trx(svp->spi, &svp->msg);

    // Передаём данные
    svp->msg.rx_data = 0;
    svp->msg.tx_data = (void *)buf;
    svp->msg.word_count = (size + 1) >> 1;
    svp->msg.mode = cur_mode;
    spim_trx(svp->spi, &svp->msg);
}
