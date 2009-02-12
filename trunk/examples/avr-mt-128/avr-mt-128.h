/*
 * Definitions of input signals and output actions for Olimex MT-128 board.
 */
inline int button_up_pressed (void)
{
	return ~PINA & 0b00000001;
}

inline int button_left_pressed (void)
{
	return ~PINA & 0b00000010;
}

inline int button_center_pressed (void)
{
	return ~PINA & 0b00000100;
}

inline int button_right_pressed (void)
{
	return ~PINA & 0b00001000;
}

inline int button_down_pressed (void)
{
	return ~PINA & 0b00010000;
}

inline int led_relay_is_on (void)
{
	return PINA & 0b01000000;
}

inline void led_relay_control (int on)
{
	if (on)
		PORTA |= 0b01000000;
	else
		PORTA &= ~0b01000000;
}

inline void led_relay_init ()
{
	led_relay_control (0);
	DDRA |= 0b01000000;
}

inline void buzzer_control (int mode)
{
	if (mode < 0)
		PORTE |= 0b00110000;				/* Disable */
	else if (mode == 0)
		PORTE = (PORTE | 0b00110000) & ~0b00010000;	/* Forward */
	else
		PORTA = (PORTE | 0b00110000) & ~0b00100000;	/* Reverse */
}

inline void buzzer_init ()
{
	buzzer_control (-1);
	DDRE |= 0b00110000;
}
