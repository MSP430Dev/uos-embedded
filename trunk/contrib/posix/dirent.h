#ifndef __DIRENT_H__
#define __DIRENT_H__

#include <sys/types.h>

#define NAME_MAX 256

struct dirent {
	long d_ino;			/* номер inode */
	off_t d_off;			/* смещение на dirent */
	unsigned short d_reclen;	/* длина d_name */
	char d_name [NAME_MAX+1];	/* имя файла (оканчивающееся нулем) */
};

static inline int readdir (unsigned int fd, struct dirent *dirp,
    unsigned int count)
	{ return 0; }

static inline int scandir (const char *dir, struct dirent ***namelist,
	int (*select) (const struct dirent*),
	int (*compar) (const void*, const void*))
	{ return -1; }
#endif
