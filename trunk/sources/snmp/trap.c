#include <runtime/lib.h>
#include <kernel/uos.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <timer/timer.h>
#include <buf/buf.h>

#define TRAP_PACKET_SIZE	512

/*
 * Send trap, SNMP v1 (RFC 1157).
 * SEQUENCE {
 *	version: INTEGER [=0]
 *	community: STRING
 *	pdu: SEQUENCE [=TRAP] {
 *		enterprise: OBJECT IDENTIFIER
 *		agent-addr: NetworkAddress
 *		generic-trap: INTEGER
 *		specific-trap: INTEGER
 *		time-stamp: INTEGER
 *		varlist: SEQUENCE {
 *			var1: SEQUENCE {
 *				name: OID
 *				value: ...
 *			}
 *			...
 * }	}	}
 */
bool_t
snmp_trap_v1 (snmp_t *snmp, udp_socket_t *sock, unsigned char *local_ip,
	unsigned trap_type, asn_t *oid, asn_t *value)
{
	asn_t *pdu, *trap, *varlist, *var1;
	buf_t *pkt;
	unsigned char *output, status;
	unsigned long santisec;

	/*debug_printf ("snmp_trap(%#x)\n", traptype);*/
	if (oid) {
		var1 = asn_make_seq (snmp->pool, 2, ASN_SEQUENCE);
		if (! var1) {
			asn_free (oid);
			asn_free (value);
			return 0;
		}
		var1->seq.count = 2;
		var1->seq.arr[0] = oid;
		var1->seq.arr[1] = value;
	} else
		var1 = 0;

	/*
	 * List of variables.
	 */
	varlist = asn_make_seq (snmp->pool, 1, ASN_SEQUENCE);
	if (! varlist) {
		asn_free (var1);
		return 0;
	}
	varlist->seq.count = var1 ? 1 : 0;
	varlist->seq.arr[0] = var1;

	/*
	 * Protocol data unit.
	 */
	pdu = asn_make_seq (snmp->pool, 6, ASN_TRAP);
	if (! pdu) {
		asn_free (varlist);
		return 0;
	}
	pdu->seq.count = 6;
	pdu->seq.arr[5] = varlist;

	/* Enterprise */
	pdu->seq.arr[0] = asn_make_oid (snmp->pool, "1.3.6.1.4.1.20520");
	pdu->seq.arr[0]->oid.id[6] = snmp->enterprise;

	/* Agent IP address */
	pdu->seq.arr[1] = asn_make_int (snmp->pool,
		NTOHL (*(unsigned long*) local_ip), ASN_IP_ADDRESS);

	/* Generic trap code */
	pdu->seq.arr[2] = asn_make_int (snmp->pool, trap_type & 0xff,
		ASN_INTEGER);

	/* Specific trap code */
	pdu->seq.arr[3] = asn_make_int (snmp->pool, trap_type >> 8,
		ASN_INTEGER);

	/* Timestamp */
	santisec = 0;
	if (snmp->ip && snmp->ip->timer) {
		unsigned long msec;
		unsigned days = timer_days (snmp->ip->timer, &msec);
		santisec = msec / 10 + days * 24L * 60 * 60 * 100;
	}
	pdu->seq.arr[4] = asn_make_int (snmp->pool, santisec, ASN_TIME_TICKS);

	/*
	 * Trap.
	 */
	trap = asn_make_seq (snmp->pool, 3, ASN_SEQUENCE);
	if (! trap) {
		asn_free (pdu);
		return 0;
	}
	trap->seq.count = 3;
	trap->seq.arr[2] = pdu;

	/* Version */
	trap->seq.arr[0] = asn_make_int (snmp->pool, 0, ASN_INTEGER);

	/* Community */
	trap->seq.arr[1] = asn_make_string (snmp->pool, snmp->trap_community);

	/* Encode and send */
	status = 0;
	pkt = buf_alloc (snmp->pool, TRAP_PACKET_SIZE, 50);
	if (pkt) {
		output = asn_encode (trap, pkt->payload + pkt->len, pkt->len);
		if (! output) {
			buf_free (pkt);
		} else {
			/* Send */
			assert (output >= pkt->payload);
			buf_add_header (pkt, - (output - pkt->payload));
			/* debug_dump ("trap-v1", pkt->payload, pkt->len); */
			status = udp_sendto (sock, pkt, snmp->trap_addr, 162);
			++snmp->out_traps;
			++snmp->out_pkts;
		}
	}
	asn_free (trap);
	return status;
}
