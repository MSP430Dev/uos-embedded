#ifndef __FS_INTERFACE_H__
#define __FS_INTERFACE_H__

#define FS_ERR_OK               0
#define FS_ERR_SMALL_BUF        -1
#define FS_ERR_BAD_FORMAT       -2
#define FS_ERR_IO               -3
#define FS_ERR_NO_MEM           -4
#define FS_ERR_EOF              -5
#define FS_ERR_BAD_ARG          -6
#define FS_ERR_BAD_STATE        -7
#define FS_ERR_PROHIBITED       -8

typedef uint32_t filsiz_t;
#define FS_INFINITE_SIZE    0xFFFFFFFF

#define FS_TYPE_UNKNOWN     0
#define FS_TYPE_FAT16       1
#define FS_TYPE_FAT32       2

// Значения первых 6 значений атрибутов такие же, как в файловой
// системе FAT.
// НЕ МЕНЯТЬ ЗНАЧЕНИЯ этих первых 6 макроопределений!
// First 6 attribute values are they are as in FAT file system. 
// DO NOT MODIFY VALUES of this first 6 defines!
#define FS_ATTR_READ_ONLY      0x01
#define FS_ATTR_HIDDEN         0x02
#define FS_ATTR_SYSTEM         0x04
#define FS_ATTR_VOLUME_ID      0x08
#define FS_ATTR_DIRECTORY      0x10
#define FS_ATTR_ARCHIVE        0x20

typedef struct _fsif_t fsif_t;

typedef struct __attribute__((packed)) _fs_entry_t
{
    fsif_t * fs;
    char *   name;
    filsiz_t size;
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint32_t attr;
    uint32_t parent_pos;

    uint8_t *   cache_data __attribute__((aligned(8)));
    uint8_t *   cache_p;
    uint32_t    cache_size;
    int         cache_valid;
    filsiz_t    cur_pos;
} fs_entry_t;

struct _fsif_t
{
    mutex_t      lock;
    
    unsigned     fs_type;
    int8_t       last_error;

    fs_entry_t * (*get_root)(fsif_t *fs);
    fs_entry_t * (*first_child)(fs_entry_t *entry);
    fs_entry_t * (*next_child)(fs_entry_t *entry);
    fs_entry_t * (*get_parent)(fs_entry_t *entry);
    void         (*free_fs_entry)(fs_entry_t *entry);

    void         (*create)(fs_entry_t *parent, fs_entry_t *new_entry);
    void         (*delete)(fs_entry_t *entry);
    void         (*change)(fs_entry_t *entry);
    void         (*move)(fs_entry_t *entry, fs_entry_t *new_parent);

    void         (*open)(fs_entry_t *entry);
    void         (*close)(fs_entry_t *entry);
    void         (*update_cache)(fs_entry_t *entry);
    void         (*flush_cache)(fs_entry_t *entry);
    void         (*seek_start)(fs_entry_t *entry);
    unsigned     (*advance)(fs_entry_t *entry, unsigned offset);
};

static inline unsigned __attribute__((always_inline))
fs_avail_cache_size(fs_entry_t *entry)
{
    if (entry->cache_valid)
        return (entry->cache_size - (entry->cache_p - entry->cache_data));
    else return 0;
}

static inline filsiz_t __attribute__((always_inline))
fs_size_to_end_of_file(fs_entry_t *entry)
{
    return entry->size - entry->cur_pos;
}

static inline filsiz_t __attribute__((always_inline))
fs_size_to_end_of_cache(fs_entry_t *entry)
{
    unsigned ret = 0;
    if (entry->cache_valid) {
        ret = entry->cache_size - (entry->cache_p - entry->cache_data);
        if (fs_size_to_end_of_file(entry) < ret)
            ret = fs_size_to_end_of_file(entry);
    }
    return ret;
}

static inline int __attribute__((always_inline))
fs_at_end(fs_entry_t *entry)
{
    return (entry->cur_pos >= entry->size);
}



#endif
