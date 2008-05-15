/*
 * This file contains common functions for the TCP implementation,
 * such as functinos for manipulating the data structures and
 * the TCP timer functions. TCP functions related to input and
 * output is found in tcp_input.c and tcp_output.c respectively.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <mem/mem.h>
#include <net/ip.h>
#include <net/tcp.h>

/*
 * Called every 500 ms and implements the retransmission timer and the timer that
 * removes PCBs that have been in TIME-WAIT for enough time. It also increments
 * various timers such as the inactivity timer in each PCB.
 */
void __weak
tcp_slowtmr (ip_t *ip)
{
	tcp_socket_t *s, *prev;
	unsigned long eff_wnd;
	unsigned char s_remove;      /* flag if a PCB should be removed */
	static const unsigned char tcp_backoff[13] =
		{ 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7};

	++ip->tcp_ticks;
	/* if (s == 0)
		tcp_debug (CONST("tcp_slowtmr: no active sockets\n")); */

	/* Steps through all of the active PCBs. */
	prev = 0;
	for (s=ip->tcp_sockets; s; s=s->next) {
		/* tcp_debug (CONST("tcp_slowtmr: processing active sockets\n")); */
		lock_take (&s->lock);
		assert (s->state != CLOSED);
		assert (s->state != LISTEN);
		assert (s->state != TIME_WAIT);
		s_remove = 0;

		if (s->state == SYN_SENT && s->nrtx == TCP_SYNMAXRTX) {
			++s_remove;
			tcp_debug (CONST("tcp_slowtmr: max SYN retries reached\n"));
		} else if (s->nrtx == TCP_MAXRTX) {
			++s_remove;
			tcp_debug (CONST("tcp_slowtmr: max DATA retries reached\n"));
		} else {
			++s->rtime;
			if (s->unacked != 0 && s->rtime >= s->rto) {
				/* Time for a retransmission. */
				tcp_debug (CONST("tcp_slowtmr: rtime %u s->rto %u\n"),
					s->rtime, s->rto);

				/* Double retransmission time-out unless we are trying to
				 * connect to somebody (i.e., we are in SYN_SENT). */
				if (s->state != SYN_SENT) {
					s->rto = ((s->sa >> 3) + s->sv) <<
						tcp_backoff[s->nrtx];
				}
				tcp_rexmit (s);

				/* Reduce congestion window and ssthresh. */
				eff_wnd = (s->cwnd < s->snd_wnd) ?
					s->cwnd : s->snd_wnd;
				s->ssthresh = eff_wnd >> 1;
				if (s->ssthresh < s->mss) {
					s->ssthresh = s->mss * 2;
				}
				s->cwnd = s->mss;
				tcp_debug (CONST("tcp_slowtmr: cwnd %u ssthresh %u\n"),
					s->cwnd, s->ssthresh);
			}
		}
		/* Check if this PCB has stayed too long in FIN-WAIT-2 */
		if (s->state == FIN_WAIT_2 && ip->tcp_ticks - s->tmr >
		    TCP_FIN_WAIT_TIMEOUT / TCP_SLOW_INTERVAL) {
			++s_remove;
			tcp_debug (CONST("tcp_slowtmr: removing s stuck in FIN-WAIT-2\n"));
		}

		/* Check if this PCB has stayed too long in SYN-RCVD */
		if (s->state == SYN_RCVD && ip->tcp_ticks - s->tmr >
		    TCP_SYN_RCVD_TIMEOUT / TCP_SLOW_INTERVAL) {
			++s_remove;
			tcp_debug (CONST("tcp_slowtmr: removing s stuck in SYN-RCVD\n"));
		}

		/* If the PCB should be removed, do it. */
		if (s_remove) {
			tcp_socket_purge (s);
			tcp_set_socket_state (s, CLOSED);

			/* Remove PCB from tcp_sockets list. */
			if (prev != 0) {
				assert (s != ip->tcp_sockets);
				prev->next = s->next;
			} else {
				/* This PCB was the first. */
				assert (ip->tcp_sockets == s);
				ip->tcp_sockets = s->next;
			}
		} else {
			prev = s;
		}
		lock_release (&s->lock);
	}

	/* Steps through all of the TIME-WAIT PCBs. */
	prev = 0;
	for (s=ip->tcp_closing_sockets; s; s=s->next) {
		lock_take (&s->lock);
		assert (s->state == TIME_WAIT);

		/* Check if this PCB has stayed long enough in TIME-WAIT */
		if (ip->tcp_ticks - s->tmr > 2 * TCP_MSL / TCP_SLOW_INTERVAL) {
			tcp_socket_purge (s);

			/* Remove PCB from tcp_closing_sockets list. */
			if (prev != 0) {
				assert (s != ip->tcp_closing_sockets);
				prev->next = s->next;
			} else {
				/* This PCB was the first. */
				assert (ip->tcp_closing_sockets == s);
				ip->tcp_closing_sockets = s->next;
			}
		} else {
			prev = s;
		}
		lock_release (&s->lock);
	}
}

/*
 * Is called every TCP_FINE_TIMEOUT (100 ms) and sends delayed ACKs.
 */
void __weak
tcp_fasttmr (ip_t *ip)
{
	tcp_socket_t *s;

	/* send delayed ACKs */
	for (s=ip->tcp_sockets; s; s=s->next) {
		if (s->flags & TF_ACK_DELAY) {
			tcp_debug (CONST("tcp_fasttmr: delayed ACK\n"));
			tcp_ack_now (s);
			s->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
		}
	}
}

/*
 * Deallocates a list of TCP segments (tcp_seg structures).
 */
unsigned char
tcp_segments_free (tcp_segment_t *seg)
{
	unsigned char count = 0;
	tcp_segment_t *next;

	for (count=0; seg != 0; seg = next) {
		next = seg->next;
		count += tcp_segment_free (seg);
	}
	return count;
}

/*
 * Frees a TCP segment.
 */
unsigned char
tcp_segment_free (tcp_segment_t *seg)
{
	unsigned char count = 0;

	if (! seg)
		return 0;

	if (seg->p != 0) {
		count = buf_free (seg->p);
	}
	mem_free (seg);
	return count;
}

/*
 * Creates a new TCP protocol control block but doesn't place it on
 * any of the TCP PCB lists.
 */
tcp_socket_t *
tcp_alloc (ip_t *ip)
{
	tcp_socket_t *s;
	unsigned long iss;

	s = mem_alloc (ip->pool, sizeof(tcp_socket_t));
	if (s == 0) {
		return 0;
	}

	lock_init (&s->lock);
	s->ip = ip;
	s->snd_buf = TCP_SND_BUF;
	s->snd_queuelen = 0;
	s->rcv_wnd = TCP_WND;
	s->mss = TCP_MSS;
	s->rto = 3000 / TCP_SLOW_INTERVAL;
	s->sa = 0;
	s->sv = 3000 / TCP_SLOW_INTERVAL;
	s->rtime = 0;
	s->cwnd = 1;
	iss = tcp_next_seqno (ip);
	s->snd_wl2 = iss;
	s->snd_nxt = iss;
	s->snd_max = iss;
	s->lastack = iss;
	s->snd_lbb = iss;
	s->tmr = ip->tcp_ticks;
	return s;
}

/*
 * Purges a TCP PCB. Removes any buffered data and frees the buffer memory.
 */
void
tcp_socket_purge (tcp_socket_t *s)
{
	tcp_debug (CONST("tcp_socket_purge\n"));
	if (s->unsent != 0) {
		tcp_debug (CONST("tcp_socket_purge: not all data sent\n"));
		tcp_segments_free (s->unsent);
		s->unsent = 0;
	}
	if (s->unacked != 0) {
		tcp_debug (CONST("tcp_socket_purge: data left on ->unacked\n"));
		tcp_segments_free (s->unacked);
		s->unacked = 0;
	}
}

/*
 * Change socket state. Send a signal to notify a user.
 */
void
tcp_set_socket_state (tcp_socket_t *s, tcp_state_t newstate)
{
	s->state = newstate;
	lock_signal (&s->lock, 0);
}

/*
 * Purges the PCB and removes it from a PCB list. Any delayed ACKs are sent first.
 */
void
tcp_socket_remove (tcp_socket_t **slist, tcp_socket_t *s)
{
	tcp_list_remove (slist, s);

	tcp_socket_purge (s);

	/* if there is an outstanding delayed ACKs, send it */
	if (s->state != TIME_WAIT && s->state != LISTEN &&
	    (s->flags & TF_ACK_DELAY)) {
		s->flags |= TF_ACK_NOW;
		tcp_output (s);
	}
	tcp_set_socket_state (s, CLOSED);

	assert (tcp_debug_verify (s->ip));
}

buf_t *
tcp_queue_get (tcp_socket_t *q)
{
	buf_t *p;

	if (q->count == 0) {
		/*tcp_debug (CONST("tcp_queue_get: returned 0\n"));*/
		return 0;
	}
	assert (q->head >= q->queue);
	assert (q->head < q->queue + SOCKET_QUEUE_SIZE);
	assert (*q->head != 0);

	/* Get the first packet from queue. */
	p = *q->head;

	/* Advance head pointer. */
	++q->head;
	--q->count;
	if (q->head >= q->queue + SOCKET_QUEUE_SIZE)
		q->head = q->queue;

	/* Advertise a larger window when the data has been processed. */
	q->rcv_wnd += p->tot_len;
	if (q->rcv_wnd > TCP_WND) {
		q->rcv_wnd = TCP_WND;
	}
	/*tcp_debug (CONST("tcp_queue_get: returned 0x%04x\n"), p);*/
	return p;
}

void
tcp_queue_put (tcp_socket_t *q, buf_t *p)
{
	buf_t **tail;

	/*tcp_debug (CONST("tcp_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n"), p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < SOCKET_QUEUE_SIZE);

	if (q->head == 0)
		q->head = q->queue;

	/* Compute the last place in the queue. */
	tail = q->head + q->count;
	if (tail >= q->queue + SOCKET_QUEUE_SIZE)
		tail -= SOCKET_QUEUE_SIZE;

	/* Put the packet in. */
	*tail = p;
	++q->count;
	/*tcp_debug (CONST("    on return count = %d, head = 0x%04x\n"), q->count, q->head);*/
}

void
tcp_queue_free (tcp_socket_t *q)
{
	while (q->count > 0) {
		assert (q->head >= q->queue);
		assert (q->head < q->queue + SOCKET_QUEUE_SIZE);
		assert (*q->head != 0);

		/* Remove packet from queue. */
		buf_free (*q->head);

		/* Advance head pointer. */
		++q->head;
		--q->count;
		if (q->head >= q->queue + SOCKET_QUEUE_SIZE)
			q->head = q->queue;
	}
}

#ifdef TCP_DEBUG
void
tcp_debug_print_header (tcp_hdr_t *tcphdr)
{
	debug_printf (CONST("TCP header:\n"));
	debug_printf (CONST("+-------------------------------+\n"));
	debug_printf (CONST("|      %04x     |      %04x     | (src port, dest port)\n"),
		tcphdr->src, tcphdr->dest);
	debug_printf (CONST("+-------------------------------+\n"));
	debug_printf (CONST("|            %08lu           | (seq no)\n"),
		tcphdr->seqno);
	debug_printf (CONST("+-------------------------------+\n"));
	debug_printf (CONST("|            %08lu           | (ack no)\n"),
		tcphdr->ackno);
	debug_printf (CONST("+-------------------------------+\n"));
	debug_printf (CONST("| %2u |    |%u%u%u%u%u|    %5u      | (offset, flags ("),
		tcphdr->offset,
		tcphdr->flags >> 5 & 1,
		tcphdr->flags >> 4 & 1,
		tcphdr->flags >> 3 & 1,
		tcphdr->flags >> 2 & 1,
		tcphdr->flags >> 1 & 1,
		tcphdr->flags & 1,
		tcphdr->wnd);
	tcp_debug_print_flags (tcphdr->flags);
	debug_printf (CONST("), win)\n"));
	debug_printf (CONST("+-------------------------------+\n"));
	debug_printf (CONST("|    0x%04x     |     %5u     | (chksum, urgp)\n"),
		NTOHS (tcphdr->chksum), NTOHS (tcphdr->urgp));
	debug_printf (CONST("+-------------------------------+\n"));
}

const char *
tcp_state_name (tcp_state_t state)
{
	switch (state) {
	case CLOSED:	  return CONST("CLOSED");	break;
	case LISTEN:	  return CONST("LISTEN");	break;
	case SYN_SENT:	  return CONST("SYN_SENT");	break;
	case SYN_RCVD:	  return CONST("SYN_RCVD");	break;
	case ESTABLISHED: return CONST("ESTABLISHED");	break;
	case FIN_WAIT_1:  return CONST("FIN_WAIT_1");	break;
	case FIN_WAIT_2:  return CONST("FIN_WAIT_2");	break;
	case CLOSE_WAIT:  return CONST("CLOSE_WAIT");	break;
	case CLOSING:	  return CONST("CLOSING");	break;
	case LAST_ACK:	  return CONST("LAST_ACK");	break;
	case TIME_WAIT:	  return CONST("TIME_WAIT");	break;
	}
	return CONST("???");
}

void
tcp_debug_print_flags (unsigned char flags)
{
	if (flags & TCP_FIN) debug_printf (CONST(" FIN"));
	if (flags & TCP_SYN) debug_printf (CONST(" SYN"));
	if (flags & TCP_RST) debug_printf (CONST(" RST"));
	if (flags & TCP_PSH) debug_printf (CONST(" PSH"));
	if (flags & TCP_ACK) debug_printf (CONST(" ACK"));
	if (flags & TCP_URG) debug_printf (CONST(" URG"));
}

void
tcp_debug_print_sockets (ip_t *ip)
{
	tcp_socket_t *s;
	const char *fmt = CONST("Local port %u, foreign port %u "
			        "snd_nxt %lu rcv_nxt %lu state %S\n");

	debug_printf (CONST("Active PCB states:\n"));
	for (s=ip->tcp_sockets; s; s=s->next) {
		debug_printf (fmt, s->local_port, s->remote_port,
			s->snd_nxt, s->rcv_nxt, tcp_state_name (s->state));
	}
	debug_printf (CONST("Listen PCB states:\n"));
	for (s=ip->tcp_listen_sockets; s; s=s->next) {
		debug_printf (fmt, s->local_port, s->remote_port,
			s->snd_nxt, s->rcv_nxt, tcp_state_name (s->state));
	}
	debug_printf (CONST("TIME-WAIT PCB states:\n"));
	for (s=ip->tcp_closing_sockets; s; s=s->next) {
		debug_printf (fmt, s->local_port, s->remote_port,
			s->snd_nxt, s->rcv_nxt, tcp_state_name (s->state));
	}
}

int
tcp_debug_verify (ip_t *ip)
{
	tcp_socket_t *s;

	for (s = ip->tcp_sockets; s != 0; s = s->next) {
		assert (s->state != CLOSED);
		assert (s->state != LISTEN);
		assert (s->state != TIME_WAIT);
	}
	for (s = ip->tcp_closing_sockets; s != 0; s = s->next) {
		assert (s->state == TIME_WAIT);
	}
	return 1;
}
#endif /* TCP_DEBUG */
