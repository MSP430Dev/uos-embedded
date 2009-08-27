/*
 * LPORT API on Elvees MC-24EM board.
 *
 * Authors: Kirill Salyamov (ksalyamov@elvees.com), Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 24.08.2009
 * Last modify: 27.08.2009
 */
#ifndef LPORT_API_H
#define LPORT_API_H
#ifdef __cplusplus
extern "C" {
#endif

#include <runtime/lib.h>
#include <mem/mem.h>

#define LPORT_DEBUG 1

#ifndef LPORT_DEBUG
#define LPORT_DEBUG 0
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
	unsigned long sqr_rx:1;

	unsigned long reserved:23;
} lport_desc_t;

typedef struct _dma_chain_t {
	unsigned long ir;
	unsigned long or;
	unsigned long y;
	unsigned long cp;
	unsigned long csr;
} dma_chain_t;

typedef struct _lport_t {
	volatile lport_desc_t *lcsr_send;
	volatile lport_desc_t *lcsr_recv;

	mem_pool_t mem_pool;

	volatile dma_chain_t *dma_chain_send;
	volatile dma_chain_t *dma_chain_recv;

	unsigned long pair; 		/* Which pair of lports; 0 - LP0,LP1; 1 - LP2,LP3; */
#define LPORT_PAIR_01	0
#define LPORT_PAIR_23	1
	unsigned long dir; 		/* Direction */
#define LPORT_SEND_RECV	0
#define LPORT_RECV_SEND	1
	unsigned long dma; 		/* Usage dma */
#define LPORT_DMA_OFF	0
#define LPORT_DMA_ON	1
	unsigned long clk; 		/* Transfer speed (CLK/4  CLK/8) */
#define LPORT_CLK_4	0
#define LPORT_CLK_8	1
	unsigned long size; 		/* Transfer size (4 or 8) */
#define LPORT_SIZE_4	0
#define LPORT_SIZE_8	1
	unsigned long recv;
	unsigned long send;
} lport_t;

void lport_init (lport_t *l, unsigned long pair, unsigned long dir, unsigned long dma, unsigned long clk, unsigned long size);

void lport_set_pair (lport_t *l, unsigned long pair);
void lport_set_dir (lport_t *l, unsigned long dir);
void lport_set_dma (lport_t *l, unsigned long dma);
void lport_set_clk (lport_t *l, unsigned long clk);
void lport_set_size (lport_t *l, unsigned long size);

void lport_send_word (lport_t *l, unsigned long *word);
void lport_recv_word (lport_t *l, unsigned long *word);
void lport_send_data (lport_t *l, unsigned long *data, unsigned long size);
void lport_recv_data (lport_t *l, unsigned long *data, unsigned long size);

#if LPORT_DEBUG
void lport_debug_print (lport_t *l);
#endif

#ifdef __cplusplus
}
#endif
#endif
