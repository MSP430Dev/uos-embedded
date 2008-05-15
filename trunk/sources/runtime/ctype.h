/*
 * Процедуры классификации символов.
 *
 * Возвращаются ненулевые значения, если проверяемый символ попадает
 * в соответствующий класс символов, в противном случае возвращается ноль.
 */
extern const unsigned char _ctype_ [257];

#define	_U	0x01
#define	_L	0x02
#define	_N	0x04
#define	_S	0x08
#define	_P	0x10
#define	_C	0x20
#define	_X	0x40
#define	_E	0x80

/*
 * Проверяет, является ли символ цифрой 0..9.
 */
static inline unsigned char
isdigit (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _N;
}

/*
 * Проверяет, принадлежит ли символ к шестнадцатеричному разряду,
 * т.е. является ли он одним из: 0..9 a..f A..F.
 */
static inline unsigned char
isxdigit (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & (_N | _X);
}

/*
 * Проверяет символ на принадлежность к алфавитным
 * символам a..z A..Z 0240..0377.
 */
static inline unsigned char
isalpha (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & (_U | _L | _E);
}

/*
 * Проверяет, является ли символ символом нижнего регистра a..z.
 */
static inline unsigned char
islower (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _L;
}

/*
 * Проверяет, расположен ли символ в верхнем регистре A..Z.
 */
static inline unsigned char
isupper (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _U;
}

/*
 * Проверяет символ на принадлежность к текстовым
 * символам 0..9 a..z A..Z 0240..0377.
 */
static inline unsigned char
isalnum (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & (_U | _L | _E | _N);
}

/*
 * Преобразует символ в заглавный.
 * Если это не символ a..z, результат непредсказуем.
 */
static inline unsigned char
toupper (int c)
{
	return c + 'A' - 'a';
}

/*
 * Преобразует символ в строчный.
 * Если это не символ A..Z, результат непредсказуем.
 */
static inline unsigned char
tolower (int c)
{
	return c + 'a' - 'A';
}

/*
 * Проверяет, являются ли символы неотображаемыми, а именно:
 * пробел, символ перевода страницы \f, "новая строка" \n,
 * "перевод каретки" \r, "горизонтальная табуляция" \t и
 * "вертикальная табуляция" '\v'.
 */
static inline unsigned char
isspace (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _S;
}

/*
 * Проверяет, является ли символ печатаемым; он не
 * должен быть пробелом или текстовым символом.
 */
static inline unsigned char
ispunct (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _P;
}

/*
 * Проверяет, является ли символ управляющим.
 * То есть не isprint() и не EOF.
 */
static inline unsigned char
iscntrl (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & _C;
}

/*
 * Проверяет, является ли символ печатаемым (включая пробел).
 * То есть не iscntrl() и не EOF.
 */
static inline unsigned char
isprint (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & (_P | _U | _L | _E | _N | _S);
}

/*
 * Проверяет, является ли символ печатаемым (не пробелом).
 * То есть не isspace(), не iscntrl() и не EOF.
 */
static inline unsigned char
isgraph (int c)
{
	return FETCH_BYTE (_ctype_+1+c) & (_P | _U | _L | _E | _N);
}

#undef	_U
#undef	_L
#undef	_N
#undef	_S
#undef	_P
#undef	_C
#undef	_X
#undef	_E
