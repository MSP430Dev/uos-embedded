#ifndef __M25PXX_H__
#define __M25PXX_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

#define AT45_CMD_MAIN_MEMORY_PAGE_READ          0xD2
#define AT45_CMD_CONT_ARRAY_READ_LEGACY         0xE8
#define AT45_CMD_CONT_ARRAY_READ_LOW_FREQ       0x03
#define AT45_CMD_CONT_ARRAY_READ_HIGH_FREQ      0x0B
#define AT45_CMD_BUFFER_1_READ_LOW_FREQ         0xD1
#define AT45_CMD_BUFFER_2_READ_LOW_FREQ         0xD3
#define AT45_CMD_BUFFER_1_READ_HIGH_FREQ        0xD4
#define AT45_CMD_BUFFER_2_READ_HIGH_FREQ        0xD6
#define AT45_CMD_BUFFER_1_WRITE                 0x84
#define AT45_CMD_BUFFER_2_WRITE                 0x87
#define AT45_CMD_BUFFER_1_PROGRAM_WITH_ERASE    0x83
#define AT45_CMD_BUFFER_2_PROGRAM_WITH_ERASE    0x86
#define AT45_CMD_BUFFER_1_PROGRAM_WITHOUT_ERASE 0x88
#define AT45_CMD_BUFFER_2_PROGRAM_WITHOUT_ERASE 0x89
#define AT45_CMD_PAGE_ERASE                     0x81
#define AT45_CMD_BLOCK_ERASE                    0x50
#define AT45_CMD_SECTOR_ERASE                   0x7C
#define AT45_CMD_PAGE_PROGRAM_THROUGH_BUFFER_1  0x82
#define AT45_CMD_PAGE_PROGRAM_THROUGH_BUFFER_2  0x85
#define AT45_CMD_READ_SECTOR_PROTECTION_REG     0x32
#define AT45_CMD_READ_SECTOR_LOCKDOWN_REG       0x35
#define AT45_CMD_READ_SECURITY_REG              0x77
#define AT45_CMD_PAGE_TO_BUFFER_1_TRANSFER      0x53
#define AT45_CMD_PAGE_TO_BUFFER_2_TRANSFER      0x55
#define AT45_CMD_PAGE_TO_BUFFER_1_COMPARE       0x60
#define AT45_CMD_PAGE_TO_BUFFER_2_COMPARE       0x61
#define AT45_CMD_AUTO_PAGE_REWRITE_BUFFER_1     0x58
#define AT45_CMD_AUTO_PAGE_REWRITE_BUFFER_2     0x59
#define AT45_CMD_DEEP_POWER_DOWN                0xB9
#define AT45_CMD_RESUME_FROM_DEEP_POWER_DOWN    0xAB
#define AT45_CMD_STATUS_REG_READ                0xD7
#define AT45_CMD_MANUF_AND_DEVICE_ID_READ       0x9F

#define AT45_STATUS_PAGE_SIZE   (1 << 0)
#define AT45_STATUS_PROTECT     (1 << 1)
#define AT45_STATUS_COMP        (1 << 6)
#define AT45_STATUS_RDY         (1 << 7)

struct _at45dbxx_t
{
    flashif_t       flashif;
    spimif_t       *spi;
    spi_message_t   msg;
    uint8_t         databuf[5];
};
typedef struct _at45dbxx_t at45dbxx_t;

void at45dbxx_init(at45dbxx_t *m, spimif_t *s, unsigned freq, unsigned mode);

#endif
