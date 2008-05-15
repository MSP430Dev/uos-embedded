/*
 * Simple test program for regexp(3) stuff.  Knows about debugging hooks.
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 * Usage: try re [string [output [-]]]
 * The re is compiled and dumped, regexeced against the string, the result
 * is applied to output using regsub().  The - triggers a running narrative
 * from regexec().  Dumping and narrative don't happen unless DEBUG.
 *
 * If there are no arguments, stdin is assumed to be a stream of lines with
 * five fields:  a r.e., a string to match it against, a result code, a
 * source string for regsub, and the proper result.  Result codes are 'c'
 * for compile failure, 'y' for match success, 'n' for match failure.
 * Field separator is tab.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regexp.h"
#include "regpriv.h"

char buf [BUFSIZ];

int status = 0;			/* Exit status. */

int lineno;

regexp_t badregexp;		/* Implicit init to 0. */

void multiple ();
void complain (char *s1, char *s2);
void try (char **fields);

int
main (int argc, char **argv)
{
	regexp_t *r;
	int i;

	if (argc == 1) {
		multiple ();
		return status;
	}

	i = regexp_size (argv[1]);
	if (! i) {
		fprintf (stderr, "regexp: regsize failure\n");
		return 1;
	}
	r = (regexp_t*) calloc (1, i);
	if (! r) {
		fprintf (stderr, "cannot allocate %d bytes\n", i);
		return 1;
	}
	if (! regexp_compile (r, argv[1])) {
		fprintf (stderr, "regexp: regcomp failure\n");
		return 1;
	}

#ifdef DEBUG_REGEXP
	regsub_dump (r);
	if (argc > 4)
		regsub_narrate++;
#endif
	if (argc > 2) {
		i = regexp_execute (r, argv[2]);
		printf ("%d", i);
		for (i=1; i<NSUBEXP; i++)
			if (r->startp[i] && r->endp[i])
				printf(" \\%d", i);
		printf("\n");
	}
	if (argc > 3) {
		regexp_substitute (r, argv[3], buf);
		printf ("%s\n", buf);
	}
	return status;
}

void
multiple ()
{
	char rbuf[BUFSIZ];
	char *field[5];
	char *scan;
	int i;
	regexp_t *r;

	lineno = 0;
	while (fgets (rbuf, sizeof(rbuf), stdin)) {
		rbuf[strlen(rbuf)-1] = '\0';	/* Dispense with \n. */
		lineno++;
		scan = rbuf;
		for (i = 0; i < 5; i++) {
			field[i] = scan;
			if (! field[i]) {
				complain ("bad testfile format", "");
				exit (1);
			}
			scan = strchr (scan, '\t');
			if (scan)
				*scan++ = '\0';
		}
		try(field);
	}

	/* And finish up with some internal testing... */
	lineno = 9990;
	if (regexp_size (0))
		complain("regsize(NULL) doesn't complain", "");

	if (regexp_compile (0, "foo"))
		complain("regcomp(NULL, ...) doesn't complain", "");

	if (regexp_compile (&badregexp, 0))
		complain("regcomp(..., NULL) doesn't complain", "");

	lineno = 9991;
	if (regexp_execute (0, "foo"))
		complain("regexec(NULL, ...) doesn't complain", "");

	lineno = 9992;
	i = regexp_size ("foo");
	if (! i) {
		complain("regsize(\"foo\") fails", "");
		return;
	}

	r = (regexp_t*) calloc (1, i);
	if (! r) {
		complain("cannot allocate memory", "");
		return;
	}

	if (! regexp_compile (r, "foo")) {
		complain("regcomp(\"foo\") fails", "");
		free (r);
		return;
	}

	lineno = 9993;
	if (regexp_execute (r, 0))
		complain("regexec(..., NULL) doesn't complain", "");

	lineno = 9994;
	if (regexp_substitute (0, "foo", rbuf))
		complain("regsub(NULL, ..., ...) doesn't complain", "");

	lineno = 9995;
	if (regexp_substitute (r, 0, rbuf))
		complain("regsub(..., NULL, ...) doesn't complain", "");

	lineno = 9996;
	if (regexp_substitute (r, "foo", 0))
		complain("regsub(..., ..., NULL) doesn't complain", "");

	lineno = 9997;
	if (regexp_execute (&badregexp, "foo"))
		complain("regexec(nonsense, ...) doesn't complain", "");

	lineno = 9998;
	if (regexp_substitute (&badregexp, "foo", rbuf))
		complain("regsub(nonsense, ..., ...) doesn't complain", "");

	free ((char*) r);
}

void
try (char **fields)
{
	regexp_t *r;
	char dbuf[BUFSIZ];
	int i;

	i = regexp_size (fields[0]);
	if (! i) {
		if (*fields[2] != 'c')
			complain("regcomp failure in `%s'", fields[0]);
		return;
	}
	r = (regexp_t*) calloc (1, i);
	if (! r) {
		if (*fields[2] != 'c')
			complain("cannot allocate memory", "");
		return;
	}
	if (! regexp_compile (r, fields[0])) {
		if (*fields[2] != 'c')
			complain("regcomp failure in `%s'", fields[0]);
		free ((char*) r);
		return;
	}
	if (*fields[2] == 'c') {
		complain("unexpected regcomp success in `%s'", fields[0]);
		free((char *)r);
		return;
	}
	if (! regexp_execute (r, fields[1])) {
		if (*fields[2] != 'n')
			complain("regexec failure in `%s'", "");
		free((char *)r);
		return;
	}
	if (*fields[2] == 'n') {
		complain("unexpected regexec success", "");
		free((char *)r);
		return;
	}
	if (! regexp_substitute (r, fields[3], dbuf)) {
		complain ("regsub complaint", "");
		free ((char*) r);
		return;
	}
	if (strcmp(dbuf, fields[4]) != 0)
		complain("regsub result `%s' wrong", dbuf);
	free ((char*) r);
}

void
complain (char *s1, char *s2)
{
	fprintf (stderr, "try: %d: ", lineno);
	fprintf (stderr, s1, s2);
	fprintf (stderr, "\n");
	status = 1;
}
