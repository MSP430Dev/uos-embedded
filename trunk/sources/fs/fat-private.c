#include <runtime/lib.h>
#include <kernel/uos.h>
#include <fs/fat.h>
#include <fs/fat-private.h>

void do_fat_close(fs_entry_t *entry)
{
    mem_free(entry->cache_data);
    entry->cache_data = 0;
    entry->fs->last_error = FS_ERR_OK;
}

void do_fat_free_fs_entry(fs_entry_t *entry)
{
    if (entry->cache_data != 0)
        do_fat_close(entry);
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

fs_entry_t *do_fat_get_root(fsif_t *fs)
{
    fat_fs_t *fat = (fat_fs_t *)fs;
    fat_fs_entry_t *entry = alloc_fs_entry(fat, 1);
    
    if (!entry) {
        fs->last_error = FS_ERR_NO_MEM;
        return 0;
    }
    *(entry->fs_entry.name) = 0;
    entry->fs_entry.attr = FS_ATTR_DIRECTORY;
    entry->first_clus = fat->root_clus;
    entry->cur_clus = fat->root_clus;
    fs->last_error = FS_ERR_OK;
    return (fs_entry_t *)entry;
}

void do_fat_seek_start(fs_entry_t *entry)
{
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
   
    e->cur_clus = e->first_clus;
    e->cur_sec = 0;
    entry->cache_p = entry->cache_data;
    entry->cache_valid = 0;
    entry->cur_pos = 0;
}

void do_fat_open(fs_entry_t *entry)
{
    fat_fs_t *fat = (fat_fs_t *)entry->fs;
    
    entry->cache_data = mem_alloc_dirty(fat->pool, fat->bytes_per_sec);
    if (!entry->cache_data) {
        entry->fs->last_error = FS_ERR_NO_MEM;
        return;
    }
    entry->cache_size = fat->bytes_per_sec;
    do_fat_seek_start(entry);
    entry->fs->last_error = FS_ERR_OK;
}

static inline void move_to_next_cluster(fat_fs_t *fat, fat_fs_entry_t *f)
{
    unsigned req_sec;
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
        req_sec = fat->first_fat_sec + f->cur_clus * 2 / fat->bytes_per_sec;
        if (req_sec != fat->cached_sector) {
            if (flash_read(fat->flashif, req_sec,
                &fat->fat_cache, fat->bytes_per_sec) != FLASH_ERR_OK) {
                    fat->fsif.last_error = FS_ERR_IO;
                    return;
            }
            fat->cached_sector = req_sec;
        }
        memcpy(&f->cur_clus, &fat->fat_cache[f->cur_clus * 2 % fat->bytes_per_sec], 2);
    } else {
        req_sec = fat->first_fat_sec + f->cur_clus * 4 / fat->bytes_per_sec;
        if (req_sec != fat->cached_sector) {
            if (flash_read(fat->flashif, req_sec,
                &fat->fat_cache, fat->bytes_per_sec) != FLASH_ERR_OK) {
                    fat->fsif.last_error = FS_ERR_IO;
                    return;
            }
            fat->cached_sector = req_sec;
        }
        memcpy(&f->cur_clus, &fat->fat_cache[f->cur_clus * 4 % fat->bytes_per_sec], 4);
    }
    f->cur_sec = 0;
}

static inline void move_to_next_sector(fat_fs_t *fat, fat_fs_entry_t *e)
{
    e->fs_entry.cache_valid = 0;
    if (++e->cur_sec >= fat->sec_per_clus)
        move_to_next_cluster(fat, e);
    e->fs_entry.cache_p = e->fs_entry.cache_data;
}

void do_fat_update_cache(fs_entry_t *entry)
{
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
    fat_fs_t *fat = (fat_fs_t *)e->fs_entry.fs;
    
    if (!entry->cache_valid) {
        if (flash_read(fat->flashif, 
                fat->first_data_sec + ((e->cur_clus & 0x0FFFFFFF) - 2) * fat->sec_per_clus + e->cur_sec,
                entry->cache_data, entry->cache_size) != FLASH_ERR_OK)
        {
            entry->fs->last_error = FS_ERR_IO;
            return;
        }
        entry->cache_valid = 1;
    }
    entry->fs->last_error = FS_ERR_OK;
}

void do_fat_flush_cache(fs_entry_t *entry)
{
}

unsigned do_fat_advance(fs_entry_t *entry, unsigned offset)
{
    fat_fs_entry_t *e = (fat_fs_entry_t *)entry;
    fat_fs_t *fat = (fat_fs_t *)entry->fs;
    
    entry->fs->last_error = FS_ERR_OK;
    if (offset == 0) return 0;
    
    unsigned init_offset = offset;
    if (entry->attr & FS_ATTR_DIRECTORY) {
        if (offset & 0x1f) {
            entry->fs->last_error = FS_ERR_BAD_ARG;
            return 0;
        }
        if (*(entry->cache_p) == 0) {
            entry->fs->last_error = FS_ERR_EOF;
            return 0;
        }
        do {
            offset -= 32;
            entry->cur_pos += 32;
            entry->cache_p += 32;
            if (entry->cur_pos % fat->bytes_per_sec == 0) {
                move_to_next_sector(fat, e);
                if (entry->fs->last_error != FS_ERR_OK)
                    return init_offset - offset;
            }
            do_fat_update_cache(entry);
            if (entry->fs->last_error != FS_ERR_OK)
                return init_offset - offset;
            if (entry->cache_data[entry->cur_pos % fat->bytes_per_sec] == 0)
                return init_offset - offset;
        } while (offset);
    } else {
        if (entry->cur_pos >= entry->size) return 0;
        
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
                    if (entry->fs->last_error != FS_ERR_OK)
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
    do_fat_advance(entry, sizeof(fat_dir_ent_t));
    if (entry->fs->last_error != FS_ERR_OK)
        return 0;
    do_fat_update_cache(entry);
    if (entry->fs->last_error != FS_ERR_OK)
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
    
    do_fat_update_cache(entry);
    if (entry->fs->last_error != FS_ERR_OK)
        return 0;
    
    if (fat->fsif.fs_type == FS_TYPE_FAT16) {
    } else {
        if (de->name[0] == 0) {
            entry->fs->last_error = FS_ERR_EOF;
            return 0;
        }
        while (de->name[0] == 0xE5) {
            de = move_to_next_dir_entry(entry);
            if (!de)
                return 0;
            if (de->name[0] == 0) {
                entry->fs->last_error = FS_ERR_EOF;
                return 0;
            }
        }
        if ((de->attr & FAT_ATTR_MASK) == FAT_ATTR_LONG_NAME) {
            fat_long_ent_t *le = (fat_long_ent_t *) de;
            if (!(le->len & FAT_LAST_LONG_ENTRY)) {
                entry->fs->last_error = FS_ERR_BAD_FORMAT;
                return 0;
            }
            unsigned nb_ent = le->len & FAT_LONG_LEN_MASK;
            new_e = alloc_fs_entry(fat, nb_ent * 13 + 1);
            if (!new_e) {
                entry->fs->last_error = FS_ERR_NO_MEM;
                return 0;
            }
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
                if (le->len != nb_ent) {
                    entry->fs->last_error = FS_ERR_BAD_FORMAT;
                    return 0;
                }
                nb_ent--;
            }
            de = (fat_dir_ent_t *) le;
        } else {
            if (de->attr & FS_ATTR_DIRECTORY) {
                new_e = alloc_fs_entry(fat, 12);
                if (!new_e) {
                    entry->fs->last_error = FS_ERR_NO_MEM;
                    return 0;
                }
                new_entry = &new_e->fs_entry;
                int i;
                for (i = 0; i < 11; ++i) {
                    if (de->name[i] == ' ') break;
                    new_entry->name[i] = de->name[i];
                }
                new_entry->name[i] = 0;
            } else {
                new_e = alloc_fs_entry(fat, 13);
                if (!new_e) {
                    entry->fs->last_error = FS_ERR_NO_MEM;
                    return 0;
                }
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
        do_fat_seek_start((fs_entry_t *)new_e);
        do_fat_advance(entry, sizeof(fat_dir_ent_t));
    }
    return (fs_entry_t *)new_e;
}

fs_entry_t *do_fat_first_child(fs_entry_t *entry)
{
    entry->fs->last_error = FS_ERR_OK;
    if (!(entry->attr & FS_ATTR_DIRECTORY)) {
        entry->fs->last_error = FS_ERR_BAD_ARG;
        return 0;
    }
    if (entry->cache_data == 0)
        do_fat_open(entry);
    if (entry->fs->last_error != FS_ERR_OK)
        return 0;
    do_fat_seek_start(entry);
    return next_child(entry);
}

fs_entry_t *do_fat_next_child(fs_entry_t *entry)
{
    entry->fs->last_error = FS_ERR_OK;
    
    if (!(entry->attr & FS_ATTR_DIRECTORY)) {
        entry->fs->last_error = FS_ERR_BAD_ARG;
        return 0;
    }
    if (entry->cache_data == 0) {
        entry->fs->last_error = FS_ERR_BAD_STATE;
        return 0;
    }
    return next_child(entry);
}

