#ifndef __SYS_SELECT_H__
#define __SYS_SELECT_H__
#include <sys/time.h>

/* The fd_set member is required to be an array of longs.  */
typedef long fd_mask;

/* It's easier to assume 8-bit bytes than to get CHAR_BIT.  */
#define NFDBITS		(8 * sizeof (fd_mask))
#define FDELT(d)	((d) / NFDBITS)
#define FDMASK(d)	((fd_mask) 1 << ((d) % NFDBITS))

/* Maximum number of file descriptors in `fd_set'.  */
#define	FD_SETSIZE		32

/* fd_set for select and pselect.  */
typedef struct {
	fd_mask fds_bits [FD_SETSIZE / NFDBITS];
} fd_set;
#define FDS_BITS(set)	((set)->fds_bits)

static inline int select (int n, fd_set *readfds, fd_set *writefds,
	fd_set *exceptfds, struct timeval *timeout)
	{ return 0; }

#define FD_ZERO(set) \
	do {							\
	unsigned int i;						\
	fd_set *arr = (set);					\
	for (i = 0; i < sizeof (fd_set) / sizeof (fd_mask); ++i)\
	FDS_BITS (arr)[i] = 0;					\
	} while (0)

#define FD_SET(d, set)		(FDS_BITS (set)[FDELT (d)] |= FDMASK (d))
#define FD_CLR(d, set)		(FDS_BITS (set)[FDELT (d)] &= ~FDMASK (d))
#define FD_ISSET(d, set)	(FDS_BITS (set)[FDELT (d)] & FDMASK (d))

#endif
