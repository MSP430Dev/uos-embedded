/*
 * Stack dump routine for ARM architecture.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
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
#include <stream/stream.h>

/*
 * Print stack backtrace.
 * Usage:
 *	debug_dump_stack (task_current->name, arch_get_stack_pointer (),
 *		__builtin_frame_address (1),
 *		__builtin_return_address (0));
 */
void debug_dump_stack (const char *caption, void *sp, void *frame, void *callee)
{
	unsigned int *from, *to, *p, len;
	char c;
	bool_t callee_seen;

	from = sp;
	to = frame;
	if (! uos_valid_memory_address (to) && uos_valid_memory_address (from))
		to = from;
	if (uos_valid_memory_address (to) && ! uos_valid_memory_address (from))
		from = to;

	to -= 16;
	if ((from - to) > 128)
		from = to + 128;

	from = (unsigned*) ((size_t) from & ~(sizeof (void*) - 1));
	to = (unsigned*) ((size_t) to & ~(sizeof (void*) - 1));

	if (to > from) {
		p = to;
		to = from;
		from = p;
	}

	if (from - to < 64)
		from = to + 64;

	debug_printf ("%S.stack {%p/%p..%p/%p, %p, %p}\n", caption,
		sp, from, frame, to, callee, __builtin_frame_address (0));

	/* Stack always grows down. */
	p = from;
	callee_seen = 0;
	len = 0;
	for (;;) {
		p--;
		if (len == 0)
			debug_printf ("[%8S.%08x]", caption, (size_t) p);

		c = ' ';
		if (p == frame)
			c = '=';
		if (p == sp)
			c = '>';

		if (uos_valid_memory_address (p)) {
			if (callee && ! callee_seen && *p == (unsigned) callee) {
				c = '*';
				callee_seen = 1;
			}
			debug_printf (" %c%08x", c, *p);
		} else
			debug_printf (" %c________", c);

		if (++len == 4) {
			debug_putc('\n');
			if (p <= to)
				break;
			len = 0;
		}
	}
}
