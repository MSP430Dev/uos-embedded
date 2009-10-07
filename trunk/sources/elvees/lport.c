/*
 * LPORT API on Elvees MC-24EM board.
 *
 * Authors: Kirill Salyamov (ksalyamov@elvees.com), Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 24.08.2009
 * Last modify: 07.10.2009
 */
#include <elvees/lport.h>

ARRAY (stack_irq, 1000);

void
lport_send_data (lport_t *l, unsigned long *data, unsigned long size)
{
	unsigned long idx, crc32, magic = LPORT_MAGIC;

	if (size > MAX_QUEUE_SIZE) {
		debug_printf ("max queue size = %u\n", MAX_QUEUE_SIZE);
		return;
	}

	if (l->dma == LPORT_DMA_OFF) {
		mutex_lock (&l->irq);

		queue_free (&l->queue);

		for (idx = 0; idx < size; idx++)
			queue_push (&l->queue, &data[idx]);

		crc32 = queue_compute_crc32 (&l->queue, 0);

		queue_push_first (&l->queue, &crc32);
		queue_push_first (&l->queue, &size);
		queue_push_first (&l->queue, &magic);

		mutex_unlock (&l->irq);

		l->lcsr->len = 1;
	}
	else {
		MC_IR_LPCH  (l->port) = (unsigned long)data & 0x1FFFFFFC;
		MC_OR_LPCH  (l->port) = 1;
		MC_Y_LPCH   (l->port) = 0;
		MC_CP_LPCH  (l->port) = 0;
		MC_CSR_LPCH (l->port) = (size << 16);

		MC_CSR_LPCH (l->port) |= 1;
	}
}

int
lport_recv_data (lport_t *l, unsigned long *data, unsigned long size)
{
	int count = 0;
	unsigned char *buf = 0;

	if (l->dma == LPORT_DMA_OFF) {

		if (queue_is_empty (&l->queue))
			buf = (unsigned char*)mutex_wait (&l->receive);

		mutex_lock (&l->irq);

		while ((!queue_is_empty (&l->queue)) && (count < size)) {
			data [count++] = queue_pop (&l->queue);
		}

		mutex_unlock (&l->irq);

		l->lcsr->len = 1;
	}
	else {
		MC_IR_LPCH  (l->port) = (unsigned long)data & 0x1FFFFFFC;
		MC_OR_LPCH  (l->port) = 1;
		MC_Y_LPCH   (l->port) = 0;
		MC_CP_LPCH  (l->port) = 0;
		MC_CSR_LPCH (l->port) = (size << 16);

		MC_CSR_LPCH (l->port) |= 1;
	}

	return count;
}

void
lport_set_mode (lport_t *l, unsigned long mode)
{
	lport_kill (l);

	l->mode = mode;

	lport_reset (l);
}

void
lport_set_dma (lport_t *l, unsigned long dma)
{
	l->dma = dma;
}

void
lport_set_clk (lport_t *l, unsigned long clk)
{
	l->clk = clk;

	l->lcsr->lclk  = clk;
}

void
lport_set_size (lport_t *l, unsigned long size)
{
	l->size = size;

	l->lcsr->ldw   = size;
}

static void
lport_irq (void *arg)
{
	lport_t *l = (lport_t*)arg;
		
	mutex_lock_irq (&l->irq, LPORT_IRQ(l->port), 0, 0);
	mutex_lock (&l->receive);

	for(;;) {
		mutex_wait (&l->irq);

		l->irq_count++;

		if (l->mode == LPORT_SEND)
			lport_handle_send_irq (l);

		if (l->mode == LPORT_RECV)
			lport_handle_recv_irq (l);
	}
}

static void
lport_mes_speed (void *arg)
{
	lport_t *l = (lport_t*)arg;
	unsigned char *buf = 0;

	for(;;) {
		buf = (unsigned char*)mutex_wait (&l->mes);

		mutex_lock (&l->irq);

		debug_printf ("speed = %u words/sec (%u bytes/sec)\n", 
			(l->word_send/(l->dt/1000)),
			(l->word_send/(l->dt/1000) * (4)));

		l->word_send = 0;

		mutex_unlock (&l->irq);
	}
}

void
lport_handle_recv_irq (lport_t *l)
{
	unsigned long free = 0,
		recv = 0,
		sz = 0,
		crc32 = 0,
		idx = 0,
		magic = 0,
		queue_sz = 0;

	mutex_lock (&l->irq);

	debug_printf ("begin recv irq\n");

	while (l->lcsr->lstat == 0);
	magic = MC_LRX (l->port);

	if (magic == LPORT_MAGIC) {
		while (l->lcsr->lstat == 0);
		sz = MC_LRX (l->port);

		while (l->lcsr->lstat == 0);
		crc32 = MC_LRX (l->port);

		l->word_left = 0;
	}
	else {
		sz = l->word_left;

		if (sz == 0) {
			debug_printf ("\nWrong queue!\n");
			l->lcsr->len = 0;

			return;
		}
	}

	queue_sz = queue_size (&l->queue);

	for (idx = 0; idx < sz; idx++) {
		while (l->lcsr->lstat == 0);
		recv = MC_LRX (l->port);

		free = queue_push (&l->queue, &recv);

		if (free == 0) {
			l->word_left = sz - idx + 1;
			l->lcsr->len = 0;
#if LPORT_DEBUG
			debug_printf ("\n...queue is full...\n");
#endif
			mutex_signal (&l->receive, "recv_piece");
			mutex_unlock (&l->irq);

			return;
		}
	}

	if ((crc32 != 0)&&(crc32 != queue_compute_crc32 (&l->queue, sz))) {
#if LPORT_DEBUG
		debug_printf ("\nData was corrupted:\n");
		debug_printf ("size    = %u\n", sz);
		debug_printf ("crc32   = %p\n", crc32);
		debug_printf ("l->crc32= %p\n", queue_compute_crc32 (&l->queue, sz));
		queue_print_crc32 (&l->queue, sz);
		debug_getchar ();
		queue_print (&l->queue);
#endif
		l->lcsr->len = 0;

		return;
	}

	mutex_signal (&l->receive, "recv_all");
	debug_printf ("end recv irq\n");
	mutex_unlock (&l->irq);
}

void
lport_handle_send_irq (lport_t *l)
{
	unsigned long send = 0;

	if (queue_is_empty (&l->queue)) {
		l->lcsr->len = 0;
		return;
	}
#if LPORT_DEBUG
	l->dt = 10000;
	if (l->word_send == 0)
		l->t0 = timer_milliseconds (&l->timer);
#endif	
	mutex_lock (&l->irq);
	debug_printf ("begin send irq\n");
#if LPORT_DEBUG
	l->word_send += queue_size (&l->queue);
#endif
	while (!queue_is_empty (&l->queue)) {
		send = queue_pop (&l->queue);
		MC_LTX (l->port) = send;

		while (l->lcsr->lstat != 0);
	}
#if LPORT_DEBUG
	if (timer_milliseconds (&l->timer) - l->t0 >= l->dt) {
		mutex_signal (&l->mes, "mes");
	}
#endif
	debug_printf ("end recv irq\n");
	mutex_unlock (&l->irq);
}

void lport_reset (lport_t *l)
{
	l->lcsr = (volatile lport_desc_t*)&MC_LCSR (l->port);

	memset ((void*)l->lcsr, 0, sizeof (lport_desc_t));

	l->lcsr->ltran = l->mode;
	l->lcsr->lclk  = l->clk;
	l->lcsr->ldw   = l->size;
	l->lcsr->len   = 1;

	l->irq_count = 0;
}

void
lport_set_port (lport_t *l, unsigned long port)
{
	lport_kill (l);

	l->port = port;

	lport_reset (l);

	task_delete (l->task, 0);
	l->task = task_create (lport_irq, l, "lport_irq", l->prio,
		l->task_stack, 0x200);
}

void
lport_kill (lport_t *l)
{
	mutex_unlock_irq (&l->irq);

	memset ((void*)l->lcsr, 0, sizeof (lport_desc_t));

	task_delete (l->task, 0);
	mem_free (l->task_stack);

	queue_kill (&l->queue);
}

void
lport_init (lport_t *l,
	mem_pool_t *pool,
	unsigned long port,
	unsigned long mode,
	unsigned long dma,
	unsigned long clk,
	unsigned long size,
	unsigned long prio) 
{
	char name[10] = "lpi_";
	void *mem = 0;

	l->port = port;
	l->mode = mode;
	l->dma  = dma;
	l->clk  = clk;
	l->size = size;
	l->pool = pool;
	l->prio = prio;

	queue_init (&l->queue, l->pool, MAX_QUEUE_SIZE + 3);

	lport_reset (l);

	name[4] = (char)(l->port + '0');
	name[5] = '\0';

	l->task_stack = mem_alloc (l->pool, 0x200);
	
	l->task = task_create (lport_irq, l, name, l->prio,
		l->task_stack, 0x200);

#if LPORT_DEBUG
	mem = mem_alloc (l->pool, 0x200);
	
	l->mes_speed = task_create (lport_mes_speed, l, "mes speed", 120,
		mem, 0x200);

	timer_init (&l->timer, KHZ, 10);

	mutex_lock (&l->mes);
#endif
}

#if LPORT_DEBUG
void lport_debug_print (lport_t *l)
{
	debug_puts ("\n*****LPORT***CONFIG*****\n\n");

	debug_printf ("1. Active port: LP%u\n", l->port);

	if (l->mode == LPORT_SEND)
		debug_puts ("2. Mode: send\n");
	else
		debug_puts ("2. Mode: receive\n");

	if (l->dma == LPORT_DMA_ON)
		debug_puts ("3. DMA: on\n");
	else
		debug_puts ("3. DMA: off\n");

	if (l->clk == LPORT_CLK_4)
		debug_puts ("4. Transfer speed: CLK/4\n");
	else
		debug_puts ("4. Transfer speed: CLK/8\n");

	if (l->size == LPORT_SIZE_4)
		debug_puts ("5. Transfer size: 4\n");
	else
		debug_puts ("5. Transfer size: 8\n");

	debug_puts ("\n*****LPORT***CONFIG*****\n\n");
}
#endif
