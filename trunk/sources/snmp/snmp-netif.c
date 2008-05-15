#include <runtime/lib.h>
#include <kernel/uos.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/netif.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-netif.h>

IF_VARIABLE_LIST

asn_t *
snmp_get_ifNumber (snmp_t *snmp, ...)
{
	route_t *r;
	unsigned count;

	count = 0;
	for (r=snmp->ip->route; r; r=r->next)
		/* Count all interface records. */
		if (r->netif && ! r->gateway[0])
			++count;

	return asn_make_int (snmp->pool, count, ASN_INTEGER);
}

/*
 * Find the network interface with the given number,
 * starting from 1.
 */
netif_t *
find_netif (route_t *tab, unsigned nif)
{
	route_t *r;

	if (nif <= 0)
		return 0;
	for (r=tab; r; r=r->next)
		if (r->netif && ! r->gateway[0])
			if (--nif == 0)
				return r->netif;
	return 0;
}

asn_t *
snmp_get_ifIndex (snmp_t *snmp, unsigned nif, ...)
{
	if (! find_netif (snmp->ip->route, nif))
		return 0;
	return asn_make_int (snmp->pool, nif, ASN_INTEGER);
}

asn_t *
snmp_next_ifIndex (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	if (nextflag)
		++*nif;
	else
		*nif = 1;
	if (! find_netif (snmp->ip->route, *nif))
		return 0;
	return asn_make_int (snmp->pool, *nif, ASN_INTEGER);
}

asn_t *
snmp_get_ifDescr (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_string_flash (snmp->pool, u->name);
}

asn_t *
snmp_next_ifDescr (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_string_flash (snmp->pool, u->name);
}

asn_t *
snmp_get_ifType (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->type, ASN_INTEGER);
}

asn_t *
snmp_next_ifType (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->type, ASN_INTEGER);
}

asn_t *
snmp_get_ifSpecific (snmp_t *snmp, unsigned nif, ...)
{
	if (! find_netif (snmp->ip->route, nif))
		return 0;
	return asn_make_oid (snmp->pool, ".0.0");
}

asn_t *
snmp_next_ifSpecific (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	if (nextflag)
		++*nif;
	else
		*nif = 1;
	if (! find_netif (snmp->ip->route, *nif))
		return 0;
	return asn_make_oid (snmp->pool, ".0.0");
}

asn_t *
snmp_get_ifPhysAddress (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);

	if (! u)
		return 0;
	if (! u->arp)
		return asn_make_stringn (snmp->pool, 0, 0);
	return asn_make_stringn (snmp->pool, u->ethaddr, 6);
}

asn_t *
snmp_next_ifPhysAddress (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	if (! u->arp)
		return asn_make_stringn (snmp->pool, 0, 0);
	return asn_make_stringn (snmp->pool, u->ethaddr, 6);
}

asn_t *
snmp_get_ifSpeed (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->bps, ASN_GAUGE);
}

asn_t *
snmp_next_ifSpeed (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->bps, ASN_GAUGE);
}

asn_t *
snmp_get_ifLastChange (snmp_t *snmp, unsigned nif, ...)
{
	if (! find_netif (snmp->ip->route, nif))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_TIME_TICKS);
}

asn_t *
snmp_next_ifLastChange (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	if (nextflag)
		++*nif;
	else
		*nif = 1;
	if (! find_netif (snmp->ip->route, *nif))
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_TIME_TICKS);
}

asn_t *
snmp_get_ifMtu (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->mtu, ASN_INTEGER);
}

asn_t *
snmp_next_ifMtu (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->mtu, ASN_INTEGER);
}

asn_t *
snmp_get_ifAdminStatus (snmp_t *snmp, unsigned nif, ...)
{
	if (! find_netif (snmp->ip->route, nif))
		return 0;
	return asn_make_int (snmp->pool, SNMP_IFS_UP, ASN_INTEGER);
}

asn_t *
snmp_next_ifAdminStatus (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	if (nextflag)
		++*nif;
	else
		*nif = 1;
	if (! find_netif (snmp->ip->route, *nif))
		return 0;
	return asn_make_int (snmp->pool, SNMP_IFS_UP, ASN_INTEGER);
}

asn_t *
snmp_get_ifOperStatus (snmp_t *snmp, unsigned nif, ...)
{
	if (! find_netif (snmp->ip->route, nif))
		return 0;
	return asn_make_int (snmp->pool, SNMP_IFS_UP, ASN_INTEGER);
}

asn_t *
snmp_next_ifOperStatus (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	if (nextflag)
		++*nif;
	else
		*nif = 1;
	if (! find_netif (snmp->ip->route, *nif))
		return 0;
	return asn_make_int (snmp->pool, SNMP_IFS_UP, ASN_INTEGER);
}

asn_t *
snmp_get_ifInOctets (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_bytes, ASN_COUNTER);
}

asn_t *
snmp_next_ifInOctets (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_bytes, ASN_COUNTER);
}

asn_t *
snmp_get_ifInUcastPkts (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_packets - u->in_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_next_ifInUcastPkts (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_packets - u->in_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_get_ifInNUcastPkts (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_next_ifInNUcastPkts (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_get_ifInDiscards (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_discards, ASN_COUNTER);
}

asn_t *
snmp_next_ifInDiscards (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_discards, ASN_COUNTER);
}

asn_t *
snmp_get_ifInErrors (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_errors, ASN_COUNTER);
}

asn_t *
snmp_next_ifInErrors (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_errors, ASN_COUNTER);
}

asn_t *
snmp_get_ifInUnknownProtos (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_unknown_protos, ASN_COUNTER);
}

asn_t *
snmp_next_ifInUnknownProtos (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->in_unknown_protos, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutOctets (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_bytes, ASN_COUNTER);
}

asn_t *
snmp_next_ifOutOctets (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_bytes, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutUcastPkts (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_packets - u->out_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_next_ifOutUcastPkts (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_packets - u->out_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutNUcastPkts (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_next_ifOutNUcastPkts (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_mcast_pkts, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutDiscards (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_discards, ASN_COUNTER);
}

asn_t *
snmp_next_ifOutDiscards (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_discards, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutErrors (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_errors, ASN_COUNTER);
}

asn_t *
snmp_next_ifOutErrors (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_errors, ASN_COUNTER);
}

asn_t *
snmp_get_ifOutQLen (snmp_t *snmp, unsigned nif, ...)
{
	netif_t *u = find_netif (snmp->ip->route, nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_qlen, ASN_GAUGE);
}

asn_t *
snmp_next_ifOutQLen (snmp_t *snmp, bool_t nextflag, unsigned *nif, ...)
{
	netif_t *u;

	if (nextflag)
		++*nif;
	else
		*nif = 1;
	u = find_netif (snmp->ip->route, *nif);
	if (! u)
		return 0;
	return asn_make_int (snmp->pool, u->out_qlen, ASN_GAUGE);
}

uint_t
snmp_set_ifAdminStatus (snmp_t *snmp, asn_t *val, unsigned nif, ...)
{
	netif_t *u;

	u = find_netif (snmp->ip->route, nif);
	if (! u)
		return SNMP_NO_SUCH_NAME;
	if (val->type != ASN_INTEGER)
		return SNMP_BAD_VALUE;
	if (val->int32.val != SNMP_IFS_UP && val->int32.val != SNMP_IFS_DOWN)
		return SNMP_BAD_VALUE;
	/* TODO */
	return SNMP_NO_ERROR;
}
