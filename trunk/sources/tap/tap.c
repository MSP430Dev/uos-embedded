#include "runtime/lib.h"
#include "kernel/uos.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "tap/tap.h"

#if LINUX386
#   define _SYS_TYPES_H 1

#   include <stdio.h>
#   include <unistd.h>
#   include <signal.h>
#   include <errno.h>

#   define __USE_GNU
#   include <fcntl.h>

#   define RECEIVE_IRQ		SIGIO	/* TAP receive complete */
#   define DEVNAME		"/dev/tap0"
#endif

#define TAP_MTU			1520	/* Max size = 1518 + 2 extra bytes */

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
unsigned char
tap_output (tap_t *u, buf_t *p, unsigned char prio)
{
	buf_t *q;
	unsigned char buf [TAP_MTU], *bufptr;
	int i;

	/* Add 16 extra bytes (/dev/tap feature). */
	memcpy (buf, "\0\0" "\376\375\0\0\0\0" "\0\0\0\0\0\0" "\10\0", 16);
	bufptr = buf + 16;

	/* Copy the data to imtermediate buffer. */
	for (q=p; q; q=q->next) {
		memcpy (bufptr, q->payload, q->len);
		bufptr += q->len;
	}
	debug_printf ("tap_output %d bytes", p->tot_len);
	debug_printf (": %02x", buf[16]);
	for (i=1; i<p->tot_len; ++i)
		debug_printf ("-%02x", buf [i+16]);
	debug_printf ("\n");

	write (u->fd, buf, p->tot_len + 16);

	lock_take (&u->netif.lock);
	++u->netif.out_packets;
	u->netif.out_bytes += p->tot_len;
	lock_release (&u->netif.lock);
	buf_free (p);
	return 1;
}

static buf_t *
tap_input (tap_t *u)
{
	buf_t *p;

	lock_take (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	/*if (p) debug_printf ("tap_input returned %d bytes\n", p->tot_len);*/
	lock_release (&u->netif.lock);
	return p;
}

static void
tap_set_address (tap_t *u, unsigned char *addr)
{
	lock_take (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);
	lock_release (&u->netif.lock);
}

/*
 * Fetch and process all received data from the network controller.
 */
static void
tap_receive_data (tap_t *u)
{
	unsigned char *data;
	buf_t *p;
	int len, i;

	for (;;) {
		/* Get received packet. */
#if LINUX386
		unsigned char buf [TAP_MTU];

		len = read (u->fd, buf, TAP_MTU);
		if (len <= 0)
			return;
		if (len <= 16) {
			++u->netif.in_errors;
			continue;
		}

		/* Remove 2 extra bytes (/dev/tap feature). */
		data = buf + 16;
		len -= 16;
#endif
		++u->netif.in_packets;
		u->netif.in_bytes += len;
		debug_printf ("tap_receive %d bytes", len);
		debug_printf (": %02x", data[0]);
		for (i=1; i<len; ++i)
			debug_printf ("-%02x", data [i]);
		debug_printf ("\n");

		if (buf_queue_is_full (&u->inq)) {
			debug_printf ("tap_receiver: input overflow\n");
			++u->netif.in_discards;
			continue;
		}

		p = buf_alloc (u->pool, len, 2);
		if (! p) {
			debug_printf ("tap_receiver: ignore packet - out of memory\n");
			++u->netif.in_discards;
			continue;
		}
		memcpy (p->payload, data, len);

		buf_queue_put (&u->inq, p);
	}
}

/*
 * Receive interrupt task.
 */
static void
tap_receiver (void *arg)
{
	tap_t *u = arg;

	lock_take_irq (&u->netif.lock, RECEIVE_IRQ, 0, 0);
#if LINUX386
	/* Enable receiver interrupt. */
        fcntl (u->fd, F_SETOWN, u->pid);
        fcntl (u->fd, F_SETSIG, RECEIVE_IRQ);
	fcntl (u->fd, F_SETFL, fcntl (u->fd, F_GETFL, 0) | O_ASYNC);
#endif
	for (;;) {
		/* Process all available received data. */
		tap_receive_data (u);

		/* Wait for the receive interrupt. */
		lock_wait (&u->netif.lock);
	}
}

static netif_interface_t tap_interface = {
	(unsigned char (*) (netif_t*, buf_t*, unsigned char))
						tap_output,
	(buf_t *(*) (netif_t*))			tap_input,
	(void (*) (netif_t*, unsigned char*))	tap_set_address,
};

/*
 * Set up the network interface.
 */
void
tap_init (tap_t *u, char *name, int prio, mem_pool_t *pool, arp_t *arp)
{
	u->netif.interface = &tap_interface;
	u->netif.name = name;
	u->netif.arp = 0;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));

	/* Obtain MAC address from network interface.
	 * We just fake an address... */
	memcpy (&u->netif.ethaddr, "\1\2\3\4\5\6", 6);

#if LINUX386
	/* Do whatever else is needed to initialize interface. */
	u->pid = getpid ();

	/* Activate the interface by command:
	 * ifconfig tap0 inet XXX.XXX.XXX.XXX
	 */
	u->fd = open (DEVNAME, O_RDWR | O_NDELAY);
	if (u->fd == -1) {
		perror (DEVNAME);
		exit (1);
	}
#endif

	/* Create tap receive task. */
	task_create (tap_receiver, u, "tap", prio,
		u->stack, sizeof (u->stack));
}
