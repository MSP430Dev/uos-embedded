#include <stream/stream.h>

#ifndef UART_STACKSZ
#   if __AVR__
#      define UART_STACKSZ	0x100		/* 100 enough for AVR */
#   endif
#   if defined (__arm__) || defined (__thumb__)
#      define UART_STACKSZ	0x200
#   endif
#   if __MSDOS__
#      define UART_STACKSZ	0x100
#   endif
#   if LINUX386
#      define UART_STACKSZ	4000
#   endif
#endif
#define UART_INBUFSZ	8

#define UART_OUTBUFSZ	32

typedef struct _uart_t {
	stream_interface_t *interface;
	lock_t transmitter;
	lock_t receiver;
	small_uint_t port;
	unsigned short khz;
	unsigned char out_buf [UART_OUTBUFSZ];
	unsigned char *out_first, *out_last;
	unsigned char in_buf [UART_INBUFSZ];
	unsigned char *in_first, *in_last;
	bool_t (*cts_query) (struct _uart_t*);

	OPACITY (rstack, UART_STACKSZ);		/* task receive stack */
} uart_t;

void uart_init (uart_t *u, small_uint_t port, int prio, unsigned short khz,
	unsigned long baud);
void uart_set_cts_poller (uart_t *u, bool_t (*) (uart_t*));
void uart_cts_ready (uart_t *u);
void uart_transmit_wait (uart_t *u);

unsigned short uart_getchar (uart_t *u);
int uart_peekchar (uart_t *u);
void uart_putchar (uart_t *u, short c);
