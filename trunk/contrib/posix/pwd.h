#ifndef	__PWD_H__
#define	__PWD_H__

#include <stddef.h>

/* The passwd structure.  */
struct passwd {
	char *pw_name;		/* Username.  */
	char *pw_passwd;	/* Password.  */
	uid_t pw_uid;		/* User ID.  */
	gid_t pw_gid;		/* Group ID.  */
	char *pw_gecos;		/* Real name.  */
	char *pw_dir;		/* Home directory.  */
	char *pw_shell;		/* Shell program.  */
};

static inline struct passwd *getpwuid (uid_t uid)
	{ return 0; }

static inline struct passwd *getpwnam (const char *name)
	{ return 0; }

#endif /* pwd.h */
