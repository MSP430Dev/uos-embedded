#ifndef __FLASH_INTERFACE_H__
#define __FLASH_INTERFACE_H__

#define FLASH_ERR_OK            0
#define FLASH_ERR_NOT_CONN      -1
#define FLASH_ERR_NOT_SUPP      -2
#define FLASH_ERR_OP_FAILED     -3
#define FLASH_ERR_INVAL_SIZE    -4

typedef struct _flashif_t flashif_t;

struct _flashif_t
{
    mutex_t     lock;

    unsigned    size;
    unsigned    nb_sectors;
    unsigned    nb_pages_in_sector;
    int         direct_read;
    unsigned    min_address;
    
    // required
    int (* connect)(flashif_t *flash);
    int (* erase_all)(flashif_t *flash);
    int (* erase_sector)(flashif_t *flash, unsigned address);
    int (* program_page)(flashif_t *flash, unsigned address, void *data, unsigned size);
    unsigned (* page_address)(flashif_t *flash, unsigned page_num);
    unsigned (* sector_address)(flashif_t *flash, unsigned sector_num);
    // required if direct_read is false, otherwise optional
    int (* read)(flashif_t *flash, unsigned address, void *data, unsigned size);
};

static inline __attribute__((always_inline)) 
unsigned flash_size(flashif_t *flash)
{
    return flash->size;
}

static inline __attribute__((always_inline)) 
unsigned flash_nb_sectors(flashif_t *flash)
{
    return flash->nb_sectors;
}

static inline __attribute__((always_inline)) 
unsigned flash_nb_pages_in_sector(flashif_t *flash)
{
    return flash->nb_pages_in_sector;
}

static inline __attribute__((always_inline)) 
int flash_direct_read(flashif_t *flash)
{
    return flash->direct_read;
}

static inline __attribute__((always_inline)) 
unsigned flash_min_address(flashif_t *flash)
{
    return flash->min_address;
}

static inline __attribute__((always_inline)) 
unsigned flash_sector_size(flashif_t *flash)
{
    return flash->size / flash->nb_sectors;
}

static inline __attribute__((always_inline))
unsigned flash_page_size(flashif_t *flash)
{
    return flash_sector_size(flash) / flash->nb_pages_in_sector;
}

static inline __attribute__((always_inline))
unsigned flash_nb_pages(flashif_t *flash)
{
    return flash->nb_sectors * flash->nb_pages_in_sector;
}

static inline __attribute__((always_inline))
unsigned flash_address_of_sector(flashif_t *flash, int sector_number)
{
    return flash->min_address + sector_number * flash_sector_size(flash);
}

static inline __attribute__((always_inline))
unsigned flash_address_of_page(flashif_t *flash, int page_number)
{
    return flash->min_address + page_number * flash_page_size(flash);
}

static inline __attribute__((always_inline))
unsigned flash_max_address(flashif_t *flash)
{
    return flash->min_address + flash->size - 1;
}

static inline __attribute__((always_inline))
int flash_connect(flashif_t *flash)
{
    return flash->connect(flash);
}

static inline __attribute__((always_inline))
int flash_erase_all(flashif_t *flash)
{
    return flash->erase_all(flash);
}

static inline __attribute__((always_inline))
int flash_erase_sector(flashif_t *flash, unsigned address)
{
    return flash->erase_sector(flash, address);
}

static inline __attribute__((always_inline))
int flash_program_page(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    return flash->program_page(flash, address, data, size);
}

static inline __attribute__((always_inline))
int flash_read(flashif_t *flash, unsigned address, void *data, unsigned size)
{
    if (flash->read)
        return flash->read(flash, address, data, size);
    else return FLASH_ERR_NOT_SUPP;
}

static inline __attribute__((always_inline))
unsigned flash_page_address(flashif_t *flash, unsigned page_num)
{
    return flash->page_address(flash, page_num);
}

static inline __attribute__((always_inline))
unsigned flash_sector_address(flashif_t *flash, unsigned sector_num)
{
    return flash->sector_address(flash, sector_num);
}


#endif
