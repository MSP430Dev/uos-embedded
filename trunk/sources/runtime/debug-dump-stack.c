#include <runtime/lib.h>
#include <stream/stream.h>

/*
 * Print stack backtrace.
 * Usage:
 *	debug_dump_stack (task_current->name, arch_get_stack_pointer (),
 *		__builtin_frame_address (1),
 *		__builtin_return_address (0));
 */
void debug_dump_stack (const char *caption, void *sp, void* frame, void *callee)
{
	unsigned char *from, *to, *p;
	unsigned int len;
	char c;
	bool_t flag;

	to = frame; from = sp;
	if (! uos_valid_memory_address (to) && uos_valid_memory_address (from))
		to = from;
	if (uos_valid_memory_address (to) && ! uos_valid_memory_address (from))
		from = to;

	to -= 16 * sizeof (void*);
	if ((from - to) > 128 * sizeof (void *))
		from = to + 128 * sizeof (void*);

	from = (unsigned char*) ((size_t) from & ~(sizeof (void *) - 1));
	to = (unsigned char*) ((size_t) to & ~(sizeof (void *) - 1));

	if (to > from) {
		p = to;
		to = from;
		from = p;
	}

	if ((from - to) < 64 * sizeof (void*))
		from = to + 64 * sizeof (void*);

	debug_printf ("%S.stack {%p/%p..%p/%p, %p, %p}\n", caption,
		sp, from, frame, to, callee, __builtin_frame_address (0));

	/* Stack always grows down. */
	for (p = from, flag = 0, len = 0;; --p) {
		if (len == 0) {
			if (sizeof (p) == 1)
				debug_printf ("[%8S.%02X]", caption, (size_t) p);
			else if (sizeof (p) == 2)
				debug_printf ("[%8S.%04X]", caption, (size_t) p);
			else if (sizeof (p) == 4)
				debug_printf ("[%8S.%08X]", caption, (size_t) p);
			else if (sizeof (p) == 8)
				debug_printf ("[%8S.%016X]", caption, (size_t) p);
			else
				debug_printf ("[%8S.%0p]", caption, (size_t) p);
		}

		c = ' ';
		if (p == frame)
			c = '=';
		if (p == sp)
			c = '>';

		if (uos_valid_memory_address (p)) {
			if (callee && *(void**)p == callee && (flag & 1) == 0) {
				c = '*';
				flag |= 1;
			}
			debug_printf (" %c%02X", c, *p);
		} else
			debug_printf (" %c__", c);

		if (p == to)
			flag |= 2;
		if (++len == 16) {
			debug_putc('\n');
			if (flag & 2)
				break;
			len = 0;
		}
	}
}
