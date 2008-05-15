#include <sys/types.h>
#include <sys/reboot.h>		/* for RB_SERIAL */

#include <machine/cpufunc.h>	/* for inb/outb */

short *videomem;
int curs;
int cols;
int lines;
unsigned int port;

void putchar (unsigned char c)
{
	switch (c) {
	case '\n':      curs = (curs + cols) / cols * cols;     break;
	default:        videomem[curs++] = 0x0700 | c;          break;
	}
	while (curs >= cols*lines) {
		int col;

		memcpy (videomem, videomem+cols, (lines-1) * cols * 2);
		for (col = 0; col < cols; col++)
			videomem[(lines - 1) * cols + col] = 0x720;
		curs -= cols;
	}
	/* set cursor position */
	outb (port, 0x0e); outb (port+1, curs>>8);
	outb (port, 0x0f); outb (port+1, curs);
}

void boot (int howto)
{
	int l, c;

	/* Test for monochrome video adapter */
	if ((*((unsigned char*) 0x410) & 0x30) == 0x30)
		videomem = (void*) 0xb0000;     /* monochrome */
	else
		videomem = (void*) 0xb8000;     /* color */

	port = *(unsigned short*) 0x463;
	cols = *(unsigned short*) 0x44a;
	lines = 1 + *(unsigned char*) 0x484;
	c = *(unsigned char*) 0x450;
	l = *(unsigned char*) 0x451;

	if (lines < 25)
		lines = 25;
	curs = l*cols + c;
	if (curs > lines*cols)
		curs = (lines-1) * cols;
}
