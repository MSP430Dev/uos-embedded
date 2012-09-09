#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <crc/crc16-inet.h>
#include <net/netif.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/udp.h>

typedef struct _udp_hdr_t {
	unsigned char src_h, src_l;		/* source UDP port */
	unsigned char dest_h, dest_l;		/* destination UDP port */
	unsigned char len_h, len_l;		/* packet length */
	unsigned char chksum_h, chksum_l;	/* packet checksum */
} udp_hdr_t;

#define UDP_HLEN	8		/* UDP header length */

static buf_t *
udp_queue_get (udp_socket_t *q, unsigned char *paddr,
	unsigned short *pport)
{
	buf_t *p;
	udp_socket_queue_t *head;

	if (q->count == 0) {
		/*debug_printf ("udp_queue_get: returned 0\n");*/
		return 0;
	}
	assert (q->head >= q->queue);
	assert (q->head < q->queue + UDP_SOCKET_QUEUE_SIZE);
	assert (q->head->buf != 0);

	/* Get the first packet from queue. */
	head = q->head;
	p = head->buf;
	if (paddr)
		memcpy (paddr, head->addr, 4);
	if (pport)
		*pport = head->port;

	/* Advance head pointer. */
	++q->head;
	--q->count;
	if (q->head >= q->queue + UDP_SOCKET_QUEUE_SIZE)
		q->head = q->queue;

	/*debug_printf ("udp_queue_get: returned 0x%04x\n", p);*/
	return p;
}

static inline void
udp_queue_free (udp_socket_t *q)
{
	while (q->count > 0) {
		assert (q->head >= q->queue);
		assert (q->head < q->queue + UDP_SOCKET_QUEUE_SIZE);
		assert (q->head->buf != 0);

		/* Remove packet from queue. */
		buf_free (q->head->buf);

		/* Advance head pointer. */
		++q->head;
		--q->count;
		if (q->head >= q->queue + UDP_SOCKET_QUEUE_SIZE)
			q->head = q->queue;
	}
}

static bool_t
udp_queue_is_full (udp_socket_t *q)
{
	/*debug_printf ("udp_queue_is_full: returned %d\n", (q->count == UDP_SOCKET_QUEUE_SIZE));*/
	return (q->count == UDP_SOCKET_QUEUE_SIZE);
}

static bool_t
udp_queue_is_empty (udp_socket_t *q)
{
	/*debug_printf ("udp_queue_is_full: returned %d\n", (q->count == UDP_SOCKET_QUEUE_SIZE));*/
	return (q->count == 0);
}

static void
udp_queue_put (udp_socket_t *q, buf_t *p,
	unsigned char *addr, unsigned short port)
{
	udp_socket_queue_t *tail;

	/*debug_printf ("udp_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < UDP_SOCKET_QUEUE_SIZE);

	if (q->head == 0)
		q->head = q->queue;

	/* Compute the last place in the queue. */
	tail = q->head + q->count;
	if (tail >= q->queue + UDP_SOCKET_QUEUE_SIZE)
		tail -= UDP_SOCKET_QUEUE_SIZE;

	/* Put the packet in. */
	tail->buf = p;
	tail->port = port;
	memcpy (tail->addr, addr, 4);
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Process the packet, received from the network.
 * Called by IP layer.
 * If there is no target socket found, just return the packet.
 */
buf_t *
udp_input (ip_t *ip, buf_t *p, netif_t *inp, ip_hdr_t *iph)
{
	udp_hdr_t *h;
	udp_socket_t *s;
	unsigned short dest, src, len;

	++ip->udp_in_datagrams;

	if (p->tot_len < sizeof(udp_hdr_t)) {
		/* Bad UDP packet received. */
		/*debug_printf ("udp_input: too short packet (hlen=%d bytes)\n", p->tot_len);*/
drop:
		++ip->udp_in_errors;
		buf_free (p);
		return 0;
	}
	h = (udp_hdr_t*) p->payload;

	/* LY: p->tot_len may include a eth-tail, use udp-len instead. */
	len = (h->len_h << 8) | h->len_l;
	/*debug_printf ("udp_input: driver %s received %d bytes, useful %d bytes\n",
		inp->name, p->tot_len, len);*/
	if (len > p->tot_len)
		goto drop;

	if ((h->chksum_h | h->chksum_l) != 0 &&
	    buf_chksum (p, crc16_inet_header (iph->src,
	    iph->dest, IP_PROTO_UDP, len)) != 0) {
		/* Checksum failed for received UDP packet. */
		/*debug_printf ("udp_input: bad checksum\n");*/
		goto drop;
	}

	/* Find a destination socket. */
	dest = h->dest_h << 8 | h->dest_l;
	src = h->src_h << 8 | h->src_l;
	for (s = ip->udp_sockets; s; s = s->next) {
		/*debug_printf ("<local :%d remote %d.%d.%d.%d:%d> ",
			s->local_port, s->peer_ip[0], s->peer_ip[1],
			s->peer_ip[2], s->peer_ip[3], s->peer_port);*/
		/* Compare local port number. */
		if (s->local_port != dest)
			continue;

		/* Compare remote port number. When 0, the socket
		 * will accept packets from "any" remote port. */
		if (s->peer_port && s->peer_port != src)
			continue;

		/* Compare peer IP address (or broadcast). */
		if (memcmp (s->peer_ip, IP_ADDR(0), 4) != 0 &&
		     memcmp (s->peer_ip, iph->src, 4) != 0)
			continue;

		/* Put packet to socket. */
		buf_add_header (p, -UDP_HLEN);
		mutex_lock (&s->lock);
		if (udp_queue_is_full (s)) {
			mutex_unlock (&s->lock);
			/*debug_printf ("udp_input: socket overflow\n");*/
			goto drop;
		}
		udp_queue_put (s, p, iph->src, src);
		mutex_signal (&s->lock, p);
		mutex_unlock (&s->lock);
		/*debug_printf ("udp_input: signaling socket on port %d\n",
			s->local_port);*/
		return 0;
	}

	/* No match was found. */
	/*debug_printf ("udp_input: no socket found\n");*/
	/*debug_printf ("    source %d.%d.%d.%d port %d, destination port %d\n",
		iph->src[0], iph->src[1], iph->src[2], iph->src[3],
		src, dest);*/
	++ip->udp_no_ports;
	return p;
}

/*
 * Add UDP header and send the packet to the given interface.
 */
static bool_t
udp_send_netif (udp_socket_t *s, buf_t *p, unsigned char *dest,
	unsigned short port, netif_t *netif, unsigned char *local_ip,
	unsigned char *gateway)
{
	udp_hdr_t *h;


	/* Build UDP header. */
	if (! buf_add_header (p, UDP_HLEN)) {
		buf_free (p);	/* free buffer on error */
		return 0;
	}

	h = (udp_hdr_t*) p->payload;
	h->len_h = p->tot_len >> 8;
	h->len_l = p->tot_len;

	h->src_h = s->local_port >> 8;
	h->src_l = s->local_port;

	h->dest_h = port >> 8;
	h->dest_l = port;

	h->chksum_h = 0x00;
	h->chksum_l = 0x00;
	
#ifndef UDP_CHECKSUM_DISABLE
	{
	unsigned short chksum;

	/* Calculate checksum. */
	chksum = buf_chksum (p, crc16_inet_header (local_ip,
		dest, IP_PROTO_UDP, p->tot_len));
	if (chksum == 0x0000)
		chksum = 0xffff;
	if (p->tot_len & 1) {
		/* Invert checksum bytes. */
#if HTONS(1) == 1
		h->chksum_h = chksum;
		h->chksum_l = chksum >> 8;
#else
		h->chksum_h = chksum >> 8;
		h->chksum_l = chksum;
#endif
	} else {
#if HTONS(1) == 1
		h->chksum_h = chksum >> 8;
		h->chksum_l = chksum;
#else
		h->chksum_h = chksum;
		h->chksum_l = chksum >> 8;
#endif
	}
	}
#endif
	++s->ip->udp_out_datagrams;

    return ip_output_netif (s->ip, p, dest, local_ip, IP_PROTO_UDP,
		gateway, netif, local_ip);
}

/*
 * Send the UDP packet to the destination address/port.
 */
bool_t
udp_sendto (udp_socket_t *s, buf_t *p, unsigned char *dest, unsigned short port)
{
	netif_t *netif;
	unsigned char *local_ip, *gateway;

	/* Find the outgoing network interface. */
	netif = route_lookup (s->ip, dest, &gateway, &local_ip);
	if (! netif) {
		buf_free (p);
		return 0;
	}
	/* debug_printf ("udp_sendto: %d bytes to %d.%d.%d.%d port %d netif %s\n",
		p->tot_len, dest[0], dest[1], dest[2], dest[3], port,
		netif->name); */
	/* debug_printf ("    local %d.%d.%d.%d port %d, gateway %d.%d.%d.%d\n",
		local_ip[0], local_ip[1], local_ip[2], local_ip[3],
		s->local_port,
		gateway[0], gateway[1], gateway[2], gateway[3]); */
	return udp_send_netif (s, p, dest, port, netif, local_ip,
		gateway);
}

/*
 * Send the UDP packet to the connected socket.
 */
bool_t
udp_send (udp_socket_t *s, buf_t *p)
{
	/* To send packets using UDP socket, it must
	 * have nonzero remote IP address and port number. */
	if (! s->peer_port || memcmp (s->peer_ip, IP_ADDR(0), 4) == 0) {
		return 0;
	}

	/* The destination network interface must be already known. */
	if (! s->netif) {
		return 0;
	}
	return udp_send_netif (s, p, s->peer_ip, s->peer_port, s->netif,
		s->local_ip, s->gateway);
}

/*
 * Create the UDP socket on the given local port.
 * The newly created socket is in "unconnected" state (destination
 * address and port unspecified). Unconnected socket will receive
 * packets from any peer.
 */
void
udp_socket (udp_socket_t *s, ip_t *ip, unsigned short port)
{
	mutex_lock (&s->lock);
	s->ip = ip;
	s->local_port = port;

	/* Place the socket on the list. */
	mutex_lock (&ip->lock);
	s->next = ip->udp_sockets;
	ip->udp_sockets = s;
	mutex_unlock (&ip->lock);
	mutex_unlock (&s->lock);
}

static inline void
udp_list_remove (udp_socket_t **socklist, udp_socket_t *ns)
{
	if (*socklist == ns) {
		*socklist = ns->next;
	} else {
		udp_socket_t *p;

		for (p = *socklist; p; p = p->next) {
			if (p->next && p->next == ns) {
				p->next = ns->next;
				break;
			}
		}
	}
	ns->next = 0;
}

/*
 * Close the socket: remove queued data.
 * Remove the UDP socket from IP list.
 */
void
udp_close (udp_socket_t *s)
{
	mutex_lock (&s->ip->lock);
	udp_list_remove (&s->ip->udp_sockets, s);
	mutex_unlock (&s->ip->lock);

	mutex_lock (&s->lock);
	udp_queue_free (s);
	mutex_unlock (&s->lock);
}

/*
 * Connect the UDP socket to the given destination address and port.
 * Connected socket will receive packets only from the peer
 * with appropriate address/port.
 */
void
udp_connect (udp_socket_t *s, unsigned char *ipaddr, unsigned short port)
{
	mutex_lock (&s->lock);
	s->peer_port = port;
	if (ipaddr) {
		memcpy (s->peer_ip, ipaddr, 4);

		/* Find the outgoing network interface. */
		s->netif = route_lookup (s->ip, ipaddr, &s->gateway,
			&s->local_ip);
	}
	mutex_unlock (&s->lock);
}

/*
 * Get the received packet. Returns 0 when no data is available.
 * Stores also the address and port of the sender.
 * Recommended for use on unconnected sockets.
 * Does not block the task.
 */
buf_t *
udp_peekfrom (udp_socket_t *s, unsigned char *from_addr,
	unsigned short *from_port)
{
	buf_t *p;

	mutex_lock (&s->lock);

	p = udp_queue_get (s, from_addr, from_port);

	mutex_unlock (&s->lock);
	return p;
}

/*
 * Get the received packet. Blocks the task until the data is available.
 * Stores also the address and port of the sender.
 * Recommended for use on unconnected sockets.
 */
buf_t *
udp_recvfrom (udp_socket_t *s, unsigned char *from_addr,
	unsigned short *from_port)
{
	buf_t *p;

	mutex_lock (&s->lock);

	while (udp_queue_is_empty (s))
		mutex_wait (&s->lock);

	p = udp_queue_get (s, from_addr, from_port);

	mutex_unlock (&s->lock);
	return p;
}
