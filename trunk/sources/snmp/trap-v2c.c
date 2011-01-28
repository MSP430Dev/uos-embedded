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
 * Send trap, SNMP v2c (RFC 1901, RFC 1905).
 * SEQUENCE {
 *	version: INTEGER [=1]
 *	community: STRING
 *	pdu: SEQUENCE [=TRAP_V2] {
 *		request-id: Integer32
 *		error-status: INTEGER
 *		error-index: INTEGER
 *		varlist: SEQUENCE {
 *			var1: SEQUENCE {
 *				name: sysUpTime
 *				value: ...
 *				name: snmpTrapOID
 *				value: ...
 *				name: OID
 *				value: ...
 *			}
 *			...
 * }	}	}
 */
bool_t
snmp_trap_v2c (snmp_t *snmp, udp_socket_t *sock, const char *trap_type,
	asn_t *oid, asn_t *value)
{
	asn_t *pdu, *trap, *varlist, *var1, *var2, *var3;
	buf_t *pkt;
	unsigned char *output, status;
	unsigned long santisec;

	/*debug_printf ("snmp_trap(%#x)\n", traptype);*/
	/* Additional trap-specific variable. */
	if (oid) {
		var3 = asn_make_seq (snmp->pool, 2, ASN_SEQUENCE);
		if (! var3) {
			asn_free (oid);
			asn_free (value);
			return 0;
		}
		var3->seq.count = 2;
		var3->seq.arr[0] = oid;
		var3->seq.arr[1] = value;
	} else
		var3 = 0;

	/* First obligatory binding: sysUpTime. */
	var1 = asn_make_seq (snmp->pool, 2, ASN_SEQUENCE);
	if (! var1) {
		asn_free (var3);
		return 0;
	}
	var1->seq.count = 2;
	var1->seq.arr[0] = asn_make_oid (snmp->pool, "1.3.6.1.2.1.1.3.0");
	if (snmp->ip && snmp->ip->timer) {
		unsigned long msec;
		unsigned days = timer_days (snmp->ip->timer, &msec);
		santisec = msec / 10 + days * 24L * 60 * 60 * 100;
	} else
		santisec = 0;
	var1->seq.arr[1] = asn_make_int (snmp->pool, santisec, ASN_TIME_TICKS);

	/* Second obligatory binding: snmpTrapOID. */
	var2 = asn_make_seq (snmp->pool, 2, ASN_SEQUENCE);
	if (! var2) {
		asn_free (var1);
		asn_free (var3);
		return 0;
	}
	var2->seq.count = 2;
	var2->seq.arr[0] = asn_make_oid (snmp->pool, "1.3.6.1.6.3.1.1.4.1.0");
	var2->seq.arr[1] = asn_make_oid (snmp->pool, trap_type);

	/*
	 * List of variables.
	 */
	varlist = asn_make_seq (snmp->pool, 3, ASN_SEQUENCE);
	if (! varlist) {
		asn_free (var1);
		asn_free (var2);
		asn_free (var3);
		return 0;
	}
	varlist->seq.count = var3 ? 3 : 2;
	varlist->seq.arr[0] = var1;
	varlist->seq.arr[1] = var2;
	varlist->seq.arr[2] = var3;

	/*
	 * Protocol data unit.
	 */
	pdu = asn_make_seq (snmp->pool, 4, ASN_TRAP_V2);
	if (! pdu) {
		asn_free (varlist);
		return 0;
	}
	pdu->seq.count = 4;
	pdu->seq.arr[3] = varlist;

	/* Request Id. */
	pdu->seq.arr[0] = asn_make_int (snmp->pool, snmp->request_id++,
		ASN_INTEGER);

	/* Error status and index. */
	pdu->seq.arr[1] = asn_make_int (snmp->pool, 0, ASN_INTEGER);
	pdu->seq.arr[2] = asn_make_int (snmp->pool, 0, ASN_INTEGER);

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
	trap->seq.arr[0] = asn_make_int (snmp->pool, 1, ASN_INTEGER);

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
			/* debug_dump ("trap-v2", pkt->payload, pkt->len); */
			status = udp_sendto (sock, pkt, snmp->trap_addr, 162);
			/* debug_printf ("trap-v2-send %S\n", status ? "Ok" : "failed"); */
			++snmp->out_traps;
			++snmp->out_pkts;
		}
	}
	asn_free (trap);
	return status;
}
