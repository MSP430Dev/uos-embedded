#include "kernel/internal.h"

/*
 * Elvees Multicore-specific defines for UART driver.
 */
#define RECEIVE_IRQ(p)	4		/* both receive and transmit */

#define enable_receiver(p)		/* already enabled in init() */
#define enable_receive_interrupt(p)	(MC_IER |= MC_IER_ERXRDY | MC_IER_ERLS)
#define disable_receive_interrupt(p)	(MC_IER &= ~(MC_IER_ERXRDY | MC_IER_ERLS))
#define enable_transmit_interrupt(p)	(MC_IER |= MC_IER_ETXRDY)
#define disable_transmit_interrupt(p)	(MC_IER &= ~MC_IER_ETXRDY)

#define transmit_byte(p,c)		(MC_THR = (c))
#define get_received_byte(p)		MC_RBR

#define test_transmitter_enabled(p)	(MC_IER & MC_IER_ETXRDY)
#define test_transmitter_empty(p)	(MC_LSR & MC_LSR_TXRDY)
#define test_get_receive_data(p,d)	((__uart_lsr & MC_LSR_RXRDY) ? \
					((*d) = MC_RBR, 1) : 0)
#define test_frame_error(p)		((__uart_lsr = MC_LSR) & MC_LSR_FE)
#define test_parity_error(p)		(__uart_lsr & MC_LSR_PE)
#define test_overrun_error(p)		(__uart_lsr & MC_LSR_OE)
#define test_break_error(p)		(__uart_lsr & MC_LSR_BI)
#define clear_frame_error(p)		/* Cleared by reading LSR */
#define clear_parity_error(p)		/* --//-- */
#define clear_overrun_error(p)		/* --//-- */
#define clear_break_error(p)		/* --//-- */

/*
 * Baudrate generator source - CLK
 */
#define setup_baud_rate(p, khz, baud) {					\
		unsigned divisor = MC_DL_BAUD (khz * 1000, baud);	\
		MC_LCR = MC_LCR_8BITS | MC_LCR_DLAB;			\
		MC_DLM = divisor >> 8;					\
		MC_DLL = divisor;					\
		MC_LCR = MC_LCR_8BITS;					\
	}

static unsigned __uart_lsr;
