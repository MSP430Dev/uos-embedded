/*
 * SPI driver for Milandr 1986ВЕ91 microcontroller.
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
#ifndef __SPI_H_
#define __SPI_H_

/*
 * Queue of SPI packets.
 */
#define SPI_QUEUE_SIZE		64	/* max number of packets in queue */

typedef struct _spi_queue_t {
	unsigned count;
	unsigned short *tail;
	unsigned short queue [SPI_QUEUE_SIZE];
} spi_queue_t;

/*
 * Data structure for SPI channel.
 */
typedef struct _spi_t {
	mutex_t lock;			/* interrupt goes here */

	unsigned port;			/* port number */
	unsigned master;		/* master or slave */
	unsigned kbps;			/* kbits per second */
	spi_queue_t inq;		/* queue of received packets */

	/* Statistics. */
	unsigned long intr;		/* interrupt counter */
	unsigned long out_packets;	/* transmitted packets */
	unsigned long in_packets;	/* received packets */
	unsigned long in_discards;	/* ignored packets, due to lack of memory */
} spi_t;

/*
 * User level API.
 */
void spi_init (spi_t *c, int port, int bits_per_word, unsigned nsec_per_bit);
void spi_output (spi_t *c, unsigned word);
int spi_input (spi_t *c, unsigned *word);
void spi_input_wait (spi_t *c, unsigned *word);

#endif /* !__SPI_H_ */
