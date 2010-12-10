/*
 * UART driver for external 3-channel controller.
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
#include <stream/stream.h>

/**\~english
 * Size of input buffer.
 *
 * \~russian
 * Размер буфера ввода.
 */
#define UART_INBUFSZ	256

/**\~english
 * Size of output buffer.
 *
 * \~russian
 * Размер буфера вывода.
 */
#define UART_OUTBUFSZ	16

/**\~english
 * Data structure of UART driver.
 *
 * \~russian
 * Структура данных для драйвера UART.
 */
typedef struct _uartx_t {
	stream_interface_t *interface;
	mutex_t transmitter;
	mutex_t receiver;
	unsigned int khz;
	unsigned char out_buf [UART_OUTBUFSZ];
	unsigned char *out_first, *out_last;
	unsigned char in_buf [UART_INBUFSZ];
	unsigned char *in_first, *in_last;

	unsigned port;
	unsigned lsr;
	unsigned frame_errors;
	unsigned parity_errors;
	unsigned overruns;
} uartx_t;

extern array_t uartx_rstack[];

void uartx_init (uartx_t *u, int prio, unsigned int khz, unsigned long baud);
