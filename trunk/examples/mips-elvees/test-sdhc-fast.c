#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/spi.h>
#include <timer/timer.h>
#include <flash/sdhc-spi.h>

#define SPI_NUM     0

const char *flash_name = "SD over SPI";
sdhc_spi_t flash;

ARRAY (task, 1000);
elvees_spim_t spi;
timer_t timer;

uint32_t buf[1024] __attribute__((aligned(8)));

/* software breakpoint */
#define STOP            asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile(".word 0x4d"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop")

unsigned sdhc_found = 0;
unsigned nerr = 0;

void hello (void *arg)
{
    flashif_t *f = (flashif_t *)arg;
    unsigned i, j;
    int cnt = 0;
    unsigned long t0, t1;

    if (flash_connect(f) == FLASH_ERR_OK) {
        debug_printf("Found %s, size: %u Kb, nb pages: %u, page size: %d b\n\
            nb sectors: %u, sector size: %d b\n",
            flash_name, (unsigned)(flash_size(f) >> 10),
            flash_nb_pages(f), flash_page_size(f),
            flash_nb_sectors(f), flash_sector_size(f));
        sdhc_found = 1;
    } else {
        debug_printf("%s not found\n", flash_name);
        nerr++;
        STOP;
    }
    
    f->nb_sectors = 4;
    debug_printf("Checking size: only %u Kb\n", 
        (unsigned)(flash_size(f) >> 10));
    
    debug_printf("Erasing all... ");
    t0 = timer_milliseconds(&timer);
    if (flash_erase_all(f) == FLASH_ERR_OK) {
        t1 = timer_milliseconds(&timer);
        debug_printf("OK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));
    }
    else {
        nerr++;
        STOP;
    }

    debug_printf("Checking erasure... 00%%");
    t0 = timer_milliseconds(&timer);
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            nerr++;
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != 0xFFFFFFFF && buf[j] != 0x00000000) {
                debug_printf("FAIL, page #%d, word#%d, data = %08X\n", 
                    i, j, buf[j]);
                nerr++;
            }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));
            
    debug_printf("Filling memory with counter... 00%%");
    t0 = timer_milliseconds(&timer);
    i = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            buf[j] = cnt++;

        if (flash_write(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("FAIL!\n");
            nerr++;
        }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));

    debug_printf("Checking filling... 00%%");
    t0 = timer_milliseconds(&timer);
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            nerr++;
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != cnt++) {
                debug_printf("FAIL, page #%d, word#%d, data = %08X\n", 
                    i, j, buf[j]);
                nerr++;
            }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));

    STOP;
}

void uos_init (void)
{
	debug_printf("\n\nTesting %s\n", flash_name);
    
    timer_init(&timer, KHZ, 1);

    spim_init(&spi, SPI_NUM, SPI_MOSI_OUT | SPI_SS0_OUT | SPI_SS1_OUT | SPI_SS0_OUT | SPI_TSCK_OUT);
    sd_spi_init(&flash, (spimif_t *)&spi, SPI_MODE_CS_NUM(0));
	
	task_create (hello, &flash, "hello", 1, task, sizeof (task));
}
