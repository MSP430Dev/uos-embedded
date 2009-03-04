/*
 * Definitions of input signals and output actions for Olimex easyWeb2 board.
 */
inline int button1_pressed (void)
{
	return ~P4IN & (1 << 4);
}

inline int button2_pressed (void)
{
	return ~P4IN & (1 << 5);
}

inline int button3_pressed (void)
{
	return ~P4IN & (1 << 6);
}

inline int button4_pressed (void)
{
	return ~P4IN & (1 << 7);
}

inline void relay1_control (int on)
{
	if (on)
		P1OUT |= BIT5;
	else
		P1OUT &= ~BIT5;
}

inline void relay2_control (int on)
{
	if (on)
		P1OUT |= BIT6;
	else
		P1OUT &= ~BIT6;
}

inline void relay_init ()
{
	relay1_control (0);
	relay2_control (0);
	P1DIR = BIT5 | BIT6;
}

inline void buzzer_control (int mode)
{
	if (mode > 0) {
		P4OUT &= ~BIT2;			/* Forward */
		P4OUT |= BIT3;
	} else if (mode < 0) {
		P4OUT |= BIT2;			/* Reverse */
		P4OUT &= ~BIT3;
	} else {
		P4OUT &= ~(BIT2 | BIT3);	/* Disable */
	}
}

inline void buzzer_init ()
{
	buzzer_control (0);
	P4DIR = BIT2 | BIT3;
}

inline void led_control (int on)
{
	if (on)
		P2OUT &= ~BIT1;
	else
		P2OUT |= BIT1;
}

inline void led_init ()
{
	led_control (0);
	P2DIR = ~BIT0;
}
