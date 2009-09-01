/*
 * Definitions etc. for regexp(3) routines.
 */
#ifndef _REGEXP9_H_
#define _REGEXP9_H_ 1
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _regexp_t	regexp_t;
typedef struct _regexp_match_t	regexp_match_t;

/*
 * Sub expression matches
 */
struct _regexp_match_t {
	union
	{
		const char *sp;
		unsigned short *rsp;
	} s;
	union
	{
		const char *ep;
		unsigned short *rep;
	} e;
};

/*
 * Compile a regular expression into internal code.
 * Returns 0 on failure.
 */
extern regexp_t	*regexp_compile (const char*);
extern regexp_t	*regexp_compile_literal (const char*);
extern regexp_t	*regexp_compile_newline (const char*);

/*
 * Match a regular expression against a string.
 * Returns 0 on no match, >=1 on match, or <0 on failure.
 */
extern int regexp_execute (regexp_t*, const char*, regexp_match_t*, int);
extern int regexp_execute_unicode (regexp_t*, unsigned short*, regexp_match_t*, int);

/*
 * Perform substitutions after a regexp match.
 */
extern void regexp_substitute (const char*, char*, int, regexp_match_t*, int);
extern void regexp_substitute_unicode (const unsigned short*, unsigned short*, int, regexp_match_t*, int);

extern void regexp_error (char*);

#if defined(__cplusplus)
}
#endif
#endif
