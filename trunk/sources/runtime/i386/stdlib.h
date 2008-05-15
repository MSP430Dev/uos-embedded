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

extern unsigned long strtoul (const char *, char **, int);

static inline long
strtol (const char *__p, char **__ep, int b)
{
	return (long) strtoul (__p, __ep, b);
}

static inline int
atoi (const char *__p)
{
	return (int) strtol(__p, (char **) 0, 10);
}

static inline long
atol (const char *__p)
{
	return strtol(__p, (char **) 0, 10);
}

extern double strtod (const char *, char **);

static inline double
atof (const char *__p)
{
	return strtod (__p, 0);
}
