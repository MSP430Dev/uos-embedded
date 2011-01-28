#include <runtime/lib.h>
#include <kernel/uos.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <net/ip.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/netif.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-ip.h>

#define LONG(p)         ((unsigned long)(p[0]) << 24 | \
			 (unsigned long)(p[1]) << 16 | \
			 (unsigned long)(p[2]) << 8 | (p[3]))

IP_VARIABLE_LIST

asn_t *snmp_get_ipForwarding (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->forwarding, ASN_INTEGER);
}

small_uint_t
snmp_set_ipForwarding (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type != ASN_INTEGER)
		return SNMP_BAD_VALUE;
	switch (val->int32.val) {
	default:
		return SNMP_BAD_VALUE;
	case SNMP_IP_FORWARDING:
		snmp->ip->forwarding = 1;
		break;
	case SNMP_IP_NOT_FORWARDING:
		snmp->ip->forwarding = 0;
		break;
	}
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipDefaultTTL (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->default_ttl, ASN_INTEGER);
}

small_uint_t
snmp_set_ipDefaultTTL (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type != ASN_INTEGER ||
	    val->int32.val < 1 || val->int32.val > 255)
		return SNMP_BAD_VALUE;
	snmp->ip->default_ttl = val->int32.val;
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipInDiscards (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_discards, ASN_COUNTER);
}

asn_t *snmp_get_ipInReceives (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_receives, ASN_COUNTER);
}

asn_t *snmp_get_ipInHdrErrors (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_hdr_errors, ASN_COUNTER);
}

asn_t *snmp_get_ipInAddrErrors (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_addr_errors, ASN_COUNTER);
}

asn_t *snmp_get_ipForwDatagrams (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->forw_datagrams, ASN_COUNTER);
}

asn_t *snmp_get_ipInUnknownProtos (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_unknown_protos, ASN_COUNTER);
}

asn_t *snmp_get_ipInDelivers (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->in_delivers, ASN_COUNTER);
}

asn_t *snmp_get_ipOutRequests (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->out_requests, ASN_COUNTER);
}

asn_t *snmp_get_ipOutDiscards (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->out_discards, ASN_COUNTER);
}

asn_t *snmp_get_ipOutNoRoutes (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->out_no_routes, ASN_COUNTER);
}

/*
 * Fragmentation not implemented yet.
 */
asn_t *snmp_get_ipReasmTimeout (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip->reasm_timeout*/, ASN_INTEGER);
}

asn_t *snmp_get_ipReasmReqds (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip->reasm_reqds*/, ASN_COUNTER);
}

asn_t *snmp_get_ipReasmOKs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip_reasm_oks*/, ASN_COUNTER);
}

asn_t *snmp_get_ipReasmFails (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip_reasm_fails*/, ASN_COUNTER);
}

asn_t *snmp_get_ipFragOKs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip_frag_oks*/, ASN_COUNTER);
}

asn_t *snmp_get_ipFragFails (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip_frag_fails*/, ASN_COUNTER);
}

asn_t *snmp_get_ipFragCreates (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0 /*snmp->ip_frag_creates*/, ASN_COUNTER);
}

asn_t *snmp_get_ipRoutingDiscards (snmp_t *snmp, ...)
{
	/* Routing protocols not implemented. */
	return asn_make_int (snmp->pool, 0, ASN_COUNTER);
}

/*
 * Find the interface with the given IP address.
 */
static route_t *
find_netif_by_addr (ip_t *ip, unsigned long addr)
{
	route_t *u;

	for (u=ip->route; u; u=u->next)
		if (u->netif && ! u->gateway[0] &&
		    memcmp (u->ipaddr, &addr, 4) == 0)
			return u;
	return 0;
}

/*
 * Find the interface with the minimal IP address.
 * Store the found IP address into `addr'.
 */
static route_t *
find_first_netif_by_addr (ip_t *ip, unsigned long *addr)
{
	route_t *u, *found;
	unsigned long found_addr, a;

	found = 0;
	found_addr = 0;
	for (u=ip->route; u; u=u->next) {
		if (! u->netif || u->gateway[0])
			continue;

		a = LONG (u->ipaddr);
/*debug_printf ("find_first_socket compare %p\n", found);*/
		if (! found || a < found_addr) {
			found = u;
			found_addr = a;
		}
	}
	*addr = found_addr;
	return found;
}

/*
 * Find the interface with the minimal IP address,
 * but greater than given addr/port.
 * Store the found IP address into `addr'.
 */
static route_t *
find_next_netif_by_addr (ip_t *ip, unsigned long *addr)
{
	route_t *u, *found;
	unsigned long found_addr, a;

	found = 0;
	found_addr = 0;
	for (u=ip->route; u; u=u->next) {
		if (! u->netif || u->gateway[0])
			continue;

		a = LONG (u->ipaddr);
		if (a <= *addr)
			continue;

		if (! found || a < found_addr) {
			found = u;
			found_addr = a;
		}
	}
	*addr = found_addr;
	return found;
}

/*
 * Compute the interface index, starting from 1.
 */
static unsigned
get_netif_index (ip_t *ip, route_t *target)
{
	route_t *r;
	unsigned count;

	count = 0;
	for (r=ip->route; r; r=r->next) {
		/* Count all interface records. */
		if (r->netif && ! r->gateway[0])
			++count;
		if (r == target)
			return count;
	}
	return 0;
}

/*
 * Compute the interface netmask.
 */
static unsigned long
get_route_mask (ip_t *ip, route_t *u)
{
	if (u->masklen == 0)
		return 0;
	return 0xffffffffL << (32 - u->masklen);
}

asn_t *snmp_get_ipAdEntAddr (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_netif_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, addr, ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipAdEntAddr (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_netif_by_addr (snmp->ip, addr) :
		find_first_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, *addr, ASN_IP_ADDRESS);
}

asn_t *snmp_get_ipAdEntIfIndex (snmp_t *snmp, unsigned long addr, ...)
{
	route_t *u;

	u = find_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index (snmp->ip, u),
		ASN_INTEGER);
}

asn_t *snmp_next_ipAdEntIfIndex (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_netif_by_addr (snmp->ip, addr) :
		find_first_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index (snmp->ip, u),
		ASN_INTEGER);
}

asn_t *snmp_get_ipAdEntNetMask (snmp_t *snmp, unsigned long addr, ...)
{
	route_t *u;

	u = find_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_route_mask (snmp->ip, u),
		ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipAdEntNetMask (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_netif_by_addr (snmp->ip, addr) :
		find_first_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_route_mask (snmp->ip, u),
		ASN_IP_ADDRESS);
}

asn_t *snmp_get_ipAdEntBcastAddr (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_netif_by_addr (snmp->ip, addr))
		return 0;
	/* Our broadcast addresses always end up with 1. */
	return asn_make_int (snmp->pool, 1, ASN_INTEGER);
}

asn_t *snmp_next_ipAdEntBcastAddr (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_netif_by_addr (snmp->ip, addr) :
		find_first_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 1, ASN_INTEGER);
}

asn_t *snmp_get_ipAdEntReasmMaxSize (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_netif_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, IP_MAXPACKET, ASN_INTEGER);
}

asn_t *snmp_next_ipAdEntReasmMaxSize (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_netif_by_addr (snmp->ip, addr) :
		find_first_netif_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, IP_MAXPACKET, ASN_INTEGER);
}

/*
 * Find the routing record with the given IP address.
 */
static route_t *
find_route_by_addr (ip_t *ip, unsigned long addr)
{
	route_t *u;

	for (u=ip->route; u; u=u->next)
		if (u->netif && u->gateway[0] &&
		    memcmp (u->ipaddr, &addr, 4) == 0)
			return u;
	return 0;
}

/*
 * Find the interface with the minimal IP address.
 * Store the found IP address into `addr'.
 */
static route_t *
find_first_route_by_addr (ip_t *ip, unsigned long *addr)
{
	route_t *u, *found;
	unsigned long found_addr, a;

	found = 0;
	found_addr = 0;
	for (u=ip->route; u; u=u->next) {
		if (! u->netif || ! u->gateway[0])
			continue;

		a = LONG (u->ipaddr);
/*debug_printf ("find_first_socket compare %p\n", found);*/
		if (! found || a < found_addr) {
			found = u;
			found_addr = a;
		}
	}
	*addr = found_addr;
	return found;
}

/*
 * Find the interface with the minimal IP address,
 * but greater than given address.
 * Store the found IP address into `addr'.
 */
static route_t *
find_next_route_by_addr (ip_t *ip, unsigned long *addr)
{
	route_t *u, *found;
	unsigned long found_addr, a;

	found = 0;
	found_addr = 0;
	for (u=ip->route; u; u=u->next) {
		if (! u->netif || ! u->gateway[0])
			continue;

		a = LONG (u->ipaddr);
		if (a <= *addr)
			continue;

		if (! found || a < found_addr) {
			found = u;
			found_addr = a;
		}
	}
	*addr = found_addr;
	return found;
}

asn_t *snmp_get_ipRouteDest (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, addr, ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipRouteDest (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, *addr, ASN_IP_ADDRESS);
}

small_uint_t
snmp_set_ipRouteDest (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	route_t *u;
	unsigned char ipaddr [4];

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_IP_ADDRESS)
		return SNMP_BAD_VALUE;
	memcpy (ipaddr, &val->int32.val, 4);
	route_setup (snmp->ip, u, ipaddr, u->masklen, u->gateway);
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipRouteMask (snmp_t *snmp, unsigned long addr, ...)
{
	route_t *u;

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_route_mask (snmp->ip, u),
		ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipRouteMask (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_route_mask (snmp->ip, u),
		ASN_IP_ADDRESS);
}

small_uint_t
snmp_set_ipRouteMask (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	route_t *u;
	unsigned masklen;
	unsigned long v;

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_IP_ADDRESS)
		return SNMP_BAD_VALUE;

	/* Compute the length of mask. */
	v = val->int32.val;
	for (masklen=0; v & 0x80000000; ++masklen)
		v <<= 1;

	route_setup (snmp->ip, u, u->ipaddr, masklen, u->gateway);
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipRouteNextHop (snmp_t *snmp, unsigned long addr, ...)
{
	route_t *u;

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, LONG (u->gateway), ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipRouteNextHop (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, LONG (u->gateway), ASN_IP_ADDRESS);
}

small_uint_t
snmp_set_ipRouteNextHop (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	route_t *u;
	netif_t *netif;
	unsigned char ipaddr [4];

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_IP_ADDRESS)
		return SNMP_BAD_VALUE;
	memcpy (ipaddr, &val->int32.val, 4);

	/* Find the network interface. */
	netif = route_lookup (snmp->ip, ipaddr, 0, 0);
	if (! netif)
		return SNMP_GEN_ERR;

	route_setup (snmp->ip, u, ipaddr, u->masklen, u->gateway);
	u->netif = netif;
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipRouteMetric1 (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteMetric1 (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteMetric1 (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteMetric2 (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteMetric2 (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteMetric2 (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteMetric3 (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteMetric3 (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteMetric3 (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteMetric4 (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteMetric4 (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteMetric4 (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteMetric5 (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteMetric5 (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteMetric5 (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

/*
 * Given the netif pointer, compute the interface index, starting from 1.
 */
static unsigned
get_netif_index_by_netif (ip_t *ip, netif_t *target)
{
	route_t *r;
	unsigned count;

/*debug_printf ("find_netif_index_by_netif %s\n", target->name);*/
	count = 0;
	for (r=ip->route; r; r=r->next) {
		/* Count all interface records. */
		if (r->netif && ! r->gateway[0]) {
			++count;
			if (r->netif == target)
				return count;
		}
	}
	return 0;
}

asn_t *snmp_get_ipRouteIfIndex (snmp_t *snmp, unsigned long addr, ...)
{
	route_t *u;

	u = find_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index_by_netif (snmp->ip, u->netif),
		ASN_INTEGER);
}

asn_t *snmp_next_ipRouteIfIndex (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index_by_netif (snmp->ip, u->netif),
		ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteIfIndex (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteType (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, SNMP_ROUTE_TYPE_DIRECT, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteType (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, SNMP_ROUTE_TYPE_DIRECT, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteType (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteProto (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, SNMP_ROUTE_PROTO_LOCAL, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteProto (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, SNMP_ROUTE_PROTO_LOCAL, ASN_INTEGER);
}

asn_t *snmp_get_ipRouteAge (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

asn_t *snmp_next_ipRouteAge (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_INTEGER);
}

small_uint_t
snmp_set_ipRouteAge (snmp_t *snmp, asn_t *val, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipRouteInfo (snmp_t *snmp, unsigned long addr, ...)
{
	if (! find_route_by_addr (snmp->ip, addr))
		return 0;
	return asn_make_oid (snmp->pool, ".0.0");
}

asn_t *snmp_next_ipRouteInfo (snmp_t *snmp, bool_t nextflag, unsigned long *addr, ...)
{
	route_t *u;

	u = nextflag ? find_next_route_by_addr (snmp->ip, addr) :
		find_first_route_by_addr (snmp->ip, addr);
	if (! u)
		return 0;
	return asn_make_oid (snmp->pool, ".0.0");
}

extern netif_t *find_netif (route_t *tab, unsigned nif);

/*
 * Find the ARP record with the given IP address.
 */
static arp_entry_t *
find_arp_by_addr (ip_t *ip, unsigned nif, unsigned long addr)
{
	arp_entry_t *e;
	netif_t *netif;
	arp_t *arp = ip->arp;

	if (! arp)
		return 0;
	netif = find_netif (ip->route, nif);
	if (! netif)
		return 0;
	for (e=arp->table; e<arp->table+arp->size; ++e)
		if (e->netif == netif && memcmp (e->ipaddr, &addr, 4) == 0)
			return e;
	return 0;
}

/*
 * Find the ARP record with the minimal IP address.
 * Store the found IP address into `addr'.
 */
static arp_entry_t *
find_first_arp_by_addr (ip_t *ip, unsigned *nif, unsigned long *addr)
{
	arp_entry_t *e, *found;
	unsigned long found_addr, a;
	unsigned found_nif, n;
	arp_t *arp = ip->arp;

	if (! arp)
		return 0;
	found = 0;
	found_addr = 0;
	found_nif = 0;
	for (e=arp->table; e<arp->table+arp->size; ++e) {
		/* Only check those entries that are actually in use. */
		if (! e->netif)
			continue;

		a = LONG (e->ipaddr);
		n = get_netif_index_by_netif (ip, e->netif);
		if (! found || n < found_nif ||
		    (n == found_nif && a < found_addr)) {
			found = e;
			found_addr = a;
			found_nif = n;
		}
	}
	*addr = found_addr;
	*nif = found_nif;
	return found;
}

/*
 * Find the ARP record with the minimal IP address,
 * but greater than given addr.
 * Store the found IP address into `addr'.
 */
static arp_entry_t *
find_next_arp_by_addr (ip_t *ip, unsigned *nif, unsigned long *addr)
{
	arp_entry_t *e, *found;
	unsigned long found_addr, a;
	unsigned found_nif, n;
	arp_t *arp = ip->arp;

	if (! arp)
		return 0;
	found = 0;
	found_addr = 0;
	found_nif = 0;
	for (e=arp->table; e<arp->table+arp->size; ++e) {
		/* Only check those entries that are actually in use. */
		if (! e->netif)
			continue;

		n = get_netif_index_by_netif (ip, e->netif);
		if (n < *nif)
			continue;

		a = LONG (e->ipaddr);
		if (n == *nif && a <= *addr)
			continue;

		if (! found || n < found_nif ||
		    (n == found_nif && a < found_addr)) {
			found = e;
			found_addr = a;
			found_nif = n;
		}
	}
	*addr = found_addr;
	*nif = found_nif;
	return found;
}

asn_t *snmp_get_ipNetToMediaIfIndex (snmp_t *snmp, unsigned nif, unsigned long addr, ...)
{
	arp_entry_t *e;

	e = find_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index_by_netif (snmp->ip, e->netif),
		ASN_INTEGER);
}

asn_t *snmp_next_ipNetToMediaIfIndex (snmp_t *snmp, bool_t nextflag, unsigned *nif, unsigned long *addr, ...)
{
	arp_entry_t *e;

	e = nextflag ? find_next_arp_by_addr (snmp->ip, nif, addr) :
		find_first_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_int (snmp->pool, get_netif_index_by_netif (snmp->ip, e->netif),
		ASN_INTEGER);
}

small_uint_t
snmp_set_ipNetToMediaIfIndex (snmp_t *snmp, asn_t *val, unsigned nif, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipNetToMediaNetAddress (snmp_t *snmp, unsigned nif, unsigned long addr, ...)
{
	if (! find_arp_by_addr (snmp->ip, nif, addr))
		return 0;
	return asn_make_int (snmp->pool, addr, ASN_IP_ADDRESS);
}

asn_t *snmp_next_ipNetToMediaNetAddress (snmp_t *snmp, bool_t nextflag, unsigned *nif, unsigned long *addr, ...)
{
	arp_entry_t *e;

	e = nextflag ? find_next_arp_by_addr (snmp->ip, nif, addr) :
		find_first_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_int (snmp->pool, *addr, ASN_IP_ADDRESS);
}

small_uint_t
snmp_set_ipNetToMediaNetAddress (snmp_t *snmp, asn_t *val, unsigned nif, unsigned long addr, ...)
{
	arp_entry_t *e;

	e = find_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_IP_ADDRESS)
		return SNMP_BAD_VALUE;
	memcpy (e->ipaddr, &val->int32.val, 4);
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_ipNetToMediaType (snmp_t *snmp, unsigned nif, unsigned long addr, ...)
{
	if (! find_arp_by_addr (snmp->ip, nif, addr))
		return 0;
	return asn_make_int (snmp->pool, SNMP_NTM_TYPE_DYNAMIC, ASN_INTEGER);
}

asn_t *snmp_next_ipNetToMediaType (snmp_t *snmp, bool_t nextflag, unsigned *nif, unsigned long *addr, ...)
{
	arp_entry_t *e;

	e = nextflag ? find_next_arp_by_addr (snmp->ip, nif, addr) :
		find_first_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_int (snmp->pool, SNMP_NTM_TYPE_DYNAMIC, ASN_INTEGER);
}

small_uint_t
snmp_set_ipNetToMediaType (snmp_t *snmp, asn_t *val, unsigned nif, unsigned long addr, ...)
{
	return SNMP_GEN_ERR;
}

asn_t *snmp_get_ipNetToMediaPhysAddress (snmp_t *snmp, unsigned nif, unsigned long addr, ...)
{
	arp_entry_t *e;

	e = find_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_stringn (snmp->pool, e->ethaddr, 6);
}

asn_t *snmp_next_ipNetToMediaPhysAddress (snmp_t *snmp, bool_t nextflag, unsigned *nif, unsigned long *addr, ...)
{
	arp_entry_t *e;

	e = nextflag ? find_next_arp_by_addr (snmp->ip, nif, addr) :
		find_first_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return 0;
	return asn_make_stringn (snmp->pool, e->ethaddr, 6);
}

small_uint_t
snmp_set_ipNetToMediaPhysAddress (snmp_t *snmp, asn_t *val, unsigned nif, unsigned long addr, ...)
{
	arp_entry_t *e;

	e = find_arp_by_addr (snmp->ip, nif, addr);
	if (! e)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_STRING || val->type != 6)
		return SNMP_BAD_VALUE;
	memcpy (e->ethaddr, &val->string.str, 6);
	return SNMP_NO_ERROR;
}
