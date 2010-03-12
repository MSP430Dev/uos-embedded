/*
 * Copyright (C) 2000-2010 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

#ifdef ARCH_HAVE_FPU
/*
 * Control modes of float point coprocessor.
 * Return a previous mode of FPU.
 */
unsigned int
task_fpu_control (task_t *t, unsigned int mode, unsigned int mask)
{
	arch_state_t x;
	unsigned int old;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));

	old = arch_fpu_control (t, mode, mask);
	arch_intr_restore (x);

	return old;
}
#endif
