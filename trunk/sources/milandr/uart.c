/*
 * Драйвер UART для 1986ВЕ9x.
 * ARM PrimeCell UART (PL010).
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
#include <kernel/internal.h>
#include <uart/uart.h>

#define UART_IRQ(port)	(((port)==ARM_UART1_BASE) ? 6 : 7)

/*
 * Ожидание окончания передачи данных..
 */
static void
uart_fflush (uart_t *u)
{
	UART_t *reg = (UART_t*) u->port;

	mutex_lock (&u->receiver);

	/* Проверяем, что передатчик включен. */
	if (reg->CTL & ARM_UART_CTL_TXE)
		while (u->out_first != u->out_last)
			mutex_wait (&u->receiver);

	mutex_unlock (&u->receiver);
}

/*
 * Отправка одного байта.
 */
void
uart_putchar (uart_t *u, short c)
{
	UART_t *reg = (UART_t*) u->port;
	unsigned char *newlast;

	mutex_lock (&u->receiver);

	/* Check that transmitter is enabled. */
	if (reg->CTL & ARM_UART_CTL_TXE) {
again:		newlast = u->out_last + 1;
		if (newlast >= u->out_buf + UART_OUTBUFSZ)
			newlast = u->out_buf;
		while (u->out_first == newlast)
			mutex_wait (&u->receiver);

		*u->out_last = c;
		u->out_last = newlast;
		if (! (reg->FR & ARM_UART_FR_TXFF)) {
			/* В буфере FIFO передатчика есть место. */
			reg->DR = *u->out_first++;
			if (u->out_first >= u->out_buf + UART_OUTBUFSZ)
				u->out_first = u->out_buf;
		}

		if (u->onlcr && c == '\n') {
			c = '\r';
			goto again;
		}
	}
	mutex_unlock (&u->receiver);
}

/*
 * Приём одного байта с ожиданием.
 */
unsigned short
uart_getchar (uart_t *u)
{
	unsigned char c;

	mutex_lock (&u->receiver);

	/* Wait until receive data available. */
	while (u->in_first == u->in_last)
		mutex_wait (&u->receiver);

	c = *u->in_first++;
	if (u->in_first >= u->in_buf + UART_INBUFSZ)
		u->in_first = u->in_buf;

	mutex_unlock (&u->receiver);
	return c;
}

/*
 * Просмотр первого принятого байта, без ожидания.
 * Байт остаётся в буфере и должен быть извлечён вызовом uart_getchar().
 * Если буфер пустой, возвращается -1.
 */
int
uart_peekchar (uart_t *u)
{
	int c;

	mutex_lock (&u->receiver);
	c = (u->in_first == u->in_last) ? -1 : *u->in_first;
	mutex_unlock (&u->receiver);
	return c;
}

/*
 * Быстрый обработчик прерывания.
 * Возвращает 0, если требуется послать сигнал мутексу.
 * Возвращает 1, если прерывание полностью обработано и
 * нет необходимости беспокоить мутекс.
 */
static bool_t
uart_interrupt (void *arg)
{
	uart_t *u = arg;
	UART_t *reg = (UART_t*) u->port;
	bool_t passive = 1;

	/* Приём. */
	while (! (reg->FR & ARM_UART_FR_RXFE)) {
		/* В буфере FIFO приемника есть данные. */
		unsigned c = reg->DR;

		unsigned char *newlast = u->in_last + 1;
		if (newlast >= u->in_buf + UART_INBUFSZ)
			newlast = u->in_buf;

		/* Если нет места в буфере - теряем данные. */
		if (u->in_first != newlast) {
			*u->in_last = c;
			u->in_last = newlast;
		}
		passive = 0;
	}

	/* Передача. */
	if (reg->RIS & ARM_UART_RIS_TX) {
		if (u->out_first != u->out_last) {
			/* Шлём очередной байт. */
			reg->DR = *u->out_first;
			if (++u->out_first >= u->out_buf + UART_OUTBUFSZ)
				u->out_first = u->out_buf;
		} else {
			/* Нет данных для передачи - сброс прерывания. */
			reg->ICR = ARM_UART_RIS_TX;
			passive = 0;
		}
	}
	arch_intr_allow (UART_IRQ (u->port));
	return passive;
}

/*
 * Возвращает адрес мутекса, получающего сигналы при приёме новых данных.
 */
mutex_t *
uart_receive_lock (uart_t *u)
{
	return &u->receiver;
}

/*
 * Stream-интерфейс для UART.
 */
static stream_interface_t uart_interface = {
	.putc = (void (*) (stream_t*, short))		uart_putchar,
	.getc = (unsigned short (*) (stream_t*))	uart_getchar,
	.peekc = (int (*) (stream_t*))			uart_peekchar,
	.flush = (void (*) (stream_t*))			uart_fflush,
	.receiver = (mutex_t *(*) (stream_t*))		uart_receive_lock,
};

/*
 * Инициализация UART:
 * port	- номер порта, 0 или 1
 * prio - приоритет задачи обработки прерываний
 * khz  - опорная частота, кГц
 * baud - требуемая скорость передачи данных, бит/сек
 */
void
uart_init (uart_t *u, small_uint_t port, int prio, unsigned int khz,
	unsigned long baud)
{
	u->interface = &uart_interface;
	u->in_first = u->in_last = u->in_buf;
	u->out_first = u->out_last = u->out_buf;
	u->khz = khz;
	u->onlcr = 1;
	u->port = (port == 0) ? ARM_UART1_BASE : ARM_UART2_BASE;

	/*
	 * Enable UART.
	 */
	UART_t *reg = (UART_t*) u->port;
	mutex_lock_irq (&u->receiver, UART_IRQ (u->port), uart_interrupt, u);

	/*
	 * Установка скорости передачи данных.
	 */
	reg->CTL = 0;						// останов приемопередатчика
	if (port == 0) {
		ARM_RSTCLK->UART_CLOCK &= ~(ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(7));

		/* Set pins and clock for UART1. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA;	// вкл. тактирования PORTA
		ARM_GPIOA->FUNC |= ARM_FUNC_REDEF(6) |		// переопределенная функция для
				   ARM_FUNC_REDEF(7);		// PA6(UART1_RXD) и PA7(UART1_TXD)
		ARM_GPIOA->ANALOG |= 0xC0;			// цифровые выводы
		ARM_GPIOA->PWR &= ~(ARM_PWR_MASK(6) || ARM_PWR_MASK(7));
		ARM_GPIOA->PWR |= ARM_PWR_SLOW(6) | ARM_PWR_SLOW(7);
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_UART1;	// вкл. тактирования UART1
	} else {
		/* Pins and clock for UART2 already initialized by _init_(). */
		ARM_RSTCLK->UART_CLOCK &= ~(ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(7));
	}
#ifdef SETUP_HCLK_HSI
	// HCLK (8 МГц)
	if (port == 0) {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(0);
	} else {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(0);
	}
	reg->IBRD = ARM_UART_IBRD (8000000, baud);
	reg->FBRD = ARM_UART_FBRD (8000000, baud);
#else
	// HCLK/4 (KHZ/4)
	if (port == 0) {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(2);
	} else {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(2);
	}
	reg->IBRD = ARM_UART_IBRD (khz*1000/4, baud);
	reg->FBRD = ARM_UART_FBRD (khz*1000/4, baud);
#endif
	/* Управление линией */
	reg->LCRH = ARM_UART_LCRH_WLEN8 |	// длина слова 8 бит
		   ARM_UART_LCRH_FEN;		// FIFO

	/* Пороги FIFO */
	reg->IFLS = ARM_UART_IFLS_RX_1_2 |	// приём от половины буфера
		  ARM_UART_IFLS_TX_1_2;		// передача от половины буфера

	/* Управление */
	reg->CTL = ARM_UART_CTL_RXE |		// приём разрешен
		ARM_UART_CTL_TXE;		// передача разрешена
	reg->ICR = ~0;				// сброс прерывания
	reg->DMACTL = 0;			// управление DMA
	reg->IM = ARM_UART_RIS_TX |		// прерывания от передатчика
		ARM_UART_RIS_RX |		// от приемника
		ARM_UART_RIS_RT;		// таймаут приема данных
	reg->CTL |= ARM_UART_CTL_UARTEN;	// пуск приемопередатчика

	mutex_unlock (&u->receiver);
}
