/*
 * Atmel AT91SAM-specific defines for UART driver.
 * Interrupt is the same for both receive and transmit.
 * Baudrate generator source is MCK.
 */

#ifdef ARM_AT91SAM_DEBUG_UART
/*
 * Use DBGU UART on signals DTXD/DRXD.
 */

#define RECEIVE_IRQ(p)			AT91C_ID_SYS

#define enable_receiver(p)		*AT91C_DBGU_IDR = ~0
					/* DBGU UART already enabled in bootstrap */
#define enable_receive_interrupt(p)	*AT91C_DBGU_IER = AT91C_US_RXRDY | \
						AT91C_US_RXBRK | \
						AT91C_US_OVRE | \
						AT91C_US_FRAME | \
						AT91C_US_PARE
#define enable_transmit_interrupt(p)	*AT91C_DBGU_IER = AT91C_US_TXRDY
#define disable_transmit_interrupt(p)	*AT91C_DBGU_IDR = AT91C_US_TXRDY
#define transmit_byte(p,c)		*AT91C_DBGU_THR = (c)
#define get_received_byte(p)		*AT91C_DBGU_RHR

#define test_transmitter_enabled(p)	1
#define test_transmitter_empty(p)	(*AT91C_DBGU_CSR & AT91C_US_TXRDY)
#define test_get_receive_data(p,d)	((*AT91C_DBGU_CSR & AT91C_US_RXRDY) ? \
						((*d) = *AT91C_DBGU_RHR, 1) : 0)
#define test_frame_error(p)		(*AT91C_DBGU_CSR & AT91C_US_FRAME)
#define test_parity_error(p)		(*AT91C_DBGU_CSR & AT91C_US_PARE)
#define test_overrun_error(p)		(*AT91C_DBGU_CSR & AT91C_US_OVRE)
#define test_break_error(p)		(*AT91C_DBGU_CSR & AT91C_US_RXBRK)
#define clear_frame_error(p)		/* --//-- */
#define clear_parity_error(p)		/* --//-- */
#define clear_overrun_error(p)		/* --//-- */
#define clear_break_error(p)		/* --//-- */
#define clear_receive_errors(p)		*AT91C_DBGU_CR = AT91C_US_RSTSTA

#define setup_baud_rate(p, khz, baud)	*AT91C_DBGU_BRGR = (khz * 1000 / baud + 8) / 16

#else /* ARM_AT91SAM_DEBUG_UART */
/*
 * Only two uarts US0 and US1 are implemented.
 */

#define RECEIVE_IRQ(p)			(p ? AT91C_ID_US1 : AT91C_ID_US0)

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

#define setup_baud_rate(p, khz, baud)	if (p) *AT91C_US1_BRGR = (khz * 1000 / baud + 8) / 16; \
					else *AT91C_US0_BRGR = (khz * 1000 / baud + 8) / 16

#endif /* ARM_AT91SAM_DEBUG_UART */
