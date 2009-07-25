#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <stream/pipe.h>

#define MASTER_TO_PIPE(m)	((pipe_t*) ((m) - 1))
#define SLAVE_TO_PIPE(s)	((pipe_t*) (s))

static void
master_putchar (stream_t *master, short c)
{
	pipe_t *u = MASTER_TO_PIPE (master);
	unsigned short *next;

	mutex_lock (&u->lock);

	/* Wait for free space in FIFO. */
	for (;;) {
		if (u->slave_closed) {
			mutex_unlock (&u->lock);
			return;
		}
		next = u->out_last + 1;
		if (next >= u->out_fifo + u->fifo_chars)
			next = u->out_fifo;

		if (next != u->out_first)
			break;

		mutex_wait (&u->lock);
	}

	/* Put byte into FIFO. */
	*u->out_last = c;
	u->out_last = next;
	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
}

static void
slave_putchar (stream_t *slave, short c)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);
	unsigned short *next;

	mutex_lock (&u->lock);

	/* Wait for free space in FIFO. */
	for (;;) {
		if (u->master_closed) {
			mutex_unlock (&u->lock);
			return;
		}
		next = u->in_last + 1;
		if (next >= u->in_fifo + u->fifo_chars)
			next = u->in_fifo;

		if (next != u->in_first)
			break;

		mutex_wait (&u->lock);
	}

	/* Put byte into FIFO. */
	*u->in_last = c;
	u->in_last = next;
	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
}

static unsigned short
master_getchar (stream_t *master)
{
	pipe_t *u = MASTER_TO_PIPE (master);
	unsigned short c;

	mutex_lock (&u->lock);

	/* Wait for data in FIFO. */
	while (u->in_first == u->in_last) {
		if (u->slave_closed) {
			mutex_unlock (&u->lock);
			return -1;
		}
		mutex_wait (&u->lock);
	}

	/* Get byte from FIFO. */
	c = *u->in_first++;
	if (u->in_first >= u->in_fifo + u->fifo_chars)
		u->in_first = u->in_fifo;

	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
	return c;
}

static unsigned short
slave_getchar (stream_t *slave)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);
	unsigned short c;

	mutex_lock (&u->lock);

	/* Wait for data in FIFO. */
	while (u->out_first == u->out_last) {
		if (u->master_closed) {
			mutex_unlock (&u->lock);
			return -1;
		}
		mutex_wait (&u->lock);
	}

	/* Get byte from FIFO. */
	c = *u->out_first++;
	if (u->out_first >= u->out_fifo + u->fifo_chars)
		u->out_first = u->out_fifo;

	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
	return c;
}

static int
master_peekchar (stream_t *master)
{
	pipe_t *u = MASTER_TO_PIPE (master);
	unsigned short c;

	mutex_lock (&u->lock);

	/* Is fifo empty? */
	if (u->in_first == u->in_last) {
		mutex_unlock (&u->lock);
		return -1;
	}
	c = *u->in_first;

	mutex_unlock (&u->lock);
	return c;
}

static int
slave_peekchar (stream_t *slave)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);
	unsigned short c;

	mutex_lock (&u->lock);

	/* Is fifo empty? */
	if (u->out_first == u->out_last) {
		mutex_unlock (&u->lock);
		return -1;
	}
	c = *u->out_first;

	mutex_unlock (&u->lock);
	return c;
}

static void
master_flush (stream_t *master)
{
	pipe_t *u = MASTER_TO_PIPE (master);

	mutex_lock (&u->lock);

	/* Wait until output FIFO becomes empty. */
	while (u->out_first != u->out_last && ! u->slave_closed)
		mutex_wait (&u->lock);

	mutex_unlock (&u->lock);
}

static void
slave_flush (stream_t *slave)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);

	mutex_lock (&u->lock);

	/* Wait until input FIFO becomes empty. */
	while (u->in_first != u->in_last && ! u->master_closed)
		mutex_wait (&u->lock);

	mutex_unlock (&u->lock);
}

static bool_t
master_eof (stream_t *master)
{
	pipe_t *u = MASTER_TO_PIPE (master);
	bool_t ret;

	mutex_lock (&u->lock);
	ret = u->slave_closed;
	mutex_unlock (&u->lock);
	return ret;
}

static bool_t
slave_eof (stream_t *slave)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);
	bool_t ret;

	mutex_lock (&u->lock);
	ret = u->master_closed;
	mutex_unlock (&u->lock);
	return ret;
}

static void
master_close (stream_t *master)
{
	pipe_t *u = MASTER_TO_PIPE (master);

	mutex_lock (&u->lock);

	u->master_closed = 1;
	u->in_first = u->in_last;
	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
}

static void
slave_close (stream_t *slave)
{
	pipe_t *u = SLAVE_TO_PIPE (slave);

	mutex_lock (&u->lock);

	u->slave_closed = 1;
	u->out_first = u->out_last;
	mutex_signal (&u->lock, 0);

	mutex_unlock (&u->lock);
}

static stream_interface_t master_interface = {
	.putc = (void (*) (stream_t*, short))	master_putchar,
	.getc = (unsigned short (*) (stream_t*))	master_getchar,
	.peekc = (int (*) (stream_t*))		master_peekchar,
	.flush = (void (*) (stream_t*))		master_flush,
	.eof = (bool_t (*) (stream_t*))		master_eof,
	.close = (void (*) (stream_t*))		master_close,
};

static stream_interface_t slave_interface = {
	.putc = (void (*) (stream_t*, short))	slave_putchar,
	.getc = (unsigned short (*) (stream_t*))	slave_getchar,
	.peekc = (int (*) (stream_t*))		slave_peekchar,
	.flush = (void (*) (stream_t*))		slave_flush,
	.eof = (bool_t (*) (stream_t*))		slave_eof,
	.close = (void (*) (stream_t*))		slave_close,
};

/*
 * Create pipe. Use the buffer of given size.
 * Return pointers to master and slave interfaces.
 */
pipe_t *pipe_init (char *buf, int bytes, stream_t **master, stream_t **slave)
{
	pipe_t *u;

	memset (buf, 0, bytes);
	u = (pipe_t*) buf;

	*master = &u->master;
	*slave = &u->slave;
	u->slave.interface = &slave_interface;
	u->master.interface = &master_interface;

	u->fifo_chars = (bytes - sizeof (pipe_t) + sizeof(u->data)) /
		sizeof (unsigned short) / 2;
	assert (u->fifo_chars > 1);

	u->out_fifo = u->data;
	u->in_fifo = u->data + u->fifo_chars;

	u->out_first = u->out_fifo;
	u->out_last = u->out_fifo;

	u->in_first = u->in_fifo;
	u->in_last = u->in_fifo;

	return u;
}
