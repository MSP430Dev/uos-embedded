/*
 * Copyright (C) 2000-2005 Serge Vakulenko, <vak@cronyx.ru>
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

/*
 * Suspend the current task so that other tasks can run.
 */
void
task_yield ()
{
	arch_state_t x;

	arch_intr_disable (&x);

	/* Enqueue always puts element at the tail of the list. */
	task_enqueue (&task_active, task_current);

	/* Scheduler selects the first task.
	 * If there are several tasks with equal priority,
	 * this will allow them to run round-robin. */
	task_schedule ();

	arch_intr_restore (x);
}
