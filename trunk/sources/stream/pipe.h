/*
 * Pipe - communitation channel with two symmetrical stream interfaces.
 */
typedef struct _pipe_t {
	stream_t slave;
	stream_t master;

	mutex_t lock;

	unsigned char master_closed;
	unsigned char slave_closed;
	unsigned short fifo_chars;

	/* From master to slave. */
	unsigned short *out_fifo;
	unsigned short *out_first;
	unsigned short *out_last;

	/* From slave to master. */
	unsigned short *in_fifo;
	unsigned short *in_first;
	unsigned short *in_last;

	unsigned short data [4];
	/* More data space here. */
} pipe_t;

pipe_t *pipe_init (char *buf, int bytes, stream_t **master, stream_t **slave);
