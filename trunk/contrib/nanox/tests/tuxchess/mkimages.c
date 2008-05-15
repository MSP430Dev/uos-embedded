#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mkdata (char *name)
{
	FILE *fd;
	char buf [128];
	int c, len;

	sprintf (buf, "images/%s.gif", name);
	fd = fopen (buf, "r");
	if (! fd) {
		perror (buf);
		exit (-1);
	}
	printf ("static const char %s_gif[] = {\n\t", name);
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

int main (void)
{
	/* Make embedded images. */
	mkdata ("board");
	mkdata ("w_p");
	mkdata ("w_n");
	mkdata ("w_b");
	mkdata ("w_r");
	mkdata ("w_k");
	mkdata ("w_q");
	mkdata ("b_p");
	mkdata ("b_n");
	mkdata ("b_b");
	mkdata ("b_r");
	mkdata ("b_k");
	mkdata ("b_q");

	return(0);
}
