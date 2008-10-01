/*
 * Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * without any conditions or restrictions.  This software is provided
 * "as is" without express or implied warranty.
 */
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#ifdef __AVR__
#	define watchdog_alive() asm volatile ("wdr")
#if defined(WDP3)
#       define _WD_PS3_MASK       (1<<WDP3)
#else
#       define _WD_PS3_MASK       0x00
#endif /* WDP3 */

#endif /* __AVR__ */

void watchdog_enable (int timeout);
void watchdog_disable (void);

#endif
