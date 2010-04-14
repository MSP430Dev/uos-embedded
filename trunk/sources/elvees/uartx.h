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
