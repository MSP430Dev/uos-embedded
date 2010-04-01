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
#   if defined (__arm__) || defined (__thumb__)
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
#define UART_INBUFSZ	8

/**\~english
 * Size of output buffer.
 *
 * \~russian
 * Размер буфера вывода.
 */
#define UART_OUTBUFSZ	32

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

	ARRAY (rstack, UART_STACKSZ);		/* task receive stack */
} uartx_t;

void uartx_init (uartx_t *u, int prio, unsigned int khz, unsigned long baud);
