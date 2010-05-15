/*
 * LPORT API on Elvees MC-24EM board.
 *
 * Authors: Kirill Salyamov (ksalyamov@elvees.com), Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 24.08.2009
 * Last modify: 07.10.2009
 */
#ifndef LPORT_API_H
#define LPORT_API_H
#ifdef __cplusplus
extern "C" {
#endif

#include <runtime/lib.h>
#include <mem/mem.h>
#include <elvees/queue.h>

#define LPORT_MAGIC (unsigned long)0x7f7f7f7f

#define LPORT_DEBUG 1

#ifndef LPORT_DEBUG
#define LPORT_DEBUG 0
#endif

#ifdef LPORT_DEBUG
#include <timer/timer.h>
#endif

extern void _end();

typedef struct _lport_desc_t {
	unsigned long len:1;
	unsigned long ltran:1;
	unsigned long lclk:1;
	unsigned long lstat:2;
	unsigned long lrerr:1;
	unsigned long ldw:1;
	unsigned long srq_tx:1;
	unsigned long srq_rx:1;

	unsigned long reserved:23;
} lport_desc_t;

typedef struct _lport_t {
	volatile lport_desc_t *lcsr;

#ifdef LPORT_DEBUG
	task_t *mes_speed;
	mutex_t mes;
	timer_t timer;
	unsigned long word_send;
	unsigned long t0;
	unsigned long dt;
#endif
	task_t *task;
	void *task_stack;
	unsigned long prio;

	mutex_t irq;
	unsigned long irq_count;
#define LPORT_IRQ(port)  (port*2 + 7)

	queue_t queue;
	unsigned long word_left;
	mutex_t receive;
#define MAX_QUEUE_SIZE 512

	mem_pool_t *pool;

	unsigned long mode; 		/* Mode */
#define LPORT_SEND	1
#define LPORT_RECV	0
	unsigned long dma; 		/* Usage dma */
#define LPORT_DMA_OFF	0
#define LPORT_DMA_ON	1
	unsigned long clk; 		/* Transfer speed (CLK/4  CLK/8) */
#define LPORT_CLK_4	0
#define LPORT_CLK_8	1
	unsigned long size; 		/* Transfer size (4 or 8) */
#define LPORT_SIZE_8	0
#define LPORT_SIZE_4	1
	unsigned long port;
} lport_t;

void lport_init (lport_t *l,
	mem_pool_t *pool,
	unsigned long port,
	unsigned long mode,
	unsigned long dma,
	unsigned long clk,
	unsigned long size,
	unsigned long prio);

void lport_set_port (lport_t *l, unsigned long port);
void lport_set_mode (lport_t *l, unsigned long mode);
void lport_set_dma (lport_t *l, unsigned long dma);
void lport_set_clk (lport_t *l, unsigned long clk);
void lport_set_size (lport_t *l, unsigned long size);

void lport_reset (lport_t *l);

void lport_send_data (lport_t *l, unsigned long *data, unsigned long size);
int  lport_recv_data (lport_t *l, unsigned long *data, unsigned long size);

void lport_handle_send_irq (lport_t *l);
void lport_handle_recv_irq (lport_t *l);

void lport_kill (lport_t *l);

#if LPORT_DEBUG
void lport_debug_print (lport_t *l);
#endif

#ifdef __cplusplus
}
#endif
#endif
