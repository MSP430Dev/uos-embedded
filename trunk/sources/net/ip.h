#ifndef __IP_H_
#define __IP_H_ 1

#ifndef IP_STACKSZ
#   if __AVR__
#      define IP_STACKSZ	0x400
#   endif
#   if I386
#      define IP_STACKSZ	0x800
#   endif
#   if MIPS32
#      define IP_STACKSZ	0x800
#   endif
#   if LINUX386
#      define IP_STACKSZ	4000
#   endif
#   if defined (__arm__) || defined (__thumb__)
#      define IP_STACKSZ	0x600
#   endif
#endif

typedef struct _ip_t {
	mutex_t		lock;
	mutex_group_t	*netif_group;	/* list of network drivers */
	struct _mem_pool_t *pool;	/* pool for memory allocation */
	struct _timer_t *timer;		/* timer driver */
	struct _route_t *route;		/* routing table */
	struct _arp_t	*arp;		/* ARP protocol data */
	unsigned char	forwarding;	/* forwarding enabled */
	unsigned char	default_ttl;	/* default time-to-live value */
	unsigned char	tos;		/* type of service value */
	unsigned short	id;		/* output packet number */

	/*
	 * UDP
	 */
	struct _udp_socket_t *udp_sockets;	/* list of UDP sockets */

	/*
	 * TCP
	 */
	struct _tcp_socket_t *tcp_listen_sockets;	/* LISTEN state */
	struct _tcp_socket_t *tcp_sockets;		/* active sockets */
	struct _tcp_socket_t *tcp_closing_sockets;	/* TIME-WAIT state */

	/* Incremented every coarse grained timer shot
	 * (typically every 500 ms, determined by TCP_COARSE_TIMEOUT). */
	unsigned long	tcp_ticks;

	unsigned char	tcp_timer;
	unsigned short	tcp_port;	/* local port number to allocate */
	unsigned long	tcp_seqno;	/* initial sequence number for
					 * new TCP connections */

	/* These variables are global to all functions involved in the input
	 * processing of TCP segments. Set by the tcp_input() function. */
	struct _tcp_socket_t *tcp_input_socket;
	unsigned short	tcp_input_len;
	unsigned char	tcp_input_flags;
	unsigned long	tcp_input_seqno;
	unsigned long	tcp_input_ackno;

	/*
	 * IP statistics.
	 */
	unsigned long	in_receives;	/* total input packets */
	unsigned long	in_hdr_errors;	/* input errors: checksum, version,
					   length, IP options, fragmentation */
	unsigned long	in_addr_errors;	/* received packet was not for us */
	unsigned long	in_discards;	/* ignored input packets, due to
					   lack of memory */
	unsigned long	in_unknown_protos; /* input packets with unsupported
					   transport protocols */
	unsigned long	in_delivers;	/* successfully delivered packets */
	unsigned long	out_requests;	/* output requests */
	unsigned long	out_discards;	/* lost output packets due
					   to not enough memory */
	unsigned long	out_no_routes;	/* lost output packets due to
					   no route to host */
	unsigned long	forw_datagrams;	/* forwarded packets */

	/*
	 * ICMP statistics.
	 */
        unsigned long   icmp_in_msgs;		/* total input packets */
        unsigned long   icmp_in_errors;		/* invalid input packets */
        unsigned long   icmp_in_dest_unreachs;	/*  */
        unsigned long   icmp_in_time_excds;	/*  */
        unsigned long   icmp_in_parm_probs;	/*  */
        unsigned long   icmp_in_src_quenchs;	/*  */
        unsigned long   icmp_in_redirects;	/*  */
        unsigned long   icmp_in_echos;		/* input echos */
        unsigned long   icmp_in_echo_reps;	/*  */
        unsigned long   icmp_in_timestamps;	/*  */
        unsigned long   icmp_in_timestamp_reps;	/*  */
        unsigned long   icmp_in_addr_masks;	/*  */
        unsigned long   icmp_in_addr_mask_reps;	/*  */
        unsigned long   icmp_out_msgs;		/* total output packets */
        unsigned long   icmp_out_errors;	/* output errors */
        unsigned long   icmp_out_dest_unreachs;	/* output destination unreachables */
        unsigned long   icmp_out_time_excds;	/* output time exceeds */
        unsigned long   icmp_out_parm_probs;	/*  */
        unsigned long   icmp_out_src_quenchs;	/*  */
        unsigned long   icmp_out_redirects;	/*  */
        unsigned long   icmp_out_echos;		/*  */
        unsigned long   icmp_out_echo_reps;	/* output echo replies */
        unsigned long   icmp_out_timestamps;	/*  */
        unsigned long   icmp_out_timestamp_reps; /*  */
        unsigned long   icmp_out_addr_masks;	/*  */
        unsigned long   icmp_out_addr_mask_reps; /*  */

	/*
	 * UDP statistics.
	 */
	unsigned long   udp_out_datagrams; /* total output packets */
	unsigned long	udp_in_datagrams; /* total input packets */
	unsigned long   udp_in_errors;	/* input packets with invalid length,
					   checksum errors or socket overflow */
	unsigned long   udp_no_ports;	/* no listener on port */

	/*
	 * TCP statistics.
	 */
	unsigned long   tcp_out_datagrams; /* total output packets */
	unsigned long   tcp_out_errors;	/* output errors */

	unsigned long	tcp_in_datagrams; /* total input packets */
	unsigned long   tcp_in_errors;	/* input packets with invalid length,
					   checksum errors or socket overflow */
	unsigned long	tcp_in_discards; /* ignored input packets, due to
					   lack of memory */

	ARRAY (stack, IP_STACKSZ);	/* task stack */
} ip_t;

typedef struct _ip_hdr_t {
	unsigned char	version;	/* version / header length */
	unsigned char	tos;		/* type of service */
	unsigned char	len_h, len_l;	/* total length */
	unsigned char	id_h, id_l;	/* identification */

	unsigned char	offset_h, offset_l; /* fragment offset field */
#define IP_DF		0x40		/* dont fragment flag */
#define IP_MF		0x20		/* more fragments flag */
#define IP_OFFMASK	0x1f		/* mask for fragmenting bits */

	unsigned char	ttl;		/* time to live */

	unsigned char	proto;		/* protocol */
#define IP_PROTO_ICMP	1
#define IP_PROTO_TCP	6
#define IP_PROTO_UDP	17

	unsigned char	chksum_h, chksum_l; /* checksum */

	unsigned char	src [4];	/* source destination IP address */
	unsigned char	dest [4];	/* destination IP address */
} ip_hdr_t;

#define IP_HLEN		20		/* IP header length */
#define IP_MAXPACKET	1500		/* max packet size after reassemble */

#define IP_ADDR(val)	({unsigned long addr = val; (unsigned char*) &addr; })

#define IS_BROADCAST(ipaddr)	(memcmp (ipaddr, IP_ADDR(0xffffffff), 4) == 0 ||\
				 memcmp (ipaddr, IP_ADDR(0), 4) == 0)
#define IS_MULTICAST(ipaddr)	((ipaddr[0] & 0xf0) == 0xe0)

#define ICMP_ER		0	/* echo reply */
#define ICMP_DUR	3	/* destination unreachable */
#define ICMP_SQ		4	/* source quench */
#define ICMP_RD		5	/* redirect */
#define ICMP_ECHO	8	/* echo */
#define ICMP_TE		11	/* time exceeded */
#define ICMP_PP		12	/* parameter problem */
#define ICMP_TS		13	/* timestamp */
#define ICMP_TSR	14	/* timestamp reply */
#define ICMP_IRQ	15	/* information request */
#define ICMP_IR		16	/* information reply */

#define	ICMP_DUR_NET	0	/* net unreachable */
#define	ICMP_DUR_HOST	1	/* host unreachable */
#define	ICMP_DUR_PROTO	2	/* protocol unreachable */
#define	ICMP_DUR_PORT	3	/* port unreachable */
#define	ICMP_DUR_FRAG	4	/* fragmentation needed and DF set */
#define	ICMP_DUR_SR	5	/* source route failed */

#define	ICMP_TE_TTL	0	/* time to live exceeded in transit */
#define ICMP_TE_FRAG	1	/* fragment reassembly time exceeded */

typedef struct _icmp_hdr_t {
	ip_hdr_t	ip;
	unsigned char	type;
	unsigned char	code;
	unsigned short	chksum;
	unsigned short	id;
	unsigned short	seqno;
} icmp_hdr_t;

struct _buf_t;
struct _netif_t;

void ip_init (ip_t *ip, struct _mem_pool_t *pool, int prio,
	struct _timer_t *timer, struct _arp_t *arp, mutex_group_t *g);
void ip_input (ip_t *ip, struct _buf_t *p, struct _netif_t *inp);
bool_t ip_output (ip_t *ip, struct _buf_t *p, unsigned char *dest,
	unsigned char *src, small_uint_t proto);
bool_t ip_output_netif (ip_t *ip, struct _buf_t *p, unsigned char *dest,
	unsigned char *src, small_uint_t proto, unsigned char *gateway,
	struct _netif_t *netif, unsigned char *netif_ipaddr);

void icmp_echo_request (ip_t *ip, struct _buf_t *p, struct _netif_t *inp);
void icmp_dest_unreach (ip_t *ip, struct _buf_t *p, small_uint_t op);
void icmp_time_exceeded (ip_t *ip, struct _buf_t *p);

#endif /* __IP_H_ */
