/*
 * CAN driver for Milandr 1986ВЕ91 microcontroller.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
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
#include <milandr/can.h>
#include <kernel/internal.h>

#define NRBUF		16		/* number of receive buffers */
#define NTBUF		16		/* number of transmit buffers */

#define ALL_RBUFS	((1 << NRBUF) - 1)
#define ALL_TBUFS	(((1 << NTBUF) - 1) << NRBUF)

/*
 * Initialize queue.
 */
static inline __attribute__((always_inline))
void can_queue_init (can_queue_t *q)
{
	q->tail = q->queue;
	q->count = 0;
}

/*
 * Add a packet to queue.
 * Before call, a user should check that the queue is not full.
 */
static inline __attribute__((always_inline))
void can_queue_put (can_queue_t *q, can_frame_t *fr)
{
	can_frame_t *head;

	/*debug_printf ("can_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < CAN_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += CAN_QUEUE_SIZE;

	/* Put the packet in. */
	*head = *fr;
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Get a packet from queue.
 * When empty, returns {0,0,0,0};
 */
static inline __attribute__((always_inline))
void can_queue_get (can_queue_t *q, can_frame_t *fr)
{
	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + CAN_QUEUE_SIZE);
	if (q->count > 0) {
		/* Get the first packet from queue. */
		*fr = *q->tail;

		/* Advance head pointer. */
		if (--q->tail < q->queue)
			q->tail += CAN_QUEUE_SIZE;
		--q->count;
	}
}

/*
 * Check that queue is full.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_full (can_queue_t *q)
{
	return (q->count == CAN_QUEUE_SIZE);
}

/*
 * Check that queue is empty.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_empty (can_queue_t *q)
{
	return (q->count == 0);
}

/*
 * Задание скорости работы.
 */
static void can_setup_timing (can_t *c, int kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	/* Изначально разбиваем битовый интервал на 10 квантов:
	 * 1) Первый квант - всегда - сегмент синхронизации.
	 * 2) Второй квант - сегмент компенсации задержки распространения (PSEG).
	 * 3) Четыре кванта - сегмент фазы 1 (SEG1).
	 * 4) Четыре кванта - сегмент фазы 2 (SEG2).
	 * Такое разбиение совместно с SJW = 4 обеспечивает наибольшее возможное расхождение часов устройств
	 * (так написано в рекомендациях BOSCH)
	 */
	/* Рассчитываем значение делителя частоты исходя из того, что битовый интервал разбит на 10 квантов */
	unsigned brp = (KHZ / kbitsec / 10) - 1;
	/* Из-за дискретности brp, его значение может быть слишком грубым.
	 * Посчитаем, сколько реально квантов получится при таком brp?
         */
	unsigned nq = KHZ / (brp + 1) / kbitsec;
	/* Если квантов больше, чем 10, то за счет разницы увеличим сегмент компенсации
	 * задержки распространения. Тем самым "подстроимся" под необходимую частоту
	 */
	unsigned pseg = 1 + (nq - 10);
	c->kbitsec = kbitsec;
	
    reg->BITTMNG = /*CAN_BITTMNG_SB |*/ CAN_BITTMNG_SJW (4) |
            CAN_BITTMNG_SEG2 (1) | CAN_BITTMNG_SEG1 (7) |
            CAN_BITTMNG_PSEG (pseg) | CAN_BITTMNG_BRP (brp);

/*
	reg->BITTMNG = CAN_BITTMNG_SB | CAN_BITTMNG_SJW (4) |
		CAN_BITTMNG_SEG2 (4) | CAN_BITTMNG_SEG1 (4) |
		CAN_BITTMNG_PSEG (pseg) | CAN_BITTMNG_BRP (brp);
*/
/*
	reg->BITTMNG = CAN_BITTMNG_SB | CAN_BITTMNG_SJW (3) |
		CAN_BITTMNG_SEG2 (7) | CAN_BITTMNG_SEG1 (7) |
		CAN_BITTMNG_PSEG (2) | CAN_BITTMNG_BRP (3);
*/

//debug_printf ("can: %u (%u) kbit/sec, brp=%u, BITTMNG = %08x\n", kbitsec, KHZ / (brp + 1) / nq, brp, reg->BITTMNG);
}

/*
 * Инициализация всех регистров контроллера CAN.
 */
static void can_setup (can_t *c, int kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	if (c->port == 0) {
		/* Включаем тактирование порта CAN1, PORTC. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN1;

		milandr_init_pin (CAN1_RX_GPIO, PORT(CAN1_RX), PIN(CAN1_RX), CAN1_RX_FUNC);
		milandr_init_pin (CAN1_TX_GPIO, PORT(CAN1_TX), PIN(CAN1_TX), CAN1_TX_FUNC);

		/* Разрешение тактовой частоты на CAN1, источник HCLK. */
		ARM_RSTCLK->CAN_CLOCK = (ARM_RSTCLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG1(7)) |
			ARM_CAN_CLOCK_EN1 | ARM_CAN_CLOCK_BRG1(0);
	} else {
		/* Включаем тактирование порта CAN2, PORTD. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN2;

		milandr_init_pin (CAN2_RX_GPIO, PORT(CAN2_RX), PIN(CAN2_RX), CAN2_RX_FUNC);
		milandr_init_pin (CAN2_TX_GPIO, PORT(CAN2_TX), PIN(CAN2_TX), CAN2_TX_FUNC);

		/* Разрешение тактовой частоты на CAN2, источник HCLK. */
		ARM_RSTCLK->CAN_CLOCK = (ARM_RSTCLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG2(7)) |
			ARM_CAN_CLOCK_EN2 | ARM_CAN_CLOCK_BRG2(0);
	}
	reg->CONTROL = 0;
	if (c->loop)
		reg->CONTROL |= CAN_CONTROL_STM |
			CAN_CONTROL_SAP | CAN_CONTROL_ROP;
	can_setup_timing (c, kbitsec);

	unsigned i;
	for (i=0; i<32; i++) {
		reg->BUF_CON[i] = CAN_BUF_CON_EN;
		reg->BUF[i].ID = 0;
		reg->BUF[i].DLC = CAN_DLC_LEN(8) | CAN_DLC_R1 | CAN_DLC_SSR;
		reg->BUF[i].DATAL = 0;
		reg->BUF[i].DATAH = 0;
		reg->MASK[i].MASK = 0;
		reg->MASK[i].FILTER = 0;

		/* Первые 16 буферов используются для приёма. */
		if (i < NRBUF) {
			reg->BUF_CON[i] |= CAN_BUF_CON_RX_ON;
			if (i == NRBUF-1) {
				/* 16-й буфер служит для обнаружения потерь. */
				reg->BUF_CON[i] |= CAN_BUF_CON_OVER_EN;
			}
		}
	}

	/* Максимальное значение счётчика ошибок. */
	reg->OVER = 255;

	/* Разрешение прерываний от приёмных буферов. */
	reg->INT_RX = ALL_RBUFS;
	reg->INT_TX = 0;
	reg->INT_EN = CAN_INT_EN_RX | CAN_INT_EN_TX | CAN_INT_EN_GLB;
	reg->STATUS = 0;
	reg->CONTROL |= CAN_CONTROL_EN;
}

/*
 * Запись пакета в передающий буфер.
 */
static int transmit_enqueue (can_t *c, const can_frame_t *fr)
{
//debug_printf ("snd frame, id = %08X, d0 = %08X, d1 = %08X\n", fr->id, fr->data[0], fr->data[1]);
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	/* Проверяем, что есть свободный буфер для передачи. */
	int i = 32 - arm_count_leading_zeroes (~(reg->TX & ALL_TBUFS));
	if (i < NRBUF || i >= NRBUF + NTBUF)
		return 0;

	/* Нашли свободный буфер. */
	if (reg->BUF_CON[i] & CAN_BUF_CON_TX_REQ) {
		/* Такого не может быть. */
		debug_printf ("can_output: tx buffer busy\n");
		return 0;
	}

	CAN_BUF_t *buf = &reg->BUF[i];
	if (fr->dlc & CAN_DLC_IDE) {
		/* Расширенный формат кадра */
		buf->ID = fr->id & (CAN_ID_EID_MASK | CAN_ID_SID_MASK);
		buf->DLC = CAN_DLC_LEN (fr->dlc) |
			CAN_DLC_IDE | CAN_DLC_SSR /*| CAN_DLC_R1*/;
	} else {
		/* Стандартный формат кадра */
		buf->ID = fr->id & CAN_ID_SID_MASK;
		buf->DLC = CAN_DLC_LEN (fr->dlc);
	}
	buf->DATAL = fr->data[0];
	buf->DATAH = fr->data[1];
	reg->BUF_CON[i] |= CAN_BUF_CON_TX_REQ;
	c->out_packets++;

	/* Разрешение прерывания от передающего буфера. */
	reg->INT_TX |= 1 << i;

	return 1;
}

/*
 * Установка скорости интерфейса CAN.
 */
void can_set_speed (can_t *c, unsigned kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	reg->CONTROL &= ~CAN_CONTROL_EN;
	can_setup_timing (c, kbitsec);
	reg->CONTROL |= CAN_CONTROL_EN;
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

/*
 * Управление режимом шлейфа.
 */
void can_set_loop (can_t *c, int on)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	c->loop = on;
	reg->CONTROL = 0;
	if (c->loop)
		reg->CONTROL |= CAN_CONTROL_STM |
			CAN_CONTROL_SAP | CAN_CONTROL_ROP;
	reg->CONTROL |= CAN_CONTROL_EN;
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
int can_output (can_t *c, const can_frame_t *fr)
{
#ifndef CAN_NO_MUTEX
	if (c->flags & POLLONLY) {
		return transmit_enqueue (c, fr);
	} else {
		mutex_lock (&c->lock);
		while (! transmit_enqueue (c, fr)) {
			/* Ждём появления места в буфере устройства. */
			mutex_wait (&c->lock);
		}
		mutex_unlock (&c->lock);
		return 1;
	}
#else
	return transmit_enqueue (c, fr);
#endif
}

/*
 * Fetch received frame.
 */
void can_input (can_t *c, can_frame_t *fr)
{
#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	while (can_queue_is_empty (&c->inq)) {
		/* Ждём приёма пакета. */
		mutex_wait (&c->lock);
	}
	can_queue_get (&c->inq, fr);
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

static int can_get_next_frame (can_t *c, can_frame_t *fr)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	/* Есть ли буферы с принятым сообщением. */
	if (reg->STATUS & CAN_STATUS_RX_READY) {
		unsigned i;
		reg->STATUS = 0;
		for (i=0; i<NRBUF; i++) {
			unsigned bufcon = reg->BUF_CON[i];
			if (! (bufcon & CAN_BUF_CON_RX_FULL))
				continue;
			if (bufcon & CAN_BUF_CON_OVER_WR) {
				/* Сообщение перезаписано */
				c->in_discards++;
				reg->BUF_CON[i] = bufcon &
					~(CAN_BUF_CON_RX_FULL |
					CAN_BUF_CON_OVER_WR);
				continue;
			}
			CAN_BUF_t *buf = &reg->BUF[i];
			fr->id = buf->ID;
			if (fr->id == 0)
				fr->id = buf->ID;
			if (fr->id == 0)
				fr->id = buf->ID;
			fr->dlc = buf->DLC;
			if (fr->dlc == 0)
				fr->dlc = buf->DLC;
			if (fr->dlc == 0)
				fr->dlc = buf->DLC;
			fr->data[0] = buf->DATAL;
			if (fr->data[0] == 0)
				fr->data[0] = buf->DATAL;
			if (fr->data[0] == 0)
				fr->data[0] = buf->DATAL;
			fr->data[1] = buf->DATAH;
			if (fr->data[1] == 0)
				fr->data[1] = buf->DATAH;
			if (fr->data[1] == 0)
				fr->data[1] = buf->DATAH;
			reg->BUF_CON[i] = bufcon & ~CAN_BUF_CON_RX_FULL;

			if (CAN_DLC_LEN (fr->dlc) > 8) {
				c->in_errors++;
				continue;
			}
			/* Пакет успешно принят. */
			c->in_packets++;
//debug_printf ("rcv frame, id = %08X, d0 = %08X, d1 = %08X\n", fr->id, fr->data[0], fr->data[1]);
			return 1;
		}
	}

	return 0;
}

int can_poll (can_t *c, can_frame_t *fr)
{
	int res;

#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif

	res = can_get_next_frame (c, fr);
	
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif

	return res;
}

/*
 * Sets given channel disabled (channel is enabled after can_init())
 */
void can_stop (can_t *c)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;
#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	reg->CONTROL &= ~CAN_CONTROL_EN;
	reg->STATUS = 0;
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

/*
 * Sets given channel enabled (needed to call only after can_stop())
 */
void can_start (can_t *c)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;
#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	reg->CONTROL |= CAN_CONTROL_EN;
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

void can_set_filter (can_t *c, unsigned mask, unsigned pattern)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;
	int i;

#ifndef CAN_NO_MUTEX
	mutex_lock (&c->lock);
#endif
	for (i = 0; i < NRBUF; ++i) {
		reg->MASK[i].MASK = mask;
		reg->MASK[i].FILTER = pattern;
	}
#ifndef CAN_NO_MUTEX
	mutex_unlock (&c->lock);
#endif
}

static bool_t can_handle_interrupt (void *arg)
{
	can_t *c = (can_t *)arg;
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;
	can_frame_t fr;

//debug_printf ("can interrupt: STATUS = %08x\n", status);
#if 0
	c->rec = CAN_STATUS_REC (status);
	if (status & CAN_STATUS_REC8)
		c->rec += 256;

	c->tec = CAN_STATUS_TEC (status);
	if (status & CAN_STATUS_TEC8)
		c->tec += 256;

	if (status & CAN_STATUS_BUS_OFF) {
		/* Шина CAN отвалилась, требуется перезапуск. */
		debug_printf ("can interrupt: BUS OFF\n");
	}
	if (status & CAN_STATUS_ID_LOWER) {
		/* При передаче был проигран арбитраж */
		c->out_collisions++;
	}
	if (status & CAN_STATUS_ERR_ACK) {
		/* Ошибка подтверждения приема */
		c->out_nacks++;
	}
	if (status & CAN_STATUS_ERR_FRAME) {
		/* Ошибка формата принимаемого пакета */
		c->in_frame_errors++;
	}
	if (status & CAN_STATUS_ERR_CRC) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_crc_errors++;
	}
	if (status & CAN_STATUS_ERR_BITSTUFF) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_bitstuff_errors++;
	}
	if (status & CAN_STATUS_ERR_BIT) {
		/* Ошибка передаваемых битов пакета */
		c->out_bit_errors++;
	}
#endif
	if (can_get_next_frame (c, &fr)) {
		if (can_queue_is_full (&c->inq))
			c->in_discards++;
		else
			can_queue_put (&c->inq, &fr);
	}

	/* Есть ли закончившие передающие буферы. */
	if (reg->STATUS & CAN_STATUS_TX_READY) {
		/* Оставляем маску только для активных буферов. */
		reg->INT_TX = ~reg->TX & ALL_TBUFS;
	}

	arch_intr_allow (c->port);

	return 0;
}

/*
 * Set up the CAN driver.
 */
void can_init (can_t *c, int port, unsigned kbitsec, unsigned flags)
{
	c->port = port;
	c->flags = flags;
	can_setup (c, kbitsec);

	int irq = (port == 0) ? CAN1_IRQn : CAN2_IRQn;
#ifndef CAN_NO_MUTEX
	if (! (flags & POLLONLY)) {
		can_queue_init (&c->inq);
		//mutex_lock_irq (&c->lock, irq, can_handle_interrupt, c);
		//mutex_unlock (&c->lock);
		mutex_attach_irq (&c->lock, irq, can_handle_interrupt, c);
	}
#endif
}
