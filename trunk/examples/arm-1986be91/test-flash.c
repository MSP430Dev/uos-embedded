#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>

//#define M25PXX
#define AT45DBXX

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
milandr_spim_t spi;

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
    else {
        debug_printf("FAIL!\n");
        for (;;);
    }

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

void init_pins()
{
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD | ARM_PER_CLOCK_GPIOC;
    
    ARM_GPIOD->ANALOG |= (1 << 2) | (1 << 5) | (1 << 6);
    ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC & 
        ~(ARM_FUNC_MASK(2) | ARM_FUNC_MASK(5) | ARM_FUNC_MASK(6))) | 
        ARM_FUNC_ALT(2) | ARM_FUNC_ALT(5) | ARM_FUNC_ALT(6);
    ARM_GPIOD->PWR |= ARM_PWR_FASTEST(5) | ARM_PWR_FASTEST(6);
    
    ARM_GPIOC->ANALOG |= (1 << 3);
    ARM_GPIOC->FUNC = (ARM_GPIOC->FUNC & ~ARM_FUNC_MASK(3)) | ARM_FUNC_PORT(3);
    ARM_GPIOC->PWR |= ARM_PWR_FASTEST(3);
    ARM_GPIOC->DATA = (1 << 3);
    ARM_GPIOC->OE |= (1 << 3);
}

void spi_cs_control(unsigned port, unsigned cs_num, int level)
{
    if (level)
        ARM_GPIOC->DATA |= (1 << 3);
    else
        ARM_GPIOC->DATA &= ~(1 << 3);
}

void uos_init (void)
{
	debug_printf("\nTesting %s\n", flash_name);
	
	init_pins();
	
    milandr_spim_init(&spi, 1, spi_cs_control);
    
#if defined(M25PXX)
    m25pxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(1));
#elif defined(AT45DBXX)
    at45dbxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(1));
#endif
	
	task_create (hello, &flash, "hello", 1, task, sizeof (task));
}
