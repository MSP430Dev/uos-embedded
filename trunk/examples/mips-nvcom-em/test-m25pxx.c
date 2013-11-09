#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/spi.h>
#include <flash/m25pxx.h>

#define SPI_FREQUENCY   20000000

#define ERASE_SECTOR    23

ARRAY (task, 1000);
elvees_spim_t spi;
m25pxx_t m25;

uint32_t buf[1024] __attribute__((aligned(8)));

void hello (void *arg)
{
    flashif_t *f = &m25.flashif;
    int i, j;
    int cnt = 0;

    if (flash_connect(f) == FLASH_ERR_OK)
        debug_printf("Found M25PXX, size: %d\n", f->size);
    else {
        debug_printf("M25PXX not found\n");
        for (;;);
    }

    debug_printf("Erasing all... ");
    if (flash_erase_all(f) == FLASH_ERR_OK)
        debug_printf("OK!\n");
    else debug_printf("FAIL!\n");

    debug_printf("Checking erasure... ");
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i * flash_page_size(f), buf, 
                flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != 0xFFFFFFFF) {
                debug_printf("FAIL, page #%d, word#%d, data = %08X\n", i, j, buf[j]);
                for (;;);
            }
    }
    debug_printf("OK!\n");

    debug_printf("Filling memory with counter... ");
    i = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            buf[j] = cnt++;

        if (flash_program_page(f, i * flash_page_size(f), buf, 
            flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("FAIL!\n");
            for (;;);
        }
    }
    debug_printf("OK!\n");

    debug_printf("Checking memory... ");
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i * flash_page_size(f), buf, 
                flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != cnt++) {
                debug_printf("FAIL, page #%d, word#%d, data = %08X\n", i, j, buf[j]);
                for (;;);
            }
    }
    debug_printf("OK!\n");

    debug_printf("Erasing sector #%d... ", ERASE_SECTOR);
    if (flash_erase_sector(f, ERASE_SECTOR * flash_sector_size(f)) == FLASH_ERR_OK)
        debug_printf("OK!\n");
    else debug_printf("FAIL!\n");

    debug_printf("Checking erasure... ");
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i * flash_page_size(f), buf, 
                flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        if (i >= ERASE_SECTOR * flash_nb_pages_in_sector(f) &&
            i < (ERASE_SECTOR + 1) * flash_nb_pages_in_sector(f)) {
            for (j = 0; j < flash_page_size(f) / 4; ++j) {
                if (buf[j] != 0xFFFFFFFF) {
                    debug_printf("FAIL, page #%d, word#%d, data = %08X\n", i, j, buf[j]);
                    for (;;);
                }
                cnt++;
            }
        } else {
            for (j = 0; j < flash_page_size(f) / 4; ++j)
                if (buf[j] != cnt++) {
                    debug_printf("FAIL, page #%d, word#%d, data = %08X\n", i, j, buf[j]);
                    for (;;);
                }
            }
    }
    debug_printf("OK!\n");

    for (;;);
}

void uos_init (void)
{
	debug_puts ("\nTesting M25PXX\n");

    spim_init(&spi, 2, SPI_MOSI_OUT | SPI_SS0_OUT | SPI_SS1_OUT | SPI_SS0_OUT | SPI_TSCK_OUT);
    m25pxx_init(&m25, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(1));
	
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
