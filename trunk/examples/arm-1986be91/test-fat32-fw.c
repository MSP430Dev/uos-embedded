#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <timer/timer.h>
#include <flash/sdhc-spi.h>
#include <fs/fat32-fast-write.h>

const char *flash_name = "SD over SPI";
sdhc_spi_t flash;

DMA_Data_t dma_prim[8] __attribute__((aligned(1024)));

#define SPI_FREQUENCY   20000000

ARRAY (task, 1000);
milandr_spim_t spi;
fat32_fw_t ffw;
timer_t timer;

fs_entry_t file_entry;

uint8_t buf[4096] __attribute__((aligned(8)));


void print_fat32fw_info()
{
    debug_printf("Reserved sectors:  %d\n", ffw.rsvd_sec_cnt);
    debug_printf("FAT table size:    %d\n", ffw.fat_sz32);
    debug_printf("Total clusters:    %d\n", ffw.tot_clus);
    debug_printf("Next free cluster: %d\n", ffw.nxt_free);
}

void test(const char *title, fs_entry_t *file, unsigned filesize, unsigned portion)
{
    const unsigned portion_start_const = 1;
    unsigned long t0, t1;
    unsigned i;
    
    debug_printf("============================================================================\n");
    debug_printf("%s\n", title);
    debug_printf("============================================================================\n");

    debug_printf("Creating new file...        ");
    t0 = timer_milliseconds(&timer);
    fat32_fw_create(&ffw, &file_entry);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
        return;
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("OK! took %d ms\n", t1 - t0);

    debug_printf("Opening file for writing... ");
    t0 = timer_milliseconds(&timer);
    fat32_fw_open(&ffw, O_WRITE);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
        return;
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("OK! took %d ms\n", t1 - t0);

    debug_printf("Writing data...             ");
    t0 = timer_milliseconds(&timer);
    unsigned portion_start = portion_start_const;
    unsigned filsz = filesize;
    while (filsz) {
        for (i = 0; i < portion; ++i)
            buf[i] = i + portion_start;
        fat32_fw_write(&ffw, buf, portion);
        if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
            debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
            fat32_fw_close(&ffw);
            return;
        }
        filsz -= portion;
        portion_start++;
    }
    t1 = timer_milliseconds(&timer);
    if (t1 == t0)
		debug_printf("OK! took %d ms, rate: INFINITY\n");
	else
		debug_printf("OK! took %d ms, rate: %d bytes/sec\n", 
				t1 - t0, 1000ULL * filesize / (t1 - t0));

    debug_printf("Closing the file...         ");
    t0 = timer_milliseconds(&timer);        
    fat32_fw_close(&ffw);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
        fat32_fw_close(&ffw);
        return;
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("OK! took %d ms\n", t1 - t0);

    debug_printf("Opening file for reading... ");
    t0 = timer_milliseconds(&timer);
    fat32_fw_open(&ffw, O_READ);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
        fat32_fw_close(&ffw);
        return;
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("OK! took %d ms\n", t1 - t0);

    debug_printf("Сhecking data...            ");
    t0 = timer_milliseconds(&timer);
    portion_start = portion_start_const;
    filsz = filesize;
    while (filsz) {
        unsigned rd_size = fat32_fw_read(&ffw, buf, portion);
        if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
            debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
            fat32_fw_close(&ffw);
            return;
        }
        if (rd_size != portion) {
            debug_printf("FAILED! Expected read size: %d, got %d\n", portion, rd_size);
            fat32_fw_close(&ffw);
            return;
        }
        for (i = 0; i < portion; ++i)
            if (buf[i] != (uint8_t)(i + portion_start))  {
                debug_printf("FAILED! Bad data: %d, expected: %d, pos = %d, i = %d\n", 
                    buf[i], (uint8_t)(i + portion_start), filesize - filsz + i, i);
                fat32_fw_close(&ffw);
                return;
            }
        filsz -= portion;
        portion_start++;
    }
    t1 = timer_milliseconds(&timer);
    if (t1 == t0)
		debug_printf("OK! took %d ms, rate: INFINITY\n");
	else
		debug_printf("OK! took %d ms, rate: %d bytes/sec\n", 
				t1 - t0, 1000ULL * filesize / (t1 - t0));

    debug_printf("Closing the file...         ");
    t0 = timer_milliseconds(&timer);        
    fat32_fw_close(&ffw);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        debug_printf("FAILED! Error code: %d\n", fat32_fw_last_error(&ffw));
        fat32_fw_close(&ffw);
        return;
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("OK! took %d ms\n", t1 - t0);
    
    debug_printf("--- SUCCESS!\n\n");
}

void hello (void *arg)
{
    flashif_t *f = (flashif_t *)arg;
    int choice;

    if (flash_connect(f) == FLASH_ERR_OK)
        debug_printf("Found %s, size: %u Kb, nb pages: %u, page size: %d b\n\
            nb sectors: %u, sector size: %d b\n",
            flash_name, (unsigned)(flash_size(f) >> 10),
            flash_nb_pages(f), flash_page_size(f),
            flash_nb_sectors(f), flash_sector_size(f));
    else {
        debug_printf("%s not found\n", flash_name);
        for (;;);
    }
    
    fat32_fw_init(&ffw, (flashif_t *)&flash);
    if (fat32_fw_last_error(&ffw) != FS_ERR_OK) {
        if (fat32_fw_last_error(&ffw) == FS_ERR_BAD_FORMAT) {
            debug_printf("Flash is not formatted for FAT32 Fast Write!\n");
            debug_printf("Do you want to format it? [Y/n] ");
            choice = debug_getchar();
            debug_printf("%c\n", choice);
            if (choice == 'N' || choice == 'n') {
                debug_printf("As you wish! Program stopped...\n");
                for (;;) ;
            } else {
                fat32_fw_format(&ffw, flash_nb_pages(f), "TST FAT32FW");
                if (fat32_fw_last_error(&ffw) == FS_ERR_OK) {
                    debug_printf("Flash formatted successfully!\n");
                } else  {
                    debug_printf("Error while formatting flash!\n");
                    for (;;);
                }
            }
        } else {
            debug_printf("Failed to initialize FAT32 Fast Write filesystem!\n");
            for (;;);
        }
    }

    debug_printf("Found FAT32 Fast Write flash file system:\n");
    print_fat32fw_info();
    
    debug_printf("\nReformat? [y/N] ");
    choice = debug_getchar();
    debug_printf("%c\n", choice);
    if (choice == 'Y' || choice == 'y') {
        fat32_fw_format(&ffw, flash_nb_pages(f), "TST_FAT32FW");
        if (fat32_fw_last_error(&ffw) == FS_ERR_OK) {
            debug_printf("Flash formatted successfully!\n");
            print_fat32fw_info();
        } else  {
            debug_printf("Error while formatting flash!\n");
            for (;;);
        }
    }

    file_entry.name = "TEST_01.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 1. File of size 1 byte", &file_entry, 1, 1);

    file_entry.name = "TEST_02.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 2. File of size 511 bytes, writing portion 1 byte", &file_entry, 511, 1);

    file_entry.name = "TEST_03.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 3. File of size 512 bytes, writing portion 1 byte", &file_entry, 512, 1);

    file_entry.name = "TEST_04.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 4. File of size 513 bytes, writing portion 1 byte", &file_entry, 513, 1);

    file_entry.name = "TEST_05.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 5. File of size 32767 bytes, writing portion 1 byte", &file_entry, 32767, 1);

    file_entry.name = "TEST_06.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 6. File of size 32768 bytes, writing portion 1 byte", &file_entry, 32768, 1);
    
    file_entry.name = "TEST_07.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 7. File of size 32769 bytes, writing portion 1 byte", &file_entry, 32769, 1);

    file_entry.name = "TEST_08.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 8. File of size 65536 bytes, writing portion 1 byte", &file_entry, 65536, 1);

    file_entry.name = "TEST_09.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 9. File of size 65537 bytes, writing portion 1 byte", &file_entry, 65537, 1);

    file_entry.name = "TEST_10.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 10. File of size 512 bytes, writing portion 512 bytes", &file_entry, 512, 512);

    file_entry.name = "TEST_11.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 11. File of size 1024 bytes, writing portion 512 bytes", &file_entry, 1024, 512);
    
    file_entry.name = "TEST_12.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 12. File of size 32768 bytes, writing portion 512 bytes", &file_entry, 32768, 512);

    file_entry.name = "TEST_13.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 13. File of size 65536 bytes, writing portion 512 bytes", &file_entry, 65536, 512);

    file_entry.name = "TEST_14.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 14. File of size 511 bytes, writing portion 511 bytes", &file_entry, 511, 511);

    file_entry.name = "TEST_15.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 15. File of size 1022 bytes, writing portion 511 bytes", &file_entry, 1022, 511);

    file_entry.name = "TEST_16.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 16. File of size 33215 bytes, writing portion 511 bytes", &file_entry, 33215, 511);

    file_entry.name = "TEST_17.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 17. File of size 65919 bytes, writing portion 511 bytes", &file_entry, 65919, 511);

    file_entry.name = "TEST_18.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 18. File of size 513 bytes, writing portion 513 bytes", &file_entry, 513, 513);

    file_entry.name = "TEST_19.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 19. File of size 1026 bytes, writing portion 513 bytes", &file_entry, 1026, 513);

    file_entry.name = "TEST_20.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 20. File of size 32832 bytes, writing portion 513 bytes", &file_entry, 32832, 513);

    file_entry.name = "TEST_21.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 21. File of size 65664 bytes, writing portion 513 bytes", &file_entry, 65664, 513);

    file_entry.name = "TEST_22.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 22. File of size 1024 bytes, writing portion 1024 bytes", &file_entry, 1024, 1024);

    file_entry.name = "TEST_23.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 23. File of size 32768 bytes, writing portion 1024 bytes", &file_entry, 32768, 1024);

    file_entry.name = "TEST_24.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 24. File of size 65538 bytes, writing portion 1024 bytes", &file_entry, 65536, 1024);

    file_entry.name = "TEST_25.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 25. File of size 1048576 bytes, writing portion 512 bytes", &file_entry, 1048576, 512);

    file_entry.name = "TEST_26.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 26. File of size 1048576 bytes, writing portion 1024 bytes", &file_entry, 1048576, 1024);

    file_entry.name = "TEST_27.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 27. File of size 1048576 bytes, writing portion 2048 bytes", &file_entry, 1048576, 2048);
    
    file_entry.name = "TEST_28.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 28. File of size 1049083 bytes, writing portion 511 bytes", &file_entry, 1049083, 511);

    file_entry.name = "TEST_29.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 29. File of size 1049085 bytes, writing portion 513 bytes", &file_entry, 1049085, 513);
    
    file_entry.name = "TEST_30.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 30. File of size 10 Mbytes, writing portion 512 bytes", &file_entry, 10 * 1024 * 1024, 512);

    file_entry.name = "TEST_31.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 31. File of size 10 Mbytes, writing portion 1024 bytes", &file_entry, 10 * 1024 * 1024, 1024);

    file_entry.name = "TEST_32.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 32. File of size 10 Mbytes, writing portion 256 bytes", &file_entry, 10 * 1024 * 1024, 256);

    file_entry.name = "TEST_33.BIN";
    file_entry.year = 2015;
    file_entry.month = 6;
    file_entry.day = 1;
    file_entry.hour = 18;
    file_entry.minute = 15;
    file_entry.second = 7;
    test("TEST 33. File of size 50 Mbytes, writing portion 512 bytes", &file_entry, 50 * 1024 * 1024, 512);

    debug_printf("\nFiles in the filesystem:\n");
    fs_entry_t *entry = get_first_entry(&ffw);
    while (entry) {
        debug_printf("%12s: %d byte(s)\n", entry->name, entry->size);
        entry = get_next_entry(&ffw);
    }
    
    debug_printf("============= FINISHED! ==============\n\n");

    for (;;);
}

void init_pins()
{
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;
    
    ARM_GPIOD->ANALOG |= (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6);
    ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC & 
        ~(ARM_FUNC_MASK(2) | ARM_FUNC_MASK(3) | ARM_FUNC_MASK(5) | ARM_FUNC_MASK(6))) | 
        ARM_FUNC_ALT(2) | ARM_FUNC_PORT(3) | ARM_FUNC_ALT(5) | ARM_FUNC_ALT(6);
    ARM_GPIOD->PWR |= ARM_PWR_FASTEST(3) | ARM_PWR_FASTEST(5) | ARM_PWR_FASTEST(6);

    ARM_GPIOD->DATA = (1 << 3);
    ARM_GPIOD->OE |= (1 << 3);
}

void spi_cs_control(unsigned port, unsigned cs_num, int level)
{
    if (level)
        ARM_GPIOD->DATA |= (1 << 3);
    else
        ARM_GPIOD->DATA &= ~(1 << 3);
}

void uos_init (void)
{
    debug_printf("\nTesting FAT32 Fast Write on %s\n", flash_name);

    init_pins();
    
    timer_init(&timer, KHZ, 1);
    
    milandr_spim_init(&spi, 1, spi_cs_control, dma_prim);
    
    sd_spi_init(&flash, (spimif_t *)&spi, SPI_MODE_CS_NUM(1));
    
    task_create (hello, &flash, "hello", 1, task, sizeof (task));
}
