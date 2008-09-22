#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

void
#ifndef NDEBUG
__assert_fail_debug (const char *cond, const char *file, unsigned int line,
	const char *func)
#else
__assert_fail_nodebug ()
#endif
{
	uint_t n;
	task_t *t;
	void *callee = __builtin_return_address (0);
	void *sp = arch_get_stack_pointer ();

#ifndef NDEBUG
	debug_printf ("\nAssertion failed in function `%S':\n%S, %u: %S\n\n",
		func, file, line, cond);
#else
	debug_printf ("\nAssertion failed @%p\n\n", callee);
#endif

	debug_task_print (0);
	n = 0;
	list_iterate_entry (t, &task_active, entry) {
		if (t != task_idle && t != task_current)
			debug_task_print (t);
		if (! uos_valid_memory_address (t))
			break;
		if (++n > 32 || t == list_entry (t->entry.f, task_t, entry)) {
			debug_puts ("...\n");
			break;
		}
	}
	if (task_current && task_current != task_idle)
		debug_task_print (task_current);

	debug_dump_stack (__debug_task_name (task_current), sp,
		(void*) task_current->stack_context, callee);
	debug_printf ("\n*** Please report this information, device halted.\7\n");
	breakpoint ();
	uos_halt ();
}
