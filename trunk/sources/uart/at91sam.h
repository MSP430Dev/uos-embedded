#include "kernel/internal.h"

/*
 * Atmel AT91SAM-specific defines for UART driver.
 */
#define RECEIVE_IRQ(p) (p ? AT91C_ID_US1 : AT91C_ID_US0) /* both receive and transmit */

#define enable_receiver(p)		if (p) { \
						*AT91C_PIOA_PDR = 0x60; /* RXD1 = PA5 */ \
						*AT91C_PIOA_ASR = 0x60; /* TXD1 = PA6 */ \
						*AT91C_PIOA_BSR = 0; \
						*AT91C_PMC_PCER = 1 << AT91C_ID_US1; \
						*AT91C_US1_CR = AT91C_US_RSTRX | \
							AT91C_US_RSTTX | \
							AT91C_US_RXDIS | \
							AT91C_US_TXDIS; \
						*AT91C_US1_TTGR = 0; \
						*AT91C_US1_MR = AT91C_US_CHRL_8_BITS | \
							AT91C_US_PAR_NONE; \
						*AT91C_US1_PTCR = AT91C_PDC_TXTEN | \
							AT91C_PDC_RXTEN; \
						*AT91C_US1_CR = AT91C_US_TXEN | \
							AT91C_US_RXEN; \
						*AT91C_US1_IDR = ~0; \
					} else { \
						/* UART0 already enabled in init() */; \
						*AT91C_US0_IDR = ~0; \
					}
#define enable_receive_interrupt(p)	if (p) *AT91C_US1_IER = AT91C_US_RXRDY | \
						AT91C_US_RXBRK | \
						AT91C_US_OVRE | \
						AT91C_US_FRAME | \
						AT91C_US_PARE; \
					else *AT91C_US0_IER = AT91C_US_RXRDY | \
						AT91C_US_RXBRK | \
						AT91C_US_OVRE | \
						AT91C_US_FRAME | \
						AT91C_US_PARE
#define enable_transmit_interrupt(p)	if (p) *AT91C_US1_IER = AT91C_US_TXRDY; \
					else *AT91C_US0_IER = AT91C_US_TXRDY
#define disable_transmit_interrupt(p)	if (p) *AT91C_US1_IDR = AT91C_US_TXRDY; \
					else *AT91C_US0_IDR = AT91C_US_TXRDY
#define transmit_byte(p,c)		if (p) *AT91C_US1_THR = (c); \
					else *AT91C_US0_THR = (c)
#define get_received_byte(p)		((p) ? *AT91C_US1_RHR : *AT91C_US0_RHR)

#define test_transmitter_enabled(p)	1
#define test_transmitter_empty(p)	((p) ? (*AT91C_US1_CSR & AT91C_US_TXRDY) : \
						(*AT91C_US0_CSR & AT91C_US_TXRDY))
#define test_get_receive_data(p,d)	((p) ? ((*AT91C_US1_CSR & AT91C_US_RXRDY) ? \
						((*d) = *AT91C_US1_RHR, 1) : 0) : \
					((*AT91C_US0_CSR & AT91C_US_RXRDY) ? \
						((*d) = *AT91C_US0_RHR, 1) : 0))
#define test_frame_error(p)		((p) ? (*AT91C_US1_CSR & AT91C_US_FRAME) : \
						(*AT91C_US0_CSR & AT91C_US_FRAME))
#define test_parity_error(p)		((p) ? (*AT91C_US1_CSR & AT91C_US_PARE) : \
						(*AT91C_US0_CSR & AT91C_US_PARE))
#define test_overrun_error(p)		((p) ? (*AT91C_US1_CSR & AT91C_US_OVRE) : \
						(*AT91C_US0_CSR & AT91C_US_OVRE))
#define test_break_error(p)		((p) ? (*AT91C_US1_CSR & AT91C_US_RXBRK) : \
						(*AT91C_US0_CSR & AT91C_US_RXBRK))
#define clear_frame_error(p)		/* --//-- */
#define clear_parity_error(p)		/* --//-- */
#define clear_overrun_error(p)		/* --//-- */
#define clear_break_error(p)		/* --//-- */
#define clear_receive_errors(p)		if (p) *AT91C_US1_CR = AT91C_US_RSTSTA; \
					else *AT91C_US0_CR = AT91C_US_RSTSTA

/*
 * Baudrate generator source - CLK
 */
#define setup_baud_rate(p, khz, baud)	if (p) *AT91C_US1_BRGR = (khz * 1000 / baud + 8) / 16; \
					else *AT91C_US0_BRGR = (khz * 1000 / baud + 8) / 16
