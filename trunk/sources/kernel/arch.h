#ifndef __UOS_ARCH_H_
#define __UOS_ARCH_H_

#include <runtime/arch.h>

struct _task_t;
extern struct _task_t *task_current;
void task_exit_0 (void);

#if __AVR__
#	include <kernel/avr/machdep.h>
#endif
#if defined (__arm__) || defined (__thumb__)
#	include <kernel/arm/machdep.h>
#endif
#if I386
#	include <kernel/i386/machdep.h>
#endif
#if MIPS32
#	include <kernel/mips32/machdep.h>
#endif
#if LINUX386
#	include <kernel/linux386/machdep.h>
#endif
#if __MSDOS__
#	include <kernel/i86-dos/machdep.h>
#endif

#ifndef __arch_intr_is_enabled_now
#	define __arch_intr_is_enabled_now() ({		\
		arch_flags_t __arch_flags;		\
		__arch_flags_save (&__arch_flags);	\
		__arch_intr_is_enabled (__arch_flags);	\
	})
#endif /* __arch_intr_is_enabled_now */

#ifndef __arch_cli_save
#	define __arch_cli_save(arch_flags) do {		\
		__arch_flags_save (arch_flags);		\
		__arch_cli ();				\
	} while (0)
#endif /* __arch_cli_save */

#ifndef __arch_idle
#	define __arch_idle() __noop
#endif /* __arch_idle */

#ifndef __arch_frame_address
#	define __arch_frame_address(count, arg0)	\
		__builtin_frame_address (count)
#endif /* __arch_frame_address */

#ifndef __arch_return_address
#	define __arch_return_address(count, arg0)	\
		__builtin_return_address (count)
#endif /* __arch_return_address */

#ifndef __arch_halt
#	define __arch_halt() do {			\
		for (;;)				\
			__arch_cli ();			\
	} while (0)
#endif /*  __arch__halt */

#endif /* __UOS_ARCH_H_ */
