#ifndef __ROUTE_H_
#define	__ROUTE_H_ 1

struct _netif_t;
struct _ip_t;

typedef struct _route_t {
	struct _route_t *next;
	struct _netif_t	*netif;
	unsigned char	ipaddr [4];
	unsigned char	broadcast [4];
	unsigned char	netaddr [4];
	unsigned char	gateway [4];
	unsigned char	gwifaddr [4];
	unsigned char	masklen;
} route_t;

struct _netif_t *route_lookup (struct _ip_t *ip, unsigned char *ipaddr,
	unsigned char **gateway, unsigned char **netif_ipaddr);
struct _netif_t *route_lookup_self (struct _ip_t *ip, unsigned char *ipaddr,
	unsigned char *broadcast);
unsigned char *route_lookup_ipaddr (struct _ip_t *ip, unsigned char *ipaddr,
	struct _netif_t *netif);

void route_add_netif (struct _ip_t *ip, struct _route_t *r,
	unsigned char *ipaddr, unsigned char masklen, struct _netif_t *netif);
bool_t route_add_gateway (struct _ip_t *ip, struct _route_t *r,
	unsigned char *ipaddr, unsigned char masklen, unsigned char *gateway);

void route_setup (struct _ip_t *ip, struct _route_t *r, unsigned char *ipaddr,
	unsigned char masklen, unsigned char *gateway);

#endif /* !__ROUTE_H_ */
