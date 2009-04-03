/*
 * Definitions of input signals and output actions for Olimex MT-128 board.
 */
inline int button1_pressed (void)
{
	return ! (*AT91C_PIOB_PDSR & AT91C_PIO_PB24);
}

inline int button2_pressed (void)
{
	return ! (*AT91C_PIOB_PDSR & AT91C_PIO_PB25);
}

inline int joystick_left (void)
{
	return ! (*AT91C_PIOA_PDSR & AT91C_PIO_PA7);
}

inline int joystick_down (void)
{
	return ! (*AT91C_PIOA_PDSR & AT91C_PIO_PA8);
}

inline int joystick_up (void)
{
	return ! (*AT91C_PIOA_PDSR & AT91C_PIO_PA9);
}

inline int joystick_right (void)
{
	return ! (*AT91C_PIOA_PDSR & AT91C_PIO_PA14);
}

inline int joystick_pressed (void)
{
	return ! (*AT91C_PIOA_PDSR & AT91C_PIO_PA15);
}

inline void buttons_init (void)
{
	/* Button SW1. */
	*AT91C_PIOB_ODR = AT91C_PIO_PB24;
	*AT91C_PIOB_PER = AT91C_PIO_PB24;

	/* Button SW2. */
	*AT91C_PIOB_ODR = AT91C_PIO_PB25;
	*AT91C_PIOB_PER = AT91C_PIO_PB25;

	/* Joystick left. */
	*AT91C_PIOA_ODR = AT91C_PIO_PA7;
	*AT91C_PIOA_PER = AT91C_PIO_PA7;

	/* Joystick down. */
	*AT91C_PIOA_ODR = AT91C_PIO_PA8;
	*AT91C_PIOA_PER = AT91C_PIO_PA8;

	/* Joystick up. */
	*AT91C_PIOA_ODR = AT91C_PIO_PA9;
	*AT91C_PIOA_PER = AT91C_PIO_PA9;

	/* Joystick right. */
	*AT91C_PIOA_ODR = AT91C_PIO_PA14;
	*AT91C_PIOA_PER = AT91C_PIO_PA14;

	/* Joystick pressed. */
	*AT91C_PIOA_ODR = AT91C_PIO_PA15;
	*AT91C_PIOA_PER = AT91C_PIO_PA15;
}
