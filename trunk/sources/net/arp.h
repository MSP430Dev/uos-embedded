#ifndef __ARP_H_
#define	__ARP_H_ 1

typedef struct _arp_entry_t {
	struct _netif_t	*netif;
	unsigned char	ipaddr [4];
	unsigned char	ethaddr [6];
	unsigned char	age;
} arp_entry_t;

typedef struct _arp_t {
	struct _ip_t	*ip;
	unsigned char	size;
	unsigned char	timer;
	arp_entry_t	table [1];
} arp_t;

/* MUST be compiled with "pack structs" or equivalent! */
struct eth_hdr {
	unsigned char	dest [6];	/* destination MAC address */
	unsigned char	src [6];	/* source MAC address */

	unsigned short	proto;		/* protocol type */
#define PROTO_ARP	HTONS (0x0806)
#define PROTO_IP	HTONS (0x0800)
} __attribute__ ((packed));

struct arp_hdr {
	struct eth_hdr	eth;		/* ethernet header */

	unsigned short	hwtype;		/* hardware address type */
#define HWTYPE_ETHERNET	HTONS (1)

	unsigned short	proto;		/* protocol type */

	unsigned char	hwlen;		/* hardware address length */
	unsigned char	protolen;	/* protocol address length */

	unsigned short	opcode;		/* ARP operation code */
#define ARP_REQUEST	HTONS (1)
#define ARP_REPLY	HTONS (2)

	unsigned char	src_hwaddr [6];	/* source ethernet address */
	unsigned char	src_ipaddr [4];	/* source IP address */
	unsigned char	dst_hwaddr [6];	/* dest. ethernet address */
	unsigned char	dst_ipaddr [4];	/* dest. IP address */
} __attribute__ ((packed));

struct ethip_hdr {
	/* Ethernet header */
	struct eth_hdr	eth;

	/* IP header */
	unsigned char	ip_hdr0 [4];
	unsigned char	ip_hdr1 [4];
	unsigned char	ip_hdr2 [4];
	unsigned char	ip_src [4];
	unsigned char	ip_dst [4];
} __attribute__ ((packed));

arp_t *arp_init (opacity_t *buf, unsigned bytes, struct _ip_t *ip);
struct _buf_t *arp_input (struct _netif_t *netif, struct _buf_t *p);
bool_t arp_request (struct _netif_t *netif, struct _buf_t *p,
	unsigned char *ipdest, unsigned char *ipsrc);
bool_t arp_add_header (struct _netif_t *netif, struct _buf_t *p,
	unsigned char *ipdest, unsigned char *ethdest);
unsigned char *arp_lookup (struct _netif_t *netif, unsigned char *ipaddr);
void arp_timer (arp_t *arp);

#endif /* !__ARP_H_ */
