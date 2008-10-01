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

extern double strtod (const unsigned char *, unsigned char **);

static inline double
atof (const unsigned char *__p)
{
	return strtod (__p, 0);
}
