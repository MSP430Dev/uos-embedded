#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Read game configuration file (on stdin)
 * and output embedded data.
 */
int main (void)
{
	FILE *fp = stdin;
	char buf [256], *p;
	int line;

	printf ("static const char *gamefile_data[] = {\n");

	line = 0;
	while (fgets(buf, 256, fp)) {
		++line;

		/* Look for the newline at the end of the line. */
		p = strchr (buf, '\n');
		if (! p) {
			/* There wasn't one, which probably means that the
			 * line was longer than 255 characters. */
			fprintf (stderr, "Too long line: %d\n", line);
			exit (1);
		}
		*p = 0;

		/* Ignore comments and blank lines: */
		if (*buf == '#' || *buf == 0)
			continue;

		printf ("\t\"%s\\n\",\n", buf);
	}
	printf ("\t0,\n};\n");
	return(0);
}
