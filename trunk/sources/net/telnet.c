#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/telnet.h>

/*
 * Definitions for the TELNET protocol.
 */
#define	IAC	255		/* interpret as command: */
#define	DONT	254		/* you are not to use option */
#define	DO	253		/* please, you use option */
#define	WONT	252		/* I won't use option */
#define	WILL	251		/* I will use option */
#define	SB	250		/* interpret as subnegotiation */
#define	AYT	246		/* are you there */
#define	NOP	241		/* nop */

/*
 * Telnet options
 */
#define TELOPT_BINARY	0	/* 8-bit data path */
#define TELOPT_ECHO	1	/* echo */
#define	TELOPT_SGA	3	/* suppress go ahead */
#define	TELOPT_TM	6	/* timing mark */

/*
 * Get/set local and remote options.
 */
static void
set_local_option (telnet_t *u, int opt)
{
	u->local_option [opt >> 3] |= 1 << (opt & 7);
}

static void
clear_local_option (telnet_t *u, int opt)
{
	u->local_option [opt >> 3] &= ~(1 << (opt & 7));
}

static int
get_local_option (telnet_t *u, int opt)
{
	return (u->local_option [opt >> 3] >> (opt & 7)) & 1;
}

static void
set_remote_option (telnet_t *u, int opt)
{
	u->remote_option [opt >> 3] |= 1 << (opt & 7);
}

static void
clear_remote_option (telnet_t *u, int opt)
{
	u->remote_option [opt >> 3] &= ~(1 << (opt & 7));
}

static int
get_remote_option (telnet_t *u, int opt)
{
	return (u->remote_option [opt >> 3] >> (opt & 7)) & 1;
}

/*
 * Send reply to remote host
 */
static void
reply (telnet_t *u, int cmd, int opt)
{
	putchar (&u->ts.stream, IAC);
	putchar (&u->ts.stream, cmd);
	putchar (&u->ts.stream, opt);
}

static void
will_option (telnet_t *u, int opt)
{
	int cmd;

	switch (opt) {
	case TELOPT_BINARY:
	case TELOPT_ECHO:
	case TELOPT_SGA:
		set_remote_option (u, opt);
		cmd = DO;
		break;

	case TELOPT_TM:
	default:
		cmd = DONT;
		break;
	}
	reply (u, cmd, opt);
}

static void
wont_option (telnet_t *u, int opt)
{
	switch (opt) {
	case TELOPT_ECHO:
	case TELOPT_BINARY:
	case TELOPT_SGA:
		clear_remote_option (u, opt);
		break;
	}
	reply (u, DONT, opt);
}

static void
do_option (telnet_t *u, int opt)
{
	int cmd;

	switch (opt) {
	case TELOPT_ECHO:
	case TELOPT_BINARY:
	case TELOPT_SGA:
		set_local_option (u, opt);
		cmd = WILL;
		break;

	case TELOPT_TM:
	default:
		cmd = WONT;
		break;
	}
	reply (u, cmd, opt);
}

static void
dont_option (telnet_t *u, int opt)
{
	switch (opt) {
	case TELOPT_ECHO:
	case TELOPT_BINARY:
	case TELOPT_SGA:
		clear_local_option (u, opt);
		break;
	}
	reply (u, WONT, opt);
}

static void
telnet_command (telnet_t *u, int c)
{
	int i;

	switch (c) {
	case AYT:
		/* Send reply to AYT ("Are You There") request */
		putchar (&u->ts.stream, IAC);
		putchar (&u->ts.stream, NOP);
		break;
	case WILL:
		/* IAC WILL received (get next character) */
		c = getchar (&u->ts.stream);
		if (c >= 0 && c < 256 && ! get_remote_option (u, c))
			will_option (u, c);
		break;
	case WONT:
		/* IAC WONT received (get next character) */
		c = getchar (&u->ts.stream);
		if (c >= 0 && c < 256 && get_remote_option (u, c))
			wont_option (u, c);
		break;
	case DO:
		/* IAC DO received (get next character) */
		c = getchar (&u->ts.stream);
		if (c >= 0 && c < 256 && ! get_local_option (u, c))
			do_option (u, c);
		break;
	case DONT:
		/* IAC DONT received (get next character) */
		c = getchar (&u->ts.stream);
		if (c >= 0 && c < 256 && get_local_option (u, c))
			dont_option (u, c);
		break;
	case SB:
		for (i=0; i<128; i++) {
			c = getchar (&u->ts.stream);
			if (c == IAC) {
				c = getchar (&u->ts.stream);
				break;
			}
		}
		break;
	}
}

/*
 * Telnet stream interface is implemented as a proxy to tcp-stream.
 */
static void
telnet_putchar (telnet_t *u, short c)
{
	if (c == '\n')
		putchar (&u->ts.stream, '\r');
	putchar (&u->ts.stream, c);

	/* IAC -> IAC IAC */
	if (c == IAC)
		putchar (&u->ts.stream, c);
}

static unsigned short
telnet_getchar (telnet_t *u)
{
	int c;
again:
	c = getchar (&u->ts.stream);
	if (c == (unsigned short)-1)
		return -1;

	if (c == '\r') {
		c = peekchar (&u->ts.stream);
		if (c == -1)
			return '\n';		/* no characters in buffer */

		if (c == '\0' || c == '\n') {
			getchar (&u->ts.stream); /* drop character */
			return '\n';
		}
	}
	if (c == IAC) {
		c = getchar (&u->ts.stream);
		if (c == (unsigned short)-1)
			return -1;

		if (c != IAC) {
			telnet_command (u, c);
			goto again;
		}
	}
	return c;
}

static int
telnet_peekchar (telnet_t *u)
{
	return peekchar (&u->ts.stream);
}

static void
telnet_flush (telnet_t *u)
{
	fflush (&u->ts.stream);
}

static bool_t
telnet_eof (telnet_t *u)
{
	return feof (&u->ts.stream);
}

static void
telnet_close (telnet_t *u)
{
	fclose (&u->ts.stream);
	mem_free (u);
}

static mutex_t *
telnet_receiver (telnet_t *u)
{
	return freceiver (&u->ts.stream);
}

static stream_interface_t telnet_interface = {
	.putc = (void (*) (stream_t*, short))	telnet_putchar,
	.getc = (unsigned short (*) (stream_t*))telnet_getchar,
	.peekc = (int (*) (stream_t*))		telnet_peekchar,
	.flush = (void (*) (stream_t*))		telnet_flush,
	.eof = (bool_t (*) (stream_t*))		telnet_eof,
	.close = (void (*) (stream_t*))		telnet_close,
	.receiver = (mutex_t *(*) (stream_t*))	telnet_receiver,
};

/*
 * Initialize a telnet control block.
 */
stream_t *telnet_init (tcp_socket_t *sock)
{
	telnet_t *u;

	u = mem_alloc (sock->ip->pool, sizeof (*u));
	if (! u)
		return 0;
	u->stream.interface = &telnet_interface;
	tcp_stream_init (&u->ts, sock);

	/* Setup telnet options */
	do_option (u, TELOPT_ECHO);
	do_option (u, TELOPT_SGA);
	will_option (u, TELOPT_SGA);
	fflush (&u->ts.stream);

	/* We will receive:
	 * ff-fd-03 - will_option TELOPT_SGA
	 * ff-fb-18 - do_option TELOPT_TTYPE, reply "dont"
	 * ff-fb-1f - do_option TELOPT_NAWS, reply "dont"
	 * ff-fb-20 - do_option TELOPT_TSPEED, reply "dont"
	 * ff-fb-21 - do_option TELOPT_LFLOW, reply "dont"
	 * ff-fb-22 - do_option TELOPT_LINEMODE, reply "dont"
	 * ff-fb-27 - do_option TELOPT_NEW_ENVIRON, reply "dont"
	 * ff-fd-05 - will_option TELOPT_STATUS, reply "wont"
	 */
	return &u->stream;
}
