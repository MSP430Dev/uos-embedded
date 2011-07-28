/*
 * PIC32-specific defines for UART driver.
 */
#include "kernel/internal.h"

/*
 * Two UARTs on chip.
 */
#define RECEIVE_IRQ(p)			((p) ? 32 : 24)	/* both receive and transmit */

#define enable_receiver(p)		if (p) {		\
	U2STA = 0;						\
	U2MODE = PIC32_UMODE_PDSEL_8NPAR | PIC32_UMODE_ON;	\
	U2STASET = PIC32_USTA_URXEN | PIC32_USTA_UTXEN;		\
	}

#define enable_receive_interrupt(p)	if (p) IECSET(1) = 1 << (PIC32_IRQ_U2RX-32); \
					else IECSET(0) = 1 << PIC32_IRQ_U1RX
#define enable_transmit_interrupt(p)	if (p) IECSET(1) = 1 << (PIC32_IRQ_U2TX-32); \
					else IECSET(0) = 1 << PIC32_IRQ_U1TX
#define disable_transmit_interrupt(p)	if (p) IECCLR(1) = 1 << (PIC32_IRQ_U2TX-32); \
                                        else IECCLR(0) = 1 << PIC32_IRQ_U1TX

#define transmit_byte(p,c)		if (p) U2TXREG = (c); else U1TXREG = (c)
#define get_received_byte(p)		((p) ? U2RXREG : U1RXREG)

#define test_transmitter_enabled(p)	1
#define test_transmitter_empty(p)	(((p) ? U2STA : U1STA) & PIC32_USTA_TRMT)
#define test_get_receive_data(p,d)	((p) ? ((U2STA & PIC32_USTA_URXDA) ? \
					((*d) = U2RXREG, 1) : 0) : \
					((U1STA & PIC32_USTA_URXDA) ? \
					((*d) = U1RXREG, 1) : 0))

#define test_frame_error(p)		((p) ? (U2STA & PIC32_USTA_FERR) : (U1STA & PIC32_USTA_FERR))
#define test_parity_error(p)		((p) ? (U2STA & PIC32_USTA_PERR) : (U1STA & PIC32_USTA_PERR))
#define test_overrun_error(p)		((p) ? (U2STA & PIC32_USTA_OERR) : (U1STA & PIC32_USTA_OERR))
#define test_break_error(p)		0

#define clear_frame_error(p)		/* Cleared by reading RXREG */
#define clear_parity_error(p)		/* Cleared by reading RXREG */
#define clear_overrun_error(p)		if (p) U2STACLR = PIC32_USTA_OERR; \
					else   U1STACLR = PIC32_USTA_OERR
#define clear_break_error(p)		/* Empty */

/*
 * Baudrate generator source - CLK
 */
#define setup_baud_rate(p, khz, baud)	{			\
	unsigned divisor = PIC32_BRG_BAUD (khz * 1000, baud);	\
	if (p) U2BRG = divisor;					\
	else   U1BRG = divisor;					\
	}
