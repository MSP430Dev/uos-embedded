/*
 * The output functions of TCP.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <mem/mem.h>
#include <crc/crc16-inet.h>
#include <net/netif.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>

/*
#undef tcp_debug
#define tcp_debug			wdog_alive(); debug_printf
*/

/*
 * Send data or options or flags.
 * Allocate a new buf and append it to socket send queue.
 * Must be called with socket locked.
 * Return 0 on error.
 */
int
tcp_enqueue (tcp_socket_t *s, void *arg, unsigned short len,
	unsigned char flags, unsigned char *optdata, unsigned char optlen)
{
	tcp_segment_t *seg, *useg, *queue;
	unsigned long left, seqno;
	unsigned short seglen;
	void *ptr;
	unsigned char queuelen;

	tcp_debug (CONST("tcp_enqueue(s=%p, arg=%p, len=%u, flags=%x) queuelen = %u\n"),
		(void*) s, arg, len, flags, s->snd_queuelen);
	left = len;
	ptr = arg;
	/* fail on too much data */
	if (len > s->snd_buf) {
		tcp_debug (CONST("tcp_enqueue: too much data (len=%u > snd_buf=%u)\n"),
			len, s->snd_buf);
		return 0;
	}

	/* Check if the queue length exceeds the configured maximum queue
	 * length. If so, we return an error. */
	queue = 0;
	queuelen = s->snd_queuelen;
	if (queuelen >= TCP_SND_QUEUELEN) {
		tcp_debug (CONST("tcp_enqueue: too long queue %u (max %u)\n"),
			queuelen, TCP_SND_QUEUELEN);
		return 0;
	}
	if (queuelen != 0) {
		assert (s->unacked != 0 || s->unsent != 0);
	}
	seg = 0;
	seglen = 0;

	/* seqno will be the sequence number of the first segment enqueued
	 * by the call to this function. */
	seqno = s->snd_lbb;

	/* First, break up the data into segments and tuck them together in
	 * the local "queue" variable. */
	while (queue == 0 || left > 0) {

		/* The segment length should be the MSS if the data to be
		 * enqueued is larger than the MSS. */
		seglen = left > s->mss ? s->mss : left;

		/* Allocate memory for tcp_segment, and fill in fields. */
		seg = mem_alloc (s->ip->pool, sizeof (tcp_segment_t));
		if (seg == 0) {
			tcp_debug (CONST("tcp_enqueue: cannot allocate tcp_segment\n"));
			goto memerr;
		}

		if (queue == 0) {
			queue = seg;
		} else {
			/* Attach the segment to the end of the queued segments. */
			for (useg = queue; useg->next != 0; useg = useg->next)
				;
			useg->next = seg;
		}

		/* Allocate memory and copy data into it.
		 * If optdata is != NULL, we have options instead of data. */
		seg->p = buf_alloc (s->ip->pool, optdata ? optlen : seglen,
			16 + IP_HLEN + TCP_HLEN);
		if (seg->p == 0) {
			tcp_debug (CONST("tcp_enqueue: could not allocate %u bytes\n"),
				optdata ? optlen : seglen);
			goto memerr;
		}
		++queuelen;
		if (arg != 0) {
			memcpy (seg->p->payload, ptr, seglen);
		}
		seg->dataptr = seg->p->payload;
		seg->len = seglen;

		/* Build TCP header. */
		if (! buf_add_header (seg->p, TCP_HLEN)) {
			tcp_debug (CONST("tcp_enqueue: no room for TCP header\n"));
			goto memerr;
		}
		seg->tcphdr = (tcp_hdr_t*) seg->p->payload;
		seg->tcphdr->src = HTONS (s->local_port);
		seg->tcphdr->dest = HTONS (s->remote_port);
		seg->tcphdr->seqno = HTONL (seqno);
		seg->tcphdr->urgp = 0;
		seg->tcphdr->flags = flags;
		/* don't fill in tcphdr->ackno and tcphdr->wnd until later */

		if (optdata) {
			/* Copy options into data portion of segment.
			 * Options can thus only be sent in non data carrying
			 * segments such as SYN|ACK. */
			memcpy (seg->dataptr, optdata, optlen);
			seg->tcphdr->offset = (5 + optlen / 4) << 4;
		} else {
			seg->tcphdr->offset = 5 << 4;
		}
		tcp_debug (CONST("tcp_enqueue: queueing %lu:%lu (flags 0x%x)\n"),
			NTOHL (seg->tcphdr->seqno),
			NTOHL (seg->tcphdr->seqno) + TCP_TCPLEN (seg),
			flags);

		left -= seglen;
		seqno += seglen;
		ptr = (void*) ((char*) ptr + seglen);
	}

	/* Now that the data to be enqueued has been broken up into TCP
	segments in the queue variable, we add them to the end of the
	s->unsent queue. */
	if (s->unsent == 0) {
		useg = 0;
	} else {
		for (useg = s->unsent; useg->next != 0; useg = useg->next)
			;
	}

	/* If there is room in the last buf on the unsent queue,
	chain the first buf on the queue together with that. */
	if (useg != 0 && TCP_TCPLEN (useg) != 0 &&
	    ! (useg->tcphdr->flags & (TCP_SYN | TCP_FIN)) &&
	    ! (flags & (TCP_SYN | TCP_FIN)) &&
	    useg->len + queue->len <= s->mss) {
		/* Remove TCP header from first segment. */
		buf_add_header (queue->p, -TCP_HLEN);
		buf_chain (useg->p, queue->p);

		useg->len += queue->len;
		useg->next = queue->next;

		tcp_debug (CONST("tcp_enqueue: chaining, new len %u\n"),
			useg->len);
		if (seg == queue) {
			seg = 0;
		}
		mem_free (queue);
	} else {
		if (useg == 0) {
			s->unsent = queue;
		} else {
			useg->next = queue;
		}
	}
	if ((flags & TCP_SYN) || (flags & TCP_FIN)) {
		++len;
	}
	s->snd_lbb += len;
	s->snd_buf -= len;
	s->snd_queuelen = queuelen;
	tcp_debug (CONST("tcp_enqueue: done, queuelen = %d\n"),
		s->snd_queuelen);
	if (s->snd_queuelen != 0) {
		assert (s->unacked != 0 || s->unsent != 0);
	}

	/* Set the PSH flag in the last segment that we enqueued, but only
	if the segment has data (indicated by seglen > 0). */
	if (seg != 0 && seglen > 0 && seg->tcphdr != 0) {
		seg->tcphdr->flags |= TCP_PSH;
	}
	return 1;

memerr:
	++s->ip->tcp_out_errors;
	if (queue != 0) {
		tcp_segments_free (queue);
	}
	if (s->snd_queuelen != 0) {
		assert (s->unacked != 0 || s->unsent != 0);
	}
	tcp_debug (CONST("tcp_enqueue: %d (with mem err)\n"),
		s->snd_queuelen);
	return 0;
}

static void
tcp_output_segment (tcp_segment_t *seg, tcp_socket_t *s)
{
	unsigned int n;
	netif_t *netif;
	buf_t *p;

	/* The TCP header has already been constructed, but the ackno and
	 * wnd fields remain. */
	seg->tcphdr->ackno = HTONL (s->rcv_nxt);

	/* silly window avoidance */
	if (s->rcv_wnd < s->mss) {
		seg->tcphdr->wnd = 0;
	} else {
		/* advertise our receive window size in this TCP segment */
		seg->tcphdr->wnd = HTONS (s->rcv_wnd);
	}

	/* If we don't have a local IP address, we get one by
	 * calling ip_route(). */
	if (memcmp (s->local_ip, IP_ADDR(0), 4) == 0) {
		unsigned char *local_ip;

		netif = route_lookup (s->ip, s->remote_ip, 0, &local_ip);
		if (! netif)
			return;
		memcpy (s->local_ip, local_ip, 4);
	}

	s->rtime = 0;

	if (s->rttest == 0) {
		s->rttest = s->ip->tcp_ticks;
		s->rtseq = NTOHL (seg->tcphdr->seqno);
	}
	s->snd_nxt = NTOHL (seg->tcphdr->seqno) + TCP_TCPLEN (seg);
	if (TCP_SEQ_LT (s->snd_max, s->snd_nxt)) {
		s->snd_max = s->snd_nxt;
	}
	tcp_debug (CONST("tcp_output_segment: %lu:%lu, snd_nxt = %u\n"),
		HTONL (seg->tcphdr->seqno),
		HTONL (seg->tcphdr->seqno) + seg->len, s->snd_nxt);

	p = seg->p;
	n = (unsigned int) ((unsigned char*) seg->tcphdr -
		(unsigned char*) p->payload);
	p->len -= n;
	p->tot_len -= n;
	p->payload = (unsigned char*) seg->tcphdr;

	seg->tcphdr->chksum = 0;
	n = buf_chksum (p, crc16_inet_header (s->local_ip, s->remote_ip,
		IP_PROTO_TCP, p->tot_len));
	if (p->tot_len & 1) {
		/* Invert checksum bytes. */
		n = (n << 8) | (unsigned char) (n >> 8);
	}
	seg->tcphdr->chksum = n;

	p = buf_copy (p);
	if (! p)
		return;
/*	buf_print_tcp (p);*/

	++s->ip->tcp_out_datagrams;
	ip_output (s->ip, p, s->remote_ip, s->local_ip, IP_PROTO_TCP);
}

/*
 * Find out what we can send and send it.
 * Must be called with ip locked.
 * Return 0 on error.
 */
int
tcp_output (tcp_socket_t *s)
{
	buf_t *p;
	tcp_hdr_t *tcphdr;
	tcp_segment_t *seg, *useg;
	unsigned long wnd;

	/* First, check if we are invoked by the TCP input processing code.
	 * If so, we do not output anything. Instead, we rely on the input
	 * processing code to call us when input processing is done with. */
	if (s->ip->tcp_input_socket == s) {
		return 1;
	}

	wnd = (s->snd_wnd < s->cwnd) ? s->snd_wnd : s->cwnd;
	seg = s->unsent;

	/* If the TF_ACK_NOW flag is set, we check if there is data that is
	 * to be sent. If data is to be sent out, we'll just piggyback our
	 * acknowledgement with the outgoing segment. If no data will be
	 * sent (either because the ->unsent queue is empty or because the
	 * window doesn't allow it) we'll have to construct an empty ACK
	 * segment and send it. */
	if ((s->flags & TF_ACK_NOW) && (seg == 0 ||
	    NTOHL (seg->tcphdr->seqno) - s->lastack + seg->len > wnd)) {
		s->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
		p = buf_alloc (s->ip->pool, TCP_HLEN, 16 + IP_HLEN);
		if (p == 0) {
			tcp_debug (CONST("tcp_output: (ACK) could not allocate buf\n"));
			return 0;
		}
		tcp_debug (CONST("tcp_output: sending ACK for %lu\n"),
			s->rcv_nxt);

		tcphdr = (tcp_hdr_t*) p->payload;
		tcphdr->src = HTONS (s->local_port);
		tcphdr->dest = HTONS (s->remote_port);
		tcphdr->seqno = HTONL (s->snd_nxt);
		tcphdr->ackno = HTONL (s->rcv_nxt);
		tcphdr->flags = TCP_ACK;
		tcphdr->wnd = HTONS (s->rcv_wnd);
		tcphdr->urgp = 0;
		tcphdr->offset = 5 << 4;

		tcphdr->chksum = 0;
		tcphdr->chksum = buf_chksum (p,
			crc16_inet_header (s->local_ip,	s->remote_ip,
			IP_PROTO_TCP, p->tot_len));

		ip_output (s->ip, p, s->remote_ip, s->local_ip, IP_PROTO_TCP);
		return 1;
	}
	if (seg == 0) {
		tcp_debug (CONST("tcp_output: nothing to send, snd_wnd %lu, cwnd %lu, wnd %lu, ack %lu\n"),
			s->snd_wnd, s->cwnd, wnd, s->lastack);
	}

	while (seg != 0 &&
	    NTOHL (seg->tcphdr->seqno) - s->lastack + seg->len <= wnd) {
		tcp_debug (CONST("tcp_output: snd_wnd %lu, cwnd %lu, wnd %lu, effwnd %lu, seq %lu, ack %lu\n"),
			s->snd_wnd, s->cwnd, wnd,
			NTOHL (seg->tcphdr->seqno) + seg->len - s->lastack,
			NTOHL (seg->tcphdr->seqno), s->lastack);
		s->unsent = seg->next;

		if (s->state != SYN_SENT) {
			seg->tcphdr->flags |= TCP_ACK;
			s->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
		}

		tcp_output_segment (seg, s);

		/* put segment on unacknowledged list if length > 0 */
		if (TCP_TCPLEN (seg) > 0) {
			seg->next = 0;
			if (s->unacked == 0) {
				s->unacked = seg;
			} else {
				for (useg = s->unacked; useg->next != 0; useg = useg->next)
					;
				useg->next = seg;
			}
		} else {
			tcp_segment_free (seg);
		}
		seg = s->unsent;
	}
	return 1;
}

void
tcp_rst (ip_t *ip, unsigned long seqno, unsigned long ackno,
	unsigned char *local_ip, unsigned char *remote_ip,
	unsigned short local_port, unsigned short remote_port)
{
	buf_t *p;
	tcp_hdr_t *tcphdr;

	p = buf_alloc (ip->pool, TCP_HLEN, 16 + IP_HLEN);
	if (p == 0) {
		tcp_debug (CONST("tcp_rst: could not allocate memory\n"));
		return;
	}

	tcphdr = (tcp_hdr_t*) p->payload;
	tcphdr->src = HTONS (local_port);
	tcphdr->dest = HTONS (remote_port);
	tcphdr->seqno = HTONL (seqno);
	tcphdr->ackno = HTONL (ackno);
	tcphdr->flags = TCP_RST | TCP_ACK;
	tcphdr->wnd = HTONS (TCP_WND);
	tcphdr->urgp = 0;
	tcphdr->offset = 5 << 4;

	tcphdr->chksum = 0;
	tcphdr->chksum = buf_chksum (p, crc16_inet_header (local_ip,
		remote_ip, IP_PROTO_TCP, p->tot_len));

	++ip->tcp_out_datagrams;
	ip_output (ip, p, remote_ip, local_ip, IP_PROTO_TCP);
	tcp_debug (CONST("tcp_rst: seqno %lu ackno %lu.\n"), seqno, ackno);
}

void
tcp_rexmit (tcp_socket_t *s)
{
	tcp_segment_t *seg;

	if (s->unacked == 0) {
		return;
	}

	/* Move all unacked segments to the unsent queue. */
	for (seg = s->unacked; seg->next != 0; seg = seg->next)
		;

	seg->next = s->unsent;
	s->unsent = s->unacked;

	s->unacked = 0;

	s->snd_nxt = NTOHL (s->unsent->tcphdr->seqno);
	tcp_debug (CONST("tcp_rexmit: snd_nxt = %u\n"), s->snd_nxt);

	++s->nrtx;

	/* Don't take any rtt measurements after retransmitting. */
	s->rttest = 0;

	/* Do the actual retransmission. */
	tcp_output (s);
}
