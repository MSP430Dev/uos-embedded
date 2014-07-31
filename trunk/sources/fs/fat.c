#include <runtime/lib.h>
#include <kernel/uos.h>
#include <fs/fat.h>
#include <fs/fat-private.h>

// 1. Избавиться от %
// 2. Защитить операции мьютексом

void fat_close(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_close(entry);
    mutex_unlock(lock);
}

void fat_free_fs_entry(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_free_fs_entry(entry);
    mutex_unlock(lock);
}

fs_entry_t *fat_get_root(fsif_t *fs)
{
    fs_entry_t *res;
    mutex_lock(&fs->lock);
    res = do_fat_get_root(fs);
    mutex_unlock(&fs->lock);
    return res;
}

void fat_seek_start(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_seek_start(entry);
    mutex_unlock(lock);
}

void fat_open(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_open(entry);
    mutex_unlock(lock);
}

void fat_update_cache(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_update_cache(entry);
    mutex_unlock(lock);
}

void fat_flush_cache(fs_entry_t *entry)
{
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    do_fat_flush_cache(entry);
    mutex_unlock(lock);
}

unsigned fat_advance(fs_entry_t *entry, unsigned offset)
{
    unsigned res;
    
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    res = do_fat_advance(entry, offset);
    mutex_unlock(lock);
    return res;
}

fs_entry_t *fat_first_child(fs_entry_t *entry)
{
    fs_entry_t *res;
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    res = do_fat_first_child(entry);
    mutex_unlock(lock);
    return res;
}

fs_entry_t *fat_next_child(fs_entry_t *entry)
{
    fs_entry_t *res;
    mutex_t *lock = &entry->fs->lock;
    mutex_lock(lock);
    res = do_fat_next_child(entry);
    mutex_unlock(lock);
    return res;
}

fs_entry_t *fat_get_parent(fs_entry_t *entry)
{
    return 0;
}

void fat_create(fs_entry_t *parent, fs_entry_t *new_entry)
{
}

void fat_delete(fs_entry_t *entry)
{
}

void fat_change(fs_entry_t *entry)
{
}

void fat_move(fs_entry_t *entry, fs_entry_t *new_parent)
{
}

int common_check_fat_bs(fat32_bs_t *bs32)
{
    if ((bs32->jump_boot[0] != 0xEB) && (bs32->jump_boot[0] != 0xE9))
        return 0;
    if ((bs32->bytes_per_sec != 512) && (bs32->bytes_per_sec != 1024) &&
        (bs32->bytes_per_sec != 2048) && (bs32->bytes_per_sec != 4096))
            return 0;
    if ((bs32->sec_per_clus != 1) && (bs32->sec_per_clus != 2) &&
        (bs32->sec_per_clus != 4) && (bs32->sec_per_clus != 8) &&
        (bs32->sec_per_clus != 16) && (bs32->sec_per_clus != 32) &&
        (bs32->sec_per_clus != 64) && (bs32->sec_per_clus != 128))
            return 0;
    if (bs32->rsvd_sec_cnt == 0) return 0;
    if (bs32->num_fats == 0) return 0;
    if (bs32->media < 0xF0) return 0;
    return 1;
}

uint8_t check_fat_type(void *bs)
{
    fat16_bs_t *bs16 = (fat16_bs_t *)bs;
    fat32_bs_t *bs32 = (fat32_bs_t *)bs;
    uint8_t *data = (uint8_t *)bs;
    
    if ((data[510] != 0x55) && (data[511] != 0xAA))
        return FS_TYPE_UNKNOWN;
    if (!common_check_fat_bs(bs32))
        return FS_TYPE_UNKNOWN;
    if ((bs32->fat_sz16 == 0) && (bs32->fat_sz32 != 0) && 
        (bs32->boot_sig == 0x29))
            return FS_TYPE_FAT32;
    if ((bs16->fat_sz16 != 0) && (bs16->boot_sig == 0x29))
        return FS_TYPE_FAT16;
    return FS_TYPE_UNKNOWN;
}

void fat_fs_init(fat_fs_t *fat, mem_pool_t *pool, 
    flashif_t * flash, uint32_t first_sector)
{
    fat->fsif.fs_type = FS_TYPE_UNKNOWN;
    fat->pool = pool;
    fat->flashif = flash;
    fat->first_sec = first_sector;
    if (flash_read(flash, first_sector, fat->fat_cache, 512) != FLASH_ERR_OK) {
        fat->fsif.last_error = FS_ERR_IO;
        return;
    }
    fat->cached_sector = first_sector;
    fat->fsif.fs_type = check_fat_type(fat->fat_cache);
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
        fat16_bs_t *bs = (fat16_bs_t *)fat->fat_cache;
        if (bs->bytes_per_sec > FAT_MAX_BYTES_PER_SECTOR) {
            fat->fsif.last_error = FS_ERR_SMALL_BUF;
            return;
        }
        fat->first_fat_sec = first_sector + bs->rsvd_sec_cnt;
        fat->sec_per_clus = bs->sec_per_clus;
        fat->bytes_per_sec = bs->bytes_per_sec;
        fat->root_clus = 0;
        //fat->first_data_sec = fat->first_root_sec +
        //    (bs->root_ent_cnt * 32 + bs->bytes_per_sec - 1) / bs->bytes_per_sec;
        fat->root_ent_cnt = bs->root_ent_cnt;
    } else if (fat->fsif.fs_type == FS_TYPE_FAT32) {
        fat32_bs_t *bs = (fat32_bs_t *)fat->fat_cache;
        if (bs->bytes_per_sec > FAT_MAX_BYTES_PER_SECTOR) {
            fat->fsif.last_error = FS_ERR_SMALL_BUF;
            return;
        }
        fat->first_fat_sec = first_sector + bs->rsvd_sec_cnt;
        fat->sec_per_clus = bs->sec_per_clus;
        fat->bytes_per_sec = bs->bytes_per_sec;
        fat->first_data_sec = fat->first_fat_sec +  
            bs->num_fats * bs->fat_sz32;
        fat->root_clus = bs->root_clus;
        fat->root_ent_cnt = 0;
    }
    
    fat->fsif.get_root = fat_get_root;
    fat->fsif.first_child = fat_first_child;
    fat->fsif.next_child = fat_next_child;
    fat->fsif.get_parent = fat_get_parent;
    fat->fsif.free_fs_entry = fat_free_fs_entry;
    fat->fsif.create = fat_create;
    fat->fsif.delete = fat_delete;
    fat->fsif.change = fat_change;
    fat->fsif.move = fat_move;
    fat->fsif.open = fat_open;
    fat->fsif.close = fat_close;
    fat->fsif.update_cache = fat_update_cache;
    fat->fsif.flush_cache = fat_flush_cache;
    fat->fsif.seek_start = fat_seek_start;
    fat->fsif.advance = fat_advance;
    
    fat->fsif.last_error = FS_ERR_OK;
}
