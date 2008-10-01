/*
 * The input processing functions of TCP.
 *
 * These functions are generally called in the order:
 * (ip_input() ->) tcp_input() -> tcp_process() -> tcp_receive() (-> application).
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <mem/mem.h>
#include <crc/crc16-inet.h>
#include <net/netif.h>
#include <net/ip.h>
#include <net/tcp.h>

/*
 * Called by tcp_process. Checks if the given segment is an ACK for outstanding
 * data, and if so frees the memory of the buffered data. Next, it places the
 * segment on any of the receive queues (s->recved or s->ooseq).
 *
 * If the incoming segment constitutes an ACK for a segment that was used
 * for RTT estimation, the RTT is estimated here as well.
 */
static void
tcp_receive (tcp_socket_t *s, tcp_segment_t *inseg, tcp_hdr_t *h)
{
	tcp_segment_t *next;
	buf_t *p;
	long off;
	int m;
	unsigned long right_wnd_edge;

	if (s->ip->tcp_input_flags & TCP_ACK) {
		right_wnd_edge = s->snd_wnd + s->snd_wl1;

		/* Update window. */
		if (TCP_SEQ_LT (s->snd_wl1, s->ip->tcp_input_seqno) ||
		    (s->snd_wl1 == s->ip->tcp_input_seqno &&
		    TCP_SEQ_LT (s->snd_wl2, s->ip->tcp_input_ackno)) ||
		    (s->snd_wl2 == s->ip->tcp_input_ackno &&
		    h->wnd > s->snd_wnd)) {
			s->snd_wnd = h->wnd;
			s->snd_wl1 = s->ip->tcp_input_seqno;
			s->snd_wl2 = s->ip->tcp_input_ackno;
			tcp_debug (CONST("tcp_receive: window update %lu\n"),
				s->snd_wnd);
		} else if (s->snd_wnd != h->wnd) {
			tcp_debug (CONST("tcp_receive: no window update lastack %lu snd_max %lu ackno %lu wl1 %lu seqno %lu wl2 %lu\n"),
				s->lastack, s->snd_max, s->ip->tcp_input_ackno,
				s->snd_wl1, s->ip->tcp_input_seqno, s->snd_wl2);
		}

		if (s->lastack == s->ip->tcp_input_ackno) {
			s->acked = 0;

			if (s->snd_wl1 + s->snd_wnd == right_wnd_edge) {
				++s->dupacks;
				if (s->dupacks >= 3 && s->unacked != 0) {
					if (! (s->flags & TF_INFR)) {
						/* This is fast retransmit. Retransmit the first unacked segment. */
						tcp_debug (CONST("tcp_receive: dupacks %u (%lu), fast retransmit %lu\n"),
							(unsigned int) s->dupacks, s->lastack,
							NTOHL (s->unacked->tcphdr->seqno));
						tcp_rexmit (s);
						/* Set ssthresh to max (FlightSize / 2, 2*SMSS) */
						s->ssthresh = (s->snd_max -
							s->lastack) / 2;
						if (s->ssthresh < 2 * s->mss)
							s->ssthresh = 2 * s->mss;

						s->cwnd = s->ssthresh + 3 * s->mss;
						s->flags |= TF_INFR;
					} else {
						/* Inflate the congestion window,
						 * but not if it means that
						 * the value overflows. */
						if ((unsigned short) (s->cwnd + s->mss) > s->cwnd) {
							s->cwnd += s->mss;
						}
					}
				}
			} else {
				tcp_debug (CONST("tcp_receive: dupack averted %lu %lu\n"),
					s->snd_wl1 + s->snd_wnd, right_wnd_edge);
			}
		} else if (TCP_SEQ_LT (s->lastack, s->ip->tcp_input_ackno) &&
		    TCP_SEQ_LEQ (s->ip->tcp_input_ackno, s->snd_max)) {
			/* We come here when the ACK acknowledges new data. */

			/* Reset the "IN Fast Retransmit" flag, since we are
			 * no longer in fast retransmit. Also reset
			 * the congestion window to the slow start threshold. */
			if (s->flags & TF_INFR) {
				s->flags &= ~TF_INFR;
				s->cwnd = s->ssthresh;
			}

			/* Reset the number of retransmissions. */
			s->nrtx = 0;

			/* Reset the retransmission time-out. */
			s->rto = (s->sa >> 3) + s->sv;

			/* Update the send buffer space. */
			s->acked = s->ip->tcp_input_ackno - s->lastack;
			if (s->acked > 0) {
				s->snd_buf += s->acked;
				/* Send a signal for tcp_write when
				 * s->snd_queuelen is decreased. */
				lock_signal (&s->lock, 0);
			}
			/* Reset the fast retransmit variables. */
			s->dupacks = 0;
			s->lastack = s->ip->tcp_input_ackno;

			/* Update the congestion control variables (cwnd and
			 * ssthresh). */
			if (s->state >= ESTABLISHED) {
				if (s->cwnd < s->ssthresh) {
					if ((unsigned short)(s->cwnd + s->mss) > s->cwnd) {
						s->cwnd += s->mss;
					}
					tcp_debug (CONST("tcp_receive: slow start cwnd %u\n"),
						s->cwnd);
				} else {
					unsigned short new_cwnd = (s->cwnd + s->mss * s->mss / s->cwnd);
					if (new_cwnd > s->cwnd) {
						s->cwnd = new_cwnd;
					}
					tcp_debug (CONST("tcp_receive: congestion avoidance cwnd %u\n"),
						s->cwnd);
				}
			}
			tcp_debug (CONST("tcp_receive: ACK for %lu, unacked->seqno %lu:%lu\n"),
				s->ip->tcp_input_ackno, s->unacked != 0 ?
				NTOHL (s->unacked->tcphdr->seqno) : 0,
				s->unacked != 0 ?
				NTOHL (s->unacked->tcphdr->seqno) +
				TCP_TCPLEN (s->unacked) : 0);

			/* Remove segment from the unacknowledged list if
			 * the incoming ACK acknowlegdes them. */
			while (s->unacked != 0 &&
			    TCP_SEQ_LEQ (NTOHL (s->unacked->tcphdr->seqno) +
			    TCP_TCPLEN (s->unacked), s->ip->tcp_input_ackno)) {
				tcp_debug (CONST("tcp_receive: removing %lu:%lu from s->unacked\n"),
					NTOHL (s->unacked->tcphdr->seqno),
					NTOHL (s->unacked->tcphdr->seqno) +
					TCP_TCPLEN (s->unacked));

				tcp_debug (CONST("tcp_receive: queuelen %u ... "),
					(unsigned int) s->snd_queuelen);

				next = s->unacked;
				s->unacked = s->unacked->next;
				s->snd_queuelen -= buf_chain_len (next->p);
				tcp_segment_free (next);

				tcp_debug (CONST("%u (after freeing unacked)\n"),
					(unsigned int) s->snd_queuelen);
				if (s->snd_queuelen != 0) {
					assert (s->unacked != 0 ||
						s->unsent != 0);
				}
				/* Send a signal for tcp_write when
				 * s->snd_queuelen is decreased. */
				lock_signal (&s->lock, 0);
			}
		}

		/* We go through the ->unsent list to see if any
		 * of the segments on the list are acknowledged by the ACK.
		 * This may seem strange since an "unsent" segment shouldn't
		 * be acked. The rationale is that lwIP puts all outstanding
		 * segments on the ->unsent list after a retransmission,
		 * so these segments may in fact have been sent once. */
		while (s->unsent != 0 &&
		    TCP_SEQ_LEQ (NTOHL (s->unsent->tcphdr->seqno) +
		    TCP_TCPLEN (s->unsent), s->ip->tcp_input_ackno) &&
		    TCP_SEQ_LEQ (s->ip->tcp_input_ackno, s->snd_max)) {
			tcp_debug (CONST("tcp_receive: removing %lu:%lu from s->unsent, queuelen = %u\n"),
				NTOHL (s->unsent->tcphdr->seqno),
				NTOHL (s->unsent->tcphdr->seqno) +
				TCP_TCPLEN (s->unsent),  s->snd_queuelen);

			next = s->unsent;
			s->unsent = s->unsent->next;
			s->snd_queuelen -= buf_chain_len (next->p);
			tcp_segment_free (next);
			if (s->snd_queuelen != 0) {
				assert (s->unacked != 0 || s->unsent != 0);
			}
			if (s->unsent != 0) {
				s->snd_nxt = HTONL (s->unsent->tcphdr->seqno);
			}
			tcp_debug (CONST("tcp_receive: queuelen = %u, snd_nxt = %u\n"),
				s->snd_queuelen, s->snd_nxt);

			/* Send a signal for tcp_write when
			 * s->snd_queuelen is decreased. */
			lock_signal (&s->lock, 0);
		}

		/* End of ACK for new data processing. */

		tcp_debug (CONST("tcp_receive: s->rttest %u rtseq %lu ackno %lu\n"),
			s->rttest, s->rtseq, s->ip->tcp_input_ackno);

		/* RTT estimation calculations. This is done by checking
		 * if the incoming segment acknowledges the segment we use
		 * to take a round-trip time measurement. */
		if (s->rttest && TCP_SEQ_LT (s->rtseq, s->ip->tcp_input_ackno)) {
			m = s->ip->tcp_ticks - s->rttest;

			tcp_debug (CONST("tcp_receive: experienced rtt %u ticks (%u msec).\n"),
				m, m * TCP_SLOW_INTERVAL);

			/* This is taken directly from VJs original code
			 * in his paper */
			m = m - (s->sa >> 3);
			s->sa += m;
			if (m < 0) {
				m = -m;
			}
			m = m - (s->sv >> 2);
			s->sv += m;
			s->rto = (s->sa >> 3) + s->sv;

			tcp_debug (CONST("tcp_receive: RTO %u (%u miliseconds)\n"),
				s->rto, s->rto * TCP_SLOW_INTERVAL);

			s->rttest = 0;
		}
	}

	/* Segments with length 0 is taken care of here. */
	if (s->ip->tcp_input_len == 0) {
		/* Segments that fall out of the window are ACKed. */
		if (TCP_SEQ_GT (s->rcv_nxt, s->ip->tcp_input_seqno) ||
		    TCP_SEQ_GEQ (s->ip->tcp_input_seqno, s->rcv_nxt + s->rcv_wnd)) {
			tcp_ack_now (s);
		}
		return;
	}

	/* If the incoming segment contains data, we must process it further.
	 * This code basically does three things:
	 * +) If the incoming segment contains data that is the next
	 *    in-sequence data, this data is passed to the application.
	 *    This might involve trimming the first edge of the data.
	 *    The rcv_nxt variable and the advertised window are
	 *    adjusted.
	 * +) If the incoming segment has data that is above the next
	 *    sequence number expected (->rcv_nxt), the segment is
	 *    placed on the ->ooseq queue. This is done by finding
	 *    the appropriate place in the ->ooseq queue (which is
	 *    ordered by sequence number) and trim the segment in both
	 *    ends if needed. An immediate ACK is sent to indicate that
	 *    we received an out-of-sequence segment.
	 * +) Finally, we check if the first segment on the ->ooseq
	 *    queue now is in sequence (i.e., if rcv_nxt >=
	 *    ooseq->seqno). If rcv_nxt > ooseq->seqno, we must trim
	 *    the first edge of the segment on ->ooseq before we adjust
	 *    rcv_nxt. The data in the segments that are now on
	 *    sequence are chained onto the incoming segment so that
	 *    we only need to call the application once.
	 */

	/* First, we check if we must trim the first edge. We have
	 * to do this if the sequence number of the incoming segment
	 * is less than rcv_nxt, and the sequence number plus
	 * the length of the segment is larger than rcv_nxt. */
	if (TCP_SEQ_LT (s->ip->tcp_input_seqno, s->rcv_nxt)) {
		if (TCP_SEQ_LT (s->rcv_nxt,
		    s->ip->tcp_input_seqno + s->ip->tcp_input_len)) {
			/* Trimming the first edge is done by pushing
			 * the payload pointer in the buf downwards.
			 * This is somewhat tricky since we do not want
			 * to discard the full contents of the buf
			 * up to the new starting point of the data
			 * since we have to keep the TCP header which
			 * is present in the first buf in the chain.
			 *
			 * What is done is really quite a nasty hack:
			 * the first buf in the buf chain is pointed
			 * to by inseg->p. Since we need to be able
			 * to deallocate the whole buf, we cannot
			 * change this inseg->p pointer to point to any
			 * of the later bufs in the chain. Instead,
			 * we point the ->payload pointer in the first
			 * buf to data in one of the later bufs.
			 * We also set the inseg->data pointer to point
			 * to the right place. This way, the ->p
			 * pointer will still point to the first buf,
			 * but the ->p->payload pointer will point to
			 * data in another buf.
			 *
			 * After we are done with adjusting the buf
			 * pointers we must adjust the ->data pointer
			 * in the seg and the segment length.*/
			off = s->rcv_nxt - s->ip->tcp_input_seqno;
			if (inseg->p->len < off) {
				p = inseg->p;
				while (p->len < off) {
					off -= p->len;
					inseg->p->tot_len -= p->len;
					p->len = 0;
					p = p->next;
				}
				buf_add_header (p, -off);
			} else {
				buf_add_header (inseg->p, -off);
			}
			inseg->dataptr = inseg->p->payload;
			inseg->len -= s->rcv_nxt - s->ip->tcp_input_seqno;
			inseg->tcphdr->seqno = s->ip->tcp_input_seqno = s->rcv_nxt;
		} else {
			/* The whole segment is < rcv_nxt.
			 * Must be a duplicate of a packet that has
			 * already been correctly handled */
			tcp_debug (CONST("tcp_receive: duplicate seqno %lu\n"),
				s->ip->tcp_input_seqno);
			tcp_ack_now (s);
		}
	}

	/* The sequence number must be within the window (above rcv_nxt
	 * and below rcv_nxt + rcv_wnd) in order to be further
	 * processed. */
	if (TCP_SEQ_GEQ (s->ip->tcp_input_seqno, s->rcv_nxt) &&
	    TCP_SEQ_LT (s->ip->tcp_input_seqno, s->rcv_nxt + s->rcv_wnd)) {
		if (s->rcv_nxt == s->ip->tcp_input_seqno) {
			/* The incoming segment is the next in
			 * sequence. Pass the data to the application. */
			s->ip->tcp_input_len = TCP_TCPLEN (inseg);
			s->rcv_nxt += s->ip->tcp_input_len;

			/* Update the receiver's (our) window. */
			if (s->rcv_wnd < s->ip->tcp_input_len) {
				s->rcv_wnd = 0;
			} else {
				s->rcv_wnd -= s->ip->tcp_input_len;
			}

			/* If there is data in the segment, we make
			 * preparations to pass this up to the
			 * application. The ->recv_data variable is
			 * used for holding the buf that goes to the
			 * application. The code for reassembling
			 * out-of-sequence data chains its data on
			 * this buf as well.
			 *
			 * If the segment was a FIN, we set the
			 * TF_GOT_FIN flag that will be used to
			 * indicate to the application that the remote
			 * side has closed its end of the connection. */
			if (inseg->p->tot_len > 0) {
				if (tcp_queue_is_full (s)) {
					tcp_debug (CONST("tcp_receive: socket overflow\n"));
					++s->ip->tcp_in_errors;
				} else {
					tcp_queue_put (s, inseg->p);
					lock_signal (&s->lock, inseg->p);
					inseg->p = 0;
				}
			}
			if (inseg->tcphdr->flags & TCP_FIN) {
				tcp_debug (CONST("tcp_receive: received FIN."));
				if (tcp_queue_is_full (s)) {
					tcp_debug (CONST("tcp_receive: socket overflow\n"));
					++s->ip->tcp_in_errors;
				} else {
					/* Enqueue empty buf. */
					p = buf_alloc (s->ip->pool, 0, 0);
					if (p == 0) {
						tcp_debug (CONST("tcp_receive: could not allocate empty buf\n"));
						++s->ip->tcp_in_errors;
					} else {
						tcp_queue_put (s, p);
						lock_signal (&s->lock, p);
					}
				}
			}

			/* Acknowledge the segment(s). */
			tcp_ack (s);
		} else {
			/* We get here if the incoming segment is out-of-sequence. */
			tcp_ack_now (s);
		}
	}
}

/*
 * Parses the options contained in the incoming segment.
 * (Code taken from uIP with only small changes.)
 */
void
tcp_parseopt (tcp_socket_t *s, tcp_hdr_t *h)
{
	unsigned char c;
	unsigned char *opts, opt;
	unsigned short mss;

	opts = (unsigned char*) h + TCP_HLEN;

	/* Parse the TCP MSS option, if present. */
	if ((h->offset & 0xf0) > 0x50) {
		for (c = 0; c < ((h->offset >> 4) - 5) << 2 ;) {
			opt = opts[c];
			if (opt == 0x00) {
				/* End of options. */
				break;

			} else if (opt == 0x01) {
				++c;
				/* NOP option. */

			} else if (opt == 0x02 && opts[c + 1] == 0x04) {
				/* An MSS option with the right option
				 * length. */
				mss = (opts[c + 2] << 8) | opts[c + 3];
				s->mss = (mss > TCP_MSS) ? TCP_MSS : mss;

				/* And we are done processing options. */
				break;

			} else {
				if (opts[c + 1] == 0) {
					/* If the length field is zero,
					 * the options are malformed and
					 * we don't process them further. */
					break;
				}
				/* All other options have a length field,
				 * so that we easily can skip past them. */
				c += opts[c + 1];
			}
		}
	}
}

/*
 * Implements the TCP state machine. Called by tcp_input. In some
 * states tcp_receive() is called to receive data. The tcp_segment
 * argument will be freed by the caller (tcp_input()) unless the
 * recv_data pointer in the s is set.
 */
static int
tcp_process (tcp_socket_t *s, tcp_segment_t *inseg, tcp_hdr_t *h)
{
	tcp_segment_t *rseg;
	unsigned char acceptable = 0;
	int err;

	err = 0;

	/* Process incoming RST segments. */
	if (s->ip->tcp_input_flags & TCP_RST) {
		/* First, determine if the reset is acceptable. */
		if (s->state == SYN_SENT) {
			if (s->ip->tcp_input_ackno == s->snd_nxt) {
				acceptable = 1;
			}
		} else {
			if (TCP_SEQ_GEQ (s->ip->tcp_input_seqno, s->rcv_nxt) &&
			    TCP_SEQ_LEQ (s->ip->tcp_input_seqno, s->rcv_nxt + s->rcv_wnd)) {
				acceptable = 1;
			}
		}

		if (! acceptable) {
			tcp_debug (CONST("tcp_process: unacceptable reset seqno %lu rcv_nxt %lu\n"),
				s->ip->tcp_input_seqno, s->rcv_nxt);
			tcp_debug (CONST("tcp_process: unacceptable reset seqno %lu rcv_nxt %lu\n"),
				s->ip->tcp_input_seqno, s->rcv_nxt);
			return 1;
		}
		tcp_debug (CONST("tcp_process: Connection RESET\n"));
		assert (s->state != CLOSED);

		/* Connection was reset by the other end. Notify a user. */
		tcp_socket_remove (&s->ip->tcp_sockets, s);
		s->flags &= ~TF_ACK_DELAY;
		return 0;
	}

	/* Update the PCB (in)activity timer. */
	s->tmr = s->ip->tcp_ticks;

	/* Do different things depending on the TCP state. */
	switch (s->state) {
	case SYN_SENT:
		tcp_debug (CONST("SYN-SENT: ackno %lu s->snd_nxt %lu unacked %lu\n"),
			s->ip->tcp_input_ackno, s->snd_nxt,
			NTOHL (s->unacked->tcphdr->seqno));
		if (s->ip->tcp_input_flags & (TCP_ACK | TCP_SYN) &&
		    s->ip->tcp_input_ackno == NTOHL (s->unacked->tcphdr->seqno) + 1) {
			s->rcv_nxt = s->ip->tcp_input_seqno + 1;
			s->lastack = s->ip->tcp_input_ackno;
			s->snd_wnd = s->snd_wl1 = h->wnd;
			s->cwnd = s->mss;
			--s->snd_queuelen;
			tcp_debug (CONST("tcp_process: queuelen = %u\n"),
				s->snd_queuelen);
			rseg = s->unacked;
			s->unacked = rseg->next;
			tcp_segment_free (rseg);

			/* Parse any options in the SYNACK. */
			tcp_parseopt (s, h);

			/* Notify a user that we are successfully connected. */
			tcp_set_socket_state (s, ESTABLISHED);

			tcp_ack (s);

			/* Send a signal for tcp_write when
			 * s->snd_queuelen is decreased. */
			lock_signal (&s->lock, 0);
		}
		break;

	case SYN_RCVD:
		if ((s->ip->tcp_input_flags & TCP_ACK) &&
		    ! (s->ip->tcp_input_flags & TCP_RST)) {
			if (TCP_SEQ_LT (s->lastack, s->ip->tcp_input_ackno) &&
			    TCP_SEQ_LEQ (s->ip->tcp_input_ackno, s->snd_nxt)) {
				tcp_debug (CONST("TCP connection established %u -> %u.\n"),
					inseg->tcphdr->src, inseg->tcphdr->dest);
				/* Notify user. */
				tcp_set_socket_state (s, ESTABLISHED);

				/* If there was any data contained within
				 * this ACK, we'd better pass it on to
				 * the application as well. */
				tcp_receive (s, inseg, h);
				s->cwnd = s->mss;
			}
		}
		break;

	case CLOSE_WAIT:
		/* FALLTHROUGH */
	case ESTABLISHED:
		tcp_receive (s, inseg, h);
		if (s->ip->tcp_input_flags & TCP_FIN) {
			tcp_ack_now (s);
			tcp_set_socket_state (s, CLOSE_WAIT);
		}
		break;

	case FIN_WAIT_1:
		tcp_receive (s, inseg, h);
		if (s->ip->tcp_input_flags & TCP_FIN) {
			if ((s->ip->tcp_input_flags & TCP_ACK) &&
			    s->ip->tcp_input_ackno == s->snd_nxt) {
				tcp_debug (CONST("TCP connection closed %d -> %d.\n"),
					inseg->tcphdr->src, inseg->tcphdr->dest);
				tcp_ack_now (s);
				tcp_list_remove (&s->ip->tcp_sockets, s);
				tcp_set_socket_state (s, TIME_WAIT);
				tcp_list_add (&s->ip->tcp_closing_sockets, s);
			} else {
				tcp_ack_now (s);
				tcp_set_socket_state (s, CLOSING);
			}
		} else if ((s->ip->tcp_input_flags & TCP_ACK) &&
		    s->ip->tcp_input_ackno == s->snd_nxt && ! s->unsent) {
			tcp_set_socket_state (s, FIN_WAIT_2);
		}
		break;

	case FIN_WAIT_2:
		tcp_receive (s, inseg, h);
		if (s->ip->tcp_input_flags & TCP_FIN) {
			tcp_debug (CONST("TCP connection closed %u -> %u.\n"),
				inseg->tcphdr->src, inseg->tcphdr->dest);
			tcp_ack_now (s);
			tcp_list_remove (&s->ip->tcp_sockets, s);
			tcp_set_socket_state (s, TIME_WAIT);
			tcp_list_add (&s->ip->tcp_closing_sockets, s);
		}
		break;

	case CLOSING:
		tcp_receive (s, inseg, h);
		if ((s->ip->tcp_input_flags & TCP_ACK) &&
		    s->ip->tcp_input_ackno == s->snd_nxt) {
			tcp_debug (CONST("TCP connection closed %u -> %u.\n"),
				inseg->tcphdr->src, inseg->tcphdr->dest);
			tcp_ack_now (s);
			tcp_list_remove (&s->ip->tcp_sockets, s);
			tcp_set_socket_state (s, TIME_WAIT);
			tcp_list_add (&s->ip->tcp_closing_sockets, s);
		}
		break;

	case LAST_ACK:
		tcp_receive (s, inseg, h);
		if ((s->ip->tcp_input_flags & TCP_ACK) &&
		    s->ip->tcp_input_ackno == s->snd_nxt) {
			/* The connection has been closed. Notify a user. */
			tcp_debug (CONST("TCP connection closed %u -> %u.\n"),
				inseg->tcphdr->src, inseg->tcphdr->dest);
			tcp_socket_remove (&s->ip->tcp_sockets, s);
		}
		break;

	default:
		break;
	}
	return 1;
}

static tcp_socket_t *
find_active_socket (ip_t *ip, tcp_hdr_t *h, ip_hdr_t *iph)
{
	tcp_socket_t *s, *prev;

	prev = 0;
	for (s=ip->tcp_sockets; s; prev=s, s=s->next) {
		assert (s->state != CLOSED);
		assert (s->state != TIME_WAIT);
		assert (s->state != LISTEN);

		if (s->local_port != h->dest || s->remote_port != h->src ||
		    memcmp (s->remote_ip, iph->src, 4) != 0 ||
		    memcmp (s->local_ip, iph->dest, 4) != 0)
			continue;

		/* Move this PCB to the front of the list so that
		 * subsequent lookups will be faster (we exploit
		 * locality in TCP segment arrivals). */
		assert (s->next != s);
		if (prev != 0) {
			prev->next = s->next;
			s->next = ip->tcp_sockets;
			ip->tcp_sockets = s;
		}
		assert (s->next != s);
		return s;
	}
	return 0;
}

static tcp_socket_t *
find_closing_socket (ip_t *ip, tcp_hdr_t *h, ip_hdr_t *iph)
{
	tcp_socket_t *s;

	for (s=ip->tcp_closing_sockets; s; s=s->next) {
		assert (s->state == TIME_WAIT);
		if (s->local_port != h->dest || s->remote_port != h->src ||
		    memcmp (s->remote_ip, iph->src, 4) != 0 ||
		    memcmp (s->local_ip, iph->dest, 4) != 0)
			continue;

		/* We don't really care enough to move this PCB
		 * to the front of the list since we are not
		 * very likely to receive that many segments
		 * for connections in TIME-WAIT. */
		return s;
	}
	return 0;
}

static tcp_socket_t *
find_listen_socket (ip_t *ip, tcp_hdr_t *h, ip_hdr_t *iph)
{
	tcp_socket_t *ls, *prev;

	prev = 0;
	for (ls=ip->tcp_listen_sockets; ls; prev=ls, ls=ls->next) {
		if (ls->local_port != h->dest)
			continue;
		if (memcmp (ls->local_ip, IP_ADDR(0), 4) != 0 &&
		    memcmp (ls->local_ip, iph->dest, 4) != 0)
			continue;

		/* Move this PCB to the front of the list so
		 * that subsequent lookups will be faster
		 * (we exploit locality in TCP segment
		 * arrivals). */
		if (prev != 0) {
			prev->next = ls->next;
			/* our successor is the remainder of the listening list */
			ls->next = ip->tcp_listen_sockets;
			/* put this listening s at the head of the listening list */
			ip->tcp_listen_sockets = ls;
		}
		return ls;
	}
	return 0;
}

/*
 * The initial input processing of TCP. It verifies the TCP header,
 * demultiplexes the segment between the PCBs and passes it on
 * to tcp_process(), which implements the TCP finite state machine.
 * This function is called by the IP layer (in ip_input()).
 */
void __attribute__((weak))
tcp_input (ip_t *ip, buf_t *p, netif_t *netif, ip_hdr_t *iph)
{
	tcp_hdr_t *h;
	tcp_socket_t *s;
	tcp_segment_t inseg;

	++ip->tcp_in_datagrams;

	if (p->tot_len < sizeof(tcp_hdr_t)) {
		/* Bad TCP packet received. */
		tcp_debug (CONST("tcp_input: too short packet (hlen=%d bytes)\n"), p->tot_len);
		++ip->tcp_in_errors;
drop:		buf_free (p);
		return;
	}
	tcp_debug (CONST("tcp_input: driver %s received %d bytes\n"),
		netif->name, p->tot_len);
/*	buf_print_tcp (p);*/

	/* Don't even process incoming broadcasts/multicasts. */
	if (IS_BROADCAST (iph->dest) || IS_MULTICAST (iph->dest)) {
		/* TODO: increment statistics counters */
		goto drop;
	}
	h = (tcp_hdr_t*) p->payload;

	/* Verify TCP checksum. */
	if (buf_chksum (p, crc16_inet_header (iph->src,
	    iph->dest, IP_PROTO_TCP, p->tot_len)) != 0) {
		tcp_debug (CONST("tcp_input: bad checksum\n"));
		tcp_debug_print_header (h);
		++ip->tcp_in_errors;
		goto drop;
	}

	/* Move the payload pointer in the buf so that it points to the
	   TCP data instead of the TCP header. */
	buf_add_header (p, - ((h->offset >> 2) & 0x3c));

	/* Convert fields in TCP header to host byte order. */
	h->src = NTOHS (h->src);
	h->dest = NTOHS (h->dest);
	ip->tcp_input_seqno = h->seqno = NTOHL (h->seqno);
	ip->tcp_input_ackno = h->ackno = NTOHL (h->ackno);
	h->wnd = NTOHS (h->wnd);

	ip->tcp_input_flags = h->flags & TCP_FLAGS;
	ip->tcp_input_len = p->tot_len;
	if (ip->tcp_input_flags & (TCP_FIN | TCP_SYN))
		++ip->tcp_input_len;

	/* Demultiplex an incoming segment. First, we check if it is destined
	 * for an active connection. */
	s = find_active_socket (ip, h, iph);
	if (! s) {
		/* If it did not go to an active connection, we check
		 * the connections in the TIME-WAIT state. */
		s = find_closing_socket (ip, h, iph);
		if (s) {
			tcp_debug (CONST("tcp_input: packet for TIME_WAITing connection.\n"));
			if (TCP_SEQ_GT (ip->tcp_input_seqno +
			    ip->tcp_input_len, s->rcv_nxt)) {
				s->rcv_nxt = ip->tcp_input_seqno +
					ip->tcp_input_len;
			}
			if (ip->tcp_input_len > 0) {
				tcp_ack_now (s);
			}
			tcp_output (s);
			goto drop;
		}

		/* Finally, if we still did not get a match, we check
		 * all PCBs that are LISTENing for incoming connections. */
		s = find_listen_socket (ip, h, iph);
		if (s) {
			/* In the LISTEN state, we check for incoming
			 * SYN segments. */
			if (ip->tcp_input_flags & TCP_ACK) {
				/* For incoming segments with the ACK flag set,
				 * respond with a RST. */
				tcp_debug (CONST("tcp_input: ACK in LISTEN, sending reset\n"));
				tcp_rst (ip, ip->tcp_input_ackno + 1,
					ip->tcp_input_seqno + ip->tcp_input_len,
					iph->dest, iph->src, h->dest, h->src);
				goto drop;
			}
			if (! (ip->tcp_input_flags & TCP_SYN)) {
				tcp_debug (CONST("tcp_input: no SYN in incoming connection\n"));
				goto drop;
			}
			tcp_debug (CONST("tcp_input: connection request from port %u to port %u\n"),
				h->src, h->dest);
			if (tcp_queue_is_full (s)) {
				tcp_debug (CONST("tcp_input: socket overflow\n"));
				++ip->tcp_in_errors;
				goto drop;
			}
			p->payload = (unsigned char*) h;
			tcp_queue_put (s, p);
			lock_signal (&s->lock, p);
			return;
		}

		/* If no matching PCB was found, send a TCP RST (reset) to
		 * the sender. */
		tcp_debug (CONST("tcp_input: no PCB match found, resetting.\n"));
		if (! (h->flags & TCP_RST)) {
			/* TODO: increment statistics counters */
			tcp_rst (ip, ip->tcp_input_ackno,
				ip->tcp_input_seqno + ip->tcp_input_len,
				iph->dest, iph->src, h->dest, h->src);
		}
		goto drop;
	}
	lock_take (&s->lock);
	tcp_debug (CONST("tcp_input: flags ="));
	tcp_debug_print_flags (h->flags);
	tcp_debug (CONST(", state=%S\n"), tcp_state_name (s->state));

	/* The incoming segment belongs to a connection. */
	/* Set up a tcp_segment structure. */
	inseg.next = 0;
	inseg.len = p->tot_len;
	inseg.dataptr = p->payload;
	inseg.p = p;
	inseg.tcphdr = h;

	ip->tcp_input_socket = s;
	tcp_process (s, &inseg, h);
	ip->tcp_input_socket = 0;

	/* We deallocate the incoming buf, if it was not buffered by
	 * the application. */
	if (inseg.p != 0)
		buf_free (inseg.p);

	if (s->unsent != 0 || (s->flags & TF_ACK_NOW))
		tcp_output (s);

	tcp_debug (CONST("tcp_input: done, state=%S\n\n"),
		tcp_state_name (s->state));
	lock_release (&s->lock);

	assert (tcp_debug_verify (ip));
}
