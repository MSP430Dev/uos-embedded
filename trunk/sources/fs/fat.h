#ifndef __FAT_H__
#define __FAT_H__

#include <fs/fs-interface.h>
#include <flash/flash-interface.h>
#include <mem/mem.h>

#define FAT_MAX_BYTES_PER_SECTOR    512

//
// Структура драйвера файловой системы FAT
//
typedef struct _fat_fs_t
{
    fsif_t      fsif;
    mem_pool_t  *pool;
    flashif_t   *flashif;
    uint8_t     sec_per_clus;
    uint16_t    bytes_per_sec;
    uint32_t    first_sec;
    uint32_t    first_fat_sec;
    uint32_t    first_data_sec;
    uint32_t    root_clus;
    uint16_t    root_ent_cnt;
    uint8_t     fat_cache[512] __attribute__((aligned(8)));
    uint32_t    cached_sector;
} fat_fs_t;

void fat_fs_init(fat_fs_t *fat, mem_pool_t *pool, 
    flashif_t * flash, uint32_t first_sector);

#endif
