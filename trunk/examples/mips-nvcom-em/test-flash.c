#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/spi.h>

#define M25PXX
//#define AT45DBXX

#if defined(M25PXX)
    #include <flash/m25pxx.h>
    const char *flash_name = "M25Pxx";
    m25pxx_t flash;
#elif defined(AT45DBXX)
    #include <flash/at45dbxx.h>
    const char *flash_name = "AT45DBxx";
    at45dbxx_t flash;
#endif

#define SPI_FREQUENCY   20000000

#define ERASE_SECTOR    23

ARRAY (task, 1000);
elvees_spim_t spi;

uint32_t buf[1024] __attribute__((aligned(8)));

void hello (void *arg)
{
    flashif_t *f = (flashif_t *)arg;
    int i, j;
    int cnt = 0;

    if (flash_connect(f) == FLASH_ERR_OK)
        debug_printf("Found %s, size: %d bytes, page size: %d bytes\n", 
            flash_name, flash_size(f), flash_page_size(f));
    else {
        debug_printf("%s not found\n", flash_name);
        for (;;);
    }

    debug_printf("Erasing all... ");
    if (flash_erase_all(f) == FLASH_ERR_OK)
        debug_printf("OK!\n");
    else debug_printf("FAIL!\n");

    debug_printf("Checking erasure... ");
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, flash_page_address(f, i), buf, 
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

        if (flash_program_page(f, flash_page_address(f, i), buf, 
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
        if (flash_read(f, flash_page_address(f, i), buf, 
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
    if (flash_erase_sector(f, flash_sector_address(f, ERASE_SECTOR)) == FLASH_ERR_OK)
        debug_printf("OK!\n");
    else debug_printf("FAIL!\n");

    debug_printf("Checking erasure... ");
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, flash_page_address(f, i), buf, 
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
	debug_printf("\nTesting %s\n", flash_name);

    spim_init(&spi, 2, SPI_MOSI_OUT | SPI_SS0_OUT | SPI_SS1_OUT | SPI_SS0_OUT | SPI_TSCK_OUT);
#if defined(M25PXX)
    m25pxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(1));
#elif defined(AT45DBXX)
    at45dbxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(1));
#endif
	
	task_create (hello, &flash, "hello", 1, task, sizeof (task));
}
