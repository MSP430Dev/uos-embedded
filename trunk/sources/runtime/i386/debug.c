#include <runtime/lib.h>
#include <runtime/i386/int86.h>
#include <i8042/i8042.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

#define SCAN_CTRL	0x1D	/* Control */
#define SCAN_LSHIFT	0x2A	/* Left shift */
#define SCAN_RSHIFT	0x36	/* Right shift */
#define SCAN_ALT	0x38	/* Right shift */
#define SCAN_DEL	0x53	/* Del */

static int debug_char = -1;

static int ctrl, shift, alt, ctrl_alt_del;

static unsigned short *videomem = 0;
static int curs, cols, lines;
static int port = 0;

static void
scroll (unsigned short *to, unsigned short *from, int len)
{
	while (len-- > 0)
		*to++ = *from++;
}

static void (*hook) (void *arg, short c);
static void *hook_arg;

/*
 * Redirect debug output.
 */
void
debug_redirect (void (*func) (void*, short), void *arg)
{
	hook = func;
	hook_arg = arg;
}

/*
 * Put a character on the screen, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	MACHDEP_INTR_DISABLE (&x);

	if (hook) {
		hook (hook_arg, c);
		MACHDEP_INTR_RESTORE (x);
	}

	if (! videomem) {
		videomem = (unsigned short*) 0xb8000;
		port = 0x3D4;
		cols = 80;
		lines = 25;
		for (curs=0; curs<cols*lines; ++curs)
			videomem [curs] = 0x0700 | ' ';
		curs = 0;

		/* Set video origin. */
		outb (12, port); outb (0, port+1);
		outb (13, port); outb (0, port+1);

		/* Make cursor visible. */
		outb (10, port); outb (0, port+1);
		outb (11, port); outb (15, port+1);
	}

	/* Put character, move cursor. */
	/* TODO: unicode to utf8 conversion. */
	if (c == '\n')
		curs = (curs + cols) / cols * cols;
	else if (c == '\b') {
		if (curs % cols)
			--curs;
	} else
		videomem [curs++] = 0x0700 | c;

	/* Scroll the screen. */
	while (curs >= cols*lines) {
		int col;

		scroll (videomem, videomem + cols, (lines-1) * cols);
		for (col = 0; col < cols; col++)
			videomem [(lines - 1) * cols + col] = 0x0720;
		curs -= cols;
	}

	/* Set cursor position. */
	outb (14, port); outb (curs>>8, port+1);
	outb (15, port); outb (curs, port+1);

	MACHDEP_INTR_RESTORE (x);
}

static int get_scancode (int waitflag)
{
	unsigned char c, val;

	/* Get the scan code for the key. */
	while (! (inb (KBDC_AT_CTL) & KBSTS_DATAVL)) {
		if (! waitflag)
			return -1;
	}
	c = inb (KBD_DATA);

	/* Strobe the keyboard to ack the char. */
	val = inb (KBDC_XT_CTL);
	outb (val | KBDC_XT_CLEAR, KBDC_XT_CTL);
	outb (val, KBDC_XT_CTL);

	/*debug_printf ("scancode 0x%02x\n", c);*/
	return c;
}

static int modified_letter (int c)
{
	if (ctrl)
		return c & 0x1f;
	if (shift)
		return c + 'A' - 'a';
	return c;
}

static int scancode_to_unicode (int c)
{
again:
	switch (c) {
	default:
		return -1;

        case 0x0E: return ctrl ? 0x7f : '\b';	/* Backspace */
        case 0x1C: return ctrl ? '\n' : '\r';	/* Enter */
        case 0x01: return '\e';			/* Esc */
        case 0x39: return ' ';			/* Space */
        case 0x0F: return '\t';			/* Tab */

        case 0x1E: return modified_letter ('a');
        case 0x30: return modified_letter ('b');
        case 0x2E: return modified_letter ('c');
        case 0x20: return modified_letter ('d');
        case 0x12: return modified_letter ('e');
        case 0x21: return modified_letter ('f');
        case 0x22: return modified_letter ('g');
        case 0x23: return modified_letter ('h');
        case 0x17: return modified_letter ('i');
        case 0x24: return modified_letter ('j');
        case 0x25: return modified_letter ('k');
        case 0x26: return modified_letter ('l');
        case 0x32: return modified_letter ('m');
        case 0x31: return modified_letter ('n');
        case 0x18: return modified_letter ('o');
        case 0x19: return modified_letter ('p');
        case 0x10: return modified_letter ('q');
        case 0x13: return modified_letter ('r');
        case 0x1F: return modified_letter ('s');
        case 0x14: return modified_letter ('t');
        case 0x16: return modified_letter ('u');
        case 0x2F: return modified_letter ('v');
        case 0x11: return modified_letter ('w');
        case 0x2D: return modified_letter ('x');
        case 0x15: return modified_letter ('y');
        case 0x2C: return modified_letter ('z');

        case 0x02: return ctrl ? -1 :          shift ? '!' : '1';
        case 0x03: return ctrl ? ('@'&0x1f) :  shift ? '@' : '2';
        case 0x04: return ctrl ? -1 :          shift ? '#' : '3';
        case 0x05: return ctrl ? -1 :          shift ? '$' : '4';
        case 0x06: return ctrl ? -1 :          shift ? '%' : '5';
        case 0x07: return ctrl ? ('^'&0x1f) :  shift ? '^' : '6';
        case 0x08: return ctrl ? -1 :          shift ? '&' : '7';
        case 0x09: return ctrl ? -1 :          shift ? '*' : '8';
        case 0x0A: return ctrl ? -1 :          shift ? '(' : '9';
        case 0x0B: return ctrl ? -1 :          shift ? ')' : '0';
        case 0x0C: return ctrl ? ('_'&0x1f) :  shift ? '_' : '-';
        case 0x0D: return ctrl ? -1 :          shift ? '+' : '=';
        case 0x1A: return ctrl ? ('['&0x1f) :  shift ? '{' : '[';
        case 0x1B: return ctrl ? (']'&0x1f) :  shift ? '}' : ']';
        case 0x27: return ctrl ? -1 :          shift ? ':' : ';';
        case 0x28: return ctrl ? -1 :          shift ? '\'' : '\'';
        case 0x29: return ctrl ? -1 :          shift ? '~' : '`';
        case 0x2B: return ctrl ? ('\\'&0x1f) : shift ? '|' : '\\';
        case 0x33: return ctrl ? -1 :          shift ? '<' : ',';
        case 0x34: return ctrl ? -1 :          shift ? '>' : '.';

	case SCAN_CTRL:
		ctrl = 1;
		return -1;

	case SCAN_CTRL | 0x80:
		ctrl = 0;
		ctrl_alt_del = 0;
		return -1;

	case SCAN_LSHIFT:
	case SCAN_RSHIFT:
		shift = 1;
		return -1;

	case SCAN_LSHIFT | 0x80:
	case SCAN_RSHIFT | 0x80:
		shift = 0;
		return -1;

	case SCAN_ALT:
		alt = 1;
		return -1;

	case SCAN_ALT | 0x80:
		alt = 0;
		ctrl_alt_del = 0;
		return -1;

	case SCAN_DEL:
		if (ctrl && alt) {
			/* Reboot on Ctrl-Alt-Del. */
			++ctrl_alt_del;
			if (ctrl_alt_del >= 3) {
				debug_printf ("Rebooting...\n");
				i386_reboot (0x1234);
			}
		}
		return -1;

	case SCAN_DEL | 0x80:
		ctrl_alt_del = 0;
		return -1;

	case 0xE0:
		/* Extended key prefix. */
		c = get_scancode (1);
		goto again;

	case 0xE1:
		/* Pause key. */
		if (get_scancode (1) != 0x1D ||
		    get_scancode (1) != 0x45 ||
		    get_scancode (1) != 0xE1 ||
		    get_scancode (1) != 0x9D ||
		    get_scancode (1) != 0xC5)
			return -1;
		return -1;
	}
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	int c, x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
/*debug_printf ("getchar -> 0x%02x\n", c);*/
		return c;
	}
	MACHDEP_INTR_DISABLE (&x);
again:
	c = get_scancode (1);

	/* Perform make/break processing. */
	c = scancode_to_unicode (c);
	if (c < 0)
		goto again;
	/*debug_printf ("getchar -> 0x%02x\n", c);*/

	MACHDEP_INTR_RESTORE (x);
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	int c, x;

	if (debug_char >= 0)
		return debug_char;

	MACHDEP_INTR_DISABLE (&x);

	/* Get the scan code for the key. */
	c = get_scancode (0);

	/* Perform make/break processing. */
	if (c >= 0)
		c = scancode_to_unicode (c);

	MACHDEP_INTR_RESTORE (x);
	debug_char = c;
	return c;
}

void
debug_puts (const char *p)
{
	char c;

	for (;;) {
		c = *p;
		if (! c)
			return;
		debug_putchar (0, c);
		++p;
	}
}
