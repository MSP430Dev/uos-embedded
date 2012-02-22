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
	//udelay (1);
}

static inline unsigned char lcd_cursor_pos ()
{
	ARM_GPIOE->DATA &= ~(1 << PORTE_RS);
	return LCD_REG & 0x7F;
}

static void lcd_set_line (lcd_t * lcd, int line)
{
	unsigned char pos;
	
//debug_printf ("set line %d\n", line);
	lcd->line = line;
	lcd->col = 0;
	switch (line) {
	case 0: pos = 0x00; break;
	case 1: pos = 0x40; break;
	case 2: pos = 0x14; break;
	case 3: pos = 0x54; break;
	default: return;
	}
	lcd_command (LCD_CMD_DDRAM_ADDR | pos);
}

static inline void lcd_set_data (lcd_t * lcd, unsigned char data)
{
	ARM_GPIOE->DATA |= (1 << PORTE_RS);
	LCD_REG = data;
	//udelay (1);
	
	lcd->new_line = 0;
	if (++lcd->col == 20) {
		lcd->new_line = 1;
		lcd->line = (lcd->line + 1) & 3;
		lcd_set_line (lcd, lcd->line);
	}
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
	lcd->unicode_char = 0;
	lcd->col = 0;
	lcd->line = 0;
}

/* Russian UTF-8 symbols decoding */
static short translate_russian (lcd_t *lcd, short c)
{
	unsigned short ic = c | (lcd->unicode_char << 8);
	
	switch (ic) {
	case 'А': case 'а': c = 'A';  break;
	case 'Б': case 'б': c = 0x80; break;
	case 'В': case 'в': c = 'B';  break;
	case 'Г': case 'г': c = 0x92; break;			
	case 'Д': case 'д': c = 0x81; break;
	case 'Е': case 'е': c = 'E';  break;
	case 'Ё': case 'ё': c = 0xCB; break;
	case 'Ж': case 'ж': c = 0x82; break;
	case 'З': case 'з': c = 0x83; break;
	case 'И': case 'и': c = 0x84; break;
	case 'Й': case 'й': c = 0x85; break;
	case 'К': case 'к': c = 'K';  break;
	case 'Л': case 'л': c = 0x86; break;
	case 'М': case 'м': c = 'M';  break;			
	case 'Н': case 'н': c = 'H';  break;
	case 'О': case 'о': c = 'O';  break;
	case 'П': case 'п': c = 0x87; break;
	case 'Р': case 'р': c = 'P';  break;
	case 'С': case 'с': c = 'C';  break;
	case 'Т': case 'т': c = 'T';  break;
	case 'У': case 'у': c = 0x88; break;
	case 'Ф': case 'ф': c = 0xD8; break;
	case 'Х': case 'х': c = 'X';  break;
	case 'Ц': case 'ц': c = 0x89; break;
	case 'Ч': case 'ч': c = 0x8A; break;			
	case 'Ш': case 'ш': c = 0x8B; break;
	case 'Щ': case 'щ': c = 0x8C; break;
	case 'Ъ': case 'ъ': c = 0x8D; break;
	case 'Ы': case 'ы': c = 0x8E; break;
	case 'Ь': case 'ь': c = 'b';  break;
	case 'Э': case 'э': c = 0x8F; break;
	case 'Ю': case 'ю': c = 0xAC; break;
	case 'Я': case 'я': c = 0xAD; break;
	}
	
	return c;
}

static void lcd_putchar (lcd_t *lcd, short c)
{
	switch (lcd->esc_mode)  {
	case 0:
		if (! lcd->unicode_char) {
			switch (c) {
			case '\n':
				if (lcd->new_line) {
					lcd->new_line = 0;
					return;
				}
				lcd->line = (lcd->line + 1) & 3;
				lcd_set_line (lcd, lcd->line);
				return;
			case '\t':		/* tab replaced by space */
				c = ' ';
				break;
			case '\r':		/* carriage return - go to begin of line */
				lcd_set_line (lcd, lcd->line);
				return;
			case '\33':
				lcd->esc_mode = 1;
				return;
			case 0xD0:
			case 0xD1:
				lcd->unicode_char = c;
				return;
			}
		} else {
			c = translate_russian (lcd, c);
			lcd->unicode_char = 0;
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
			lcd->col = 0;
			lcd->esc_mode = 0;
			return;
		} else if (c == '2') {
			lcd->esc_mode = 3;
			lcd->esc_buf = c;
			return;
		} else {
			lcd_set_data (lcd, '[');
			lcd->esc_mode = 0;
		}
	break;
	
	case 3:
		if (c == 'J') {
			lcd_command (LCD_CMD_CLEAR_DISPLAY);
			lcd->line = 0;
			lcd->col = 0;
			lcd->esc_mode = 0;
			return;
		} else {
			lcd_set_data (lcd, '[');
			lcd_set_data (lcd, lcd->esc_buf);
			lcd->esc_mode = 0;
		}
	break;
	default:
		lcd->esc_mode = 0;
	break;
	}
	
	lcd_set_data (lcd, c);
}
