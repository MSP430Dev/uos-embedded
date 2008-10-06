#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>

extern stream_t debug_stream;

const char* __debug_task_name (task_t *t)
{
	small_uint_t n;
	const char *str = t->name;

	for (n = 0; n < 16; n++) {
		unsigned char b = flash_fetch (str++);
		if (! b)
			break;
		if (b < ' ' || b > '~')
			goto ballout;
	}

	if (n > 2 && n < 9)
		return t->name;

ballout:
	return "@JUNK";
}

const char *__debug_ptr_name (void *ptr)
{
	return uos_valid_memory_address (ptr) ? "" : " (junk)";
}

void debug_dump_stack_task (task_t *t)
{
	debug_dump_stack (__debug_task_name (t),
		task_stack_avail (t) + (char*) &t->stack,
		(void*) t->stack_context,  0);
}

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
		__debug_task_name (t), t, t == task_current ? '>' : ' ', t->prio,
		t->stack_context, task_stack_avail (t), t->message, t->ticks);
	if (t->wait)
		printf (stream, "\n\tWaiting for %p%S", t->wait, __debug_ptr_name (t->wait));
	if (t->lock)
		printf (stream, "\n\tLocked by %p%S", t->lock, __debug_ptr_name (t->lock));

	if (! list_is_empty (&t->slaves)) {
		lock_t *m;
		puts (stream, "\n\t\t\t\t\tOwning:");
		n = 0;
		list_iterate (m, &t->slaves) {
			printf (stream, " %p%S", m, __debug_ptr_name (m));
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

void debug_task_print (task_t *t)
{
	task_print (&debug_stream, t);
}
