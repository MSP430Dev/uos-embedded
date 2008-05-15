#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <buf/buf.h>
#include <mem/mem.h>
#include <net/ip.h>
#include <net/tcp.h>

/*
 * Flush output buffer.
 */
static void
stream_flush (tcp_stream_t *u)
{
	int len = u->outptr - u->outdata;

/*debug_printf ("tstream output"); buf_print_data (u->outdata, len);*/
	while (! tcp_enqueue (u->socket, (void*) u->outdata, len, 0, 0, 0))
		lock_wait (&u->socket->lock);
	u->outptr = u->outdata;
}

static void
socket_flush (tcp_socket_t *s)
{
	/* Force IP level to send a packet. */
        lock_take (&s->ip->lock);
	tcp_output (s);
	lock_release (&s->ip->lock);
}

static void
tcp_stream_putchar (tcp_stream_t *u, short c)
{
	if (! u->socket)
		return;
	lock_take (&u->socket->lock);
	if (u->socket->state != SYN_SENT && u->socket->state != SYN_RCVD &&
	    u->socket->state != ESTABLISHED && u->socket->state != CLOSE_WAIT) {
		/* Connection was closed. */
		lock_release (&u->socket->lock);
		return;
	}
	/* Put byte into output buffer. */
	*u->outptr++ = c;
	if (u->outptr < u->outdata + sizeof(u->outdata)) {
		lock_release (&u->socket->lock);
		return;
	}
	stream_flush (u);
	lock_release (&u->socket->lock);

	/* Force IP level to send a packet. */
	socket_flush (u->socket);
}

static unsigned short
tcp_stream_getchar (tcp_stream_t *u)
{
	unsigned short c;

	if (! u->socket)
		return -1;
	lock_take (&u->socket->lock);

	/* Flush output buffer. */
	if (u->outptr > u->outdata) {
		stream_flush (u);
		lock_release (&u->socket->lock);
		socket_flush (u->socket);
		lock_take (&u->socket->lock);
	}

	/* Wait for data. */
	if (u->inbuf)
		lock_release (&u->socket->lock);
	else {
		while (tcp_queue_is_empty (u->socket)) {
			if (u->socket->state != SYN_SENT &&
			    u->socket->state != SYN_RCVD &&
			    u->socket->state != ESTABLISHED) {
				lock_release (&u->socket->lock);
				return -1;
			}
			lock_wait (&u->socket->lock);
		}
		u->inbuf = tcp_queue_get (u->socket);
		u->inptr = u->inbuf->payload;
		lock_release (&u->socket->lock);
/*debug_printf ("tstream input"); buf_print (u->inbuf);*/

		lock_take (&u->socket->ip->lock);
		if (! (u->socket->flags & TF_ACK_DELAY) &&
		    ! (u->socket->flags & TF_ACK_NOW)) {
			tcp_ack (u->socket);
		}
		lock_release (&u->socket->ip->lock);
	}

	/* Get byte from buffer. */
	c = *u->inptr++;
	if (u->inptr >= u->inbuf->payload + u->inbuf->len) {
		buf_t *old = u->inbuf;

		u->inbuf = old->next;
		if (u->inbuf) {
			u->inptr = u->inbuf->payload;
			old->next = 0;
		}
		buf_free (old);
	}
	return c;
}

static int
tcp_stream_peekchar (tcp_stream_t *u)
{
	if (! u->socket)
		return -1;
	lock_take (&u->socket->lock);

	/* Flush output buffer. */
	if (u->outptr > u->outdata) {
		stream_flush (u);
		lock_release (&u->socket->lock);
		socket_flush (u->socket);
		lock_take (&u->socket->lock);
	}

	/* Any data available? */
	if (u->inbuf)
		lock_release (&u->socket->lock);
	else {
		if (tcp_queue_is_empty (u->socket)) {
			lock_release (&u->socket->lock);
			return -1;
		}
		u->inbuf = tcp_queue_get (u->socket);
		u->inptr = u->inbuf->payload;
		lock_release (&u->socket->lock);

		lock_take (&u->socket->ip->lock);
		if (! (u->socket->flags & TF_ACK_DELAY) &&
		    ! (u->socket->flags & TF_ACK_NOW)) {
			tcp_ack (u->socket);
		}
		lock_release (&u->socket->ip->lock);
	}
	return *u->inptr;
}

static void
tcp_stream_flush (tcp_stream_t *u)
{
	if (! u->socket)
		return;
	lock_take (&u->socket->lock);

	/* Flush output buffer. */
	if (u->outptr <= u->outdata) {
		lock_release (&u->socket->lock);
		return;
	}
	stream_flush (u);
	lock_release (&u->socket->lock);

	/* Force IP level to send a packet. */
	socket_flush (u->socket);
}

static bool_t
tcp_stream_eof (tcp_stream_t *u)
{
	bool_t ret;

	if (! u->socket)
		return 1;
	lock_take (&u->socket->lock);
	ret = (u->socket->state != SYN_SENT && u->socket->state != SYN_RCVD &&
		u->socket->state != ESTABLISHED);
	lock_release (&u->socket->lock);
	return ret;
}

static void
tcp_stream_close (tcp_stream_t *u)
{
	if (! u->socket)
		return;
	lock_take (&u->socket->lock);
	lock_signal (&u->socket->lock, 0);
	lock_release (&u->socket->lock);

	tcp_close (u->socket);
	mem_free (u->socket);
	u->socket = 0;
}

/*
 * Returns an address of a lock, which is signaled on data receive.
 */
static lock_t *
tcp_stream_receiver (tcp_stream_t *u)
{
	if (! u->socket)
		return 0;
	return &u->socket->lock;
}

static stream_interface_t tcp_interface = {
	.putc = (void (*) (stream_t*, short))	tcp_stream_putchar,
	.getc = (unsigned short (*) (stream_t*))tcp_stream_getchar,
	.peekc = (int (*) (stream_t*))		tcp_stream_peekchar,
	.flush = (void (*) (stream_t*))		tcp_stream_flush,
	.eof = (bool_t (*) (stream_t*))		tcp_stream_eof,
	.close = (void (*) (stream_t*))		tcp_stream_close,
	.receiver = (lock_t *(*) (stream_t*))	tcp_stream_receiver,
};

/*
 * Initialize a stream adapter for TCP socket.
 */
stream_t *tcp_stream_init (tcp_stream_t *u, tcp_socket_t *sock)
{
	u->stream.interface = &tcp_interface;
	u->socket = sock;
	u->outptr = u->outdata;
	return &u->stream;
}
