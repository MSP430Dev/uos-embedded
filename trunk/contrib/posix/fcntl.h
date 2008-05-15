#ifndef __FCNTL_H__
#define __FCNTL_H__

#include <runtime/lib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* open/fcntl - O_SYNC is only implemented on blocks devices and on files
   located on an ext2 file system */
#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	/* not fcntl */
#define O_EXCL		   0200	/* not fcntl */
#define O_NOCTTY	   0400	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC		 010000
#define O_FSYNC		 O_SYNC
#define O_ASYNC		 020000

static inline int open (const char *pathname, int flags)
	{ return -1; }

static inline int creat (const char *pathname, mode_t mode)
	{ return -1; }

#ifdef __cplusplus
}
#endif

#endif
