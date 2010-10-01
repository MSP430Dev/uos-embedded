/*
 * Definitions of input signals and output actions for Milandr 1986BE91 board.
 */
#define BUTTON_RIGHT	14	/* pins of PORTC */
#define BUTTON_LEFT	13
#define BUTTON_DOWN	12
#define BUTTON_UP	11
#define BUTTON_SEL	10

#define MASK_RIGHT	(1 << BUTTON_RIGHT)
#define MASK_LEFT	(1 << BUTTON_LEFT)
#define MASK_DOWN	(1 << BUTTON_DOWN)
#define MASK_UP		(1 << BUTTON_UP)
#define MASK_SEL	(1 << BUTTON_SEL)

#define ALL_BUTTONS	(MASK_RIGHT | MASK_LEFT | MASK_DOWN | \
			 MASK_UP | MASK_SEL)

inline int joystick_left (void)
{
	return (~ARM_GPIOC->DATA & ALL_BUTTONS) == MASK_LEFT;
}

inline int joystick_down (void)
{
        return (~ARM_GPIOC->DATA & ALL_BUTTONS) == MASK_DOWN;
}

inline int joystick_up (void)
{
        return (~ARM_GPIOC->DATA & ALL_BUTTONS) == MASK_UP;
}

inline int joystick_right (void)
{
        return (~ARM_GPIOC->DATA & ALL_BUTTONS) == MASK_RIGHT;
}

inline int joystick_pressed (void)
{
        return (~ARM_GPIOC->DATA & ALL_BUTTONS) == MASK_SEL;
}

inline void buttons_init (void)
{
	/* Enable clock for PORTC. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOC;

	/* Функция по умолчанию для PC10-PC14. */
	unsigned func = ARM_GPIOC->FUNC;
	func &= ~(ARM_FUNC_MASK(BUTTON_RIGHT) | ARM_FUNC_MASK(BUTTON_LEFT) |
		ARM_FUNC_MASK(BUTTON_DOWN) | ARM_FUNC_MASK(BUTTON_UP) |
		ARM_FUNC_MASK(BUTTON_SEL));
	ARM_GPIOC->FUNC = func;

	/* Цифровые выводы. */
	ARM_GPIOC->ANALOG |= ALL_BUTTONS;

	/* Обычный фронт. */
	ARM_GPIOC->PWR &= ~(ARM_PWR_MASK(BUTTON_RIGHT) |
		ARM_PWR_MASK(BUTTON_LEFT) | ARM_PWR_MASK(BUTTON_DOWN) |
		ARM_PWR_MASK(BUTTON_UP) | ARM_PWR_MASK(BUTTON_SEL));
}
