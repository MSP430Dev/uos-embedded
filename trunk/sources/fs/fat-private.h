#ifndef __FAT_PRIVATE_H__
#define __FAT_PRIVATE_H__

#include <fs/fs-interface.h>

#define FAT_ATTR_READ_ONLY      0x01
#define FAT_ATTR_HIDDEN         0x02
#define FAT_ATTR_SYSTEM         0x04
#define FAT_ATTR_VOLUME_ID      0x08
#define FAT_ATTR_DIRECTORY      0x10
#define FAT_ATTR_ARCHIVE        0x20
#define FAT_ATTR_LONG_NAME      0x0F

#define FAT_ATTR_MASK           0x3F

#define FAT_LAST_LONG_ENTRY     0x40
#define FAT_LONG_LEN_MASK       0x3F

#define FAT_FSINFO_LEAD_SIG     0x41615252
#define FAT_FSINFO_STRUC_SIG    0x61417272
#define FAT_FSINFO_TRAIL_SIG    0xAA550000

#define FAT32_FILE_END			0x0FFFFFFF


typedef struct __attribute__((packed)) _fat16_bs_t
{
    uint8_t     jump_boot[3];
    uint8_t     oem_name[8];
    uint16_t    bytes_per_sec;
    uint8_t     sec_per_clus;
    uint16_t    rsvd_sec_cnt;
    uint8_t     num_fats;
    uint16_t    root_ent_cnt;
    uint16_t    tot_sec16;
    uint8_t     media;
    uint16_t    fat_sz16;
    uint16_t    sec_per_trk;
    uint16_t    num_heads;
    uint32_t    hidd_sec;
    uint32_t    tot_sec32;
    uint8_t     drv_num;
    uint8_t     reserved1;
    uint8_t     boot_sig;
    uint32_t    vol_id;
    uint8_t     vol_lab[11];
    uint8_t     file_sys_type[8];
} fat16_bs_t;

typedef struct __attribute__((packed)) _fat32_bs_t
{
    uint8_t     jump_boot[3];
    uint8_t     oem_name[8];
    uint16_t    bytes_per_sec;
    uint8_t     sec_per_clus;
    uint16_t    rsvd_sec_cnt;
    uint8_t     num_fats;
    uint16_t    root_ent_cnt;
    uint16_t    tot_sec16;
    uint8_t     media;
    uint16_t    fat_sz16;
    uint16_t    sec_per_trk;
    uint16_t    num_heads;
    uint32_t    hidd_sec;
    uint32_t    tot_sec32;
    uint32_t    fat_sz32;
    uint16_t    ext_flags;
    uint16_t    fs_ver;
    uint32_t    root_clus;
    uint16_t    fs_info;
    uint16_t    bk_boot_sec;
    uint8_t     reserved[12];
    uint8_t     drv_num;
    uint8_t     reserved1;
    uint8_t     boot_sig;
    uint32_t    vol_id;
    uint8_t     vol_lab[11];
    uint8_t     file_sys_type[8];
} fat32_bs_t;

typedef struct __attribute__((packed)) _fat32_fsinfo_t
{
    uint32_t    lead_sig;
    uint32_t    reserved1[120];
    uint32_t    struc_sig;
    uint32_t    free_count;
    uint32_t    nxt_free;
    uint32_t    reserved2[3];
    uint32_t    trail_sig;
} fat32_fsinfo_t;

typedef struct __attribute__((packed)) _fat_dir_ent_t
{
    uint8_t     name[11];
    uint8_t     attr;
    uint8_t     nt_res;
    uint8_t     crt_time_tenth;
    uint8_t     reserved[6];
    uint16_t    fst_clus_hi;
    uint16_t    wrt_time;
    uint16_t    wrt_date;
    uint16_t    fst_clus_lo;
    uint32_t    file_size;
} fat_dir_ent_t;

typedef struct __attribute__((packed)) _fat_long_ent_t
{
    uint8_t     len;
    uint16_t    name0[5];
    uint8_t     attr;
    uint8_t     res0;
    uint8_t     chk_sum;
    uint16_t    name1[6];
    uint16_t    res1;
    uint16_t    name2[2];
} fat_long_ent_t;

typedef struct _fat_fs_entry_t
{
    fs_entry_t  fs_entry;
    uint32_t    first_clus;
    uint32_t    parent_clus;
    uint32_t    parent_pos;
    uint32_t    cur_clus;
    uint8_t     cur_sec;
} fat_fs_entry_t;


int common_check_fat_bs(fat32_bs_t *bs32);
uint8_t check_fat_type(void *bs);

void do_fat_close(fs_entry_t *entry);
void do_fat_free_fs_entry(fs_entry_t *entry);
fs_entry_t *do_fat_get_root(fsif_t *fs);
void do_fat_seek_start(fs_entry_t *entry);
void do_fat_open(fs_entry_t *entry);
void do_fat_update_cache(fs_entry_t *entry);
void do_fat_flush_cache(fs_entry_t *entry);
unsigned do_fat_advance(fs_entry_t *entry, unsigned offset);
fs_entry_t *do_fat_first_child(fs_entry_t *entry);
fs_entry_t *do_fat_next_child(fs_entry_t *entry);


#endif
