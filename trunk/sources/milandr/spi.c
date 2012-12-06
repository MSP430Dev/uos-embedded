/*
 * SPI driver for Milandr 1986ВЕ91 microcontroller.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *               2011 Dmitry Podkhvatilin, <vatilin@gmail.com>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <kernel/internal.h>

/*
 * Номера прерываний от интерфейсов SSP.
 */
#define IRQ_SSP1	8
#define IRQ_SSP2	20
#define IRQ_SSP3	19

/*
 * Initialize queue.
 */
static inline __attribute__((always_inline))
void spi_queue_init (spi_queue_t *q)
{
	q->tail = q->queue;
	q->count = 0;
}

/*
 * Add a packet to queue.
 * Before call, a user should check that the queue is not full.
 */
static inline __attribute__((always_inline))
void spi_queue_put (spi_queue_t *q, unsigned short word)
{
	unsigned short *head;

	/*debug_printf ("spi_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < SPI_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += SPI_QUEUE_SIZE;

	/* Put the packet in. */
	*head = word;
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Get a packet from queue.
 * When empty, returns 0.
 */
static inline __attribute__((always_inline))
unsigned short spi_queue_get (spi_queue_t *q)
{
	unsigned word = 0;
	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + SPI_QUEUE_SIZE);
	if (q->count > 0) {
		/* Get the first packet from queue. */
		word = *q->tail;

		/* Advance head pointer. */
		if (--q->tail < q->queue)
			q->tail += SPI_QUEUE_SIZE;
		--q->count;
	}
	return word;
}

/*
 * Check that queue is full.
 */
static inline __attribute__((always_inline))
bool_t spi_queue_is_full (spi_queue_t *q)
{
	return (q->count == SPI_QUEUE_SIZE);
}

/*
 * Check that queue is empty.
 */
static inline __attribute__((always_inline))
bool_t spi_queue_is_empty (spi_queue_t *q)
{
	return (q->count == 0);
}

/*
 * Инициализация внешних сигналов SSP1.
 */
static void spi_setup_ssp1 ()
{
	/* Включаем тактирование порта SSP1. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP1;

	milandr_init_pin (SSP1_RXD_GPIO, PORT(SSP1_RXD), PIN(SSP1_RXD), SSP1_RXD_FUNC);
	milandr_init_pin (SSP1_TXD_GPIO, PORT(SSP1_TXD), PIN(SSP1_TXD), SSP1_TXD_FUNC);
	milandr_init_pin (SSP1_FSS_GPIO, PORT(SSP1_FSS), PIN(SSP1_FSS), SSP1_FSS_FUNC);
	milandr_init_pin (SSP1_CLK_GPIO, PORT(SSP1_CLK), PIN(SSP1_CLK), SSP1_CLK_FUNC);

	/* Разрешение тактовой частоты на SSP1, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
		ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
}

/*
 * Инициализация внешних сигналов SSP2.
 */
static void spi_setup_ssp2 ()
{
	/* Включаем тактирование порта SSP2. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP2;

	milandr_init_pin (SSP2_RXD_GPIO, PORT(SSP2_RXD), PIN(SSP2_RXD), SSP2_RXD_FUNC);
	milandr_init_pin (SSP2_TXD_GPIO, PORT(SSP2_TXD), PIN(SSP2_TXD), SSP2_TXD_FUNC);
	milandr_init_pin (SSP2_FSS_GPIO, PORT(SSP2_FSS), PIN(SSP2_FSS), SSP2_FSS_FUNC);
	milandr_init_pin (SSP2_CLK_GPIO, PORT(SSP2_CLK), PIN(SSP2_CLK), SSP2_CLK_FUNC);


	/* Разрешение тактовой частоты на SSP2, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
		ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
}

#ifdef ARM_1986BE1
/*
 * Инициализация внешних сигналов SSP3.
 */
static void spi_setup_ssp3 ()
{
	/* Включаем тактирование порта SSP3. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP3;

	milandr_init_pin (SSP3_RXD_GPIO, PORT(SSP3_RXD), PIN(SSP3_RXD), SSP3_RXD_FUNC);
	milandr_init_pin (SSP3_TXD_GPIO, PORT(SSP3_TXD), PIN(SSP3_TXD), SSP3_TXD_FUNC);
	milandr_init_pin (SSP3_FSS_GPIO, PORT(SSP3_FSS), PIN(SSP3_FSS), SSP3_FSS_FUNC);
	milandr_init_pin (SSP3_CLK_GPIO, PORT(SSP3_CLK), PIN(SSP3_CLK), SSP3_CLK_FUNC);


	/* Разрешение тактовой частоты на SSP2, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG3(7)) |
		ARM_SSP_CLOCK_EN3 | ARM_SSP_CLOCK_BRG3(0);
}
#endif

/*
 * Transmit the word.
 */
void spi_output (spi_t *c, unsigned short word)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	mutex_lock (&c->lock);
	while (! (reg->SR & ARM_SSP_SR_TNF)) {
		/* Ждём появления места в FIFO передатчика. */
		mutex_wait (&c->lock);
	}
	reg->DR = word;
	arch_intr_allow (c->irq);
	c->out_packets++;
	mutex_unlock (&c->lock);
}

void spi_output_block (spi_t *c, unsigned short *data, int count)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;
	int i;

	mutex_lock (&c->lock);
	reg->CR1 &= ~ARM_SSP_CR1_SSE;
	for (i = 0; i < count; ++i) {
		while (! (reg->SR & ARM_SSP_SR_TNF)) {
			/* Ждём появления места в FIFO передатчика. */
			mutex_wait (&c->lock);
		}
		reg->DR = *(data + i);	
		c->out_packets++;
	}
	reg->CR1 |= ARM_SSP_CR1_SSE;
	arch_intr_allow (c->irq);
	mutex_unlock (&c->lock);
}

/*
 * Извлекаем данные из приёмного FIFO.
 */
static int receive_data (spi_t *c)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;
	unsigned sr = reg->SR;
	int nwords = 0;

	while (sr & ARM_SSP_SR_RNE) {
		unsigned short word = reg->DR;
		nwords++;
//debug_printf ("<%04x> ", word);
		sr = reg->SR;

		if (spi_queue_is_full (&c->inq)) {
			c->in_discards++;
			continue;
		}
		/* Пакет успешно принят. */
		c->in_packets++;
		spi_queue_put (&c->inq, word);
	}
	return nwords;
}

/*
 * Fetch received word.
 * Returns 0 when no data is avaiable.
 */
int spi_input (spi_t *c, unsigned short *word)
{
	int reply = 0;

	mutex_lock (&c->lock);
	if (! spi_queue_is_empty (&c->inq)) {
		*word = spi_queue_get (&c->inq);
		reply = 1;
	}
	mutex_unlock (&c->lock);
	return reply;
}

/*
 * Wait for word received.
 */
void spi_input_wait (spi_t *c, unsigned short *word)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;
	mutex_lock (&c->lock);
	while (spi_queue_is_empty (&c->inq)) {
		/* Ждём приёма пакета. */
		mutex_wait (&c->lock);
	}
	*word = spi_queue_get (&c->inq);

	mutex_unlock (&c->lock);
}

/*
 * Fast interrupt handler.
 */
static bool_t spi_handle_interrupt (void *arg)
{
	spi_t *c = (spi_t*) arg;
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	c->interrupts++;
	receive_data (c);
	arch_intr_allow (c->irq);
	if (!c->master) {
		reg->CR1 = ARM_SSP_CR1_MS;
		reg->CR1 |= ARM_SSP_CR1_SSE;
	}
	return 0;
}

/*
 * Set up the SPI driver.
 */
void spi_init (spi_t *c, int port, int bits_per_word, unsigned nsec_per_bit, unsigned mode)
{
	/* Инициализация структуры данных драйвера. */
	c->port = port;
	c->master = (nsec_per_bit > 0);
	spi_queue_init (&c->inq);

	/* Выбор соответствующего интерфейса SSP и
	 * установка внешних сигналов. */
	SSP_t *reg;
	if (c->port == 0) {
		spi_setup_ssp1 ();
		reg = ARM_SSP1;
		c->irq = IRQ_SSP1;
	} else if (c->port == 1) {
		spi_setup_ssp2 ();
		reg = ARM_SSP2;
		c->irq = IRQ_SSP2;
	} 
#ifdef ARM_1986BE1
	else {
		spi_setup_ssp3 ();
		reg = ARM_SSP3;
		c->irq = IRQ_SSP3;
    }
#endif

	/* Инициализация всех регистров данного интерфейса SSP.
	 * Ловим прерывания от приёмника. */
	reg->CR0 = ARM_SSP_CR0_FRF_SPI | ARM_SSP_CR0_DSS (bits_per_word) | mode;
	if (c->master) {
		/* Режим master. */
		unsigned divisor = (KHZ * nsec_per_bit + 1999999) / 2000000;
		reg->CR0 |= ARM_SSP_CR0_SCR (divisor - 1);
		reg->CR1 = 0;
		reg->CPSR = 2;
		c->kbps = (KHZ / divisor + 1) / 2;
	} else {
		/* Режим slave.
		 * Максимальная частота равна KHZ/12. */
		reg->CR1 = ARM_SSP_CR1_MS;
		reg->CPSR = 12;
		c->kbps = (KHZ + 6) / 12;
	}
	reg->DMACR = 0;
	reg->IM = ARM_SSP_IM_RX | ARM_SSP_IM_RT;
	reg->CR1 |= ARM_SSP_CR1_SSE;

	/* Подключение к нужному номеру прерывания. */
	mutex_lock_irq (&c->lock, c->irq, spi_handle_interrupt, c);
	mutex_unlock (&c->lock);
}
