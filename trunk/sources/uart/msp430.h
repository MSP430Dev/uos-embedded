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

#define test_transmitter_empty(p)	((p) ? (UTCTL1 & TXEPT) : (UTCTL0 & TXEPT))
#define test_receive_data(p)		((p) ? (U1IFG & URXIFG1) : (U0IFG & (URXIFG0 | 1)))
#define test_get_receive_data(p,d)	((p) ? ((U1IFG & URXIFG1) ? \
						((*d) = RXBUF1, 1) : 0) : \
					((U0IFG & (URXIFG0 | 1)) ? \
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

#define test_transmitter_empty(p)	(UTCTL0 & TXEPT)
#define test_receive_data(p)		(U0IFG & (URXIFG0 | 1))
#define test_get_receive_data(p,d)	((U0IFG & (URXIFG0 | 1)) ? \
					((*d) = RXBUF0, 1) : 0)

#define test_frame_error(p)		(URCTL0 & FE)
#define test_parity_error(p) 		(URCTL0 & PE)
#define test_overrun_error(p) 		(URCTL0 & OE)
#define test_break_error(p)		(URCTL0 & BRK)

#define clear_frame_error(p)		(URCTL0 &= ~FE)
#define clear_parity_error(p)		(URCTL0 &= ~PE)
#define clear_overrun_error(p)		(URCTL0 &= ~OE)
#define clear_break_error(p)		(URCTL0 &= ~BRK)

#elif defined (__MSP430_HAS_USCI_AB0__)
/*
 * Using new USCI.
 */
#define RECEIVE_IRQ(p)			(p==0 ? USCIA0_RXTX_VECTOR/2 : \
					 p==1 ? USCIA1_RXTX_VECTOR/2 : \
					 p==2 ? USCIA2_RXTX_VECTOR/2 : \
					        USCIA3_RXTX_VECTOR/2)

#define enable_receiver(p)		((&UCA0CTL1)[p<<6] &= ~UCSWRST)
#define test_transmitter_enabled(p)	1

#define enable_receive_interrupt(p)	((&UCA0IE)[p<<6] |= UCRXIE)
#define enable_transmit_interrupt(p)	((&UCA0IE)[p<<6] |= UCTXIE)
#define disable_transmit_interrupt(p)	((&UCA0IE)[p<<6] &= ~UCTXIE)

#define transmit_byte(p,c)		(&UCA0TXBUF)[p<<6] = (c)
#define get_received_byte(p)		(&UCA0RXBUF)[p<<6]

#define test_transmitter_empty(p)	((&UCA0IFG)[p<<6] & UCTXIFG)
#define test_receive_data(p)		((&UCA0IFG)[p<<6] & UCRXIFG)
#define test_get_receive_data(p,d)	(((&UCA0IFG)[p<<6] & UCRXIFG) ? \
					((*d) = (&UCA0RXBUF)[p<<6], 1) : 0)

#define test_frame_error(p)		((&UCA0STAT)[p<<6] & UCFE)
#define test_parity_error(p) 		((&UCA0STAT)[p<<6] & UCPE)
#define test_overrun_error(p) 		((&UCA0STAT)[p<<6] & UCOE)
#define test_break_error(p)		((&UCA0STAT)[p<<6] & UCBRK)

#define clear_frame_error(p)		((&UCA0STAT)[p<<6] &= ~UCFE)
#define clear_parity_error(p)		((&UCA0STAT)[p<<6] &= ~UCPE)
#define clear_overrun_error(p)		/* empty */
#define clear_break_error(p)		((&UCA0STAT)[p<<6] &= ~UCBRK)

#elif defined (__MSP430_HAS_USCI__)
/*
 * Using USCI.
 */
#define RECEIVE_IRQ(p)			(USCIAB0RX_VECTOR/2)
#define TRANSMIT_IRQ(p)			(USCIAB0TX_VECTOR/2)

#define enable_receiver(p)		/* empty */
#define enable_transmitter(p)		/* empty */
#define test_transmitter_enabled(p)	1

#define enable_receive_interrupt(p)	(IE2 |= UCA0RXIE)
#define enable_transmit_interrupt(p)	(IE2 |= UCA0TXIE)
#define disable_transmit_interrupt(p)	(IE2 &= ~UCA0TXIE)

#define transmit_byte(p,c)		UCA0TXBUF = (c)
#define get_received_byte(p)		UCA0RXBUF

#define test_transmitter_empty(p)	(IFG2 & UCA0TXIFG)
#define test_receive_data(p)		(IFG2 & UCA0RXIFG)
#define test_get_receive_data(p,d)	((IFG2 & UCA0RXIFG) ? \
					((*d) = UCA0RXBUF, 1) : 0)

#define test_frame_error(p)		(UCA0STAT & UCFE)
#define test_parity_error(p) 		(UCA0STAT & UCPE)
#define test_overrun_error(p) 		(UCA0STAT & UCOE)
#define test_break_error(p)		(UCA0STAT & UCBRK)

#define clear_frame_error(p)		(UCA0STAT &= ~UCFE)
#define clear_parity_error(p)		(UCA0STAT &= ~UCPE)
#define clear_overrun_error(p)		/* empty */
#define clear_break_error(p)		(UCA0STAT &= ~UCBRK)

#endif

/*
 * Setting baud rate is somewhat complicated on MSP430:
 * we need to compute a modulation byte.
 */
#define setup_baud_rate(port, khz, baud) msp430_set_baud (port, khz, baud)

extern void msp430_set_baud (int port, unsigned khz, unsigned long baud);
