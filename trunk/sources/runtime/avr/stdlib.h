/*
 * Standard numeric routines.
 */
static inline int
abs (int __x)
{
	return (__x < 0) ? -__x : __x;
}

static inline long
labs (long __x)
{
	return (__x < 0) ? -__x : __x;
}

extern unsigned long strtoul (const unsigned char *, unsigned char **, int);

static inline long
strtol (const unsigned char *__p, unsigned char **__ep, int b)
{
	return (long) strtoul (__p, __ep, b);
}

static inline int
atoi (const unsigned char *__p)
{
	return (int) strtol(__p, (unsigned char **) 0, 10);
}

static inline long
atol (const unsigned char *__p)
{
	return strtol(__p, (unsigned char **) 0, 10);
}

extern const unsigned char *strmatch (const unsigned char*, const unsigned char*);

/*
 * Watchdog timer..
 */
#define watchdog_alive()	asm volatile ("wdr")

void watchdog_enable (int timeout);
void watchdog_disable (void);
