#ifndef __RAND15_H_
#define	__RAND15_H_ 1

#include <kernel/uos.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Simple 15-bit random number generator.
 */
short rand15 (void);
void srand15 (unsigned short);

#ifdef __cplusplus
}
#endif

#endif /* !__RAND15_H_ */
