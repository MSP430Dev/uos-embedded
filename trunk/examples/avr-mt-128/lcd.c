#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "timer/timer.h"
#include "lcd.h"

#define NCOL	16		/* Количество символов в строке */

/*
 * LCD indicator is connected to port C.
 */
#define RS	0b00000001	/* 0 - command, 1 - data */
#define RW	0b00000010	/* 0 - write, 1 - read */
#define E	0b00000100	/* strobe */
#define D4	0b00010000	/* data */
#define D5	0b00100000
#define D6	0b01000000
#define D7	0b10000000

/*
 * LCD commands.
 */
#define DISP_CLR	0x01
#define DISP_ON		0x0C
#define DISP_OFF	0x08
#define CUR_HOME        0x02
#define CUR_OFF 	0x0C
#define CUR_ON_UNDER    0x0E
#define CUR_ON_BLINK    0x0F
#define CUR_LEFT        0x10
#define CUR_RIGHT       0x14
#define CUR_UP  	0x80
#define CUR_DOWN	0xC0
#define DD_RAM_ADDR	0x80
#define DD_RAM_ADDR2	0xC0

static stream_interface_t lcd_interface = {
	(void (*) (stream_t*, short)) lcd_putchar,
	0, 0,
};

/*
 * Микросекундная задержка.
 */
static void usleep (small_uint_t usec)
{
	do {
		/* Два такта уходит на декремент и переход */
#if KHZ > 2000
		asm volatile ("nop");
#endif
#if KHZ > 3000
		asm volatile ("nop");
#endif
#if KHZ > 4000
		asm volatile ("nop");
#endif
#if KHZ > 5000
		asm volatile ("nop");
#endif
#if KHZ > 6000
		asm volatile ("nop");
#endif
#if KHZ > 7000
		asm volatile ("nop");
#endif
#if KHZ > 8000
		asm volatile ("nop");
#endif
#if KHZ > 9000
		asm volatile ("nop");
#endif
#if KHZ > 10000
		asm volatile ("nop");
#endif
#if KHZ > 11000
		asm volatile ("nop");
#endif
#if KHZ > 12000
		asm volatile ("nop");
#endif
#if KHZ > 13000
		asm volatile ("nop");
#endif
#if KHZ > 14000
		asm volatile ("nop");
#endif
#if KHZ > 15000
		asm volatile ("nop");
#endif
#if KHZ > 16000
		asm volatile ("nop");
#endif
	} while (--usec);
}

/*
 * Миллисекундная задержка.
 */
static void msleep (small_uint_t usec)
{
	while (usec-- > 0) {
		usleep (250);
		usleep (250);
		usleep (250);
		usleep (250);
	}
}

static void lcd_pulse ()
{
	PORTC |= E;
	usleep (5);
	PORTC &= ~E;
}

/*
 * Запись команды, с ожиданием готовности.
 */
static void lcd_write_ctl (unsigned char val)
{
	/* Set RS port to 0 */
	unsigned char lowbits = PORTC & ~(D7 | D6 | D5 | D4 | RS);

	/* High nibble. */
	PORTC = (val & 0b11110000) | lowbits;
	lcd_pulse ();

	/* Low nibble. */
	PORTC = (val << 4) | lowbits;
	lcd_pulse ();

	msleep (2);
}

/*
 * Запись байта данных, с ожиданием готовности.
 */
static void lcd_write_data (unsigned char val)
{
	/* Set RS port to 1 */
	unsigned char lowbits = (PORTC & ~(D7 | D6 | D5 | D4)) | RS;

	/* High nibble. */
	PORTC = (val & 0b11110000) | lowbits;
	lcd_pulse ();

	/* Low nibble. */
	PORTC = (val << 4) | lowbits;
	lcd_pulse ();

	msleep (2);
}

/*
 * Инициализация контроллера.
 * Экран состоит из двух строк.
 * Вывод на каждую из строк делается независимо.
 * Если задан таймер, то при выходе за границу экрана строка плавно прокручивается.
 */
void lcd_init (lcd_t *line1, lcd_t *line2, timer_t *timer)
{
	unsigned char i;

	PORTC &= ~(RS | RW | E | D4 | D5 | D6 | D7);
	DDRC |= RS | RW | E | D4 | D5 | D6 | D7;
	msleep (110);

	/* Initialize 4-bit bus. */
	PORTC |= D5 | D4;
	lcd_pulse ();
	msleep (10);

	lcd_pulse ();
	msleep (10);

	lcd_pulse ();
	msleep (10);

	PORTC &= ~D4;
	lcd_pulse ();
	msleep (10);

	/* Стираем экран */
	lcd_write_ctl (DISP_CLR);

	/* Включаем отображение */
	lcd_write_ctl (DISP_ON);

	line1->interface = &lcd_interface;
	line1->timer = timer;
	line1->base = DD_RAM_ADDR;
	line1->col = 0;
	line1->timer = timer;
	line2->interface = &lcd_interface;
	line2->base = DD_RAM_ADDR2;
	line2->col = 0;
	for (i=0; i<NCOL; ++i) {
		line1->data[i] = ' ';
		line2->data[i] = ' ';
	}
}

/*
 * Стирание одной строки.
 */
void lcd_clear (lcd_t *line)
{
	unsigned char i;

	lcd_write_ctl (line->base);		/* установка адреса */
	for (i=0; i<NCOL; ++i) {
		lcd_write_data (' ');
		line->data[i] = ' ';
	}
	line->col = 0;
}

/*
 * Стирание всего экрана.
 */
void lcd_clear_all (lcd_t *line1, lcd_t *line2)
{
	unsigned char i;

	/* Стираем экран */
	lcd_write_ctl (DISP_CLR);

	for (i=0; i<NCOL; ++i) {
		line1->data[i] = ' ';
		line2->data[i] = ' ';
	}
	line1->col = 0;
	line2->col = 0;
}

/*
 * Прокрутка одной строки на один символ влево.
 */
static void lcd_scroll (lcd_t *line)
{
	unsigned char i, c;

	if (line->col <= 0)
		return;

	lcd_write_ctl (line->base);		/* установка адреса */
	for (i=1; i<line->col; ++i) {
		c = line->data[i];
		lcd_write_data (c);
		line->data[i-1] = c;
	}
	lcd_write_data (' ');
	--line->col;
}

/*
 * Печать одного символа.
 * Некоторые символы обрабатываются специальным образом.
 */
void lcd_putchar (lcd_t *line, short c)
{
	switch (c) {
	case '\n':		/* перевод строки игнорируем */
		return;
	case '\t':		/* табуляцию заменяем на пробел */
		c = ' ';
		break;
	case '\f':		/* конец страницы - стираем строку */
		lcd_clear (line);
		return;
	case '\r':		/* возврат каретки - переход в начало строки */
		line->col = 0;
		return;
	}

	/* Перекодировка по таблице. */
	if (c < 0 || c >= 256)
		c = '?';
/*	c = readb (encoding + c);*/

	if (line->col >= NCOL) {
		/* Прокрутка, с задержкой на 150 мсек. */
		if (line->timer)
			timer_delay (line->timer, 200);
		lcd_scroll (line);
	}
	lcd_write_ctl (line->base | line->col);	/* установка адреса */
	lcd_write_data (c);
	line->data [line->col++] = c;
}
