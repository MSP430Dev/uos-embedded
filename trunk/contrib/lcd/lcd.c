#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "timer/timer.h"
#include "lcd/lcd.h"

#define NCOL	16	/* Количество символов в строке */

/*
 * Адреса для обращения к LCD.
 * Сигнал E подключен к адресной линии A15,
 * сигнал RS - к адресной линии A14.
 * Младшие разряды адреса игнорируются, но мы устанавливаем
 * их в 1 чтобы избежать влияния на данные при переключении шины.
 * Признак volatile нужен, чтобы компилятор не пытался оптимизировать
 * обращения к этим адресам, а выполнял все как написано.
 */
#define CTL	(* (volatile unsigned char*) 0x80ff)
#define DATA	(* (volatile unsigned char*) 0xc0ff)

/*
 * Таблица перекодировки.
 */
static unsigned char encoding [256]  = {
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0040, 0041, 0042, 0043, 0044, 0045, 0046, 0047, /*  !"#$%&' */
	0050, 0051, 0052, 0053, 0054, 0055, 0056, 0057,	/* ()*+,-./ */
	0060, 0061, 0062, 0063, 0064, 0065, 0066, 0067,	/* 01234567 */
	0070, 0071, 0072, 0073, 0074, 0075, 0076, 0077,	/* 89:;<=>? */

	0100, 0101, 0102, 0103, 0104, 0105, 0106, 0107, /* @ABCDEFG */
	0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117, /* HIJKLMNO */
	0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127, /* PQRSTUVW */
	0130, 0131, 0132, 0133, 0004, 0135, 0136, 0137, /* XYZ[\]^_ */
	0140, 0141, 0142, 0143, 0144, 0145, 0146, 0147, /* `abcdefg */
	0150, 0151, 0152, 0153, 0154, 0155, 0156, 0157, /* hijklmno */
	0160, 0161, 0162, 0163, 0164, 0165, 0166, 0167, /* pqrstuvw */
	0170, 0171, 0172, 0005, 0321, 0006, 0351, 0377, /* xyz{|}~\177 */

	0134, 0173, 0174, 0175, 0176, 0177, 0347, 0376,	/* ........ */
	0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317,	/* ........ */
	0320, 0321, 0322, 0323, 0324, 0325, 0326, 0327,	/* ........ */
	0330, 0331, 0332, 0333, 0334, 0335, 0336, 0337,	/* ........ */
	0350, 0355, 0352, 0265, 0353, 0354, 0355, 0356, /* ...ё.... */
	0357, 0360, 0361, 0362, 0363, 0364, 0365, 0366,	/* ........ */
	0367, 0370, 0371, 0242, 0372, 0373, 0374, 0375,	/* ...Ё.... */
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,	/* ........ */

	0306, 0141, 0262, 0345, 0343, 0145, 0344, 0264, /* юабцдефг */
	0170, 0270, 0271, 0272, 0273, 0274, 0275, 0157, /* хийклмно */
	0276, 0307, 0160, 0143, 0277, 0171, 0266, 0263, /* пярстужв */
	0304, 0303, 0267, 0301, 0305, 0346, 0300, 0302, /* ьызшэщчъ */
	0260, 0101, 0240, 0341, 0340, 0105, 0252, 0241, /* ЮАБЦДЕФГ */
	0130, 0245, 0246, 0113, 0247, 0115, 0110, 0117, /* ХИЙКЛМНО */
	0250, 0261, 0120, 0103, 0124, 0251, 0243, 0102, /* ПЯРСТУЖВ */
	0007, 0256, 0244, 0254, 0257, 0342, 0253, 0255, /* ЬЫЗШЭЩЧЪ */
};

/*
 * Некоторых сиволов недостает, будем загружать их сами.
 */
#define ROW(a,b,c,d,e) (a<<4 | b<<3 | c<<2 | d<<1 | e)

static const char backslash [8]  = {
	ROW( 0,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,0,0,1 ),
	ROW( 0,0,0,0,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char leftbrace [8]  = {
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char rightbrace [8]  = {
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char hardsign [8]  = {
	ROW( 1,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 1,1,1,0,0 ),
	ROW( 1,0,0,1,0 ),
	ROW( 1,0,0,1,0 ),
	ROW( 1,1,1,0,0 ),
	ROW( 0,0,0,0,0 ),
};

static stream_interface_t lcd_interface = {
	(void (*) (stream_t*, short)) lcd_putchar,
	0, 0,
};

/*
 * Ожидание готовности контроллера.
 */
static void lcd_wait ()
{
	unsigned short cnt = 20000;

	while (--cnt > 0)
		if (! (CTL & 0x80))
			break;
}

/*
 * Запись команды, с ожиданием готовности.
 */
static inline void lcd_write_ctl (unsigned char val)
{
	CTL = val;
	lcd_wait ();
}

/*
 * Запись байта данных, с ожиданием готовности.
 */
static inline void lcd_write_data (unsigned char val)
{
	DATA = val;
	lcd_wait ();
}

/*
 * Инициализация контроллера.
 * Экран состоит из двух строк.
 * Вывод на каждую из строк делается независимо.
 */
void lcd_init (lcd_t *line1, lcd_t *line2)
{
	unsigned char i;

	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес.
	 * Устанавливаем бит ожидания для замедления. */
	setb (SRE, MCUCR);
	setb (SRW, MCUCR);

	/* Упрощенная инициализация 8-битного подключения. */
	lcd_wait ();
	lcd_write_ctl (0x30);

	/* Две строки, символы 5x7 */
	lcd_write_ctl (0x38);

	/* Стираем экран */
	lcd_write_ctl (0x01);

	/* Включаем отображение */
	lcd_write_ctl (0x0c);

	/* Автоматический сдвиг курсора влево */
	lcd_write_ctl (0x06);

	line1->interface = &lcd_interface;
	line1->timer = 0;
	line1->base = 0x80;
	line1->col = 0;
	line1->timer = 0;
	line2->interface = &lcd_interface;
	line2->base = 0xc0;
	line2->col = 0;
	for (i=0; i<NCOL; ++i) {
		line1->data[i] = ' ';
		line2->data[i] = ' ';
	}

	/* Загружаем недостающие символы:
	 * 4 - \, 5 - {, 6 - }, 7 - Ь */
	lcd_load_glyph (4, backslash);
	lcd_load_glyph (5, leftbrace);
	lcd_load_glyph (6, rightbrace);
	lcd_load_glyph (7, hardsign);
}

/*
 * Загрузка изображения символа.
 * Восемь символов с кодами 0-7 можно загружать.
 * Данные для загрузки должны находиться в памяти flash.
 */
void lcd_load_glyph (char n, const char *data)
{
	unsigned char i;

	lcd_write_ctl (0x40 + n * 8);		/* установка адреса */
	for (i=0; i<8; ++i)
		lcd_write_data (readb (data++));
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
	lcd_write_ctl (0x01);

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
	c = readb (encoding + c);

	if (line->col >= NCOL) {
		/* Прокрутка, с задержкой на 150 мсек. */
		if (line->timer)
			timer_delay (line->timer, 150);
		lcd_scroll (line);
	}
	lcd_write_ctl (line->base | line->col);	/* установка адреса */
	lcd_write_data (c);
	line->data [line->col++] = c;
}

/*
 * Печать строки по формату, аналогично printf.
 */
int lcd_printf (lcd_t *line, const char *fmt, ...)
{
	va_list	args;
	int err;

	va_start (args, fmt);
	err = vprintf ((stream_t*) line, fmt, args);
	va_end (args);
	return err;
}

/*
 * Печать строки по формату, аналогично printf.
 * При выходе за границу экрана строка плавно прокручивается.
 */
int lcd_rprintf (lcd_t *line, timer_t *timer, const char *fmt, ...)
{
	va_list	args;
	int err;

	line->timer = timer;
	va_start (args, fmt);
	err = vprintf ((stream_t*) line, fmt, args);
	va_end (args);
	line->timer = 0;
	return err;
}
