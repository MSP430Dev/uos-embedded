#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/netif.h>

bool_t
netif_output_prio (netif_t *netif, buf_t *p, unsigned char *ipdest,
	unsigned char *ipsrc, uint_t prio)
{
	if (netif->arp && ipsrc) {	/* vch: для бриджуемых фреймов не нужен arp */
		unsigned char *ethdest = 0;

		/* For broadcasts, ipdest must be NULL. */
		if (ipdest && (ipdest[0] & 0xf0) != 0xe0) {
			/* Search the ARP table for MAC address. */
			ethdest = arp_lookup (netif, ipdest);
			if (! ethdest) {
				/* Send ARP request. */
				arp_request (netif, p, ipdest, ipsrc);
				goto discard;
			}
		}
		if (! arp_add_header (netif, p, ipdest, ethdest)) {
discard:		/* Count this packet as discarded. */
			++netif->out_discards;
			return 0;
		}
	}
	return netif->interface->output (netif, p, prio);
}

bool_t
netif_output (netif_t *netif, buf_t *p, unsigned char *ipdest,
	unsigned char *ipsrc)
{
	return netif_output_prio (netif, p, ipdest, ipsrc, 0);
}

buf_t *
netif_input (netif_t *netif)
{
	struct _buf_t *p;

	p = netif->interface->input (netif);
	if (p && netif->arp)
		p = arp_input (netif, p);
	return p;
}

void
netif_set_address (netif_t *netif, unsigned char *ethaddr)
{
	if (netif->interface->set_address)
		netif->interface->set_address (netif, ethaddr);
}
