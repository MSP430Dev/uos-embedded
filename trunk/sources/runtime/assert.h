#ifndef __UOS_ASSERT__H_
#define __UOS_ASSERT__H_ 1

/* This prints an "Assertion failed" message and aborts. */
#ifdef NDEBUG
#   define __assert_fail(a,b,c,d) __assert_fail_ndebug()
    void __assert_fail_ndebug ();
#else
    void __assert_fail (const char *expr, const char *file,
	unsigned line, const char *func);
#endif

/*
 * Permanent assertion, independent of NDEBUG macro.
 * Example:
 *	assert_always (ptr != NULL);
 */
#define assert_always(condition) do {			\
	if (__builtin_expect (! (condition), 0))	\
		 __assert_fail (#condition, __FILE__,	\
			__LINE__, __PRETTY_FUNCTION__);	\
	} while (0)

/*
 * void assert (int expression);
 *
 * If NDEBUG is defined, do nothing.
 * If not, and EXPRESSION is zero, print an error message and abort.
 */
#ifdef NDEBUG
#   define assert(expr)	do {} while (0)
#else
#   define assert(expr)	assert_always(expr)
#endif

#endif /* __UOS_ASSERT__H_ */
