#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>

extern stream_t debug_stream;

static const char *ptr_valid_or_bad (void *ptr)
{
	return uos_valid_memory_address (ptr) ? "" : " (bad)";
}

/*
 * Print the task info.
 * When t==0, print idle task with header.
 */
void task_print (stream_t *stream, task_t *t)
{
	small_uint_t n;

	if (! t) {
		puts (stream, "Task\t  Address\t Prio\t    Stack\tSpace    Msg\tTicks\n");
		t = task_idle;
	}
	if (! uos_valid_memory_address (t)) {
		printf (stream, "%p (junk)\n", t);
		return;
	}
	printf (stream, "%S\t%9p\t%c%d\t%9p\t%n\t%p\t%lu",
		task_name (t), t, t == task_current ? '*' : ' ', t->prio,
		t->stack_context, task_stack_avail (t), t->message, t->ticks);
	if (t->wait)
		printf (stream, "\n\tWaiting for %p%S", t->wait, ptr_valid_or_bad (t->wait));
	if (t->lock)
		printf (stream, "\n\tLocked by %p%S", t->lock, ptr_valid_or_bad (t->lock));

	if (! list_is_empty (&t->slaves)) {
		mutex_t *m;
		puts (stream, "\n\t\t\t\t\tOwning:");
		n = 0;
		list_iterate (m, &t->slaves) {
			printf (stream, " %p%S", m, ptr_valid_or_bad (m));
			if (! uos_valid_memory_address (m))
				break;
			if (++n > 8 || list_is_empty (&m->item)) {
				puts (stream, "...");
				break;
			}
		}
	}
	putchar (stream, '\n');
}

/*
 * Print the task to the debug output.
 */
void task_print_debug (task_t *t)
{
	task_print (&debug_stream, t);
}
