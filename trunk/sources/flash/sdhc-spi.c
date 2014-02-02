#include <runtime/lib.h>
#include <kernel/uos.h>
#include <flash/sdhc-spi.h>
#include <flash/sd-private.h>

static int send_command(sdhc_spi_t *m, uint8_t **r1)
{
	int i;
/*
debug_printf("command:  ");
for (i = 0; i < m->msg.word_count; ++i)
	debug_printf("%02X ", m->databuf[i]);
debug_printf("\n");
*/    
	if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK)
        return FLASH_ERR_IO;
/*
debug_printf("response: ");
for (i = 0; i < m->msg.word_count; ++i)
	debug_printf("%02X ", m->databuf[i]);
debug_printf("\n\n");    
*/
    for (i = 6; i < m->msg.word_count; ++i)
		if (m->databuf[i] != 0xFF) {
			*r1 = &m->databuf[i];
			break;
		}
	return FLASH_ERR_OK;
}

static void reverse_copy(uint8_t *dst, uint8_t *src, unsigned size)
{
    unsigned i, j;
    for (i = 0, j = size - 1; i < size; ++i, --j)
        dst[i] = src[j];
}

static int sd_connect(flashif_t *flash)
{
	int res;
	uint8_t *r1;
	
    sdhc_spi_t *m = (sdhc_spi_t *) flash;
    mutex_lock(&flash->lock);
    
    memset(m->databuf, 0xFF, sizeof(m->databuf));

    // Initial clock to activate SD controller
    m->msg.freq = 400000;
    m->msg.mode |= SPI_MODE_CS_HIGH | SPI_MODE_CS_HOLD;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = 10;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    // Switch SD to SPI mode
    m->databuf[0] = CMD_GO_IDLE_STATE;
    m->databuf[1] = 0x00;
    m->databuf[2] = 0x00;
    m->databuf[3] = 0x00;
    m->databuf[4] = 0x00;
    m->databuf[5] = 0x95;
    m->msg.mode &= ~(SPI_MODE_CS_HIGH | SPI_MODE_CS_HOLD);
    m->msg.word_count = 16; // cmd + max Ncr + r1 + 1byte spare
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 != IN_IDLE_STATE) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }
    
    // Checking SD version
    m->msg.word_count = 20;
    m->databuf[0] = CMD_SEND_IF_COND;
    m->databuf[1] = 0x00;
    m->databuf[2] = 0x00;
    m->databuf[3] = 0x01;
    m->databuf[4] = 0xAA;
    m->databuf[5] = 0x87;
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*(r1 + 4) != 0xAA)
    {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_CONN;
    }

    if (*r1 == ILLEGAL_COMMAND) {
        m->version = SD_VER_1_XX;
    } else if (*r1 == IN_IDLE_STATE) {
        m->version = SD_VER_2_00;
    } else {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_NOT_SUPP;
    }
    
    memset(m->databuf, 0xFF, 20);

    // Initialize SD card
    m->msg.word_count = 16;
    while (1) {
        m->databuf[0] = CMD_APP_CMD;
        m->databuf[1] = 0x00;
        m->databuf[2] = 0x00;
        m->databuf[3] = 0x00;
        m->databuf[4] = 0x00;
        m->databuf[5] = 0xFF;
		res = send_command(m, &r1);
		if (res != FLASH_ERR_OK) {
			mutex_unlock(&flash->lock);
			return res;
		}
		if (*r1 & ERROR_MASK) {
			mutex_unlock(&flash->lock);
			return FLASH_ERR_NOT_SUPP;
		}

        m->databuf[0] = ACMD_SD_SEND_OP_COND;
        if (m->version == SD_VER_2_00)
            m->databuf[1] = 0x40;
        else
            m->databuf[1] = 0x00;
        m->databuf[2] = 0x00;
        m->databuf[3] = 0x00;
        m->databuf[4] = 0x00;
        m->databuf[5] = 0xFF;

		res = send_command(m, &r1);
		if (res != FLASH_ERR_OK) {
			mutex_unlock(&flash->lock);
			return res;
		}
		if (*r1 == SD_READY)
			break;
    }
    
    // Checking if the card is SDHC (or SDXC)
    if (m->version == SD_VER_2_00) {
        m->msg.word_count = 20;
        m->databuf[0] = CMD_READ_OCR;
        m->databuf[1] = 0x00;
        m->databuf[2] = 0x00;
        m->databuf[3] = 0x00;
        m->databuf[4] = 0x00;
        m->databuf[5] = 0xFF;
		res = send_command(m, &r1);
		if (res != FLASH_ERR_OK) {
			mutex_unlock(&flash->lock);
			return res;
		}
		if (*r1 != SD_READY) {
			mutex_unlock(&flash->lock);
			return FLASH_ERR_BAD_ANSWER;
		}
        if (*(r1 + 1) & 0x40)
            m->version |= SDHC;
    }
    
    m->msg.freq = 25000000;
    
    // Read CSD for card parameters
    m->msg.word_count = 42;
	m->databuf[0] = CMD_SEND_CSD;
	m->databuf[1] = 0x00;
	m->databuf[2] = 0x00;
	m->databuf[3] = 0x00;
	m->databuf[4] = 0x00;
	m->databuf[5] = 0xFF;
	res = send_command(m, &r1);
	if (res != FLASH_ERR_OK) {
		mutex_unlock(&flash->lock);
		return res;
	}
	if (*r1 != SD_READY) {
		mutex_unlock(&flash->lock);
		return FLASH_ERR_BAD_ANSWER;
	}
    
    uint8_t *pcsd = r1;
    for (; pcsd < m->databuf + m->msg.word_count; ++pcsd) {
        if (*pcsd == 
            (((m->version & SD_VER_MASK) == SD_VER_1_XX) ? 0x00 : 0x40))
                break;
    }
    if (pcsd == m->databuf + m->msg.word_count) {
		mutex_unlock(&flash->lock);
		return FLASH_ERR_BAD_ANSWER;
	}
    
    csd_v2_t csd_v2;
    reverse_copy((uint8_t *) &csd_v2, pcsd, sizeof(csd_v2_t));
    flash->page_size = 512;
    flash->nb_pages_in_sector = 4 * 1024 * 1024 / 512;
    flash->nb_sectors = 8;
    //flash->nb_sectors = (csd_v2.c_size + 1) / 8;

    // Switching to High Speed mode
    memset(m->databuf, 0xFF, sizeof(m->databuf));
    m->msg.word_count = 42;
	m->databuf[0] = CMD_SWITCH_FUNC;
	m->databuf[1] = 0x80;
	m->databuf[2] = 0xFF;
	m->databuf[3] = 0xFF;
	m->databuf[4] = 0xF1;
	m->databuf[5] = 0xFF;
	res = send_command(m, &r1);
	if (res != FLASH_ERR_OK) {
		mutex_unlock(&flash->lock);
		return res;
	}
	if (*r1 != SD_READY) {
		mutex_unlock(&flash->lock);
		return FLASH_ERR_BAD_ANSWER;
	}
    
    memset(m->databuf, 0xFF, sizeof(m->databuf));
	if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
int i;
for (i = 0; i < m->msg.word_count; ++i)
	debug_printf("%02X ", m->databuf[i]);
debug_printf("\n\n");  

    memset(m->databuf, 0xFF, sizeof(m->databuf));
	if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
for (i = 0; i < m->msg.word_count; ++i)
	debug_printf("%02X ", m->databuf[i]);
debug_printf("\n\n");  

    memset(m->databuf, 0xFF, sizeof(m->databuf));
	if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
for (i = 0; i < m->msg.word_count; ++i)
	debug_printf("%02X ", m->databuf[i]);
debug_printf("\n\n");  

    m->msg.freq = 50000000;

    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int
sd_erase(flashif_t *flash, unsigned start_block, unsigned end_block)
{
    int res;
    sdhc_spi_t *m = (sdhc_spi_t *) flash;
    uint8_t *r1;
    
    mutex_lock(&flash->lock);
    
    memset(m->databuf, 0xFF, sizeof(m->databuf));
    m->databuf[0] = CMD_ERASE_WR_BLK_START_ADDR;
    reverse_copy(&m->databuf[1], (uint8_t*)&start_block, 4);
    //m->databuf[5] = 0xFF;
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 & ERROR_MASK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    memset(m->databuf, 0xFF, sizeof(m->databuf));
    m->databuf[0] = CMD_ERASE_WR_BLK_END_ADDR;
    reverse_copy(&m->databuf[1], (uint8_t*)&end_block, 4);
    //m->databuf[5] = 0xFF;
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 & ERROR_MASK) {
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    memset(m->databuf, 0xFF, sizeof(m->databuf));
    m->databuf[0] = CMD_ERASE;
    //m->databuf[5] = 0xFF;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 & ERROR_MASK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    /*
    do {
        memset(m->databuf, 0xFF, sizeof(m->databuf));
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
    } while (m->databuf[0] == 0);
    */
    m->msg.word_count = 1;
    do {
        m->databuf[0] = 0xFF;
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
    } while (m->databuf[0] == 0);
    
    m->msg.word_count = sizeof(m->databuf);
    
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int 
sd_erase_all(flashif_t *flash)
{
    return sd_erase(flash, 0, 
        flash_nb_sectors(flash) * flash_nb_pages_in_sector(flash) - 1);
}

static int 
sd_erase_sector(flashif_t *flash, unsigned address)
{
    return sd_erase(flash, address / flash_page_size(flash),
        address / flash_page_size(flash) + flash_nb_pages_in_sector(flash) - 1);
}

static int 
sd_program_page(flashif_t *flash, unsigned address, 
    void *data, unsigned size)
{
    int res;
    //int i;
    sdhc_spi_t *m = (sdhc_spi_t *) flash;
    uint8_t *r1;
    
    address /= flash_page_size(flash);
    
    mutex_lock(&flash->lock);
    
    m->msg.word_count = 16; // cmd + max Ncr + r1 + 0xFE
    memset(m->databuf, 0xFF, 15);
    m->databuf[15] = 0xFE;
    
    m->databuf[0] = CMD_WRITE_BLOCK;
    reverse_copy(&m->databuf[1], (uint8_t*)&address, 4);
    //m->databuf[5] = 0xFF;
    m->msg.mode |= SPI_MODE_CS_HOLD;

    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 & ERROR_MASK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    /*
    m->databuf[0] = 0xFE;
    m->msg.word_count = 1;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    */
    
    m->msg.tx_data = data;
    m->msg.rx_data = 0;
    m->msg.word_count = size;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    /*
    memset(m->databuf, 0xFF, sizeof(m->databuf));
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = m->databuf;
    m->msg.word_count = sizeof(m->databuf);
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    for (i = 0; i < m->msg.word_count; ++i)
		if (m->databuf[i] != 0xFF) {
			r1 = &m->databuf[i];
			break;
		}
    
    if ((*r1 & 0x1F) != 0x05) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    */
    m->msg.word_count = 1;
    m->msg.tx_data = m->databuf;
    m->msg.rx_data = m->databuf;
    do {
        m->databuf[0] = 0xFF;
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
    } while (m->databuf[0] == 0xFF);

    if ((m->databuf[0] & 0x1F) != 0x05) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    //m->msg.word_count = 1;
    while (1) {
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
        if (m->databuf[0] != 0)
            break;
        m->databuf[0] = 0xFF;
    }

    m->msg.word_count = sizeof(m->databuf);
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static int 
sd_read(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    int res;
    //int i;
    sdhc_spi_t *m = (sdhc_spi_t *) flash;
    uint8_t *r1;
    
    address /= flash_page_size(flash);
    
    mutex_lock(&flash->lock);
    
    memset(m->databuf, 0xFF, sizeof(m->databuf));
    
    m->databuf[0] = CMD_READ_SINGLE_BLOCK;
    reverse_copy(&m->databuf[1], (uint8_t*)&address, 4);
    //m->databuf[5] = 0xFF;
    m->msg.word_count = 16;
    m->msg.mode |= SPI_MODE_CS_HOLD;
    
    res = send_command(m, &r1);
    if (res != FLASH_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return res;
    }
    if (*r1 & ERROR_MASK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }

    /*
    r1++;
    while (1) {
        while (r1 - m->databuf < sizeof(m->databuf)) {
            if (*r1 == 0xFE)
                break;
            r1++;
        }
        if (*r1 == 0xFE) {
            ++r1;
            break;
        }
        memset(m->databuf, 0xFF, sizeof(m->databuf));
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
        r1 = m->databuf;
    }
       
//debug_printf("found data at offset %d\n", r1 - m->databuf);
    int offset = sizeof(m->databuf) - (r1 - m->databuf);
    memcpy(data, r1, offset);
    m->msg.rx_data = data + offset;
    m->msg.word_count = 512 - offset;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    */
    
    m->msg.word_count = 1;
    int cnt = 0;
    do {
        m->databuf[0] = 0xFF;
        if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
            m->msg.mode &= ~SPI_MODE_CS_HOLD;
            mutex_unlock(&flash->lock);
            return FLASH_ERR_IO;
        }
        cnt++;
    } while (m->databuf[0] != 0xFE);

    int offset = sizeof(m->databuf) - (r1 - m->databuf);
    memcpy(data, r1, offset);
    m->msg.rx_data = data;
    m->msg.tx_data = 0;
    m->msg.word_count = 512;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    m->msg.rx_data = m->databuf;
    m->msg.tx_data = m->databuf;
    memset(m->databuf, 0xFF, 4);
    m->msg.word_count = 4;
    if (spim_trx(m->spi, &m->msg) != SPI_ERR_OK) {
        m->msg.mode &= ~SPI_MODE_CS_HOLD;
        mutex_unlock(&flash->lock);
        return FLASH_ERR_IO;
    }
    
    m->msg.word_count = sizeof(m->databuf);
    m->msg.mode &= ~SPI_MODE_CS_HOLD;
    mutex_unlock(&flash->lock);
    return FLASH_ERR_OK;
}

static unsigned 
sd_page_address(flashif_t *flash, unsigned page_num)
{
    return page_num * flash_page_size(flash);
}

static unsigned 
sd_sector_address(flashif_t *flash, unsigned sector_num)
{
    return sector_num * flash_sector_size(flash);
}

void sd_spi_init(sdhc_spi_t *m, spimif_t *s, unsigned mode)
{
    m->spi = s;
    flashif_t *f = &m->flashif;

    f->connect = sd_connect;
    f->erase_all = sd_erase_all;
    f->erase_sector = sd_erase_sector;
    f->program_page = sd_program_page;
    f->read = sd_read;
    f->page_address = sd_page_address;
    f->sector_address = sd_sector_address;

    m->msg.mode = (mode & 0xFF07) | SPI_MODE_NB_BITS(8);
}
