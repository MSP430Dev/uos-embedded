/*
 * Atmel AVR-specific defines for UART driver.
 */
#ifdef __AVR_ATmega2561__
#   define RECEIVE_IRQ(p)	(p ? 35 : 24)	/* UART receive complete */
#   define TRANSMIT_IRQ(p)	(p ? 36 : 25)	/* UART transmit empty */
#endif

#ifdef __AVR_ATmega128__
#   define RECEIVE_IRQ(p)	(p ? 29 : 17)	/* UART receive complete */
#   define TRANSMIT_IRQ(p)	(p ? 30 : 18)	/* UART transmit empty */
#endif

#ifdef __AVR_ATmega161__
#   define RECEIVE_IRQ(p)	(p ? 13 : 12)	/* UART receive complete */
#   define TRANSMIT_IRQ(p)	(p ? 15 : 14)	/* UART transmit empty */
#endif

#if defined (__AVR_ATmega103__) || \
    defined (__AVR_ATmega168__)
#   define RECEIVE_IRQ(p)	17		/* UART receive complete */
#   define TRANSMIT_IRQ(p)	18		/* UART transmit empty */
#endif

#ifdef UCSR1B
/*
 * Using UART 0 and UART 1.
 */
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

#define transmit_byte(p,c)		if (p)	{			\
						  setb (TXC, UCSR1A);	\
						  UDR1 = (c);		\
						} else {		\
						  setb (TXC, USR);	\
						  UDR = (c);		\
						}
#define get_received_byte(p)		((p) ? UDR1 : UDR)

#define test_transmitter_enabled(p)	((p) ? testb (TXEN, UCSR1B) : \
						testb (TXEN, UCR))
#define test_transmitter_empty(p)	((p) ? testb (UDRE, UCSR1A) : \
						testb (UDRE, USR))
#define test_receive_data(p)		((p) ? testb (RXC, UCSR1A) : \
						testb (RXC, USR))
#define test_get_receive_data(p,d)	((p) ? (testb (RXC, UCSR1A) ? \
						((*d) = UDR1, 1) : 0) : \
					(testb (RXC, USR) ? \
						((*d) = UDR, 1) : 0))
#define test_frame_error(p)		((p) ? testb (FE, UCSR1A) : \
						testb (FE, USR))
#ifdef UPE
#define test_parity_error(p) 		((p) ? testb (UPE, UCSR1A) : \
						testb (UPE, USR))
#else
#define test_parity_error(p) 		0
#endif
#define test_overrun_error(p) 		((p) ? testb (DOR, UCSR1A) : \
						testb (DOR, USR))
#define uart_rxb8(p)			((p) ? testb (RXB81, UCSR1B) : \
						testb (RXB8, UCR))

#ifdef UBRR1
#define setup_baud_rate(port, khz, baud) \
	if (port) UBRR1 = ((int) (khz*1000L / baud) + 8) / 16 - 1; \
	else	  UBRR0 = ((int) (khz*1000L / baud) + 8) / 16 - 1
#else
#define setup_baud_rate(port, khz, baud) \
	if (port) UBRR1L = ((int) (khz*1000L / baud) + 8) / 16 - 1; \
	else	  UBRR = ((int) (khz*1000L / baud) + 8) / 16 - 1
#endif

#else
/*
 * Using UART 0 only.
 */
#define enable_transmitter(p)		setb (TXEN, UCR)
#define disable_transmitter(p)		clearb (TXEN, UCR)
#define enable_receiver(p)		setb (RXEN, UCR)
#define disable_receiver(p)		clearb (RXEN, UCR)
#define enable_receive_interrupt(p)	setb (RXCIE, UCR)
#define enable_transmit_interrupt(p)	setb (UDRIE, UCR)
#define disable_transmit_interrupt(p)	clearb (UDRIE, UCR)

#define transmit_byte(p,c)		UDR = (c)
#define get_received_byte(p)		UDR

#define test_transmitter_enabled(p)	testb (TXEN, UCR)
#define test_transmitter_empty(p)	testb (UDRE, USR)
#define test_receive_data(p)		testb (RXC, USR)
#define test_get_receive_data(p,d)	(testb (RXC, USR) ? \
					((*d) = UDR, 1) : 0)
#define test_frame_error(p)		testb (FE, USR)
#define test_parity_error(p) 		0
#define test_overrun_error(p) 		testb (DOR, USR)
#define uart_rxb8(p)			testb (RXB8, UCR)

#define setup_baud_rate(port, khz, baud) \
	UBRR = ((int) (khz * 1000L / baud) + 8) / 16 - 1
#endif

#define test_break_error(p)		0
#define clear_frame_error(p)		get_received_byte(p)
#define clear_parity_error(p)		get_received_byte(p)
#define clear_overrun_error(p)		get_received_byte(p)
#define clear_break_error(p)		/* void */
