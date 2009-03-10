#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

void watchdog_enable (int timeout);
void watchdog_disable (void);

/* watchdog_alive is declared in runtime/arch/stdlib.h */

#ifdef __AVR__
#   define watchdog_alive()	asm volatile ("wdr")
#endif

#endif
