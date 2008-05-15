/*
 * Testing memory allocation.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "random/rand15.h"
#include "mem/mem.h"

/*#include <time.h>*/

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0xffff
#define MEM_SIZE	15000

#define NPTR		40

char task [0x400];
mem_pool_t pool;

unsigned long allocated;
void *array [NPTR];
unsigned short size [NPTR];

void print_allocated ()
{
	unsigned char i;

	debug_printf ("allocated:");
	for (i=0; i<NPTR; ++i)
		if (array[i]) {
			debug_printf (" %p-%p",
				array[i], array[i] + mem_size (array[i]) - 1);
		}
	debug_printf ("\n");
}

void allocate (unsigned char i, unsigned short len)
{
	unsigned long real_size;

	size[i] = len;
	array[i] = mem_alloc (&pool, size[i]);
	if (! array[i]) {
		/* Failed to allocate */
		debug_printf ("\tfailed %d (free %d)\n",
			size[i], mem_available (&pool));
/*		mem_print_free_list (&pool);*/
/*		print_allocated ();*/
		return;
	}
	debug_printf ("+");
	real_size = mem_size (array[i]);
	allocated += real_size;
	memset (array[i], 0, size[i]);
}

void dispose (unsigned char i)
{
	unsigned long real_size;

	debug_printf ("\b \b");
	real_size = mem_size (array[i]);
	mem_free (array[i]);
	array[i] = 0;
	allocated -= real_size;
}

void hello (void *arg)
{
	unsigned long available, len;
	unsigned char i;

/*	srand15 (time(0));*/
	for (;;) {
		available = mem_available (&pool);
#if 0
		if (available + allocated > MEM_SIZE) {
			debug_printf ("Mismatch: available(%ld) + allocated(%ld) > %ld\n",
				available, allocated, MEM_SIZE);
			abort();
		}
#endif
		/* Keep 60% free memory. */
		if (available > (unsigned short) (MEM_SIZE * 60L/100)) {
			/* Try to allocate */
			for (i=0; i<NPTR; ++i) {
				if (! array[i]) {
					/* Allocate chunks not more than
					 * 10% of total memory. */
					len = rand15 () % (unsigned short) (MEM_SIZE * 10L/100);
					allocate (i, len);
					break;
				}
			}
		}

		if (array [i = rand15() % NPTR]) {
			if (rand15() & 1) {
				/* Try to truncate */
				unsigned long new_size;

				len = mem_size (array[i]);
				allocated -= len;
				size[i] = rand15() % len;
				mem_truncate (array[i], size[i]);
				new_size = mem_size (array[i]);
				allocated += new_size;
			} else {
				/* Try to free */
				dispose (i);
			}
		}
	}
}

void uos_init (void)
{
/* Baud 9600. */
outb (((int) (KHZ * 1000L / 9600) + 8) / 16 - 1, UBRR);

	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);
	mem_init (&pool, RAM_START, RAM_END);

	task_create (hello, 0, "hello", 1, task, sizeof (task), 0);
}
