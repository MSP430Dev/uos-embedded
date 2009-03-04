/*
 * Copyright (C) 2009 Serge Vakulenko, <vak@cronyx.ru>
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
 * Associate IRQ with the lock.
 * On interrupt, the signal message is sent to the lock.
 */
void
lock_attach_irq (lock_t *m, int irq, handler_t func, void *arg)
{
	arch_state_t x;

	arch_intr_disable (&x);

	m->irq = &lock_irq [irq];
	assert (m->irq->lock == 0);
	m->irq->lock = m;
	m->irq->irq = irq;
	m->irq->handler = func;
	m->irq->arg = arg;
	m->irq->pending = 0;

	arch_intr_bind (irq);
	arch_intr_allow (irq);

	arch_intr_restore (x);
}
