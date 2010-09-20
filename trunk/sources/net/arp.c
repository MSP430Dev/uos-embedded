#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/netif.h>
#include <net/route.h>

#undef ARP_TRACE

static const char BROADCAST[6] = "\377\377\377\377\377\377";

/*
 * Initialize the ARP data strucure.
 * The arp_t structure must be zeroed before calling arp_init().
 * The size of ARP table depends on the size of area, allocated for `arp'.
 * The `route' argument is a reference to routing table, needed for
 * processing incoming ARP requests.
 */
arp_t *
arp_init (array_t *buf, unsigned bytes, struct _ip_t *ip)
{
	arp_t *arp;

	/* MUST be compiled with "pack structs" or equivalent! */
	assert (sizeof (struct eth_hdr) == 14);

	arp = (arp_t*) buf;
	arp->ip = ip;
	arp->timer = 0;
	arp->size = 1 + (bytes - sizeof(arp_t)) / sizeof(arp_entry_t);
	/*debug_printf ("arp_init: %d entries\n", arp->size);*/
	return arp;
}

/*
 * Find an Ethernet address by IP address in ARP table.
 * Mark it as age = 0.
 */
unsigned char *
arp_lookup (netif_t *netif, unsigned char *ipaddr)
{
	arp_t *arp = netif->arp;
	arp_entry_t *e;

	for (e = arp->table; e < arp->table + arp->size; ++e)
		if (e->netif == netif && memcmp (e->ipaddr, ipaddr, 4) == 0) {
			/*debug_printf ("arp_lookup: %d.%d.%d.%d -> %02x-%02x-%02x-%02x-%02x-%02x\n",
				ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
				e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
				e->ethaddr[3], e->ethaddr[4], e->ethaddr[5]);*/
			/* LY-TODO: не нужно сбрасывать age. */
			e->age = 0;
			return e->ethaddr;
		}
	/*debug_printf ("arp_lookup failed: ipaddr = %d.%d.%d.%d\n",
		ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);*/
	return 0;
}

/*
 * Add a new entry to ARP table.
 * Mark new entries as age = 0.
 */
static void
arp_add_entry (netif_t *netif, unsigned char *ipaddr, unsigned char *ethaddr)
{
	arp_t *arp = netif->arp;
	arp_entry_t *e, *q;

	if (ipaddr[0] == 0)
		return;

	/* Walk through the ARP mapping table and try to find an entry to
	 * update. If none is found, the IP -> MAC address mapping is
	 * inserted in the ARP table. */
	for (e = arp->table; e < arp->table + arp->size; ++e) {
		/* Only check those entries that are actually in use. */
		if (! e->netif)
			continue;

		/* IP address match? */
		if (memcmp (e->ipaddr, ipaddr, 4) == 0) {
			/* An old entry found, update this and return. */
			if (memcmp (e->ethaddr, ethaddr, 6) != 0 ||
			    e->netif != netif) {
				/* debug_printf ("arp: entry %d.%d.%d.%d changed from %02x-%02x-%02x-%02x-%02x-%02x netif %s\n",
					ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
					e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
					e->ethaddr[3], e->ethaddr[4], e->ethaddr[5],
					e->netif->name); */
				/* debug_printf ("     to %02x-%02x-%02x-%02x-%02x-%02x netif %s\n",
					ethaddr[0], ethaddr[1], ethaddr[2],
					ethaddr[3], ethaddr[4], ethaddr[5],
					netif->name); */
				memcpy (e->ethaddr, ethaddr, 6);
				e->netif = netif;
			}
			e->age = 0;
			return;
		}
	}

	/* If we get here, no existing ARP table entry was found, so we
	 * create one. */

	/* First, we try to find an unused entry in the ARP table. */
	for (e = arp->table; e < arp->table + arp->size; ++e)
		if (! e->netif)
			break;

	/* If no unused entry is found, we try to find the oldest entry and
	 * throw it away. */
	if (e >= arp->table + arp->size) {
		e = arp->table;
		for (q = arp->table; q < arp->table + arp->size; ++q) {
			if (q->age > e->age)
				e = q;
		}
		/* debug_printf ("arp: delete entry %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x netif %s age %d\n",
			e->ipaddr[0], e->ipaddr[1], e->ipaddr[2], e->ipaddr[3],
			e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
			e->ethaddr[3], e->ethaddr[4], e->ethaddr[5],
			e->netif->name, e->age); */
	}

	/* Now, fill this table entry with the new information. */
	memcpy (e->ipaddr, ipaddr, 4);
	memcpy (e->ethaddr, ethaddr, 6);
	e->netif = netif;
	e->age = 0;
	/* debug_printf ("arp: create entry %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x netif %s\n",
		e->ipaddr[0], e->ipaddr[1], e->ipaddr[2], e->ipaddr[3],
		e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
		e->ethaddr[3], e->ethaddr[4], e->ethaddr[5], e->netif->name); */
}

/*
 * Process the packet, received by adapter.
 * Process ARP requests. For IP packets, strip off the ethernet header.
 */
buf_t *
arp_input (netif_t *netif, buf_t *p)
{
	struct ethip_hdr *h;
	struct arp_hdr *ah;
	unsigned char *ipaddr;

	/*debug_printf ("arp_input: %d bytes\n", p->tot_len);*/
	h = (struct ethip_hdr*) p->payload;
	switch (h->eth.proto) {
	default:
		/* Unknown protocol. */
		/* debug_printf ("arp_input: unknown protocol 0x%x\n", h->eth.proto); */
		buf_free (p);
		return 0;

	case PROTO_IP:
		/*debug_printf ("arp: ip packet from %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
			h->ip_src[0], h->ip_src[1], h->ip_src[2], h->ip_src[3],
			h->eth.src[0], h->eth.src[1], h->eth.src[2],
			h->eth.src[3], h->eth.src[4], h->eth.src[5]);*/

		/* For unicasts, update an ARP entry, independent of
		 * the source IP address. */
		if (h->eth.dest[0] != 255) {
			/* debug_printf ("arp: add entry %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
				h->ip_src[0], h->ip_src[1], h->ip_src[2], h->ip_src[3],
				h->eth.src[0], h->eth.src[1], h->eth.src[2],
				h->eth.src[3], h->eth.src[4], h->eth.src[5]); */
			arp_add_entry (netif, h->ip_src, h->eth.src);
		}

		/* Strip off the Ethernet header. */
		buf_add_header (p, -14);
		return p;

	case PROTO_ARP:
		if (p->tot_len < sizeof (struct arp_hdr)) {
			/* debug_printf ("arp_input: too short packet\n"); */
			buf_free (p);
			return 0;
		}
		buf_truncate (p, sizeof (struct arp_hdr));
		p = buf_make_continuous (p);

		ah = (struct arp_hdr*) p->payload;
		switch (ah->opcode) {
		default:
			/* Unknown ARP operation code. */
			/* debug_printf ("arp_input: unknown opcode 0x%x\n", ah->opcode); */
			buf_free (p);
			return 0;

		case ARP_REQUEST:
#ifdef ARP_TRACE
			debug_printf ("arp: got request for %d.%d.%d.%d\n",
				ah->dst_ipaddr[0], ah->dst_ipaddr[1], ah->dst_ipaddr[2], ah->dst_ipaddr[3]);
			debug_printf ("     from %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
				ah->src_ipaddr[0], ah->src_ipaddr[1], ah->src_ipaddr[2], ah->src_ipaddr[3],
				ah->src_hwaddr[0], ah->src_hwaddr[1], ah->src_hwaddr[2],
				ah->src_hwaddr[3], ah->src_hwaddr[4], ah->src_hwaddr[5]);
#endif
			/* ARP request. If it asked for our address,
			 * we send out a reply. */
			ipaddr = route_lookup_ipaddr (netif->arp->ip,
				ah->src_ipaddr, netif);
			if (! ipaddr || memcmp (ipaddr, ah->dst_ipaddr, 4) != 0) {
				buf_free (p);
				return 0;
			}

			ah->opcode = ARP_REPLY;

			memcpy (ah->dst_ipaddr, ah->src_ipaddr, 4);
			memcpy (ah->src_ipaddr, ipaddr, 4);

			memcpy (ah->dst_hwaddr, ah->src_hwaddr, 6);
			memcpy (ah->src_hwaddr, netif->ethaddr, 6);
			memcpy (ah->eth.dest, ah->dst_hwaddr, 6);
			memcpy (ah->eth.src, netif->ethaddr, 6);

			ah->eth.proto = PROTO_ARP;
#ifdef ARP_TRACE
			debug_printf ("arp: send reply %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
				ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
				netif->ethaddr[0], netif->ethaddr[1], netif->ethaddr[2],
				netif->ethaddr[3], netif->ethaddr[4], netif->ethaddr[5]);
			debug_printf ("     to %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
				ah->dst_ipaddr[0], ah->dst_ipaddr[1], ah->dst_ipaddr[2], ah->dst_ipaddr[3],
				ah->dst_hwaddr[0], ah->dst_hwaddr[1], ah->dst_hwaddr[2],
				ah->dst_hwaddr[3], ah->dst_hwaddr[4], ah->dst_hwaddr[5]);
#endif
			netif->interface->output (netif, p, 0);
			return 0;

		case ARP_REPLY:
			/* ARP reply. We insert or update the ARP table.
			 * No need to check the destination IP address. */
			/* debug_printf ("arp: got reply from %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
				ah->src_ipaddr[0], ah->src_ipaddr[1], ah->src_ipaddr[2], ah->src_ipaddr[3],
				ah->src_hwaddr[0], ah->src_hwaddr[1], ah->src_hwaddr[2],
				ah->src_hwaddr[3], ah->src_hwaddr[4], ah->src_hwaddr[5]); */
			arp_add_entry (netif, ah->src_ipaddr, ah->src_hwaddr);
			buf_free (p);
			return 0;
		}
	}
}

/*
 * Create ARP request packet.
 * Argument `ipsrc' is the IP address of the network interface,
 * from which we send the ARP request. The network interface could have
 * several IP adresses (aliases), so ipsrc is needed to select
 * concrete alias.
 */
bool_t
arp_request (netif_t *netif, buf_t *p, unsigned char *ipdest,
	unsigned char *ipsrc)
{
	struct arp_hdr *ah;

	/* ARP packet place at the begin of buffer (offset 2) */
	buf_add_header (p, p->payload - (unsigned char*) p - sizeof (buf_t) - 2);

	if (p->tot_len < sizeof (struct arp_hdr)) {
		/* Not enough space for ARP packet. */
		buf_free (p);
		return 0;
	}
	buf_truncate (p, sizeof (struct arp_hdr));
	p = buf_make_continuous (p);

	ah = (struct arp_hdr*) p->payload;
	ah->eth.proto = PROTO_ARP;
	memcpy_flash (ah->eth.dest, BROADCAST, 6);
	memcpy (ah->eth.src, netif->ethaddr, 6);

	ah->opcode = ARP_REQUEST;
	ah->hwtype = HWTYPE_ETHERNET;
	ah->hwlen = 6;
	ah->proto = PROTO_IP;
	ah->protolen = 4;

	/* Most implementations set dst_hwaddr to zero. */
	memset (ah->dst_hwaddr, 0, 6);
	memcpy (ah->src_hwaddr, netif->ethaddr, 6);
	memcpy (ah->dst_ipaddr, ipdest, 4);
	memcpy (ah->src_ipaddr, ipsrc, 4);

	/* debug_printf ("arp: send request for %d.%d.%d.%d\n",
		ipdest[0], ipdest[1], ipdest[2], ipdest[3]); */
	/*debug_printf ("     from %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x\n",
		ipsrc[0], ipsrc[1], ipsrc[2], ipsrc[3],
		netif->ethaddr[0], netif->ethaddr[1], netif->ethaddr[2],
		netif->ethaddr[3], netif->ethaddr[4], netif->ethaddr[5]);*/
	return netif->interface->output (netif, p, 0);
}

/*
 * Add Ethernet header for transmitted packet.
 * For broadcasts, ipdest must be NULL.
 */
bool_t
arp_add_header (netif_t *netif, buf_t *p, unsigned char *ipdest,
	unsigned char *ethdest)
{
	struct eth_hdr *h;

	/* Make room for Ethernet header. */
	if (! buf_add_header (p, 14)) {
		/* No space for header, deallocate packet. */
		buf_free (p);
		/*debug_printf ("arp_output: no space for header\n");*/
		return 0;
	}
	h = (struct eth_hdr*) p->payload;

	/* Construct Ethernet header. Start with looking up deciding which
	 * MAC address to use as a destination address. Broadcasts and
	 * multicasts are special, all other addresses are looked up in the
	 * ARP table. */
	if (! ipdest) {
		/* Broadcast. */
		memcpy_flash (h->dest, BROADCAST, 6);

	} else if ((ipdest[0] & 0xf0) == 0xe0) {
		/* Hash IP multicast address to MAC address. */
		h->dest[0] = 0x01;
		h->dest[1] = 0;
		h->dest[2] = 0x5e;
		h->dest[3] = ipdest[1] & 0x7f;
		h->dest[4] = ipdest[2];
		h->dest[5] = ipdest[3];

	} else {
		memcpy (h->dest, ethdest, 6);
	}

	h->proto = PROTO_IP;
	memcpy (h->src, netif->ethaddr, 6);
	return 1;
}

/*
 * Aging all arp entry. Deleting old entries. By Serg Lvov.
 * Called 10 times per second.
 */
void
arp_timer (arp_t *arp)
{
	arp_entry_t *e;

	++arp->timer;
	if (arp->timer < 50)
		return;
	arp->timer = 0;

	/*
	 * Every 5 seconds walk through the ARP mapping table
	 * and try to find an entry to aging.
	 */
	 for (e = arp->table; e < arp->table + arp->size; ++e) {
		/* Only check those entries that are actually in use. */
		if (! e->netif)
			continue;

		/* Standard aging time is 300 seconds. */
		if (++e->age > 300/5) {
			/* debug_printf ("arp: delete entry %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x netif %s age %d\n",
				e->ipaddr[0], e->ipaddr[1], e->ipaddr[2], e->ipaddr[3],
				e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
				e->ethaddr[3], e->ethaddr[4], e->ethaddr[5],
				e->netif->name, e->age); */
			e->netif = 0;
		}
	}
}
