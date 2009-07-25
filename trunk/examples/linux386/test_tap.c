/*
 * Testing tap.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include "mem/mem.h"
#include "buf/buf.h"
#include "tap/tap.h"

#define MEM_SIZE	15000

ARRAY (task, 6000);
char memory [MEM_SIZE];
mem_pool_t pool;
tap_t tap;

void hello (void *data)
{
	buf_t *p;

	mutex_lock (&tap.netif.lock);
	for (;;) {
		mutex_wait (&tap.netif.lock);
		p = netif_input (&tap.netif);
		if (p) {
			debug_printf ("received %d bytes\n", p->tot_len);
			buf_print_ethernet (p);
		}
	}
}

void uos_init (void)
{
	mem_init (&pool, (size_t) memory, (size_t) memory + MEM_SIZE);
	tap_init (&tap, "tap0", 80, &pool, 0);
	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
