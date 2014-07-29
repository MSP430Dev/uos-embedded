#ifndef __FLASH_INTERFACE_H__
#define __FLASH_INTERFACE_H__

#define FLASH_ERR_OK            0
#define FLASH_ERR_NOT_CONN      -1
#define FLASH_ERR_NOT_SUPP      -2
#define FLASH_ERR_IO            -3
#define FLASH_ERR_INVAL_SIZE    -4
#define FLASH_ERR_BAD_ANSWER    -5

typedef struct _flashif_t flashif_t;

struct _flashif_t
{
    mutex_t     lock;

    unsigned    nb_sectors;
    unsigned    nb_pages_in_sector;
    unsigned    page_size;
    int         direct_read;
    
    // required
    int (* connect)(flashif_t *flash);
    int (* erase_all)(flashif_t *flash);
    int (* erase_sectors)(flashif_t *flash, unsigned sector_num,
        unsigned nb_sectors);
    int (* write)(flashif_t *flash, unsigned page_num,
        void *data, unsigned size);
    // required if direct_read is false, otherwise optional
    int (* read)(flashif_t *flash, unsigned page_num,
        void *data, unsigned size);
    unsigned (*min_address)(flashif_t *flash);
    int (* flush)(flashif_t *flash);
};

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
unsigned flash_sector_size(flashif_t *flash)
{
    return flash->page_size * flash->nb_pages_in_sector;
}

static inline __attribute__((always_inline))
unsigned flash_page_size(flashif_t *flash)
{
    return flash->page_size;
}

static inline __attribute__((always_inline))
unsigned flash_nb_pages(flashif_t *flash)
{
    return flash->nb_sectors * flash->nb_pages_in_sector;
}

static inline __attribute__((always_inline)) 
uint64_t flash_size(flashif_t *flash)
{
    return (uint64_t)flash->page_size * flash_nb_pages(flash);
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
int flash_erase_sectors(flashif_t *flash, unsigned sector_num,
    unsigned nb_sectors)
{
    return flash->erase_sectors(flash, sector_num, nb_sectors);
}

static inline __attribute__((always_inline))
int flash_write(flashif_t *flash, unsigned page_num, 
                void *data, unsigned size)
{
    return flash->write(flash, page_num, data, size);
}

static inline __attribute__((always_inline))
int flash_flush(flashif_t *flash)
{
    if (flash->flush)
        return flash->flush(flash);
    else return FLASH_ERR_NOT_SUPP;
}

static inline __attribute__((always_inline))
int flash_read(flashif_t *flash, unsigned page_num, 
                void *data, unsigned size)
{
    if (flash->read)
        return flash->read(flash, page_num, data, size);
    else return FLASH_ERR_NOT_SUPP;
}

static inline __attribute__((always_inline)) 
unsigned flash_min_address(flashif_t *flash)
{
    if (flash->min_address)
        return flash->min_address(flash);
    else return FLASH_ERR_NOT_SUPP;
}


#endif
