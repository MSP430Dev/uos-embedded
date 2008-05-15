/*
 * Atmel AVR-specific defines for UART driver.
 */

#if defined (__AVR_ATmega2561__)
  /*
 * Using UART 0 and UART 1 - ATmega2561.
 */
#define RECEIVE_IRQ(p)		(p ? 35 : 24)	/* UART receive complete */
#define TRANSMIT_IRQ(p)		(p ? 36 : 25)	/* UART transmit empty */

#define enable_transmitter(p)		if (p) setb_far (TXEN, UCSR1B); else \
						setb (TXEN, UCR)
#define disable_transmitter(p)		if (p) clearb_far (TXEN, UCSR1B); else \
						clearb (TXEN, UCR)
#define enable_receiver(p)		if (p) setb_far (RXEN, UCSR1B); else \
						setb (RXEN, UCR)
#define disable_receiver(p)		if (p) clearb_far (RXEN, UCSR1B); else \
						clearb (RXEN, UCR)
#define enable_receive_interrupt(p)	if (p) setb_far (RXCIE, UCSR1B); else \
						setb (RXCIE, UCR)
#define disable_receive_interrupt(p)	if (p) clearb_far (RXCIE, UCSR1B); else \
						clearb (RXCIE, UCR)
#define enable_transmit_interrupt(p)	if (p) setb_far (UDRIE, UCSR1B); else \
						setb (UDRIE, UCR)
#define disable_transmit_interrupt(p)	if (p) clearb_far (UDRIE, UCSR1B); else \
						clearb (UDRIE, UCR)

#define transmit_byte(p,c)		if (p)	{                         \
						  setb_far (TXC, UCSR1A); \
						  outb_far ((c), UDR1);   \
						} else {                  \
						  setb (TXC, USR);        \
						  outb (c, UDR);          \
						}
#define get_received_byte(p)		((p) ? inb_far (UDR1) : inb (UDR))

#define test_transmitter_enabled(p)	((p) ? testb_far (TXEN, UCSR1B) : \
						testb (TXEN, UCR))
#define test_transmitter_empty(p)	((p) ? testb_far (UDRE, UCSR1A) : \
						testb (UDRE, USR))
#define test_transmit_complete(p)	((p) ? testb_far (TXC, UCSR1A) : \
						testb (TXC, USR))
#define test_receive_data(p)		((p) ? testb_far (RXC, UCSR1A) : \
						testb (RXC, USR))
#define test_get_receive_data(p,d)	((p) ? (testb_far (RXC, UCSR1A) ? \
						((*d) = inb_far (UDR1), 1) : 0) : \
					(testb (RXC, USR) ? \
						((*d) = inb (UDR), 1) : 0))
#define test_frame_error(p)		((p) ? testb_far (FE, UCSR1A) : \
						testb (FE, USR))
#define test_parity_error(p) 		((p) ? testb_far (UPE, UCSR1A) : \
						testb (UPE, USR))
#define test_overrun_error(p) 		((p) ? testb_far (DOR, UCSR1A) : \
						testb (DOR, USR))
#define uart_rxb8(p)			((p) ? testb_far (RXB81, UCSR1B) : \
						testb (RXB8, UCR))

#define setup_baud_rate(port, khz, baud) \
	if (port) outb_far (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR1L); \
	else	  outb (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR)



#elif defined (__AVR_ATmega128__)
/*
 * Using UART 0 and UART 1 - ATmega128.
 */
#define RECEIVE_IRQ(p)		(p ? 29 : 17)	/* UART receive complete */
#define TRANSMIT_IRQ(p)		(p ? 30 : 18)	/* UART transmit empty */

#define enable_transmitter(p)		if (p) setb_far (TXEN, UCSR1B); else \
						setb (TXEN, UCR)
#define disable_transmitter(p)		if (p) clearb_far (TXEN, UCSR1B); else \
						clearb (TXEN, UCR)
#define enable_receiver(p)		if (p) setb_far (RXEN, UCSR1B); else \
						setb (RXEN, UCR)
#define disable_receiver(p)		if (p) clearb_far (RXEN, UCSR1B); else \
						clearb (RXEN, UCR)
#define enable_receive_interrupt(p)	if (p) setb_far (RXCIE, UCSR1B); else \
						setb (RXCIE, UCR)
#define disable_receive_interrupt(p)	if (p) clearb_far (RXCIE, UCSR1B); else \
						clearb (RXCIE, UCR)
#define enable_transmit_interrupt(p)	if (p) setb_far (UDRIE, UCSR1B); else \
						setb (UDRIE, UCR)
#define disable_transmit_interrupt(p)	if (p) clearb_far (UDRIE, UCSR1B); else \
						clearb (UDRIE, UCR)

#define transmit_byte(p,c)		if (p)	{                         \
						  setb_far (TXC, UCSR1A); \
						  outb_far ((c), UDR1);   \
						} else {                  \
						  setb (TXC, USR);        \
						  outb (c, UDR);          \
						}
#define get_received_byte(p)		((p) ? inb_far (UDR1) : inb (UDR))

#define test_transmitter_enabled(p)	((p) ? testb_far (TXEN, UCSR1B) : \
						testb (TXEN, UCR))
#define test_transmitter_empty(p)	((p) ? testb_far (UDRE, UCSR1A) : \
						testb (UDRE, USR))
#define test_transmit_complete(p)	((p) ? testb_far (TXC, UCSR1A) : \
						testb (TXC, USR))
#define test_receive_data(p)		((p) ? testb_far (RXC, UCSR1A) : \
						testb (RXC, USR))
#define test_get_receive_data(p,d)	((p) ? (testb_far (RXC, UCSR1A) ? \
						((*d) = inb_far (UDR1), 1) : 0) : \
					(testb (RXC, USR) ? \
						((*d) = inb (UDR), 1) : 0))
#define test_frame_error(p)		((p) ? testb_far (FE, UCSR1A) : \
						testb (FE, USR))
#define test_parity_error(p) 		((p) ? testb_far (UPE, UCSR1A) : \
						testb (UPE, USR))
#define test_overrun_error(p) 		((p) ? testb_far (DOR, UCSR1A) : \
						testb (DOR, USR))
#define uart_rxb8(p)			((p) ? testb_far (RXB81, UCSR1B) : \
						testb (RXB8, UCR))

#define setup_baud_rate(port, khz, baud) \
	if (port) outb_far (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR1L); \
	else	  outb (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR)

#elif defined (__AVR_ATmega161__)
/*
 * Using UART 0 and UART 1 - ATmega161.
 */
#define RECEIVE_IRQ(p)		(p ? 23 : 17)	/* UART receive complete */
#define TRANSMIT_IRQ(p)		(p ? 24 : 18)	/* UART transmit empty */

#define enable_transmitter(p)		if (p) setb (TXEN, UCSR1B); else \
						setb (TXEN, UCR)
#define disable_transmitter(p)		if (p) clearb (TXEN, UCSR1B); else \
						clearb (TXEN, UCR)
#define enable_receiver(p)		if (p) setb (RXEN, UCSR1B); else \
						setb (RXEN, UCR)
#define disable_receiver(p)		if (p) clearb (RXEN, UCSR1B); else \
						clearb (RXEN, UCR)
#define enable_receive_interrupt(p)	if (p) setb (RXCIE, UCSR1B); else \
						setb (RXCIE, UCR)
#define enable_transmit_interrupt(p)	if (p) setb (UDRIE, UCSR1B); else \
						setb (UDRIE, UCR)
#define disable_transmit_interrupt(p)	if (p) clearb (UDRIE, UCSR1B); else \
						clearb (UDRIE, UCR)

#define transmit_byte(p,c)		if (p) outb (c, UDR1); else \
						outb (c, UDR)
#define get_received_byte(p)		((p) ? inb (UDR1) : inb (UDR))

#define test_transmitter_enabled(p)	((p) ? testb (TXEN, UCSR1B) : \
						testb (TXEN, UCR))
#define test_transmitter_empty(p)	((p) ? testb (UDRE, UCSR1A) : \
						testb (UDRE, USR))
#define test_receive_data(p)		((p) ? testb (RXC, UCSR1A) : \
						testb (RXC, USR))
#define test_get_receive_data(p,d)	((p) ? (testb (RXC, UCSR1A) ? \
						((*d) = inb (UDR1), 1) : 0) : \
					(testb (RXC, USR) ? \
						((*d) = inb (UDR), 1) : 0))
#define test_frame_error(p)		((p) ? testb (FE, UCSR1A) : \
						testb (FE, USR))
#define test_parity_error(p) 		0
#define test_overrun_error(p) 		((p) ? testb (DOR, UCSR1A) : \
						testb (DOR, USR))
#define uart_rxb8(p)			((p) ? testb (RXB81, UCSR1B) : \
						testb (RXB, UCR))

#define setup_baud_rate(port, khz, baud) \
	if (port) outb (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR1); \
	else	  outb (((int) (khz*1000L / baud) + 8) / 16 - 1, UBRR0)

#else
/*
 * Using UART 0 only - ATmega103, ATmega603, AT90S2313, AT90S2333,
 *	AT90S4414, AT90S4433, AT90S4434, AT90S8515, AT90S8535.
 */
#define RECEIVE_IRQ(p)			17	/* UART receive complete */
#define TRANSMIT_IRQ(p)			18	/* UART transmit empty */

#define enable_transmitter(p)		setb (TXEN, UCR)
#define disable_transmitter(p)		clearb (TXEN, UCR)
#define enable_receiver(p)		setb (RXEN, UCR)
#define disable_receiver(p)		clearb (RXEN, UCR)
#define enable_receive_interrupt(p)	setb (RXCIE, UCR)
#define enable_transmit_interrupt(p)	setb (UDRIE, UCR)
#define disable_transmit_interrupt(p)	clearb (UDRIE, UCR)

#define transmit_byte(p,c)		outb (c, UDR)
#define get_received_byte(p)		inb (UDR)

#define test_transmitter_enabled(p)	testb (TXEN, UCR)
#define test_transmitter_empty(p)	testb (UDRE, USR)
#define test_receive_data(p)		testb (RXC, USR)
#define test_get_receive_data(p,d)	(testb (RXC, USR) ? \
					((*d) = inb (UDR), 1) : 0)
#define test_frame_error(p)		testb (FE, USR)
#define test_parity_error(p) 		0
#define test_overrun_error(p) 		testb (DOR, USR)
#define uart_rxb8(p)			testb (RXB8, UCR)

#define setup_baud_rate(port, khz, baud) \
	outb (((int) (khz * 1000L / baud) + 8) / 16 - 1, UBRR)

#endif

#define test_break_error(p)		0
#define clear_frame_error(p)		get_received_byte(p)
#define clear_parity_error(p)		get_received_byte(p)
#define clear_overrun_error(p)		get_received_byte(p)
#define clear_break_error(p)		/* void */
