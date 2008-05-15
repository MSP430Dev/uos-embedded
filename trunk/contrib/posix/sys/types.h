#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#include <runtime/lib.h>

typedef int sigset_t;
typedef int pid_t;
typedef int uid_t;
typedef int gid_t;
typedef int mode_t;
typedef int dev_t;
typedef int ino_t;
typedef int nlink_t;
typedef long off_t;
typedef long blksize_t;
typedef long blkcnt_t;

#define time_t unsigned long

#endif
