#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <net/ip.h>
#include <net/netif.h>
#include <mem/mem.h>
#include <crc/crc16-inet.h>

void
icmp_echo_request (ip_t *ip, buf_t *p, netif_t *inp)
{
	icmp_hdr_t *h;
	unsigned char buf [4];

	if (p->tot_len < sizeof(icmp_hdr_t)) {
		/* Bad ICMP echo received. */
		++ip->icmp_in_errors;
		buf_free (p);
		return;
	}
	if (buf_chksum (p, 0) != 0) {
		/* Checksum failed for received ICMP echo. */
		/*debug_printf (CONST("icmp_echo_request: bad checksum\n"));*/
		++ip->icmp_in_errors;
		buf_free (p);
		return;
	}
	h = (icmp_hdr_t*) p->payload;
	memcpy (buf, h->ip.src, 4);
	memcpy (h->ip.src, h->ip.dest, 4);
	memcpy (h->ip.dest, buf, 4);
	h->type = ICMP_ER;

	/* adjust the checksum */
	if (h->chksum >= HTONS(0xffff - (ICMP_ECHO << 8))) {
		h->chksum += HTONS(ICMP_ECHO << 8) + 1;
	} else {
		h->chksum += HTONS(ICMP_ECHO << 8);
	}
	/*if (buf_chksum (p, 0) != 0) debug_printf (CONST("icmp_echo_request: bad reply checksum\n"));*/

/*	debug_printf (CONST("icmp_echo_request: send reply\n"));*/
	netif_output (inp, p, h->ip.dest, h->ip.src);
	++ip->icmp_out_msgs;
        ++ip->icmp_out_echo_reps;
	++ip->out_requests;
}

void
icmp_dest_unreach (ip_t *ip, buf_t *p, small_uint_t op)
{
	buf_t *q;
	icmp_hdr_t *h;
	ip_hdr_t *iphdr = (ip_hdr_t*) p->payload;

	/* ICMP header + IP header + 8 bytes of data */
	q = buf_alloc (ip->pool, 8 + IP_HLEN + 8, 16);
	if (! q) {
		buf_free (p);
		++ip->icmp_out_errors;
		return;
	}
	h = (icmp_hdr_t*) q->payload;
	h->type = ICMP_DUR;
	h->code = op;

	/* copy fields from original packet */
	memcpy (q->payload + 8, p->payload, IP_HLEN + 8);
	buf_free (p);

	/* calculate checksum */
	h->chksum = 0;
	h->chksum = ~crc16_inet (0, q->payload, q->len);

	ip_output (ip, q, iphdr->src, 0, IP_PROTO_ICMP);
	++ip->icmp_out_msgs;
        ++ip->icmp_out_dest_unreachs;
}

void
icmp_time_exceeded (ip_t *ip, buf_t *p)
{
	buf_t *q;
	icmp_hdr_t *h;
	ip_hdr_t *iphdr = (ip_hdr_t*) p->payload;

	/* ICMP header + IP header + 8 bytes of data */
	q = buf_alloc (ip->pool, 8 + IP_HLEN + 8, 16);
	if (! q) {
		buf_free (p);
		++ip->icmp_out_errors;
		return;
	}
	h = (icmp_hdr_t*) q->payload;
	h->type = ICMP_TE;
	h->code = ICMP_TE_TTL;

	/* copy fields from original packet */
	memcpy (q->payload + 8, p->payload, IP_HLEN + 8);
	buf_free (p);

	/* calculate checksum */
	h->chksum = 0;
	h->chksum = ~crc16_inet (0, q->payload, q->len);

	ip_output (ip, q, iphdr->src, 0, IP_PROTO_ICMP);
	++ip->icmp_out_msgs;
	++ip->icmp_out_time_excds;
}
