/*
 * Futaba Vacuum Fluorescent Display Module M204SD02AJ
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include "lcd.h"

#define PORTC_OE	1
#define PORTC_WE	2
#define PORTE_RS	6

#define LCD_REG	*((volatile unsigned *) 0x60000034)

// Display commands and their parameters
#define LCD_CMD_CLEAR_DISPLAY		0x01
#define LCD_CMD_CURSOR_HOME		0x02
#define LCD_CMD_ENTRY_MODE		0x04
#	define LCD_PARAM_SHIFT_EN		0x01
#	define LCD_PARAM_INC			0x02
#define LCD_CMD_DISPLAY_CONTROL		0x08
#	define LCD_PARAM_BLINK			0x01
#	define LCD_PARAM_CURSOR_ON		0x02
#	define LCD_PARAM_DISPLAY_ON		0x04
#define LCD_CMD_SHIFT			0x10
#	define LCD_PARAM_LEFT			0x00
#	define LCD_PARAM_RIGHT			0x04
#	define LCD_PARAM_SHIFT			0x08
#define LCD_CMD_FUNCTION_SET		0x20
#	define LCD_PARAM_BRIGHTNESS(n)		(n)
#	define LCD_PARAM_LINES(n)		((n - 1) << 3)
#	define LCD_PARAM_4BIT			0x00
#	define LCD_PARAM_8BIT			0x10
#define LCD_CMD_CGRAM_ADDR		0x40
#define LCD_CMD_DDRAM_ADDR		0x80


static void lcd_putchar (lcd_t *lcd, short c);

static stream_interface_t lcd_interface = {
	(void (*) (stream_t*, short)) lcd_putchar,
	0, 0,
};

static void init_bus ()
{
	/* Enable clock for PORTA, PORTC, PORTE, PORTF external bus. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA | ARM_PER_CLOCK_GPIOC | 
		ARM_PER_CLOCK_GPIOE | ARM_PER_CLOCK_GPIOF |
		ARM_PER_CLOCK_EXT_BUS;

	/* Enable bus */
	ARM_GPIOA->FUNC = (ARM_GPIOA->FUNC & ~0xFFFF) |	/* Main Function для DATA[7:0] */
		ARM_FUNC_MAIN(0) | ARM_FUNC_MAIN(1) |
		ARM_FUNC_MAIN(2) | ARM_FUNC_MAIN(3) |
		ARM_FUNC_MAIN(4) | ARM_FUNC_MAIN(5) |
		ARM_FUNC_MAIN(6) | ARM_FUNC_MAIN(7);
        ARM_GPIOA->ANALOG |= 0xFF;			/* Digital */
	ARM_GPIOA->PWR = (ARM_GPIOA->PWR & ~0xFFFF) |	/* Fast */
		ARM_PWR_FASTEST(0) | ARM_PWR_FASTEST(1) |
		ARM_PWR_FASTEST(2) | ARM_PWR_FASTEST(3) |
		ARM_PWR_FASTEST(4) | ARM_PWR_FASTEST(5) |
		ARM_PWR_FASTEST(6) | ARM_PWR_FASTEST(7);
		
	ARM_GPIOF->FUNC = (ARM_GPIOF->FUNC &		/* Main Function для ADDR[5:2] */
		~(ARM_FUNC_MASK (2) | ARM_FUNC_MASK (3) | ARM_FUNC_MASK (4) | ARM_FUNC_MASK (5))) |
		ARM_FUNC_MAIN (2) | ARM_FUNC_MAIN (3) | ARM_FUNC_MAIN (4) | ARM_FUNC_MAIN (5);
	ARM_GPIOF->ANALOG |= 0x3C;			/* Digital */
	ARM_GPIOF->PWR = (ARM_GPIOF->PWR &		/* Fast */
		~(ARM_PWR_MASK (2) | ARM_PWR_MASK (3) | ARM_PWR_MASK (4) | ARM_PWR_MASK (5))) |
		ARM_PWR_FASTEST (2) | ARM_PWR_FASTEST (3) | ARM_PWR_FASTEST (4) | ARM_PWR_FASTEST (5);

	/* OE, WE */
	ARM_GPIOC->FUNC = (ARM_GPIOA->FUNC & ~(ARM_FUNC_MASK (PORTC_OE) | ARM_FUNC_MASK (PORTC_WE))) |
		ARM_FUNC_MAIN (PORTC_OE) | ARM_FUNC_MAIN (PORTC_WE);
	ARM_GPIOC->ANALOG |= (1 << PORTC_OE) | (1 << PORTC_WE);	/* Digital */
	ARM_GPIOC->PWR = (ARM_GPIOC->PWR & ~(ARM_PWR_MASK (PORTC_OE) | ARM_PWR_MASK (PORTC_WE))) |
		ARM_PWR_FASTEST(PORTC_OE) | ARM_PWR_FASTEST(PORTC_WE);
		
	/* RS */
	ARM_GPIOE->FUNC = ARM_GPIOA->FUNC & ~ARM_FUNC_MASK (PORTE_RS);
	ARM_GPIOE->ANALOG |= (1 << PORTE_RS);			/* Digital */
	ARM_GPIOE->OE |= (1 << PORTE_RS);			/* Output */
	ARM_GPIOE->PWR = (ARM_GPIOE->PWR & ~ARM_PWR_MASK (PORTE_RS)) | ARM_PWR_FASTEST(PORTE_RS);

	if (ARM_EXTBUS->CONTROL & ARM_EXTBUS_RAM)
		ARM_EXTBUS->CONTROL |= ARM_EXTBUS_WS(15);
	else
		ARM_EXTBUS->CONTROL = ARM_EXTBUS_ROM | ARM_EXTBUS_WS(15);
}

static inline void lcd_command (unsigned char cmd)
{
	ARM_GPIOE->DATA &= ~(1 << PORTE_RS);
	LCD_REG = cmd;
	udelay (1);
}

static inline unsigned char lcd_cursor_pos ()
{
	ARM_GPIOE->DATA &= ~(1 << PORTE_RS);
	return LCD_REG & 0x7F;
}

static inline void lcd_carriage_return ()
{
	unsigned char pos = lcd_cursor_pos ();
	if (pos <= 0x13)			// first line
		pos = 0x00;
	else if (pos >= 0x40 && pos <= 0x53)	// second line
		pos = 0x40;
	else if (pos >= 0x14 && pos <= 0x27)	// third line
		pos = 0x14;
	else 					// fourth line
		pos = 0x54;
	lcd_command (LCD_CMD_DDRAM_ADDR | pos);
}

static inline void lcd_next_line ()
{
	unsigned char pos = lcd_cursor_pos ();
	if (pos <= 0x13)			// first line
		pos = 0x40;
	else if (pos >= 0x40 && pos <= 0x53)	// second line
		pos = 0x14;
	else if (pos >= 0x14 && pos <= 0x27)	// third line
		pos = 0x54;
	else 					// fourth line
		pos = 0x00;
	lcd_command (LCD_CMD_DDRAM_ADDR | pos);
}

static inline void lcd_set_data (unsigned char data)
{
	ARM_GPIOE->DATA |= (1 << PORTE_RS);
	LCD_REG = data;
	//udelay (1);
}

void lcd_clear (lcd_t *lcd)
{
	lcd_command (LCD_CMD_CLEAR_DISPLAY);
}

void lcd_set_brightness (lcd_t *lcd, int percent)
{
	int level = 0;
	
	if (percent <= 25) level = 3;
	else if (percent <= 50) level = 2;
	else if (percent <= 75) level = 1;
	else level = 0;
	
	lcd_command (LCD_CMD_FUNCTION_SET | LCD_PARAM_LINES(2) | LCD_PARAM_8BIT | LCD_PARAM_BRIGHTNESS(level));
}

void lcd_init (lcd_t *lcd)
{
	init_bus ();
	lcd_command (LCD_CMD_FUNCTION_SET | LCD_PARAM_LINES(2) | LCD_PARAM_8BIT);
	lcd_command (LCD_CMD_CLEAR_DISPLAY);
	lcd_command (LCD_CMD_DDRAM_ADDR | 0x00);
	lcd_command (LCD_CMD_ENTRY_MODE | LCD_PARAM_INC);
	lcd_command (LCD_CMD_DISPLAY_CONTROL | LCD_PARAM_DISPLAY_ON /*| LCD_PARAM_CURSOR_ON*/);

	lcd->interface = &lcd_interface;
	
	lcd->esc_mode = 0;
}

static void lcd_putchar (lcd_t *lcd, short c)
{
	switch (lcd->esc_mode)  {
	case 0:
	
		switch (c) {
		case '\n':
			lcd_next_line ();
			return;
		case '\t':		/* tab replaced by space */
			c = ' ';
			break;
		case '\r':		/* carriage return - go to begin of line */
			lcd_carriage_return ();
			return;
		case '\33':
			lcd->esc_mode = 1;
			return;
		}
	break;
	
	case 1:
		if (c == '[') {
			lcd->esc_mode = 2;
			return;
		} else {
			lcd->esc_mode = 0;
		}
	break;
	
	case 2:
		if (c == 'H') {
			lcd_command (LCD_CMD_CURSOR_HOME);
			lcd->esc_mode = 0;
			return;
		} else if (c == '2') {
			lcd->esc_mode = 3;
			lcd->esc_buf = c;
			return;
		} else {
			lcd_set_data ('[');
			lcd->esc_mode = 0;
		}
	break;
	
	case 3:
		if (c == 'J') {
			lcd_command (LCD_CMD_CLEAR_DISPLAY);
			lcd->esc_mode = 0;
			return;
		} else {
			lcd_set_data ('[');
			lcd_set_data (lcd->esc_buf);
			lcd->esc_mode = 0;
		}
	break;
	default:
		lcd->esc_mode = 0;
	break;
	}
	
	lcd_set_data (c);
}
