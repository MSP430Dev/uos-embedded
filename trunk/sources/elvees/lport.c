/*
 * LPORT API on Elvees MC-24EM board.
 *
 * Authors: Kirill Salyamov (ksalyamov@elvees.com), Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 24.08.2009
 * Last modify: 27.08.2009
 */
#include <elvees/lport.h>

#define CRAM_MEM_START	(uint32_t)(&_end)
#define CRAM_MEM_SIZE	(2*sizeof (dma_chain_t))
#define CRAM_MEM_END    (CRAM_MEM_START + CRAM_MEM_SIZE)

void
lport_init (lport_t *l, unsigned long pair, unsigned long dir, unsigned long dma, unsigned long clk, unsigned long size) 
{
	l->pair = pair;
	l->dir  = dir;
	l->dma  = dma;
	l->clk  = clk;
	l->size = size;

	if (l->pair == LPORT_PAIR_01) {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 0;
			l->recv = 1;
		}
		else {
			l->send = 1;
			l->recv = 0;
		}
	}
	else {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 2;
			l->recv = 3;
		}
		else {
			l->send = 3;
			l->recv = 2;
		}
	}

	l->lcsr_recv = (volatile lport_desc_t*)&MC_LCSR (l->recv);
	l->lcsr_send = (volatile lport_desc_t*)&MC_LCSR (l->send);

	l->lcsr_recv->ltran = 0;
	l->lcsr_recv->lclk  = l->clk;
	l->lcsr_recv->ldw   = l->size;
	l->lcsr_recv->len   = 1;

	l->lcsr_send->ltran = 1;
	l->lcsr_send->lclk  = l->clk;
	l->lcsr_send->ldw   = l->size;
	l->lcsr_send->len   = 1;
}

void
lport_send_word (lport_t *l, unsigned long *word)
{
	if (l->dma == 0) {
		MC_LTX (l->send) = *word;

		while (l->lcsr_send->lstat != 0);
	}
	else {
		MC_IR_LPCH  (l->send) = (unsigned long)word & 0x1FFFFFFC;
		MC_OR_LPCH  (l->send) = 1;
		MC_Y_LPCH   (l->send) = 0;
		MC_CP_LPCH  (l->send) = 0;
		MC_CSR_LPCH (l->send) = (1 << 16);

		MC_CSR_LPCH (l->send) |= 1;
	}
}

void
lport_recv_word (lport_t *l, unsigned long *word)
{
	if (l->dma == 0) {
		while (l->lcsr_recv->lstat == 0);

		*word = MC_LRX (l->recv);
	}
	else {
		MC_IR_LPCH  (l->recv) = (unsigned long)word & 0x1FFFFFFC;
		MC_OR_LPCH  (l->recv) = 1;
		MC_Y_LPCH   (l->recv) = 0;
		MC_CP_LPCH  (l->recv) = 0;
		MC_CSR_LPCH (l->recv) = (1 << 16);

		MC_CSR_LPCH (l->recv) |= 1;
	}
}

void
lport_send_data (lport_t *l, unsigned long *word, unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx < size; idx++)
		lport_send_word (l, word + idx);
}

void
lport_recv_data (lport_t *l, unsigned long *word, unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx < size; idx++)
		lport_recv_word (l, word + idx);
}

void
lport_set_pair (lport_t *l, unsigned long pair)
{
	l->pair = pair;

	if (l->pair == LPORT_PAIR_01) {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 0;
			l->recv = 1;
		}
		else {
			l->send = 1;
			l->recv = 0;
		}
	}
	else {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 2;
			l->recv = 3;
		}
		else {
			l->send = 3;
			l->recv = 2;
		}
	}

	l->lcsr_recv = (lport_desc_t*)&MC_LCSR (l->recv);
	l->lcsr_send = (lport_desc_t*)&MC_LCSR (l->send);

	memset((void*)l->lcsr_recv, 0, sizeof (lport_desc_t));
	memset((void*)l->lcsr_send, 0, sizeof (lport_desc_t));

	l->lcsr_recv->ltran = 0;
	l->lcsr_recv->lclk  = l->clk;
	l->lcsr_recv->ldw   = l->size;
	l->lcsr_recv->len   = 1;

	l->lcsr_send->ltran = 1;
	l->lcsr_send->lclk  = l->clk;
	l->lcsr_send->ldw   = l->size;
	l->lcsr_send->len   = 1;
}

void
lport_set_dir (lport_t *l, unsigned long dir)
{
	l->dir = dir;

	if (l->pair == LPORT_PAIR_01) {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 0;
			l->recv = 1;
		}
		else {
			l->send = 1;
			l->recv = 0;
		}
	}
	else {
		if (l->dir == LPORT_SEND_RECV) {
			l->send = 2;
			l->recv = 3;
		}
		else {
			l->send = 3;
			l->recv = 2;
		}
	}

	l->lcsr_recv = (lport_desc_t*)&MC_LCSR (l->recv);
	l->lcsr_send = (lport_desc_t*)&MC_LCSR (l->send);

	memset((void*)l->lcsr_recv, 0, sizeof (lport_desc_t));
	memset((void*)l->lcsr_send, 0, sizeof (lport_desc_t));

	l->lcsr_recv->ltran = 0;
	l->lcsr_recv->lclk  = l->clk;
	l->lcsr_recv->ldw   = l->size;
	l->lcsr_recv->len   = 1;

	l->lcsr_send->ltran = 1;
	l->lcsr_send->lclk  = l->clk;
	l->lcsr_send->ldw   = l->size;
	l->lcsr_send->len   = 1;
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

	l->lcsr_recv->lclk  = clk;
	l->lcsr_send->lclk  = clk;
}

void
lport_set_size (lport_t *l, unsigned long size)
{
	l->size = size;

	l->lcsr_recv->ldw   = size;
	l->lcsr_send->ldw   = size;
}

#if LPORT_DEBUG
void lport_debug_print (lport_t *l)
{
	debug_puts ("\n*****LPORT***CONFIG*****\n\n");

	if (l->pair == LPORT_PAIR_01)
		debug_puts ("1. Active pair: LP0, LP1\n");
	else
		debug_puts ("1. Active pair: LP2, LP3\n");

	if (l->pair == LPORT_PAIR_01)
		if (l->dir == LPORT_SEND_RECV)
			debug_puts ("2. Direction: LP0 => LP1\n");
		else
			debug_puts ("2. Direction: LP0 <= LP1\n");
	else
		if (l->dir == LPORT_SEND_RECV)
			debug_puts ("2. Direction: LP2 => LP3\n");
		else
			debug_puts ("2. Direction: LP2 <= LP3\n");

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
