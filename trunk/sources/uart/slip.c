#include "runtime/lib.h"
#include "kernel/uos.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "uart/slip.h"

#if __AVR__
#   include "uart/avr.h"
#endif

#if ARM_S3C4530
#   include "uart/samsung.h"
#endif

#if ELVEES_MC24
#   include "uart/elvees.h"
#endif

#if LINUX386
#   include "uart/linux.h"
#endif

#define SLIP_FLAG		0300
#define SLIP_ESC		0333
#define SLIP_ESC_FLAG		0334
#define SLIP_ESC_ESC		0335

static bool_t
slip_get_cts (slip_t *u)
{
	return u->cts_query == 0 || u->cts_query (u);
}

/*
 * Start transmitting a byte.
 * Assume the transmitter is stopped, and the transmit queue is not empty.
 * Return 1 when we are expecting the hardware interrupt.
 */
static bool_t
slip_transmit_start (slip_t *u)
{
	unsigned char c;

	/* Check that transmitter buffer is busy. */
	if (! test_transmitter_empty (u->port))
		return 1;

	/* Nothing to transmit or no CTS - stop transmitting. */
	if ((! u->out_flag && ! u->out) || ! slip_get_cts (u)) {
		/* Disable `transmitter empty' interrupt. */
		disable_transmit_interrupt (u->port);
		return 0;
	}

	if (u->out_flag) {
		c = SLIP_FLAG;
		u->out_flag = 0;
	} else {
		c = *u->out_first;
		switch (c) {
		case SLIP_FLAG:
			c = SLIP_ESC;
			*u->out_first = SLIP_ESC_FLAG;
			break;
		case SLIP_ESC:
			c = SLIP_ESC;
			*u->out_first = SLIP_ESC_ESC;
			break;
		default:
			++u->out_first;
			break;
		}
		if (u->out_first >= u->out_limit) {
			u->outseg = u->outseg->next;
			if (u->outseg) {
				u->out_first = u->outseg->payload;
				u->out_limit = u->outseg->payload + u->outseg->len;
			} else {
				/* Last segment transmitted. */
				++u->netif.out_packets;
				u->netif.out_bytes += u->out->tot_len;
/*				debug_printf ("slip: transmitted %d bytes\n", u->out->tot_len);*/
				u->out_free = u->out;
				u->out = 0;
				u->out_flag = 1;
			}
		}
	}

	transmit_byte (u->port, c);

	/* Enable `transmitter empty' interrupt. */
	enable_transmit_interrupt (u->port);
	return 1;
}

/*
 * CTS is active - wake up the transmitter.
 */
void
slip_cts_ready (slip_t *u)
{
	lock_take (&u->transmitter);
	slip_transmit_start (u);
	lock_release (&u->transmitter);
}

/*
 * Register the CTS poller function.
 */
void
slip_set_cts_poller (slip_t *u, bool_t (*func) (slip_t*))
{
	lock_take (&u->transmitter);
	u->cts_query = func;
	lock_release (&u->transmitter);
}

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This buf
 * might be chained.
 */
bool_t
slip_output (slip_t *u, buf_t *p, small_uint_t prio)
{
/*	debug_printf ("slip_output: transmit %d bytes\n", p->tot_len);*/
	lock_take (&u->transmitter);

	if (u->out) {
		/* Занято, ставим в очередь. */
		if (buf_queue_is_full (&u->outq)) {
			++u->netif.out_discards;
			lock_release (&u->transmitter);
/*			debug_printf ("slip_output: overflow\n");*/
			buf_free (p);
			return 0;
		}
		buf_queue_put (&u->outq, p);
	} else {
		u->out = p;
		u->outseg = p;
		u->out_first = u->outseg->payload;
		u->out_limit = u->outseg->payload + u->outseg->len;
		u->out_flag = 1;
		slip_transmit_start (u);
	}
	lock_release (&u->transmitter);
	return 1;
}

static buf_t *
slip_input (slip_t *u)
{
	buf_t *p;

	lock_take (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	lock_release (&u->netif.lock);
	return p;
}

/*
 * Get the received packet. Blocks the task until the data is available.
 */
buf_t *
slip_recv (slip_t *u)
{
	buf_t *p;

	lock_take (&u->netif.lock);
	for (;;) {
		p = buf_queue_get (&u->inq);
		if (p) {
			lock_release (&u->netif.lock);
			return p;
		}
		lock_wait (&u->netif.lock);
	}
}

/*
 * Send the packet. Blocks the task until the packet is enqueued.
 */
bool_t
slip_send (slip_t *u, buf_t *p)
{
	lock_take (&u->transmitter);
	while (u->out && buf_queue_is_full (&u->outq))
		lock_wait (&u->transmitter);

	if (u->out) {
		/* Занято, ставим в очередь. */
		buf_queue_put (&u->outq, p);
	} else {
		u->out = p;
		u->outseg = p;
		u->out_first = u->outseg->payload;
		u->out_limit = u->outseg->payload + u->outseg->len;
		u->out_flag = 1;
		slip_transmit_start (u);
	}
	lock_release (&u->transmitter);
	return 1;
}

/*
 * Fast receive interrupt handler.
 * Fetch and process all received data from the network controller.
 */
static int
slip_receive_data (slip_t *u)
{
	unsigned char c = 0;

	for (;;) {
		if (test_frame_error (u->port)) {
/*debug_putchar (0, '%');*/
/*			debug_printf ("slip: FRAME ERROR\n");*/
			/* Check that receive data is available,
			 * and get the received byte. */
			c = get_received_byte (u->port);
			continue;
		}
		if (test_parity_error (u->port)) {
/*debug_putchar (0, '@');*/
/*			debug_printf ("slip: PARITY ERROR\n");*/
			c = get_received_byte (u->port);
			continue;
		}
		if (test_overrun_error (u->port)) {
/*debug_putchar (0, '#');*/
/*			debug_printf ("slip: RECEIVE OVERRUN\n");*/
			c = get_received_byte (u->port);
			continue;
		}

		/* Check that receive data is available,
		 * and get the received byte. */
		if (! test_get_receive_data (u->port, &c)) {
			enable_receive_interrupt (u->port);
			return 1;
		}

/*debug_printf ("%x\n", c);*/
		if (! u->in_ptr)
			continue;

		switch (c) {
		case SLIP_FLAG:
			u->in_escape = 0;
			if (u->in_ptr > u->in->payload) {
				/* Received whole packet. */
				return 0;
			}
			break;

		case SLIP_ESC:
			u->in_escape = 1;
			break;

		default:
			if (u->in_escape) {
				u->in_escape = 0;
				switch (c) {
				case SLIP_ESC_FLAG:
					c = SLIP_FLAG;
					break;
				case SLIP_ESC_ESC:
					c = SLIP_ESC;
					break;
				}
			}
			if (u->in_ptr >= u->in_limit) {
				/* Ignore input on buffer overflow. */
/*debug_putchar (0, '!');*/
/*				debug_printf ("slip_receive_data: ignore packet - buffer overflow\n");*/
				++u->netif.in_errors;
				u->in_ptr = u->in->payload;
				break;
			}
			*u->in_ptr++ = c;
			++u->netif.in_bytes;
			break;
		}
	}
}

/*
 * Receive interrupt task.
 */
static void
slip_receiver (void *arg)
{
	slip_t *u = arg;
	unsigned short len;

	/* Start receiver. */
	lock_take_irq (&u->netif.lock, RECEIVE_IRQ (u->port),
		(handler_t) slip_receive_data, u);

	enable_receiver (u->port);
	enable_receive_interrupt (u->port);

	for (;;) {
		if (! u->in_ptr) {
			/* Allocate buffer for receive data. */
			u->in = buf_alloc (u->pool, u->netif.mtu, 16);
			if (u->in) {
				u->in_ptr = u->in->payload;
				u->in_limit = u->in_ptr + u->netif.mtu;
			} else {
				/* No buffer - ignore input. */
/*				debug_printf ("slip_receiver: out of memory\n");*/
				++u->netif.in_discards;
			}
		}

		/* Wait for the receive interrupt. */
		lock_wait (&u->netif.lock);

		/* Process all available received data. */
		if (u->in_ptr && u->in_ptr > u->in->payload) {
			len = u->in_ptr - u->in->payload;
/*			debug_printf ("slip_receiver(%ld): received %d bytes\n", u->netif.in_packets, len);*/
			buf_truncate (u->in, len);
			++u->netif.in_packets;

			if (buf_queue_is_full (&u->inq)) {
/*				debug_printf ("slip_receiver: input overflow\n");*/
				++u->netif.in_discards;

				/* Reuse the packet. */
				u->in_ptr = u->in->payload;
				u->in_limit = u->in_ptr + u->netif.mtu;
			} else {
				/* Enqueue the received packet. */
				buf_queue_put (&u->inq, u->in);
				u->in_ptr = 0;
			}
		}
	}
}

/*
 * Transmit interrupt task.
 */
static void
slip_transmitter (void *arg)
{
	slip_t *u = arg;

#ifdef TRANSMIT_IRQ
	/* Start transmitter. */
	lock_take_irq (&u->transmitter, TRANSMIT_IRQ (u->port),
		(handler_t) slip_transmit_start, u);

	enable_transmitter (u->port);
#else
	/* TODO */
#endif

	for (;;) {
		/* Wait for the transmit interrupt. */
		lock_wait (&u->transmitter);
		if (u->out_free) {
			buf_free (u->out_free);
			u->out_free = 0;
			if (u->out)
				continue;

			/* Ставим на передачу пакет из очереди. */
			u->out = buf_queue_get (&u->outq);
			if (! u->out)
				continue;
			u->outseg = u->out;
			u->out_first = u->outseg->payload;
			u->out_limit = u->outseg->payload + u->outseg->len;
			u->out_flag = 1;
			slip_transmit_start (u);
		}
	}
}

static netif_interface_t slip_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						slip_output,
	(buf_t *(*) (netif_t*))			slip_input,
};

void
slip_init (slip_t *u, small_uint_t port, const char *name, int prio,
	mem_pool_t *pool, unsigned int khz, unsigned long baud)
{
	u->netif.interface = &slip_interface;
	u->netif.name = name;
	u->netif.arp = 0;
	u->netif.mtu = 552;
	u->netif.type = NETIF_SLIP;
	u->netif.bps = baud;
	u->pool = pool;
	u->port = port;
	u->khz = khz;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));
	buf_queue_init (&u->outq, u->outqdata, sizeof (u->outqdata));

	/* Setup baud rate generator. */
	setup_baud_rate (u->port, u->khz, baud);

	/* Create slip receive task. */
	task_create (slip_receiver, u, "slipr", prio + 1,
		u->rstack, sizeof (u->rstack));

	/* Create slip transmit task. */
	task_create (slip_transmitter, u, "slipt", prio,
		u->tstack, sizeof (u->tstack));
}
