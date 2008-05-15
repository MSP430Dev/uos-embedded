#ifndef __UOS_ASSERT__H_
#define __UOS_ASSERT__H_ 1

/*
 * void assert (int expression);
 *
 * If NDEBUG is defined, do nothing.
 * If not, and EXPRESSION is zero, print an error message and abort.
 */

/* This prints an "Assertion failed" message and aborts. */

#ifdef NDEBUG

	void __assert_fail_nodebug () /*__attribute__ ((__noreturn__))*/;

#	define assert_always(expr) do {				\
		if (unlikely (! (expr)))			\
			 __assert_fail_nodebug ();		\
	} while (0)
#else

	void __assert_fail_debug (const char *expr,
		const char *file, unsigned line,
		const char *func) /*__attribute__ ((__noreturn__))*/;

#	define assert_always(expr) do {				\
		if (unlikely (! (expr)))			\
			 __assert_fail_debug (#expr, __FILE__,	\
				__LINE__, __PRETTY_FUNCTION__);	\
	} while (0)

#endif /* NDEBUG */

#if defined (NDEBUG) && ! (defined (ASSERT_CHECK) && ASSERT_CHECK)
#	define assert(expr)	__noop
#else
#	define assert(expr)	assert_always(expr)
#endif

#endif /* __UOS_ASSERT__H_ */
