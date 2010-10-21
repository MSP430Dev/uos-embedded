#ifndef __TCP_H_
#define __TCP_H_ 1

/*
 * TCP options
 */
#ifndef TCP_WND
#define TCP_WND                         2048
#endif

#ifndef TCP_MAXRTX
#define TCP_MAXRTX                      12
#endif

#ifndef TCP_SYNMAXRTX
#define TCP_SYNMAXRTX                   6
#endif

/* TCP Maximum segment size. */
#ifndef TCP_MSS
#define TCP_MSS				256	/* conservative default */
#endif

/* TCP sender buffer space (bytes). */
#ifndef TCP_SND_BUF
#define TCP_SND_BUF                     1024
#endif

/* TCP sender buffer space (pbufs).
 * This must be at least = 2 * TCP_SND_BUF/TCP_MSS for things to work. */
#ifndef TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN                4 * TCP_SND_BUF/TCP_MSS
#endif


/* Maximum number of retransmissions of data segments. */

/* Maximum number of retransmissions of SYN segments. */

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#ifndef TCP_SNDLOWAT
#define TCP_SNDLOWAT                    TCP_SND_BUF/2
#endif

typedef enum _tcp_state_t {
	CLOSED		= 0,
	LISTEN		= 1,
	SYN_SENT	= 2,
	SYN_RCVD	= 3,
	ESTABLISHED	= 4,
	FIN_WAIT_1	= 5,
	FIN_WAIT_2	= 6,
	CLOSE_WAIT	= 7,
	CLOSING		= 8,
	LAST_ACK	= 9,
	TIME_WAIT	= 10,
} tcp_state_t;

#ifndef TCP_TMR_INTERVAL
#define TCP_TMR_INTERVAL	100	/* TCP timer interval in msec. */
#endif

#ifndef TCP_FAST_INTERVAL
#define TCP_FAST_INTERVAL	200	/* fine grained timeout in msec */
#endif

#ifndef TCP_SLOW_INTERVAL
#define TCP_SLOW_INTERVAL	500	/* coarse grained timeout in msec */
#endif

#define TCP_STUCK_TIMEOUT	5000	/* milliseconds */

#define TCP_MSL			60000  /* maximum segment lifetime in usec */

#ifndef TCP_LOCAL_PORT_RANGE_START
#define TCP_LOCAL_PORT_RANGE_START 4096
#define TCP_LOCAL_PORT_RANGE_END   0x7fff
#endif

/*
 * TCP header.
 */
struct _tcp_hdr {
	unsigned short src;
	unsigned short dest;
	unsigned long seqno;
	unsigned long ackno;
#define TCP_SEQ_LT(a,b)		((long)((a)-(b)) < 0)
#define TCP_SEQ_LEQ(a,b)	((long)((a)-(b)) <= 0)
#define TCP_SEQ_GT(a,b)		((long)((a)-(b)) > 0)
#define TCP_SEQ_GEQ(a,b)	((long)((a)-(b)) >= 0)

	unsigned char offset;
	unsigned char flags;
#define TCP_FIN			0x01
#define TCP_SYN			0x02
#define TCP_RST			0x04
#define TCP_PSH			0x08
#define TCP_ACK			0x10
#define TCP_URG			0x20

#define TCP_FLAGS		0x3f

	unsigned short wnd;

	unsigned short chksum;
	unsigned short urgp;
} __attribute__ ((packed));

typedef struct _tcp_hdr tcp_hdr_t;

/* Length of the TCP header, excluding options. */
#define TCP_HLEN		20

/*
 * This structure is used to repressent TCP segments when queued.
 */
struct _tcp_segment_t {
	struct _tcp_segment_t *next; /* used when putting segements on a queue */
	struct _buf_t *p;	/* buffer containing data + TCP header */
	void *dataptr;		/* pointer to the TCP data in the buf_t */
	tcp_hdr_t *tcphdr;	/* the TCP header */
	unsigned short len;	/* the TCP length of this segment */
};
typedef struct _tcp_segment_t tcp_segment_t;

#define TCP_TCPLEN(seg)	((seg)->len + (((seg)->tcphdr->flags & \
			(TCP_FIN | TCP_SYN)) ? 1 : 0))

/*
 * The TCP protocol control block
 */
struct _tcp_socket_t {
	mutex_t lock;
	struct _ip_t *ip;
	struct _tcp_socket_t *next;	/* for the linked list */

	unsigned char local_ip [4];
	unsigned short local_port;

	tcp_state_t state;		/* TCP state */

	/* queue of received packets */
#define SOCKET_QUEUE_SIZE	8
	struct _buf_t *queue [SOCKET_QUEUE_SIZE];
	struct _buf_t **head;
	unsigned char count;

	/*
	 * Only above data are valid for sockets in LISTEN state.
	 * All the following data are for ordinary sockets only.
	 */
	unsigned char remote_ip [4];
	unsigned short remote_port;

	/* receiver varables */
	unsigned long rcv_nxt;		/* next seqno expected */
	unsigned short rcv_wnd;		/* receiver window */

	/* Timers */
	unsigned long tmr;

	/* Retransmission timer. */
	unsigned short rtime;

	unsigned short mss;		/* maximum segment size */

	unsigned char flags;
#define TF_ACK_DELAY	0x01		/* Delayed ACK. */
#define TF_ACK_NOW	0x02		/* Immediate ACK. */
#define TF_INFR		0x04		/* In fast recovery. */
#define TF_RESET	0x08		/* Connection was reset. */
#define TF_CLOSED	0x10		/* Connection was sucessfully closed. */
#define TF_GOT_FIN	0x20		/* Connection closed by remote end. */

	/* RTT estimation variables. */
	unsigned short rttest;		/* RTT estimate in 500ms ticks */
	unsigned long rtseq;		/* sequence number being timed */
	short sa, sv;

	unsigned short rto;		/* retransmission time-out */
	unsigned char nrtx;		/* number of retransmissions */

	/* fast retransmit/recovery */
	unsigned long lastack;		/* Highest acknowledged seqno. */
	unsigned char dupacks;

	/* congestion avoidance/control variables */
	unsigned short cwnd;
	unsigned short ssthresh;

	/* sender variables */
	unsigned long snd_nxt,		/* next seqno to be sent */
		snd_max,		/* Highest seqno sent. */
		snd_wnd,		/* sender window */
		snd_wl1, snd_wl2,	/* Sequence and acknowledgement numbers
					 * of last window update. */
		snd_lbb;		/* Sequence number of next byte
					 * to be buffered. */
	unsigned short acked;

	unsigned short snd_buf;		/* Available bytes for sending. */
	unsigned char snd_queuelen;	/* Available tcp_segment's for sending. */

	/* These are ordered by sequence number: */
	tcp_segment_t *unsent;		/* Unsent (queued) segments. */
	tcp_segment_t *unacked;		/* Sent but unacknowledged segments. */
};
typedef struct _tcp_socket_t tcp_socket_t;

/*
 * Application program's interface:
 */
tcp_socket_t *tcp_connect (ip_t *ip, unsigned char *ipaddr,
	unsigned short port);
tcp_socket_t *tcp_listen (ip_t *ip, unsigned char *ipaddr,
	unsigned short port);
tcp_socket_t *tcp_accept (tcp_socket_t *s);
int tcp_close (tcp_socket_t *s);
void tcp_abort (tcp_socket_t *s);
int tcp_read (tcp_socket_t *s, void *dataptr, unsigned short len);
int tcp_read_poll (tcp_socket_t *s, void *dataptr, unsigned short len, int nonblock);
int tcp_write (tcp_socket_t *s, const void *dataptr, unsigned short len);
unsigned long tcp_inactivity (tcp_socket_t *s);

#ifdef to_stream
/*
 * Stream interface adapter for TCP socket.
 * To use it, you must include stream/stream.h before net/tcp.h.
 */
typedef struct _tcp_stream_t {
	stream_t stream;
	tcp_socket_t *socket;

	unsigned char *inptr;
	struct _buf_t *inbuf;

	unsigned char *outptr;
	unsigned char outdata [1000];
} tcp_stream_t;

stream_t *tcp_stream_init (tcp_stream_t *u, tcp_socket_t *sock);
#endif /* to_stream */

/*
 * Lower layer interface to TCP:
 */
void tcp_slowtmr (ip_t *ip) __attribute__((weak));
void tcp_fasttmr (ip_t *ip) __attribute__((weak));
void tcp_input (struct _ip_t *ip, struct _buf_t *p, struct _netif_t *inp,
	struct _ip_hdr_t *iph) __attribute__((weak));

/* Used within the TCP code only: */
tcp_socket_t *tcp_alloc (ip_t *ip);
int tcp_output (tcp_socket_t *s);
void tcp_rexmit (tcp_socket_t *s);
void tcp_parseopt (tcp_socket_t *s, tcp_hdr_t *h);
struct _buf_t *tcp_queue_get (tcp_socket_t *q);
void tcp_queue_put (tcp_socket_t *q, struct _buf_t *p);
void tcp_queue_free (tcp_socket_t *q);

static inline int
tcp_queue_is_full (tcp_socket_t *q)
{
	/*debug_printf ("tcp_queue_is_full: returned %d\n", (q->count == SOCKET_QUEUE_SIZE));*/
	return (q->count == SOCKET_QUEUE_SIZE);
}

static inline int
tcp_queue_is_empty (tcp_socket_t *q)
{
	/*debug_printf ("tcp_queue_is_full: returned %d\n", (q->count == SOCKET_QUEUE_SIZE));*/
	return (q->count == 0);
}

/*
 * Internal functions and global variables:
 */
tcp_socket_t *tcp_socket_copy (tcp_socket_t *s);
void tcp_socket_purge (tcp_socket_t *s);
void tcp_socket_remove (tcp_socket_t **socklist, tcp_socket_t *s);
void tcp_set_socket_state (tcp_socket_t *s, tcp_state_t newstate);

unsigned char tcp_segments_free (tcp_segment_t *seg);
unsigned char tcp_segment_free (tcp_segment_t *seg);
tcp_segment_t *tcp_segment_copy (tcp_segment_t *seg);

static inline void
tcp_ack_now (tcp_socket_t *s)
{
	s->flags |= TF_ACK_NOW;
	tcp_output (s);
}

static inline void
tcp_ack (tcp_socket_t *s)
{
	if (s->flags & TF_ACK_DELAY) {
		s->flags &= ~TF_ACK_DELAY;
		tcp_ack_now (s);
	} else {
		s->flags |= TF_ACK_DELAY;
	}
}

/*
 * Calculates an initial sequence number for new connections.
 */
static inline unsigned long
tcp_next_seqno (ip_t *ip)
{
	ip->tcp_seqno += ip->tcp_ticks + 1;
	return ip->tcp_seqno;
}

int tcp_enqueue (tcp_socket_t *s, void *dataptr, unsigned short len,
	unsigned char flags, unsigned char *optdata, unsigned char optlen);

void tcp_rexmit_seg (tcp_socket_t *s, tcp_segment_t *seg);

void tcp_rst (ip_t *ip, unsigned long seqno, unsigned long ackno,
	unsigned char *local_ip, unsigned char *remote_ip,
	unsigned short local_port, unsigned short remote_port);

unsigned long tcp_next_seqno (ip_t *ip);

void tcp_debug_print_header (tcp_hdr_t *tcphdr);
void tcp_debug_print_flags (unsigned char flags);
const char *tcp_state_name (tcp_state_t s);
void tcp_debug_print_sockets (ip_t *ip);
int tcp_debug_verify (ip_t *ip);

#ifdef TCP_DEBUG
#define tcp_debug			debug_printf
#else
#define tcp_debug(...)			/* void */
#define tcp_debug_print_header(tcphdr)	/* void */
#define tcp_debug_print_flags(flags)	/* void */
#define tcp_debug_print_sockets(ip)	/* void */
#define tcp_debug_verify(ip)		1
#endif /* TCP_DEBUG */

/*
 * The TCP socket lists are defined in ip_t:
 * tcp_listen_sockets	-- List of all TCP sockets in LISTEN state.
 * tcp_sockets		-- List of all TCP sockets that are in a state
 *			   in which they accept or send data.
 * tcp_closing_sockets	-- List of all TCP sockets in TIME-WAIT state.
 *
 * Axioms about TCP socket lists:
 * 1) Every TCP socket that is not CLOSED is in one of the lists.
 * 2) A socket is only in one of the lists.
 * 3) All sockets in the tcp_listen_sockets list is in LISTEN state.
 * 4) All sockets in the tcp_closing_sockets list is in TIME-WAIT state.
 *
 * Define two macros, that register a TCP socket
 * with a socket list or remove a socket from a list.
 */
static inline void
tcp_list_add (tcp_socket_t **socklist, tcp_socket_t *ns)
{
	ns->next = *socklist;
	*socklist = ns;
}

static inline void
tcp_list_remove (tcp_socket_t **socklist, tcp_socket_t *ns)
{
	if (*socklist == ns) {
		*socklist = ns->next;
	} else {
		tcp_socket_t *p;

		for (p = *socklist; p; p = p->next) {
			if (p->next && p->next == ns) {
				p->next = ns->next;
				break;
			}
		}
	}
	ns->next = 0;
}

#endif /* __TCP_H_ */
