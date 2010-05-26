/*
 * Queue.
 *
 * Authors: Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 08.09.2009
 * Last modify: 08.09.2009
 */
#include <elvees/queue.h>

unsigned long array[500];

void queue_init (queue_t *q, mem_pool_t *pool, unsigned long size)
{
	q->beg = (unsigned long*)mem_alloc (pool, sizeof (unsigned long) * size);
	q->size = size;
	q->end = q->beg + (q->size - 1);
	q->cur = q->end;	
	q->next = q->end;
	q->pool = pool;
	q->free = q->size;
}

unsigned long queue_push (queue_t *q, unsigned long *word)
{
	if (q->free > 0) {
		*q->next = *word;

		q->next--;
		if (q->next < q->beg) {
			q->next = q->end;
		}
		q->free--;
	}
	else {
		//debug_puts ("\nQueue is full!\n");
	}

	return q->free;
}

unsigned long queue_push_first (queue_t *q, unsigned long *word)
{
	if (q->free > 0) {
		if ((q->cur + 1) <= q->end) {
			q->cur++;
			*q->cur = *word;
		}
		else {
			q->cur = q->beg;
			*q->cur = *word;
		}

		q->free--;
	}
	else {
		//debug_puts ("\nQueue is full!\n");
	}

	return q->free;
}

unsigned long queue_pop (queue_t *q)
{
	unsigned long res = 0;

	if (q->free < q->size) {
		res = *q->cur;
		*q->cur = 0;

		--q->cur;
		if (q->cur < q->beg) {
			q->cur = q->end;
		}
		++q->free;
	}
	else {
		//debug_puts ("\nQueue is empty!\n");
	}

	return res;
}

void queue_free (queue_t *q)
{
	q->cur = q->end;	
	q->next = q->end;
	q->free = q->size;
}

void queue_kill (queue_t *q)
{
	if (q->beg) {
		mem_free (q->beg);
		q->beg = 0;
	}

	q->end = 0;
	q->cur = 0;	
	q->next = 0;
	q->pool = 0;
	q->size = 0;
	q->free = 0;
}

int queue_is_empty (queue_t *q)
{
	return (q->size == q->free);
}

unsigned long queue_size (queue_t *q)
{
	return (q->size - q->free);
}

unsigned long crc_table[256];

unsigned long queue_compute_crc32 (queue_t *q, unsigned long offset)
{
	unsigned long buf = 0,
		beg = 0,
		end = 0,
		len = 0,
		i = 0,
		j = 0,
		crc = 0;

	if (offset > (q->size - q->free))
		return 0;

	for (i = 0; i < 256; i++) {
		crc = i;
		for (j = 0; j < 8; j++)
			crc = (crc & 1) ? ( (crc >> 1) ^ (unsigned long)0xEDB88320 ):( crc >> 1);

		crc_table[i] = crc;
	}
	 
	crc = (unsigned long)0xFFFFFFFF;

	len = (offset) * sizeof (unsigned long);

	buf = (unsigned long)(q->next) + sizeof (unsigned long);
	beg = (unsigned long)q->beg;
	end = (unsigned long)q->end;
 
	while (len--) {
		crc = crc_table[(crc ^ *(unsigned char*)buf) & 0xFF] ^ (crc >> 8);
		buf++;

		if (buf == (end + 4)) {
			buf = beg;
		}
	}

	return crc ^ (unsigned long)0xFFFFFFFF;
}

#if QDEBUG
void queue_print_crc32 (queue_t *q, unsigned long offset)
{
	unsigned long buf = 0,
		beg = 0,
		end = 0,
		len = 0;

	if (offset > (q->size - q->free))
		return;

	len = (offset) * sizeof (unsigned long);

	buf = (unsigned long)(q->next) + sizeof (unsigned long);
	beg = (unsigned long)q->beg;
	end = (unsigned long)q->end;
 
	debug_printf ("\ncomputing crc32:\n");
	while (len--) {
		debug_printf ("%02x", *(unsigned char*)buf);
		buf++;

		if (buf == (end + 4)) {
			buf = beg;
			debug_printf ("*opa*");
		}
	}
	debug_printf ("\ncomputing crc32 end.\n");
}

void queue_print (queue_t *q)
{
	unsigned long *cur = q->cur;
	unsigned long count = q->size - q->free;

	if (q->free == q->size) {
		debug_puts ("\nQueue is empty!\n");
		return;
	}

	/*debug_printf ("beg = 0x%08X\n", q->beg);
	debug_printf ("end = 0x%08X\n", q->end);
	debug_printf ("cur = 0x%08X\n", q->cur);
	debug_printf ("next = 0x%08X\n", q->next);
	debug_printf ("pool = 0x%08X\n", q->pool);
	debug_printf ("size = 0x%08X\n", q->size);
	debug_printf ("free = 0x%08X\n", q->free);
	debug_printf ("crc32 = 0x%08X\n", q->crc32);*/

	debug_puts ("\nQueue is :\n");
	while (count-- > 0) {
		debug_printf ("0x%08X\n", *cur);
		--cur;
		if (cur < q->beg) {
			cur = q->end;
		}
	}
	//debug_printf ("crc32 = 0x%08X\n", q->crc32);
}
#endif

