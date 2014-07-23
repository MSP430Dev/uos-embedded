#include <runtime/lib.h>
#include <kernel/uos.h>
#include <fs/fat.h>
#include <fs/fat-private.h>

// 1. Избавиться от %
// 2. Задействовать cached_sector для fat_cache

#define SET_LAST_ERROR(fat, err) (fat)->fsif.last_error = (err)
#define GET_LAST_ERROR(fat) (fat)->fsif.last_error
#define RETURN(fat, err) {(fat)->fsif.last_error = (err); return;}
#define RETURN2(fat, err, code) {(fat)->fsif.last_error = (err); return code;}


void fat_close(fs_entry_t *entry)
{
    mem_free(entry->cache_data);
    entry->cache_data = 0;
    entry->fs->last_error = FS_ERR_OK;
}

void fat_free_fs_entry(fs_entry_t *entry)
{
    if (entry->cache_data != 0)
        fat_close(entry);
    mem_free(entry->name);
    mem_free(entry);
    entry->fs->last_error = FS_ERR_OK;
}

static fat_fs_entry_t *alloc_fs_entry(fat_fs_t *fat, unsigned name_size)
{
    fat_fs_entry_t *e;
    e = mem_alloc(fat->pool, sizeof(fat_fs_entry_t));
    if (!e) return 0;

    e->fs_entry.name = mem_alloc_dirty(fat->pool, name_size);
    if (!e->fs_entry.name) {
        mem_free(e);
        return 0;
    }
  
    e->fs_entry.fs = (fsif_t *)fat;
    return e;
}

fs_entry_t *fat_get_root(fsif_t *fs)
{
    fat_fs_t *fat = (fat_fs_t *)fs;
    fat_fs_entry_t *entry = alloc_fs_entry(fat, 1);
    if (!entry) RETURN2(fat, FS_ERR_NO_MEM, 0);
    *(entry->fs_entry.name) = 0;
    entry->fs_entry.attr = FS_ATTR_DIRECTORY;
    entry->first_clus = fat->root_clus;
    entry->cur_clus = fat->root_clus;
    RETURN2(fat, FS_ERR_OK, (fs_entry_t *)entry);
}

void fat_open(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t *)entry->fs;
    entry->cache_data = mem_alloc_dirty(fat->pool, fat->bytes_per_sec);
    if (!entry->cache_data) 
        RETURN(fat, FS_ERR_NO_MEM);
    entry->cache_p = entry->cache_data;
    entry->cache_size = fat->bytes_per_sec;
    entry->cache_valid = 0;
    entry->cur_pos = 0;
    RETURN(fat, FS_ERR_OK);
}

static inline void move_to_next_cluster(fat_fs_t *fat, fat_fs_entry_t *f)
{
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
        if (flash_read(fat->flashif,
            fat->first_fat_sec + f->cur_clus * 2 / fat->bytes_per_sec,
            &fat->fat_cache, fat->bytes_per_sec) != FLASH_ERR_OK)
                RETURN(fat, FS_ERR_IO);
        memcpy(&f->cur_clus, &fat->fat_cache[f->cur_clus * 2 % fat->bytes_per_sec], 2);
    } else {
        if (flash_read(fat->flashif,
            fat->first_fat_sec + f->cur_clus * 4 / fat->bytes_per_sec,
            &fat->fat_cache, fat->bytes_per_sec) != FLASH_ERR_OK)
                RETURN(fat, FS_ERR_IO);
        memcpy(&f->cur_clus, &fat->fat_cache[f->cur_clus * 4 % fat->bytes_per_sec], 4);
    }
    f->cur_sec = 0;
    f->cached_sec = fat->first_data_sec + (f->cur_clus - 2) * fat->sec_per_clus;
}

static inline void move_to_next_sector(fat_fs_t *fat, fat_fs_entry_t *e)
{
    e->fs_entry.cache_valid = 0;
    if (++e->cur_sec >= fat->sec_per_clus)
        move_to_next_cluster(fat, e);
    else e->cached_sec++;
    e->fs_entry.cache_p = e->fs_entry.cache_data;
}

void fat_update_cache(fs_entry_t *entry)
{
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
    fat_fs_t *fat = (fat_fs_t *)e->fs_entry.fs;
    
    if (!entry->cache_valid) {
        if (flash_read(fat->flashif, 
            fat->first_data_sec + ((e->cur_clus & 0x0FFFFFFF) - 2) * fat->sec_per_clus + e->cur_sec,
                entry->cache_data, entry->cache_size) != FLASH_ERR_OK) {
                    RETURN(fat, FS_ERR_IO);
        }
        entry->cache_valid = 1;
    }
    RETURN(fat, FS_ERR_OK);
}

void fat_flush_cache(fs_entry_t *entry)
{
}

void fat_seek_start(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t*)entry->fs;
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;

    e->cached_sec = fat->first_data_sec + (e->first_clus - 2) * fat->sec_per_clus;
    e->cur_clus = e->first_clus;
    e->cur_sec = 0;
    entry->cache_p = entry->cache_data;
    entry->cache_valid = 0;
    entry->cur_pos = 0;
}

unsigned fat_advance(fs_entry_t *entry, unsigned offset)
{
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
    fat_fs_t *fat = (fat_fs_t *)entry->fs;
    
    SET_LAST_ERROR(fat, FS_ERR_OK);
    
    if (offset == 0)
        return 0;
    
    unsigned init_offset = offset;
    if (entry->attr & FS_ATTR_DIRECTORY) {
        if (offset & 0x1f)
            RETURN2(fat, FS_ERR_BAD_ARG, 0);
        if (*(entry->cache_p) == 0)
            RETURN2(fat, FS_ERR_EOF, 0);
        do {
            offset -= 32;
            entry->cur_pos += 32;
            entry->cache_p += 32;
            if (entry->cur_pos % fat->bytes_per_sec == 0) {
                move_to_next_sector(fat, e);
                if (GET_LAST_ERROR(fat) != FS_ERR_OK)
                    return init_offset - offset;
            }
            fat_update_cache(entry);
            if (GET_LAST_ERROR(fat) != FS_ERR_OK)
                return init_offset - offset;
            if (entry->cache_data[entry->cur_pos % fat->bytes_per_sec] == 0)
                return init_offset - offset;
        } while (offset);
    } else {
        if (entry->cur_pos >= entry->size)
            return 0;
        
        unsigned bytes_to_end_of_sec = fat->bytes_per_sec - entry->cur_pos % fat->bytes_per_sec;
        unsigned bytes_to_end_of_file = entry->size - entry->cur_pos;
        do {
            if (bytes_to_end_of_file < bytes_to_end_of_sec) {
                if (offset <= bytes_to_end_of_file) {
                    entry->cur_pos += offset;
                    entry->cache_p += offset;
                    return init_offset;
                } else {
                    entry->cur_pos = entry->size;
                    entry->cache_p += bytes_to_end_of_file;
                    offset -= bytes_to_end_of_file;
                    return init_offset - offset;
                }
            } else {
                if (offset < bytes_to_end_of_sec) {
                    entry->cur_pos += offset;
                    entry->cache_p += offset;
                    return init_offset;
                } else {
                    move_to_next_sector(fat, e);
                    if (GET_LAST_ERROR(fat) != FS_ERR_OK)
                        return init_offset - offset;
                    entry->cur_pos += bytes_to_end_of_sec;
                    offset -= bytes_to_end_of_sec;
                    bytes_to_end_of_file -= bytes_to_end_of_sec;
                    bytes_to_end_of_sec = fat->bytes_per_sec;
                }
            }
        } while (offset);
    }
    return init_offset - offset;
}

static void *
move_to_next_dir_entry(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t*)entry->fs;
    
    fat_advance(entry, sizeof(fat_dir_ent_t));
    if (GET_LAST_ERROR(fat) != FS_ERR_OK)
        return 0;
    fat_update_cache(entry);
    if (GET_LAST_ERROR(fat) != FS_ERR_OK)
        return 0;
    
    return entry->cache_p;
}

static inline fs_entry_t *
next_child(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t*)entry->fs;
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
    fat_dir_ent_t *de = (fat_dir_ent_t *)entry->cache_p;
    fat_fs_entry_t *new_e = 0;
    fs_entry_t *new_entry;
    
    fat_update_cache(entry);
    if (GET_LAST_ERROR(fat) != FS_ERR_OK)
        return 0;
    
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
    } else {
        if (de->name[0] == 0)
            RETURN2(fat, FS_ERR_EOF, 0);
        while (de->name[0] == 0xE5) {
            de = move_to_next_dir_entry(entry);
            if (!de)
                return 0;
            if (de->name[0] == 0)
                RETURN2(fat, FS_ERR_EOF, 0);
        }
        if ((de->attr & FAT_ATTR_MASK) == FAT_ATTR_LONG_NAME) {
            fat_long_ent_t *le = (fat_long_ent_t *) de;
            if (!(le->len & FAT_LAST_LONG_ENTRY))
                RETURN2(fat, FS_ERR_BAD_FORMAT, 0);
            unsigned nb_ent = le->len & FAT_LONG_LEN_MASK;
            new_e = alloc_fs_entry(fat, nb_ent * 13 + 1);
            if (!new_e) RETURN2(fat, FS_ERR_NO_MEM, 0);
            new_entry = &new_e->fs_entry;
            new_entry->name[nb_ent * 13] = 0;
            nb_ent--;
            char *name_p;
            int i;
            for (;;) {
                name_p = &new_entry->name[nb_ent * 13];
                for (i = 0; i < 5; ++i)
                    *name_p++ = le->name0[i];
                for (i = 0; i < 6; ++i)
                    *name_p++ = le->name1[i];
                for (i = 0; i < 2; ++i)
                    *name_p++ = le->name2[i];
                le = move_to_next_dir_entry(entry);
                if (!le)
                    return 0;
                if (nb_ent == 0) break;
                if (le->len != nb_ent)
                    RETURN2(fat, FS_ERR_BAD_FORMAT, 0);
                nb_ent--;
            }
            de = (fat_dir_ent_t *) le;
        } else {
            if (de->attr & FS_ATTR_DIRECTORY) {
                new_e = alloc_fs_entry(fat, 12);
                if (!new_e) RETURN2(fat, FS_ERR_NO_MEM, 0);
                new_entry = &new_e->fs_entry;
                int i;
                for (i = 0; i < 11; ++i) {
                    if (de->name[i] == ' ') break;
                    new_entry->name[i] = de->name[i];
                }
                new_entry->name[i] = 0;
            } else {
                new_e = alloc_fs_entry(fat, 13);
                if (!new_e) RETURN2(fat, FS_ERR_NO_MEM, 0);
                new_entry = &new_e->fs_entry;

                int i, j;
                for (i = 0; i < 8; ++i) {
                    if (de->name[i] == ' ') break;
                    new_entry->name[i] = de->name[i];
                }
                if (de->name[8] != ' ') {
                    new_entry->name[i++] = '.';
                    for (j = 0; j < 3; ++j)
                        new_entry->name[i+j] = de->name[8+j];
                    new_entry->name[i+j] = 0;
                } else new_entry->name[i] = 0;
            }
        }
        new_entry->attr = de->attr;
        new_entry->size = de->file_size;
        new_entry->day = de->wrt_date & 0x1F;
        new_entry->month = (de->wrt_date >> 5) & 0x0F;
        new_entry->year = 1980 + ((de->wrt_date >> 9) & 0x7F);
        new_entry->second = ((de->wrt_time & 0x1F) << 1) + 
            de->crt_time_tenth / 100;
        new_entry->minute = (de->wrt_time >> 5) & 0x3F;
        new_entry->hour = (de->wrt_time >> 11) & 0x1F;
        new_e->first_clus = (de->fst_clus_hi << 16) | de->fst_clus_lo;
        if (new_e->first_clus == 0)
            new_e->first_clus = fat->root_clus;
        new_e->parent_clus = e->first_clus;
        new_e->parent_pos = entry->cur_pos;
        fat_seek_start((fs_entry_t *)new_e);
        fat_advance(entry, sizeof(fat_dir_ent_t));
    }
    return (fs_entry_t *)new_e;
}

fs_entry_t *fat_first_child(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t*)entry->fs;
    
    SET_LAST_ERROR(fat, FS_ERR_OK);
    if (!(entry->attr & FS_ATTR_DIRECTORY))
        RETURN2(fat, FS_ERR_BAD_ARG, 0);
    if (entry->cache_data == 0)
        fat_open(entry);
    if (GET_LAST_ERROR(fat) != FS_ERR_OK)
        return 0;
    fat_seek_start(entry);
    return next_child(entry);
}

fs_entry_t *fat_next_child(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t*)entry->fs;
    
    SET_LAST_ERROR(fat, FS_ERR_OK);
    
    if (!(entry->attr & FS_ATTR_DIRECTORY))
        RETURN2(fat, FS_ERR_BAD_ARG, 0);
        
    if (entry->cache_data == 0)
        RETURN2(fat, FS_ERR_BAD_STATE, 0);
    
    return next_child(entry);
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
    if (flash_read(flash, first_sector, fat->fat_cache, 512) != FLASH_ERR_OK)
        RETURN(fat, FS_ERR_IO);
    fat->fsif.fs_type = check_fat_type(fat->fat_cache);
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
        fat16_bs_t *bs = (fat16_bs_t *)fat->fat_cache;
        if (bs->bytes_per_sec > FAT_MAX_BYTES_PER_SECTOR)
            RETURN(fat, FS_ERR_SMALL_BUF);
        fat->first_fat_sec = first_sector + bs->rsvd_sec_cnt;
        fat->sec_per_clus = bs->sec_per_clus;
        fat->bytes_per_sec = bs->bytes_per_sec;
        fat->root_clus = 0;
        //fat->first_data_sec = fat->first_root_sec +
        //    (bs->root_ent_cnt * 32 + bs->bytes_per_sec - 1) / bs->bytes_per_sec;
        fat->root_ent_cnt = bs->root_ent_cnt;
    } else if (fat->fsif.fs_type == FS_TYPE_FAT32) {
        fat32_bs_t *bs = (fat32_bs_t *)fat->fat_cache;
        if (bs->bytes_per_sec > FAT_MAX_BYTES_PER_SECTOR)
            RETURN(fat, FS_ERR_SMALL_BUF);
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
    
    RETURN(fat, FS_ERR_OK);
}
