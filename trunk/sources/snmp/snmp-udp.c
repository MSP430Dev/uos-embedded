#include <runtime/lib.h>
#include <kernel/uos.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-udp.h>

#define LONG(p)         ((unsigned long)(p)[0] << 24 | \
			 (unsigned long)(p)[1] << 16 | (p)[2] << 8 | (p)[3])

UDP_VARIABLE_LIST

asn_t *snmp_get_udpInDatagrams (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->udp_in_datagrams, ASN_COUNTER);
}

asn_t *snmp_get_udpNoPorts (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->udp_no_ports, ASN_COUNTER);
}

asn_t *snmp_get_udpInErrors (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->udp_in_errors, ASN_COUNTER);
}

asn_t *snmp_get_udpOutDatagrams (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->udp_out_datagrams, ASN_COUNTER);
}

/*
 * Check that the UDP socket with the given local addr/port numbers
 * does exist. Return 1 on success, 0 on error.
 */
static bool_t
find_socket (ip_t *ip, unsigned long addr, unsigned port)
{
	udp_socket_t *s;

	for (s=ip->udp_sockets; s; s=s->next)
		if (s->local_port == port && (! s->local_ip ||
		    memcmp (s->local_ip, &addr, 4) == 0))
			return 1;
	return 0;
}

/*
 * Find the UDP socket with the minimal local addr/port numbers.
 */
static udp_socket_t *
find_first_socket (ip_t *ip)
{
	udp_socket_t *s, *found;
	unsigned long found_addr, a;
	unsigned found_port;

	found = 0;
	found_addr = 0;
	found_port = 0;
	for (s=ip->udp_sockets; s; s=s->next) {
		a = s->local_ip ? LONG (s->local_ip) : 0;
/*debug_printf ("find_first_socket compare %p\n", found);*/
		if (! found || a < found_addr ||
		    (a == found_addr && s->local_port < found_port)) {
			found = s;
			found_addr = a;
			found_port = s->local_port;
		}
	}
	return found;
}

/*
 * Find the minimal UDP socket, but greater than given addr/port.
 */
static udp_socket_t *
find_next_socket (ip_t *ip, unsigned long addr, unsigned port)
{
	udp_socket_t *s, *found;
	unsigned long found_addr, a;
	unsigned found_port;

	found = 0;
	found_addr = 0;
	found_port = 0;
	for (s=ip->udp_sockets; s; s=s->next) {
		a = s->local_ip ? LONG (s->local_ip) : 0;
		if (a < addr || (a == addr && s->local_port <= port))
			continue;

		if (! found || a < found_addr ||
		    (a == found_addr && s->local_port < found_port)) {
			found = s;
			found_addr = a;
			found_port = s->local_port;
		}
	}
	return found;
}

asn_t *snmp_get_udpLocalAddress (snmp_t *snmp, unsigned long addr,
	unsigned port, ...)
{
	if (! find_socket (snmp->ip, addr, port))
		return 0;
	return asn_make_int (snmp->pool, addr, ASN_IP_ADDRESS);
}

asn_t *snmp_next_udpLocalAddress (snmp_t *snmp, bool_t nextflag,
	unsigned long *addr, unsigned *port, ...)
{
	udp_socket_t *s;

	if (nextflag)
		s = find_next_socket (snmp->ip, *addr, *port);
	else	s = find_first_socket (snmp->ip);
	if (s) {
		*addr = s->local_ip ? LONG (s->local_ip) : 0;
		*port = s->local_port;
	} else	return 0;

	return asn_make_int (snmp->pool, *addr, ASN_IP_ADDRESS);
}

asn_t *snmp_get_udpLocalPort (snmp_t *snmp, unsigned long addr,
	unsigned port, ...)
{
	if (! find_socket (snmp->ip, addr, port))
		return 0;
	return asn_make_int (snmp->pool, port, ASN_INTEGER);
}

asn_t *snmp_next_udpLocalPort (snmp_t *snmp, bool_t nextflag,
	unsigned long *addr, unsigned *port, ...)
{
	udp_socket_t *s;

	if (nextflag)
		s = find_next_socket (snmp->ip, *addr, *port);
	else	s = find_first_socket (snmp->ip);
	if (s) {
		*addr = s->local_ip ? LONG (s->local_ip) : 0;
		*port = s->local_port;
	} else	return 0;

	return asn_make_int (snmp->pool, *port, ASN_INTEGER);
}
