#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mkdata (char *name)
{
	FILE *fd;
	int c, len;
	char *basename;

	fd = fopen (name, "r");
	if (! fd) {
		perror (name);
		exit (-1);
	}

	/* Extract base file name. */
	basename = strrchr (name, '/');
	if (basename)
		++basename;
	else
		basename = name;
	name = strchr (basename, '.');
	if (name)
		*name = 0;

	printf ("static const char %s_data[] = {\n\t", basename);
	len = 0;
	for (;;) {
		c = getc (fd);
		if (c < 0)
			break;

		printf ("%d,", c);

		len += 2;
		if (c > 9)
			++len;
		if (c > 99)
			++len;
		if (len > 66) {
			printf ("\n\t");
			len = 0;
		}
	}
	if (len > 0)
		printf ("\n");
	printf ("};\n");
}

int main (int argc, char **argv)
{
	/* Make embedded images. */
	for (++argv; --argc>0; ++argv)
		mkdata (*argv);
	return(0);
}
