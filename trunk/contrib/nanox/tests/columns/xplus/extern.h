#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif
	void xFatal (char *, ...);
	void xError (char *, ...);
#ifdef __cplusplus
};
#endif
