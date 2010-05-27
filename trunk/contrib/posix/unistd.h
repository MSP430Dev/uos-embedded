#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <timer/timer.h>
#include <errno.h>
/*#include <dirent.h>*/

static inline int getpid()
	{ return (int) task_current; }

static inline int geteuid()
	{ return 1; }

static inline int setpgid (int pid, int pgid)
	{ return 0; }

static inline void sync()
	{}

static inline int access (const char *pathname, int mode)
	{ return -1; }

static inline int unlink (const char *pathname)
	{ return -1; }

/* Values for the second argument to access.
   These may be OR'd together.  */
#define	R_OK	4		/* Test for read permission.  */
#define	W_OK	2		/* Test for write permission.  */
#define	X_OK	1		/* Test for execute permission.  */
#define	F_OK	0		/* Test for existence.  */

static inline int close (int fd)
	{ return 0; }

static inline size_t read (int fd, void *buf, size_t count)
	{ return 0; }

static inline size_t write (int fd, const void *buf, size_t count)
	{ return count; }

#define PF_LOCAL 0
#define SOCK_STREAM 0
static inline int socketpair (int d, int type, int protocol, int sv[2])
{
	errno = EOPNOTSUPP;
	return -1;
}

static inline void usleep (unsigned long usec)
{
	extern timer_t *uos_timer;

	timer_delay (uos_timer, usec / 1000);
}

static inline char *getcwd (char *buf, size_t size)
{
	buf[0] = '/';
	buf[1] = 0;
	return buf;
}

static inline char *getwd (char *buf)
{
	buf[0] = '/';
	buf[1] = 0;
	return buf;
}

#endif
