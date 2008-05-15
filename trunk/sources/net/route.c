#include <runtime/lib.h>
#include <kernel/uos.h>
#include <net/route.h>
#include <net/netif.h>
#include <net/ip.h>

/*
 * Compare the IP address with the route address/netmask.
 * Return 1 on match, 0 on failure.
 */
static bool_t
route_match (route_t *r, unsigned char *a)
{
	switch (r->masklen) {
	case 0:	 return 1;
	case 1:	 return (a[0] & 0x80) == r->netaddr[0];
	case 2:	 return (a[0] & 0xC0) == r->netaddr[0];
	case 3:	 return (a[0] & 0xE0) == r->netaddr[0];
	case 4:	 return (a[0] & 0xF0) == r->netaddr[0];
	case 5:	 return (a[0] & 0xF8) == r->netaddr[0];
	case 6:	 return (a[0] & 0xFC) == r->netaddr[0];
	case 7:	 return (a[0] & 0xFE) == r->netaddr[0];
	}
	if (a[0] != r->ipaddr[0])
		return 0;
	switch (r->masklen) {
	case 8:	 return 1;
	case 9:	 return (a[1] & 0x80) == r->netaddr[1];
	case 10: return (a[1] & 0xC0) == r->netaddr[1];
	case 11: return (a[1] & 0xE0) == r->netaddr[1];
	case 12: return (a[1] & 0xF0) == r->netaddr[1];
	case 13: return (a[1] & 0xF8) == r->netaddr[1];
	case 14: return (a[1] & 0xFC) == r->netaddr[1];
	case 15: return (a[1] & 0xFE) == r->netaddr[1];
	}
	if (a[1] != r->ipaddr[1])
		return 0;
	switch (r->masklen) {
	case 16: return 1;
	case 17: return (a[2] & 0x80) == r->netaddr[2];
	case 18: return (a[2] & 0xC0) == r->netaddr[2];
	case 19: return (a[2] & 0xE0) == r->netaddr[2];
	case 20: return (a[2] & 0xF0) == r->netaddr[2];
	case 21: return (a[2] & 0xF8) == r->netaddr[2];
	case 22: return (a[2] & 0xFC) == r->netaddr[2];
	case 23: return (a[2] & 0xFE) == r->netaddr[2];
	}
	if (a[2] != r->ipaddr[2])
		return 0;
	switch (r->masklen) {
	case 24: return 1;
	case 25: return (a[3] & 0x80) == r->netaddr[3];
	case 26: return (a[3] & 0xC0) == r->netaddr[3];
	case 27: return (a[3] & 0xE0) == r->netaddr[3];
	case 28: return (a[3] & 0xF0) == r->netaddr[3];
	case 29: return (a[3] & 0xF8) == r->netaddr[3];
	case 30: return (a[3] & 0xFC) == r->netaddr[3];
	case 31: return (a[3] & 0xFE) == r->netaddr[3];
	}
	return a[3] == r->ipaddr[3];
}

/*
 * There are two types of routing records:
 * 1) For every real network interface IP address (alias);
 * 2) For every additional route.
 * Records of type 1 have gateway[0] = 0.
 * Records of type 2 have real gateway addresses.
 */
void route_setup (ip_t *ip, route_t *r, unsigned char *ipaddr,
	unsigned char masklen, unsigned char *gateway)
{
	unsigned long net, bcast;	/* host byte order */
	route_t *s, *best;

	memcpy (r->ipaddr, ipaddr, 4);
	r->masklen = masklen;

	net = 0xfffffffful >> masklen;
	bcast = net | (unsigned long) ipaddr[0] << 24 |
		(unsigned long) ipaddr[1] << 16 |
		(unsigned short) ipaddr[2] << 8 | ipaddr[3];
	net ^= bcast;

	memset (r->gateway, 0, 4);
	memset (r->gwifaddr, 0, 4);
	if (gateway) {
		memcpy (r->gateway, gateway, 4);
		memset (r->broadcast, 0, 4);

		/* Find the ip address of the target interface. */
		best = 0;
		for (s=ip->route; s; s=s->next) {
			/* Search through all interface records. */
			if (! s->netif || s->gateway[0])
				continue;

			/* Compare adresses using mask. */
			if (! route_match (s, gateway))
				continue;

			/* Select the longest mask. */
			if (best && best->masklen >= s->masklen)
				continue;

			best = s;
			/* debug_printf (CONST("route iface match: %d.%d.%d.%d with %d.%d.%d.%d / %d\n"),
				gateway[0], gateway[1], gateway[2], gateway[3],
				s->ipaddr[0], s->ipaddr[1], s->ipaddr[2], s->ipaddr[3],
				s->masklen); */
		}
		if (best)
			memcpy (r->gwifaddr, best->ipaddr, 4);
	}
	bcast = HTONL (bcast);
	memcpy (r->broadcast, &bcast, 4);
	net = HTONL (net);
	memcpy (r->netaddr, &net, 4);
	/* debug_printf (CONST("route: setup net %d.%d.%d.%d bcast %d.%d.%d.%d\n"),
		r->netaddr[0], r->netaddr[1], r->netaddr[2], r->netaddr[3],
		r->broadcast[0], r->broadcast[1], r->broadcast[2], r->broadcast[3]); */
}

/*
 * Add interface record to the list.
 */
void route_add_netif (ip_t *ip, route_t *r, unsigned char *ipaddr,
	unsigned char masklen, netif_t *netif)
{
	route_setup (ip, r, ipaddr, masklen, 0);
	r->netif = netif;
	r->next = ip->route;
	ip->route = r;
}

/*
 * Add routing record to the list.
 * Routing records must be added _after_ all interface records.
 */
bool_t route_add_gateway (ip_t *ip, route_t *r,
	unsigned char *ipaddr, unsigned char masklen, unsigned char *gateway)
{
	route_setup (ip, r, ipaddr, masklen, gateway);

	/* Network interface is unknown yet.
	 * Find it now. */
	r->netif = route_lookup (ip, gateway, 0, 0);
	if (! r->netif)
		return 0;

	r->next = ip->route;
	ip->route = r;
	return 1;
}

/*
 * Search the network interface and gateway address to forward the packet.
 * Return also the IP adress of the interface,
 */
netif_t *route_lookup (ip_t *ip, unsigned char *ipaddr,
	unsigned char **gateway, unsigned char **netif_ipaddr)
{
	route_t *r, *best;

	if (! ip)
		return 0;
	best = 0;
	for (r=ip->route; r; r=r->next) {
		/* Search through all records. */
		if (! r->netif)
			continue;

		/* Compare addresses using mask. */
		if (! route_match (r, ipaddr))
			continue;

		/* Select the longest mask. */
		if (best && best->masklen >= r->masklen)
			continue;

		best = r;
		/* debug_printf (CONST("route match: %d.%d.%d.%d with %d.%d.%d.%d / %d\n"),
			ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
			r->ipaddr[0], r->ipaddr[1], r->ipaddr[2], r->ipaddr[3],
			r->masklen); */
	}

	if (! best)
		return 0;

	if (gateway)
		*gateway = best->gateway[0] ? best->gateway : ipaddr;

	if (netif_ipaddr)
		*netif_ipaddr = best->gateway[0] ? best->gwifaddr :
			best->ipaddr;

	return best->netif;
}

/*
 * Assuming the packet is for us, find the network interface
 * for which it is targeted.  In case of broadcast packet,
 * return also the broadcast flag.
 */
netif_t *route_lookup_self (ip_t *ip, unsigned char *ipaddr,
	unsigned char *broadcast)
{
	route_t *r;

	for (r=ip->route; r; r=r->next) {
		/* Search through all interface records. */
		if (! r->netif || r->gateway[0])
			continue;

		/* Compare adresses using mask. */
		if (memcmp (r->ipaddr, ipaddr, 4) == 0) {
			/* Unicast address found. */
			if (broadcast)
				*broadcast = 0;
			return r->netif;
		}
		if (memcmp (r->broadcast, ipaddr, 4) == 0) {
			/* Broadcast address found. */
			if (broadcast)
				*broadcast = 1;
			return r->netif;
		}
	}
	return 0;
}

/*
 * Find the "closest" IP address of the given network interface.
 * The interface can have several IP addresses (aliases).
 */
unsigned char *route_lookup_ipaddr (ip_t *ip, unsigned char *ipaddr,
        struct _netif_t *netif)
{
	route_t *r, *best;

	best = 0;
	/* debug_printf (CONST("route: lookup ipaddr %d.%d.%d.%d for %s\n"),
		ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], netif->name); */
	for (r=ip->route; r; r=r->next) {
		/* Search through all interface records. */
		if (r->netif != netif || r->gateway[0])
			continue;

		/* Compare adresses using mask. */
		if (! route_match (r, ipaddr))
			continue;

		/* Select the longest mask. */
		if (best && best->masklen >= r->masklen)
			continue;

		best = r;
		/* debug_printf (CONST("route match: %d.%d.%d.%d with %d.%d.%d.%d / %d\n"),
			ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
			r->ipaddr[0], r->ipaddr[1], r->ipaddr[2], r->ipaddr[3],
			r->masklen); */
	}
	if (! best)
		return 0;

	/* debug_printf (CONST("route_lookup returned %d.%d.%d.%d\n"),
		best->ipaddr[0], best->ipaddr[1], best->ipaddr[2], best->ipaddr[3]); */
	return best->ipaddr;
}
