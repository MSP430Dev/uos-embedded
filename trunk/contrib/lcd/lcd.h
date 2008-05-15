#define NCOL	16	/* Количество символов в строке */

typedef struct {
	stream_interface_t *interface;	/* для printf */
	struct _timer_t *timer;		/* для прокрутки */
	unsigned char base;		/* базовый адрес */
	unsigned char col;		/* номер позиции */
	unsigned char data [NCOL];	/* символы */
} lcd_t;

/*
 * Инициализация контроллера.
 */
void lcd_init (lcd_t *line1, lcd_t *line2);

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

/*
 * Печать строки по формату, аналогично printf.
 */
int lcd_printf (lcd_t *line, const char *fmt, ...);

/*
 * Печать строки по формату, аналогично printf.
 * При выходе за границу экрана строка плавно прокручивается.
 */
int lcd_rprintf (lcd_t *line, struct _timer_t *timer, const char *fmt, ...);
