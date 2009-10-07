/*
 * Queue.
 *
 * Authors: Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 08.09.2009
 * Last modify: 08.09.2009
 */
#ifndef QUEUE_H
#define QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif

#define QDEBUG 1

#ifndef QDEBUG
#define QDEBUG 0
#endif

#include <runtime/lib.h>
#include <mem/mem.h>

typedef struct _queue_t {
	unsigned long *next;
	unsigned long *cur;
	unsigned long *beg;
	unsigned long *end;
	unsigned long size;
	unsigned long free;

	mem_pool_t *pool;
} queue_t;

void queue_init (queue_t *q, mem_pool_t *pool, unsigned long size);
unsigned long queue_push (queue_t *q, unsigned long *word);
unsigned long queue_push_first (queue_t *q, unsigned long *word);
unsigned long queue_pop (queue_t *q);
unsigned long queue_size (queue_t *q);
void queue_free (queue_t *q);
void queue_kill (queue_t *q);
int queue_is_empty (queue_t *q);

unsigned long queue_compute_crc32 (queue_t *q, unsigned long offset);

#if QDEBUG
void queue_print_crc32 (queue_t *q, unsigned long offset);
void queue_print (queue_t *q);
#endif

#ifdef __cplusplus
}
#endif
#endif
