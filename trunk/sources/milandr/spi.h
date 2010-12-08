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
void spi_init (spi_t *c, int port, int master,
	int bits_per_word, unsigned nsec_per_bit);
void spi_output (spi_t *c, unsigned word);
void spi_input (spi_t *c, unsigned *word);

#endif /* !__SPI_H_ */
