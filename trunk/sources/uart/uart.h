#include <stream/stream.h>

/**\def TIMER_STACKSZ
 * \~english
 * Size of stack for UART task in bytes.
 *
 * \~russian
 * Размер стека для задачи драйвера UART, в байтах.
 */
#ifndef UART_STACKSZ
#   if __AVR__
#      define UART_STACKSZ	0x100		/* 100 enough for AVR */
#   endif
#   if ARM_CORTEX_M3
#      define UART_STACKSZ	1		/* unused */
#   elif defined (__arm__) || defined (__thumb__)
#      define UART_STACKSZ	0x200
#   endif
#   if MIPS32
#      define UART_STACKSZ	0x400
#   endif
#   if MSP430
#      define UART_STACKSZ	0x100
#   endif
#   if LINUX386
#      define UART_STACKSZ	4000
#   endif
#endif

/**\~english
 * Size of input buffer.
 *
 * \~russian
 * Размер буфера ввода.
 */
#ifndef UART_INBUFSZ
#define UART_INBUFSZ	8
#endif

/**\~english
 * Size of output buffer.
 *
 * \~russian
 * Размер буфера вывода.
 */
#ifndef UART_OUTBUFSZ
#define UART_OUTBUFSZ	32
#endif

/**\~english
 * Data structure of UART driver.
 *
 * \~russian
 * Структура данных для драйвера UART.
 */
typedef struct _uart_t {
	stream_interface_t *interface;
	mutex_t transmitter;
	mutex_t receiver;
	small_uint_t port;
	bool_t onlcr;
	unsigned int khz;
	unsigned char out_buf [UART_OUTBUFSZ];
	unsigned char *out_first, *out_last;
	unsigned char in_buf [UART_INBUFSZ];
	unsigned char *in_first, *in_last;
	bool_t (*cts_query) (struct _uart_t*);

	ARRAY (rstack, UART_STACKSZ);		/* task receive stack */
} uart_t;

void uart_init (uart_t *u, small_uint_t port, int prio, unsigned int khz,
	unsigned long baud);
void uart_set_cts_poller (uart_t *u, bool_t (*) (uart_t*));
void uart_cts_ready (uart_t *u);
void uart_transmit_wait (uart_t *u);

unsigned short uart_getchar (uart_t *u);
int uart_peekchar (uart_t *u);
void uart_putchar (uart_t *u, short c);
