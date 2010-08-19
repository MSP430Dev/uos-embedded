/*
 * Definitions of input signals and output actions for Milandr 1986BE91 board.
 */
inline int joystick_left (void)
{
	return (ARM_GPIOC->DATA & 0x7C00) == 0x5C00;
}

inline int joystick_down (void)
{
	return (ARM_GPIOC->DATA & 0x7C00) == 0x6C00;
}

inline int joystick_up (void)
{
	return (ARM_GPIOC->DATA & 0x7C00) == 0x7400;
}

inline int joystick_right (void)
{
	return (ARM_GPIOC->DATA & 0x7C00) == 0x3C00;
}

inline int joystick_pressed (void)
{
	return (ARM_GPIOC->DATA & 0x7C00) == 0x7800;
}

inline void buttons_init (void)
{
	/* Enable clock for PORTC. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOC;

        ARM_GPIOC->FUNC = 0x15504010;   /* Main Function для RESET WE & CLOCK & KEYS */
        ARM_GPIOC->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOC->PWR = 0x0008C010;    /* Fast */
}
