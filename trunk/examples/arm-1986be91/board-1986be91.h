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
        ARM_GPIOA->FUNC = 0x00005555;   /* Main Function для DATA[7:0] */
        ARM_GPIOA->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOA->PWR = 0x00005555;    /* Fast */

        ARM_GPIOE->FUNC = 0x00400500;   /* Main Function для ADDR[20,21,27] */
        ARM_GPIOE->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOE->PWR = 0x00400500;    /* Fast */

        ARM_GPIOC->FUNC = 0x15504010;   /* Main Function для RESET WE & CLOCK & KEYS*/
        ARM_GPIOC->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOC->PWR = 0x0008C010;    /* Fast */

        ARM_EXTBUS->CONTROL = 0x0000F001;
}
