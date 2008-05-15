#ifndef __SYS_STAT_H__
#define __SYS_STAT_H__

struct stat {
	dev_t		st_dev;		/* устройство */
	ino_t		st_ino;		/* inode */
	mode_t		st_mode;	/* режим доступа */
	nlink_t		st_nlink;	/* количество жестких ссылок */
	uid_t		st_uid;		/* идентификатор пользователя-владельца */
	gid_t		st_gid;		/* идентификатор группы-владельца */
	dev_t		st_rdev;	/* тип устройства */
					/* (если это устройство) */
	off_t		st_size;	/* общий размер в байтах */
	blksize_t	st_blksize;	/* размер блока ввода-вывода */
					/* в файловой системе */
	blkcnt_t	st_blocks;	/* количество выделенных блоков */
	time_t		st_atime;	/* время последнего доступа */
	time_t		st_mtime;	/* время последней модификации */
	time_t		st_ctime;	/* время последнего изменения */
};

/* Encoding of the file mode. */

#define	S_IFMT		0170000	/* These bits determine file type. */

/* File types. */
#define	S_IFDIR		0040000	/* Directory. */
#define	S_IFCHR		0020000	/* Character device. */
#define	S_IFBLK		0060000	/* Block device. */
#define	S_IFREG		0100000	/* Regular file. */
#define	S_IFIFO		0010000	/* FIFO. */
#define	S_IFLNK		0120000	/* Symbolic link. */
#define	S_IFSOCK	0140000	/* Socket. */

/* Protection bits. */
#define	S_ISUID		04000	/* Set user ID on execution. */
#define	S_ISGID		02000	/* Set group ID on execution. */
#define	S_ISVTX		01000	/* Save swapped text after use (sticky). */
#define	S_IREAD		0400	/* Read by owner. */
#define	S_IWRITE	0200	/* Write by owner. */
#define	S_IEXEC		0100	/* Execute by owner. */

static inline int stat (const char *file_name, struct stat *buf)
	{ return -1; }

static inline int fstat (int filedes, struct stat *buf)
	{ return -1; }

static inline int lstat (const char *file_name, struct stat *buf)
	{ return -1; }

static inline int mkdir (const char *pathname, mode_t mode)
	{ return -1; }

#endif
