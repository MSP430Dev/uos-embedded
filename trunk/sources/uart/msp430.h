/*
 * MSP430-specific defines for UART driver.
 */

#ifdef __MSP430_HAS_UART1__
/*
 * Using UART 0 and UART 1.
 */
#define RECEIVE_IRQ(p)			(p ? USART1RX_VECTOR/2 : USART0RX_VECTOR/2)
#define TRANSMIT_IRQ(p)			(p ? USART1TX_VECTOR/2 : USART0TX_VECTOR/2)

#define enable_transmitter(p)		if (p) U1ME |= UTXE1; else U0ME |= UTXE0
#define disable_transmitter(p)		if (p) U1ME &= ~UTXE1; else U0ME &= ~UTXE0
#define test_transmitter_enabled(p)	((p) ? (U1ME & UTXE1) : (U0ME & UTXE0))

#define enable_receiver(p)		if (p) U1ME |= URXE1; else U0ME |= URXE0
#define disable_receiver(p)		if (p) U1ME &= ~URXE1; else U0ME &= ~URXE0

#define enable_receive_interrupt(p)	if (p) U1IE |= URXIE1; else U0IE |= URXIE0
#define enable_transmit_interrupt(p)	if (p) U1IE |= UTXIE1; else U0IE |= UTXIE0
#define disable_transmit_interrupt(p)	if (p) U1IE &= ~UTXIE1; else U0IE &= ~UTXIE0

#define transmit_byte(p,c)		if (p) TXBUF1 = (c); else TXBUF0 = (c)
#define get_received_byte(p)		((p) ? RXBUF1 : RXBUF0)

#define test_transmitter_empty(p)	((p) ? (U1IFG & UTXIFG1) : (U0IFG & UTXIFG0))
#define test_receive_data(p)		((p) ? (U1IFG & URXIFG1) : (U0IFG & URXIFG0))
#define test_get_receive_data(p,d)	((p) ? ((U1IFG & URXIFG1) ? \
						((*d) = RXBUF1, 1) : 0) : \
					((U0IFG & URXIFG0) ? \
						((*d) = RXBUF0, 1) : 0))

#define test_frame_error(p)		((p) ? (URCTL1 & FE) : (URCTL0 & FE))
#define test_parity_error(p) 		((p) ? (URCTL1 & PE) : (URCTL0 & PE))
#define test_overrun_error(p) 		((p) ? (URCTL1 & OE) : (URCTL0 & OE))
#define test_break_error(p)		((p) ? (URCTL1 & BRK) : (URCTL0 & BRK))

#define clear_frame_error(p)		if (p) URCTL1 &= ~FE; else URCTL0 &= ~FE
#define clear_parity_error(p)		if (p) URCTL1 &= ~PE; else URCTL0 &= ~PE
#define clear_overrun_error(p)		if (p) URCTL1 &= ~OE; else URCTL0 &= ~OE
#define clear_break_error(p)		if (p) URCTL1 &= ~BRK; else URCTL0 &= ~BRK

#elif defined (__MSP430_HAS_UART0__)
/*
 * Using UART 0 only.
 */
#define RECEIVE_IRQ(p)			(USART0RX_VECTOR/2)
#define TRANSMIT_IRQ(p)			(USART0TX_VECTOR/2)

#define enable_transmitter(p)		(U0ME |= UTXE0)
#define disable_transmitter(p)		(U0ME &= ~UTXE0)
#define test_transmitter_enabled(p)	(U0ME & UTXE0)

#define enable_receiver(p)		(U0ME |= URXE0)
#define disable_receiver(p)		(U0ME &= ~URXE0)

#define enable_receive_interrupt(p)	(U0IE |= URXIE0)
#define enable_transmit_interrupt(p)	(U0IE |= UTXIE0)
#define disable_transmit_interrupt(p)	(U0IE &= ~UTXIE0)

#define transmit_byte(p,c)		TXBUF0 = (c)
#define get_received_byte(p)		RXBUF0

#define test_transmitter_empty(p)	(U0IFG & UTXIFG0)
#define test_receive_data(p)		(U0IFG & URXIFG0)
#define test_get_receive_data(p,d)	((U0IFG & URXIFG0) ? \
					((*d) = RXBUF0, 1) : 0)

#define test_frame_error(p)		(URCTL0 & FE)
#define test_parity_error(p) 		(URCTL0 & PE)
#define test_overrun_error(p) 		(URCTL0 & OE)
#define test_break_error(p)		(URCTL0 & BRK)

#define clear_frame_error(p)		(URCTL0 &= ~FE)
#define clear_parity_error(p)		(URCTL0 &= ~PE)
#define clear_overrun_error(p)		(URCTL0 &= ~OE)
#define clear_break_error(p)		(URCTL0 &= ~BRK)

#endif

/*
 * Setting baud rate is somewhat complicated on MSP430:
 * we need to compute a modulation byte.
 */
#define setup_baud_rate(port, khz, baud) msp430_set_baud (port, khz, baud)

extern void msp430_set_baud (int port, unsigned khz, unsigned long baud);
