/*
 * Ethernet driver over Linux tun/tap interface.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "tap/tap.h"

#if LINUX386
#   define _SYS_TYPES_H 1
#   define _STDINT_H 1

#   include <stdio.h>
#   include <unistd.h>
#   include <signal.h>
#   include <errno.h>

#   define __USE_GNU
#   include <fcntl.h>
#   include <sys/ioctl.h>
#   include <arpa/inet.h>
#   include <linux/if.h>
#   include <linux/if_tun.h>

#   define RECEIVE_IRQ		SIGIO	/* TAP receive complete */
#endif

#define TAP_MTU			1518	/* Max size = 1500+14+4 bytes */

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
bool_t
tap_output (tap_t *u, buf_t *p, small_uint_t prio)
{
	buf_t *q;
	unsigned char buf [TAP_MTU], *bufptr;

	/* Copy the data to imtermediate buffer. */
	bufptr = buf;
	for (q=p; q; q=q->next) {
		memcpy (bufptr, q->payload, q->len);
		bufptr += q->len;
	}
#if 0
	debug_printf ("tap tx:");
	if (u->netif.arp)
		buf_print_ethernet (p);
	else
		buf_print_ip (p);
#endif
	if (write (u->fd, buf, p->tot_len) != p->tot_len)
		/*ignore*/;

	mutex_lock (&u->netif.lock);
	++u->netif.out_packets;
	u->netif.out_bytes += p->tot_len;
	mutex_unlock (&u->netif.lock);
	buf_free (p);
	return 1;
}

static buf_t *
tap_input (tap_t *u)
{
	buf_t *p;

	mutex_lock (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	/*if (p) debug_printf ("tap_input returned %d bytes\n", p->tot_len);*/
	mutex_unlock (&u->netif.lock);
	return p;
}

static void
tap_set_address (tap_t *u, unsigned char *addr)
{
	mutex_lock (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);
	mutex_unlock (&u->netif.lock);
}

/*
 * Fetch and process all received data from the network controller.
 */
static void
tap_receive_data (tap_t *u)
{
	buf_t *p;
	int len;
	unsigned char data [TAP_MTU];

	for (;;) {
		/* Get received packet. */
#if LINUX386
		len = read (u->fd, data, TAP_MTU);
		if (len <= 0)
			return;
#endif
		++u->netif.in_packets;
		u->netif.in_bytes += len;

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
#if 0
		debug_printf ("taprcv:");
		if (u->netif.arp)
			buf_print_ethernet (p);
		else
			buf_print_ip (p);
#endif
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

	mutex_lock_irq (&u->netif.lock, RECEIVE_IRQ, 0, 0);
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
		mutex_wait (&u->netif.lock);
	}
}

static netif_interface_t tap_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
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
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));

#if LINUX386
	/* Do whatever else is needed to initialize interface. */
	u->pid = getpid ();

	u->fd = open("/dev/net/tun", O_RDWR | O_NDELAY);
	if (u->fd < 0) {
		perror ("Opening /dev/net/tun");
		exit (1);
	}

	struct ifreq ifr;
	memset (&ifr, 0, sizeof(ifr));
	if (arp) {
		/* Ethernet device.
		 * Activate the interface by command:
		 * ifconfig tap0 inet XXX.XXX.XXX.XXX
		 */
		ifr.ifr_flags = IFF_TAP;
	} else {
		/* Point-to-point IP device.
		 * Activate the interface by command:
		 * ifconfig tap0 inet XXX.XXX.XXX.XXX dstaddr YYY.YYY.YYY.YYY
		 */
		ifr.ifr_flags = IFF_TUN;
	}
	ifr.ifr_flags |= IFF_NO_PI;
	memcpy (ifr.ifr_name, name, IFNAMSIZ);
	if (ioctl (u->fd, TUNSETIFF, &ifr) < 0) {
		perror ("ioctl(TUNSETIFF)");
		exit (1);
	}

	/* Obtain MAC address from network interface. */
	if (ioctl (u->fd, SIOCGIFHWADDR, &ifr) < 0) {
		perror ("ioctl(SIOCGIFHWADDR)");
		exit (1);
	}
	memcpy (&u->netif.ethaddr, ifr.ifr_hwaddr.sa_data, 6);
#endif

	/* Create tap receive task. */
	task_create (tap_receiver, u, "tap", prio,
		u->stack, sizeof (u->stack));
}
