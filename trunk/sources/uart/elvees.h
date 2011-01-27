/*
 * Elvees Multicore-specific defines for UART driver.
 */
#include "kernel/internal.h"

#ifdef MC_IER1
/*
 * Two UARTs on chip.
 */
#define RECEIVE_IRQ(p)			((p) ? 5 : 4)	/* both receive and transmit */

#define enable_receiver(p)		if (p) { \
		MC_LCR1 = MC_LCR_8BITS; \
		MC_SCLR1 = 0; \
		MC_SPR1 = 0; \
		MC_IER1 = 0; \
		MC_MSR1 = 0; \
		MC_MCR1 = MC_MCR_DTR | MC_MCR_RTS | MC_MCR_OUT2; \
		MC_FCR1 = MC_FCR_RCV_RST | MC_FCR_XMT_RST | MC_FCR_ENABLE; \
		(void) MC_LSR1; \
		(void) MC_MSR1; \
		(void) MC_RBR1; \
		(void) MC_IIR1; \
	}

#define enable_receive_interrupt(p)	if (p) MC_IER1 |= MC_IER_ERXRDY | MC_IER_ERLS; \
					else MC_IER |= MC_IER_ERXRDY | MC_IER_ERLS
#define enable_transmit_interrupt(p)	if (p) MC_IER1 |= MC_IER_ETXRDY; \
					else MC_IER |= MC_IER_ETXRDY
#define disable_transmit_interrupt(p)	if (p) MC_IER1 &= ~MC_IER_ETXRDY; \
					else MC_IER &= ~MC_IER_ETXRDY

#define transmit_byte(p,c)		if (p) MC_THR1 = (c); else MC_THR = (c)
#define get_received_byte(p)		((p) ? MC_RBR1 : MC_RBR)

#define test_transmitter_enabled(p)	1
#define test_transmitter_empty(p)	((p) ? (MC_LSR1 & MC_LSR_TXRDY) : (MC_LSR & MC_LSR_TXRDY))
#define test_get_receive_data(p,d)	((p) ? ((__uart1_lsr & MC_LSR_RXRDY) ? \
					((*d) = MC_RBR1, 1) : 0) : \
					((__uart_lsr & MC_LSR_RXRDY) ? \
					((*d) = MC_RBR, 1) : 0))

#define test_frame_error(p)		((p) ? ((__uart1_lsr = MC_LSR1) & MC_LSR_FE) : \
					((__uart_lsr = MC_LSR) & MC_LSR_FE))
#define test_parity_error(p)		((p) ? (__uart1_lsr & MC_LSR_PE) : (__uart_lsr & MC_LSR_PE))
#define test_overrun_error(p)		((p) ? (__uart1_lsr & MC_LSR_OE) : (__uart_lsr & MC_LSR_OE))
#define test_break_error(p)		((p) ? (__uart1_lsr & MC_LSR_BI) : (__uart_lsr & MC_LSR_BI))
#define clear_frame_error(p)		/* Cleared by reading LSR */
#define clear_parity_error(p)		/* --//-- */
#define clear_overrun_error(p)		/* --//-- */
#define clear_break_error(p)		/* --//-- */

/*
 * Baudrate generator source - CLK
 */
#define setup_baud_rate(p, khz, baud)	{ \
	unsigned divisor = MC_DL_BAUD (khz * 1000, baud);		\
	if (p) {							\
		MC_LCR1 = MC_LCR_8BITS | MC_LCR_DLAB;			\
		MC_DLM1 = divisor >> 8;					\
		MC_DLL1 = divisor;					\
		MC_LCR1 = MC_LCR_8BITS;					\
	} else {							\
		MC_LCR = MC_LCR_8BITS | MC_LCR_DLAB;			\
		MC_DLM = divisor >> 8;					\
		MC_DLL = divisor;					\
		MC_LCR = MC_LCR_8BITS;					\
	}}

static unsigned __uart_lsr;
static unsigned __uart1_lsr;

#else /* MC_IER1 */
/*
 * Only one UART on chip.
 */
#define RECEIVE_IRQ(p)			4	/* both receive and transmit */

#define enable_receiver(p)		/* Already initialized in _init_()  */

#define enable_receive_interrupt(p)	MC_IER |= MC_IER_ERXRDY | MC_IER_ERLS
#define enable_transmit_interrupt(p)	MC_IER |= MC_IER_ETXRDY
#define disable_transmit_interrupt(p)	MC_IER &= ~MC_IER_ETXRDY

#define transmit_byte(p,c)		MC_THR = (c)
#define get_received_byte(p)		MC_RBR

#define test_transmitter_enabled(p)	1
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
#define setup_baud_rate(p, khz, baud)	{ \
	unsigned divisor = MC_DL_BAUD (khz * 1000, baud);	\
	MC_LCR = MC_LCR_8BITS | MC_LCR_DLAB;			\
	MC_DLM = divisor >> 8;					\
	MC_DLL = divisor;					\
	MC_LCR = MC_LCR_8BITS;					\
	}

static unsigned __uart_lsr;

#endif /* MC_IER1 */
