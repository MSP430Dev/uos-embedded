
#define NCOL	16			/* Line width */

typedef struct {
	stream_interface_t *interface;	/* for printf */
	struct _timer_t *timer;		/* for scrolling */
	unsigned char base;		/* base address of this line */
	unsigned char col;		/* current column */
	unsigned char raw;		/* do not use Unicode mapping */
	unsigned char c1, c2;		/* utf8 decoder */
	unsigned char data [NCOL];	/* symbols */
} lcd_t;

/*
 * Инициализация контроллера.
 * Если задан таймер, то при выходе за границу экрана строка плавно прокручивается.
 */
void lcd_init (lcd_t *line1, lcd_t *line2, timer_t *timer);

/*
 * Загрузка изображения символа.
 * Восемь символов с кодами 0-7 можно загружать.
 */
void lcd_load_glyph (char n, const char *data);

/*
 * Стирание всего экрана.
 */
void lcd_clear_all (lcd_t *line1, lcd_t *line2);

/*
 * Стирание одной строки.
 */
void lcd_clear (lcd_t *line);

/*
 * Печать одного символа.
 * Некоторые символы обрабатываются специальным образом.
 */
void lcd_putchar (lcd_t *line, short c);
