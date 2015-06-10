#include <runtime/lib.h>
#include <kernel/uos.h>
#include <fs/fat-private.h>
#include <fs/fat32-fast-write.h>

#define FAT32_FW_FS_TYPE    "FAT32 FW"

static inline int fat_write_sector(fat32_fw_t *fat, unsigned sector, unsigned size)
{
    /*
    if (fat->cached_sector_size && sector != fat->cached_sector)
        if (flash_write(fat->flashif, sector, fat->sector, fat->cached_sector_size) != FLASH_ERR_OK)
            return FS_ERR_IO;
    */
/*
static unsigned prev_sec;
if (sector != ++prev_sec) {
    debug_printf("seq fail: sector = %d, prev_sec = %d\n", sector, prev_sec);
    prev_sec = sector;
}
*/
//debug_printf("fat_write_sector %d\n", sector);
    
    if (flash_write(fat->flashif, sector, fat->sector, size) != FLASH_ERR_OK)
        return FS_ERR_IO;
        
    fat->cached_sector = sector;
    fat->cached_sector_size = 0;
    
    return FS_ERR_OK;
}

static inline int fat_read_sector(fat32_fw_t *fat, unsigned sector)
{
//debug_printf("fat_read_sector %d\n", sector);
    if (fat->cached_sector == sector)
        return FS_ERR_OK;
        
    if (fat->cached_sector_size && sector != fat->cached_sector)
        if (flash_write(fat->flashif, fat->cached_sector, fat->sector, FAT_SECTOR_SIZE) != FLASH_ERR_OK)
            return FS_ERR_IO;

    if (flash_read(fat->flashif, sector, fat->sector, FAT_SECTOR_SIZE) != FLASH_ERR_OK)
        return FS_ERR_IO;
        
    fat->cached_sector = sector;
    fat->cached_sector_size = 0;

    return FS_ERR_OK;
}

static int write_fat(fat32_fw_t *fat, unsigned start_sector)
{
    int res;

    memset(fat->sector, 0, FAT_SECTOR_SIZE);
    fat->sector[0] = 0x0FFFFFF8; // Индекс кластера 0 (служебный)
    fat->sector[1] = 0xFFFFFFFF; // Индекс кластера 1 (служебный)
    fat->sector[2] = FAT32_FILE_END; // Индекс кластера с корневой директорией
    // Под корневую директорию выделяем только 1 кластер.

    // cluster_n содержит индекс следующего кластера
    int i;
    unsigned cluster_n = 4;
    for (i = cluster_n - 1; i < FAT_SECTOR_SIZE / 4; ++i)
        fat->sector[i] = cluster_n++;

    res = fat_write_sector(fat, start_sector++, FAT_SECTOR_SIZE);
    if (res != FS_ERR_OK) return res;

    while (cluster_n <= fat->tot_clus + 3) {
        for (i = 0; i < FAT_SECTOR_SIZE / 4; ++i) {
            fat->sector[i] = cluster_n++;
            if (cluster_n == fat->tot_clus + 3)     // Последний кластер в файловой системе.
                fat->sector[i] = FAT32_FILE_END;    // FAT содержит признак конца файла.
            else if (cluster_n > fat->tot_clus + 3)
                fat->sector[i] = 0;
        }

        res = fat_write_sector(fat, start_sector++, FAT_SECTOR_SIZE);
        if (res != FS_ERR_OK) return res;
    }

    return FS_ERR_OK;
}

static inline int write_root_dir(fat32_fw_t *fat, const char * volume_id, unsigned start_sector)
{
    fat_dir_ent_t *entry = (fat_dir_ent_t *)fat->sector;

    memset(fat->sector, 0, sizeof(fat->sector));

    memcpy(entry->name, volume_id, 11);
    entry->attr = FAT_ATTR_VOLUME_ID;

    return fat_write_sector(fat, start_sector, FAT_SECTOR_SIZE);
}

void fat32_fw_format(fat32_fw_t *fat, unsigned nb_sectors, const char * volume_id)
{
    // Стираем флеш полностью
    if (flash_erase_all(fat->flashif) != FLASH_ERR_OK) {
        fat->last_error = FS_ERR_IO;
        return;
    }
    
    memset(fat->sector, 0, FAT_SECTOR_SIZE);

    fat32_bs_t *boot_sector = (fat32_bs_t *)fat->sector;
    boot_sector->jump_boot[0] = 0xEB;
    boot_sector->jump_boot[1] = 0x58;
    boot_sector->jump_boot[2] = 0x90;
    boot_sector->oem_name[0] = 'M';
    boot_sector->oem_name[1] = 'S';
    boot_sector->oem_name[2] = 'D';
    boot_sector->oem_name[3] = 'O';
    boot_sector->oem_name[4] = 'S';
    boot_sector->oem_name[5] = '5';
    boot_sector->oem_name[6] = '.';
    boot_sector->oem_name[7] = '0';
    boot_sector->bytes_per_sec = FAT_SECTOR_SIZE;
    boot_sector->sec_per_clus = FAT_SEC_PER_CLUSTER;
    boot_sector->num_fats = 2;
    // Всего кластеров можно разместить столько:
    fat->tot_clus = nb_sectors >> FAT_SEC_PER_CLUSTER_POW;
    // Под них нужно столько секторов:
    boot_sector->tot_sec32 = nb_sectors & ~(FAT_SEC_PER_CLUSTER - 1);
    fat->tot_sec32 = boot_sector->tot_sec32;
    // Рассчитываем примерный размер FAT в секторах с округлением в большую сторону.
    // На каждый кластер нужно 4 байта и 2 технологических кластера в начале.
    boot_sector->fat_sz32 = ((fat->tot_clus + 2) * 4 + FAT_SECTOR_SIZE - 1) / FAT_SECTOR_SIZE;
    fat->fat_sz32 = boot_sector->fat_sz32;
    // Считаем количество кластеров под загрузочные сектора и FAT с округлением до
    // ближайшего кластера в большую сторону
    unsigned sectors_for_bs_fat = 9 + boot_sector->fat_sz32 * boot_sector->num_fats;
    unsigned clusters_for_bs_fat_aligned = (sectors_for_bs_fat + FAT_SEC_PER_CLUSTER - 1) >>
                                            FAT_SEC_PER_CLUSTER_POW;
    fat->tot_clus -= clusters_for_bs_fat_aligned;
    boot_sector->rsvd_sec_cnt = (clusters_for_bs_fat_aligned << FAT_SEC_PER_CLUSTER_POW) -
                                 boot_sector->fat_sz32 * boot_sector->num_fats;
    fat->rsvd_sec_cnt = boot_sector->rsvd_sec_cnt;

    boot_sector->root_ent_cnt = 0;
    boot_sector->tot_sec16 = 0;
    boot_sector->media = 0xF8;
    boot_sector->fat_sz16 = 0;
    boot_sector->sec_per_trk = 63;
    boot_sector->num_heads = 255;
    boot_sector->hidd_sec = 0;

    boot_sector->ext_flags = 0;
    boot_sector->fs_ver = 0;
    boot_sector->root_clus = 2;
    boot_sector->fs_info = 1;
    boot_sector->bk_boot_sec = 6;
    boot_sector->drv_num = 0x80;
    boot_sector->boot_sig = 0x29;
    boot_sector->vol_id = 0x111D020A;
    memcpy(boot_sector->vol_lab, volume_id, 11);
    memcpy(boot_sector->file_sys_type, FAT32_FW_FS_TYPE, 8);

    fat->sector[(FAT_SECTOR_SIZE >> 2) - 1] = 0xAA550000; // Признак конца загрузочного сектора

    // Пишем основной загрузочный сектор
    fat->last_error = fat_write_sector(fat, 0, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;

    // Пишем резервный загрузочный сектор
    fat->last_error = fat_write_sector(fat, 6, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;

    fat32_fsinfo_t *fs_info = (fat32_fsinfo_t *)fat->sector;
    memset(fs_info, 0, sizeof(fat32_fsinfo_t));
    fs_info->lead_sig = FAT_FSINFO_LEAD_SIG;
    fs_info->struc_sig = FAT_FSINFO_STRUC_SIG;
    fs_info->free_count = 0xFFFFFFFF;
    fs_info->nxt_free = fat->nxt_free = 3;
    fs_info->trail_sig = FAT_FSINFO_TRAIL_SIG;

    // Пишем основную FSInfo
    fat->last_error = fat_write_sector(fat, 1, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;
    // Пишем основную FSInfo
    fat->last_error = fat_write_sector(fat, 7, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;

    // Третий сектор тоже должен иметь признак конца
    memset(fat->sector, 0, FAT_SECTOR_SIZE);
    fat->sector[(FAT_SECTOR_SIZE >> 2) - 1] = 0xAA550000;
    fat->last_error = fat_write_sector(fat, 2, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;
    fat->last_error = fat_write_sector(fat, 8, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;

    // Основная FAT
    fat->last_error = write_fat(fat, fat->rsvd_sec_cnt);
    if (fat->last_error != FS_ERR_OK) return;
    // Резервная FAT
    fat->last_error = write_fat(fat, fat->rsvd_sec_cnt + fat->fat_sz32);
    if (fat->last_error != FS_ERR_OK) return;
    // Пишем корневую директорию
    fat->last_error = write_root_dir(fat, volume_id, fat->rsvd_sec_cnt + (fat->fat_sz32 << 1));
    if (fat->last_error != FS_ERR_OK) return;
    
    if (flash_flush(fat->flashif) != FLASH_ERR_OK) {
        fat->last_error = FS_ERR_IO;
        return;
    }

    fat->last_error = FS_ERR_OK;
}

static void fs_entry_to_dir_ent(fat_dir_ent_t *de, fat32_fw_entry_t *fw_ent)
{
    fs_entry_t *entry = &fw_ent->base;

    memset(de, 0, sizeof(fat_dir_ent_t));

    int i, j;
    for (i = 0; i < 8; ++i) {
        if (entry->name[i] == '.' || i >= strlen((uint8_t *)entry->name))
            break;
        de->name[i] = entry->name[i];
    }
    for (j = i; j < 8; ++j) {
        de->name[i] = ' ';
    }
    if (i < 8) i++;
    for (j = 0; j < 3; ++j) {
        if (i >= strlen((uint8_t *)entry->name)) break;
        de->name[8+j] = entry->name[i+j];
    }

    if (entry->second & 1) de->crt_time_tenth = 99;
    de->wrt_time = (entry->second >> 1) | (entry->minute << 5) | (entry->hour << 11);
    de->wrt_date = entry->day | (entry->month << 5) | ((entry->year - 1980) << 9);

    de->fst_clus_lo = fw_ent->first_clus;
    de->fst_clus_hi = fw_ent->first_clus >> 16;
}

static void dir_ent_to_fs_entry(fat32_fw_entry_t *fw_ent, fat_dir_ent_t *de)
{
    fs_entry_t *entry = &fw_ent->base;
    
    int i, j;
    for (i = 0; i < 8; ++i) {
        if (de->name[i] == ' ') break;
        entry->name[i] = de->name[i];
    }
    if (de->name[8] != ' ') {
        entry->name[i++] = '.';
        for (j = 0; j < 3; ++j)
            entry->name[i+j] = de->name[8+j];
        entry->name[i+j] = 0;
    } else entry->name[i] = 0;
    
    entry->attr = de->attr;
    entry->size = de->file_size;
    entry->day = de->wrt_date & 0x1F;
    entry->month = (de->wrt_date >> 5) & 0x0F;
    entry->year = 1980 + ((de->wrt_date >> 9) & 0x7F);
    entry->second = ((de->wrt_time & 0x1F) << 1) +
        de->crt_time_tenth / 100;
    entry->minute = (de->wrt_time >> 5) & 0x3F;
    entry->hour = (de->wrt_time >> 11) & 0x1F;

    fw_ent->first_clus = de->fst_clus_lo | (de->fst_clus_hi << 16);
}

void fat32_fw_create(fat32_fw_t *fat, fs_entry_t *entry)
{
    fat_dir_ent_t *e;
    unsigned nb_sec;
    
    if (fat->cur_dir_entry.mode) {
        fat->last_error = FS_ERR_PROHIBITED;
        return; 
    }

    if (fat->nb_entries == FAT_SECTOR_SIZE / sizeof(fat_dir_ent_t) * FAT_SEC_PER_CLUSTER) {
        fat->last_error = FS_ERR_EOF;
        return;
    } else if (fat->nb_entries == 0) {
        // Если корневая директория ещё не читалась, то делаем это сейчас,
        // для того чтобы найти первую свободную запись в ней.
        nb_sec = fat->rsvd_sec_cnt + 2 * fat->fat_sz32;
        unsigned sec_in_clus = 0;
        while (sec_in_clus < FAT_SEC_PER_CLUSTER) {
            fat->last_error = fat_read_sector(fat, nb_sec);
            if (fat->last_error != FS_ERR_OK) return;

            e = (fat_dir_ent_t *) fat->sector;
            while ((e->name[0] != 0) && ((uint8_t *)e - (uint8_t *)fat->sector < FAT_SECTOR_SIZE)) {
                e++;
                fat->nb_entries++;
            }

            if (e->name[0] == 0) break;
            nb_sec++;
            sec_in_clus++;
        }

        if (sec_in_clus == FAT_SEC_PER_CLUSTER) {    // Корневая директория заполнена,
            fat->last_error = FS_ERR_EOF;       // больше записывать нельзя
            return;
        }
    } else {
        // Номер свободной записи в корневой директории мы уже знаем,
        // так что читаем сразу нужный сектор.
        nb_sec = fat->rsvd_sec_cnt + 2 * fat->fat_sz32 + (fat->nb_entries >> (FAT_SECTOR_SIZE_POW - 5));
        fat->last_error = fat_read_sector(fat, nb_sec);
        if (fat->last_error != FS_ERR_OK) return;

        e = (fat_dir_ent_t *) fat->sector + (fat->nb_entries & (FAT_SECTOR_SIZE / sizeof(fat_dir_ent_t) - 1));
    }
    
    memcpy(&fat->cur_dir_entry, entry, sizeof(fs_entry_t));
    fat->cur_dir_entry.base.name = fat->name_buf;
    memcpy(fat->cur_dir_entry.base.name, entry->name, 11);
    fat->cur_dir_entry.first_clus = fat->nxt_free;
    fs_entry_to_dir_ent(e, &fat->cur_dir_entry);

    entry->parent_pos = fat->nb_entries++ * sizeof(fat_dir_ent_t);
    
    fat->last_error = fat_write_sector(fat, nb_sec, 
        (entry->parent_pos  & (FAT_SECTOR_SIZE-1)) + sizeof(fat_dir_ent_t));
    if (fat->last_error != FS_ERR_OK) return;

    fat->last_error = FS_ERR_OK;
}

void fat32_fw_open(fat32_fw_t *fat, int file_mode)
{
    if (fat->cur_dir_entry.mode) {
        fat->last_error = FS_ERR_PROHIBITED;
        return;
    }

    if (file_mode == O_READ || file_mode == O_WRITE) {
        fat->cur_dir_entry.cur_sector = fat->rsvd_sec_cnt + 2 * fat->fat_sz32 +
        ((fat->cur_dir_entry.first_clus - 2) << FAT_SEC_PER_CLUSTER_POW);
        fat->cur_dir_entry.base.cur_pos = 0;
    } else {
        fat->last_error = FS_ERR_BAD_ARG;
        return;
    }

    fat->cur_dir_entry.mode = file_mode;
}

filsiz_t fat32_fw_read(fat32_fw_t *fat, void *data, filsiz_t size)
{
    if (fat->cur_dir_entry.mode != O_READ) {
        fat->last_error = FS_ERR_BAD_STATE;
        return 0;
    }

    if (fat->cur_dir_entry.base.size - fat->cur_dir_entry.base.cur_pos < size)
        size = fat->cur_dir_entry.base.size - fat->cur_dir_entry.base.cur_pos;

    unsigned sector_offset = fat->cur_dir_entry.base.cur_pos & (FAT_SECTOR_SIZE - 1);
    unsigned bytes_to_end_of_sector = FAT_SECTOR_SIZE - sector_offset;
    filsiz_t cur_size = (size < bytes_to_end_of_sector) ? size : bytes_to_end_of_sector;
    uint8_t *p = (uint8_t *)data;
    
    fat->last_error = fat_read_sector(fat, fat->cur_dir_entry.cur_sector);
    if (fat->last_error != FS_ERR_OK) return 0;

    memcpy(data, (uint8_t *)fat->sector + sector_offset, cur_size);

    fat->cur_dir_entry.base.cur_pos += cur_size;

    while (1) {
        p += cur_size;
        sector_offset = fat->cur_dir_entry.base.cur_pos & (FAT_SECTOR_SIZE - 1);
        if (sector_offset == 0) fat->cur_dir_entry.cur_sector++;

        size -= cur_size;
        if (size == 0) break;

        cur_size = (size < FAT_SECTOR_SIZE) ? size : FAT_SECTOR_SIZE;
        fat->last_error = fat_read_sector(fat, fat->cur_dir_entry.cur_sector);
        if (fat->last_error != FS_ERR_OK)
            return (p - (uint8_t *)data);
        memcpy(p, fat->sector, cur_size);

        fat->cur_dir_entry.base.cur_pos += cur_size;
    }

    fat->last_error = FS_ERR_OK;
    return (p - (uint8_t *)data);
}

void fat32_fw_write(fat32_fw_t *fat, void *data, filsiz_t size)
{
    if (fat->cur_dir_entry.mode != O_WRITE) {
        fat->last_error = FS_ERR_BAD_STATE;
        return;
    }
    //fat->last_error = fat_read_sector(fat, fat->cur_dir_entry.cur_sector);
    //if (fat->last_error != FS_ERR_OK) return;

    unsigned sector_offset = fat->cur_dir_entry.base.cur_pos & (FAT_SECTOR_SIZE - 1);
    unsigned bytes_to_end_of_sector = FAT_SECTOR_SIZE - sector_offset;
    filsiz_t cur_size = (size < bytes_to_end_of_sector) ? size : bytes_to_end_of_sector;
    uint8_t *p = (uint8_t *)data;
    memcpy((uint8_t *)fat->sector + sector_offset, data, cur_size);
    fat->cur_dir_entry.base.cur_pos += cur_size;
    if (cur_size < bytes_to_end_of_sector) {
        fat->cached_sector_size = sector_offset + cur_size;
        fat->last_error = FS_ERR_OK;
        return;
    }
    fat->last_error = fat_write_sector(fat, fat->cur_dir_entry.cur_sector, FAT_SECTOR_SIZE);
    if (fat->last_error != FS_ERR_OK) return;

    while (1) {
        p += cur_size;
        sector_offset = fat->cur_dir_entry.base.cur_pos & (FAT_SECTOR_SIZE - 1);
        if (sector_offset == 0) fat->cur_dir_entry.cur_sector++;    
        if (fat->cur_dir_entry.cur_sector >= fat->tot_sec32) {
            fat->last_error = FS_ERR_EOF;
            return;
        }
        size -= cur_size;
        if (size == 0) break;

        cur_size = (size < FAT_SECTOR_SIZE) ? size : FAT_SECTOR_SIZE;
        if (cur_size == FAT_SECTOR_SIZE) {
            if (((unsigned) p & flash_data_align(fat->flashif)) == 0) {
                if (flash_write(fat->flashif, fat->cur_dir_entry.cur_sector, p, cur_size) != FLASH_ERR_OK) {
                    fat->last_error = FS_ERR_IO;
                    return;
                }
            } else {
                memcpy(fat->sector, p, cur_size);
                fat->last_error = fat_write_sector(fat, fat->cur_dir_entry.cur_sector, cur_size);
                if (fat->last_error != FS_ERR_OK) return;
            }
        } else {
            memcpy(fat->sector, p, cur_size);
            fat->cached_sector_size = cur_size;
        }
        fat->cur_dir_entry.base.cur_pos += cur_size;
    }
    
    fat->last_error = FS_ERR_OK;
}

void fat32_fw_close(fat32_fw_t *fat)
{
    if (fat->cur_dir_entry.mode == O_WRITE) {
        // При закрытии записи нужно выполнить несколько шагов:
        // 1. Записать кэшируемый сектор на флеш
        if (fat->cached_sector_size) {
            fat->last_error = fat_write_sector(fat, fat->cur_dir_entry.cur_sector, 
                fat->cached_sector_size);
            if (fat->last_error != FS_ERR_OK) return;
        }
        
        // 2. Записать в FAT признак конца файла.
        unsigned last_cluster = ((fat->cur_dir_entry.cur_sector - fat->rsvd_sec_cnt - 2 * fat->fat_sz32) >>
            FAT_SEC_PER_CLUSTER_POW) + 2;
        if ((fat->cur_dir_entry.base.cur_pos & (FAT_SECTOR_SIZE * FAT_SEC_PER_CLUSTER - 1)) == 0)
            last_cluster--;

        unsigned last_cluster_fat_sec = fat->rsvd_sec_cnt + (last_cluster >> (FAT_SECTOR_SIZE_POW - 2));

        fat->last_error = fat_read_sector(fat, last_cluster_fat_sec);
        if (fat->last_error != FS_ERR_OK) return;

        fat->sector[last_cluster & ((FAT_SECTOR_SIZE >> 2) - 1)] = FAT32_FILE_END;

        fat->last_error = fat_write_sector(fat, last_cluster_fat_sec, FAT_SECTOR_SIZE);
        if (fat->last_error != FS_ERR_OK) return;

        // 3. Обновить запись о файле в корневой директории.
        // Здесь считаем, что всегда sizeof(fat_dir_ent_t) == 32
        unsigned nb_sec = fat->rsvd_sec_cnt + 2 * fat->fat_sz32 +
            ((fat->nb_entries - 1) >> (FAT_SECTOR_SIZE_POW - 5));
        fat->last_error = fat_read_sector(fat, nb_sec);
        if (fat->last_error != FS_ERR_OK) return;

        fat_dir_ent_t *e = (fat_dir_ent_t *) fat->sector +
            ((fat->nb_entries - 1) & ((FAT_SECTOR_SIZE >> 5) - 1));
        e->file_size = fat->cur_dir_entry.base.cur_pos;

        fat->last_error = fat_write_sector(fat, nb_sec, FAT_SECTOR_SIZE);
        if (fat->last_error != FS_ERR_OK) return;

        if (flash_write(fat->flashif, nb_sec, fat->sector, FAT_SECTOR_SIZE) != FLASH_ERR_OK) {
            fat->last_error = FS_ERR_IO;
            return;
        }

        // 4. Посчитать новое значение номера свободного кластера
        //    и записать его в FSInfo.
        fat->nxt_free = last_cluster + 1;
        fat32_fsinfo_t *fs_info = (fat32_fsinfo_t *)fat->sector;
        fat->last_error = fat_read_sector(fat, 1);
        if (fat->last_error != FS_ERR_OK) return;
        fs_info->nxt_free = fat->nxt_free;
        fat->last_error = fat_write_sector(fat, 1, FAT_SECTOR_SIZE);
        if (fat->last_error != FS_ERR_OK) return;
        fat->last_error = fat_write_sector(fat, 7, FAT_SECTOR_SIZE);
        if (fat->last_error != FS_ERR_OK) return;

        fat->cur_dir_entry.base.size = fat->cur_dir_entry.base.cur_pos;
    }

    fat->cur_dir_entry.mode = 0;

    fat->last_error = FS_ERR_OK;
}

fs_entry_t *get_first_entry(fat32_fw_t *fat)
{
    unsigned nb_sec = fat->rsvd_sec_cnt + 2 * fat->fat_sz32;

    fat->last_error = fat_read_sector(fat, nb_sec);
    if (fat->last_error != FS_ERR_OK) return 0;

    fat->cur_parent_pos = 0;

    dir_ent_to_fs_entry(&fat->cur_dir_entry, (fat_dir_ent_t *)fat->sector);

    fat->last_error = FS_ERR_OK;

    return &fat->cur_dir_entry.base;
}

fs_entry_t *get_next_entry(fat32_fw_t *fat)
{
    if (fat->cur_parent_pos == FAT_SECTOR_SIZE * FAT_SEC_PER_CLUSTER) {
        fat->last_error = FS_ERR_EOF;
        return 0;
    }

    fat->cur_parent_pos += sizeof(fat_dir_ent_t);

    unsigned nb_sec = fat->rsvd_sec_cnt + 2 * fat->fat_sz32 +
        (fat->cur_parent_pos >> FAT_SECTOR_SIZE_POW);

    fat->last_error = fat_read_sector(fat, nb_sec);
    if (fat->last_error != FS_ERR_OK) {
        fat->cur_parent_pos -= sizeof(fat_dir_ent_t);
        return 0;
    }

    fat_dir_ent_t *de = (fat_dir_ent_t *)((unsigned)fat->sector +
        (fat->cur_parent_pos & (FAT_SECTOR_SIZE - 1)));

    if (de->name[0] == 0) {
        fat->last_error = FS_ERR_EOF;
        return 0;
    }

    dir_ent_to_fs_entry(&fat->cur_dir_entry, de);

    fat->last_error = FS_ERR_OK;

    return &fat->cur_dir_entry.base;
}

void fat32_fw_init(fat32_fw_t *fat, flashif_t * flash)
{
    fat->flashif = flash;
    fat->cached_sector = ~0;
    fat->last_error = fat_read_sector(fat, 0);
    if (fat->last_error != FS_ERR_OK) return;
    fat32_bs_t *boot_sector = (fat32_bs_t *)fat->sector;
    if (memcmp(boot_sector->file_sys_type, FAT32_FW_FS_TYPE, 8) != 0) {
        fat->last_error = FS_ERR_BAD_FORMAT;
        return;
    }
    if (boot_sector->bytes_per_sec != FAT_SECTOR_SIZE) {
        fat->last_error = FS_ERR_BAD_FORMAT;
        return;
    }
    if (boot_sector->sec_per_clus != FAT_SEC_PER_CLUSTER) {
        fat->last_error = FS_ERR_BAD_FORMAT;
        return;
    }
    fat->rsvd_sec_cnt = boot_sector->rsvd_sec_cnt;
    fat->fat_sz32 = boot_sector->fat_sz32;
    fat->tot_sec32 = boot_sector->tot_sec32;
    fat->tot_clus = (fat->tot_sec32 - fat->rsvd_sec_cnt - 2 * fat->fat_sz32)
                        >> FAT_SEC_PER_CLUSTER_POW;

    fat->last_error = fat_read_sector(fat, 1);
    if (fat->last_error != FS_ERR_OK) return;
    fat32_fsinfo_t *fs_info = (fat32_fsinfo_t *)fat->sector;
    if (fs_info->lead_sig != FAT_FSINFO_LEAD_SIG ||
        fs_info->struc_sig != FAT_FSINFO_STRUC_SIG ||
        fs_info->trail_sig != FAT_FSINFO_TRAIL_SIG) {
            fat->last_error = FS_ERR_BAD_FORMAT;
            return;
    }
    fat->nxt_free = fs_info->nxt_free;

    fat->cur_dir_entry.base.cur_pos = 0;
    fat->nb_entries = 0;    // Значение 0 означает, что в настоящий момент
    // неизвестно, сколько всего записей в корневой директории.
    // Если даже файловая система полностью чистая, то всё равно
    // есть один файл с именем тома FAT32, так что значения 0
    // после чтения корневой директории быть не может.

    fat->cur_parent_pos = 0;
    fat->cached_sector_size = 0;
    memset(&fat->cur_dir_entry, 0, sizeof(fs_entry_t));
    fat->cur_dir_entry.base.name = fat->name_buf;

    fat->last_error = FS_ERR_OK;
}

