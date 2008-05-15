/*
 * Linux-specific defines for UART driver.
 * Single port only.
 */
#include <unistd.h>
#include <signal.h>

#define __USE_GNU
#define _SYS_TYPES_H 1
#include <fcntl.h>

#define RECEIVE_IRQ(p)			SIGUSR1	/* UART receive complete */
#define TRANSMIT_IRQ(p)			SIGUSR2	/* UART transmit complete */

#define enable_transmitter(p)			{ \
	/*fcntl (1, F_SETOWN, uart_pid); \
	fcntl (1, F_SETSIG, TRANSMIT_IRQ(p)); \
	fcntl (1, F_SETFL, fcntl (1, F_GETFL, 0) | O_ASYNC); */}
#define enable_receiver(p)			{ \
	fcntl (0, F_SETOWN, uart_pid); \
	fcntl (0, F_SETSIG, RECEIVE_IRQ(p)); \
	fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) | O_ASYNC); }
#define enable_transmit_interrupt(p)		/* empty */
#define disable_transmit_interrupt(p)		/* empty */
#define enable_receive_interrupt(p)		/* empty */

#define transmit_byte(p,c)			{ \
	unsigned char _val = (c); \
	write (1, &_val, 1); \
	kill (uart_pid, TRANSMIT_IRQ(p)); }
#define get_received_byte(p)			0 /* empty */

#define test_transmitter_enabled(p)		1
#define test_transmitter_empty(p)		1
#define test_get_receive_data(p,d)		(read (0, d, 1) == 1)

#define test_frame_error(p)			0 /* empty */
#define test_parity_error(p)			0 /* empty */
#define test_overrun_error(p)			0 /* empty */
#define test_break_error(p)			0 /* empty */

#define clear_frame_error(p)			/* void */
#define clear_parity_error(p)			/* void */
#define clear_overrun_error(p)			/* void */
#define clear_break_error(p)			/* void */

#define setup_baud_rate(port, khz, baud)	{ uart_pid = getpid (); }

int uart_pid;
