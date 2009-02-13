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

inline int relay_is_on (void)
{
	return PINA & 0b01000000;
}

inline void relay_control (int on)
{
	if (on)
		setb (6, PORTA);
	else
		clearb (6, PORTA);
}

inline void relay_init ()
{
	relay_control (0);
	clearb (6, PORTA);
	setb (6, DDRA);
}

inline void buzzer_control (int mode)
{
	if (mode > 0) {
		clearb (4, PORTE);	/* Forward */
		setb (5, PORTE);
	} else if (mode < 0) {
		setb (4, PORTE);	/* Reverse */
		clearb (5, PORTE);
	} else {
		setb (4, PORTE);	/* Disable */
		setb (5, PORTE);
	}
}

inline void buzzer_init ()
{
	buzzer_control (0);
	setb (4, DDRE);
	setb (5, DDRE);
}
