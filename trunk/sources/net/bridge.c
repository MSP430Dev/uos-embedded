#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <mem/mem.h>
#include <net/bridge.h>

/*
 * Время устаревания записей в таблице MAC-адресов, в секундах.
 * По стандарту должно быть 300 секунд.
 */
#define MAXAGE_SEC	300

/*
 * Время полного сканирования таблицы, в секундах.
 */
#define FULLSCAN_SEC	10

/* MUST be compiled with "pack structs" or equivalent! */
struct eth_hdr {
	unsigned char	dest [6];	/* destination MAC address */
	unsigned char	src [6];	/* source MAC address */
	unsigned short	proto;		/* protocol type */
} __attribute__ ((packed));

/*
 * Initialize bridge data structure.
 */
bridge_t *
bridge_init (mem_pool_t *pool, int size, int msec)
{
	bridge_t *b;

	/* Size must be a power of 2! */
	assert ((size & (size-1)) == 0);

	b = (bridge_t*) mem_alloc (pool, sizeof (bridge_t) +
		(size - 1) * sizeof (bridge_entry_t));
	if (! b) {
		debug_printf ("bridge: out of memory\n");
		abort ();
	}
	b->size = size;
	b->robin = b->table;
	b->step += b->size * msec / FULLSCAN_SEC / 1000;
	return b;
}

static bridge_entry_t *
find_entry (bridge_t *b, unsigned short* mac, int allocate_flag)
{
	bridge_entry_t *e, *prev, *limit;
	unsigned short tmp [3], *addr;

	if ((int)mac & 1) {
		memcpy (tmp, mac, 6);
		addr = tmp;
	} else
		addr = mac;

	/* addr halfword aligned */
	e = b->table +
		((addr[0] + addr[1] + addr[2] * 067433) & (b->size - 1));

	for (prev=0; e; prev=e, e=e->next) {
		if (! e->time_to_live) {
			/* Found an outdated entry. */
			if (allocate_flag)
				goto allocate;
			if (prev)
				prev->next = 0;
			break;
		}
		if (((unsigned short*) e->addr) [0] == addr[0] &&
		    ((unsigned short*) e->addr) [1] == addr[1] &&
		    ((unsigned short*) e->addr) [2] == addr[2])
			return e;
	}
	/* Address not found. */
	if (! allocate_flag)
		return 0;

	/* Prev cannot be NULL here. */
	assert (prev != 0);

	/* Find an empty slot. */
	limit = b->table + b->size;
	for (e=prev+1; e->time_to_live; ++e) {
		if (e == limit)
			e = b->table;
		if (e == prev) {
			/* No empty space. */
			return 0;
		}
	}
allocate:
	((unsigned short*) e->addr) [0] = addr[0];
	((unsigned short*) e->addr) [1] = addr[1];
	((unsigned short*) e->addr) [2] = addr[2];
	if (prev)
		prev->next = e;
	/* debug_printf ("bridge: add entry %02x-%02x-%02x-%02x-%02x-%02x\n",
		e->addr[0], e->addr[1], e->addr[2],
		e->addr[3], e->addr[4], e->addr[5]); */
	return e;
}

/*
 * Process outgoing packet:
 * 1) If the target address is present in table, then ignore the packet.
 * 2) Add the source address to the table.
 */
struct _buf_t *
bridge_filter (bridge_t *b, struct _buf_t *p)
{
	struct eth_hdr *h;
	bridge_entry_t *s, *d;

	h = (struct eth_hdr*) p->payload;
	/* debug_printf ("bridge: %d bytes from %02x-%02x-%02x-%02x-%02x-%02x"
		" to %02x-%02x-%02x-%02x-%02x-%02x\n",
		p->tot_len,
		h->src[0], h->src[1], h->src[2],
		h->src[3], h->src[4], h->src[5],
		h->dest[0], h->dest[1], h->dest[2],
		h->dest[3], h->dest[4], h->dest[5]); */

	/* Update source entry. */
	s = find_entry (b, (unsigned short*) h->src, 1);
	if (s)
		s->time_to_live = MAXAGE_SEC / FULLSCAN_SEC;

	/* Broadcast or multicast - pass through. */
	if (h->dest[0] & 0x80)
		return p;

	/* Check destination. */
	d = find_entry (b, (unsigned short*) h->dest, 0);
	if (d) {
		/* Destination found in the table of local MAC addresses.
		 * Ignore the packet. */
		buf_free (p);
		/*debug_printf ("-- ignore\n");*/
		return 0;
	}
	return p;
}

/*
 * Remove outdated entries from MAC address table.
 * Called every second. According to 802.3 specification,
 * MAC addresses must be deleted after MAXAGE_SEC seconds,
 */
void
bridge_timer (bridge_t *b)
{
	bridge_entry_t *next, *limit, *e;
	int ttl;

	/* Указатель на конец таблицы. */
	limit = b->table + b->size;

	/* Применяем инкрементное сканирование таблицы.
	 * Robin указывает на место, где мы в прошлый раз закончили.
	 * Начинаем с этого места. */
	if (b->robin >= limit)
		b->robin = b->table;
	e = b->robin;

	/* Продвигаем robin пропорционально периоду сканирования.
	 * Участок сканирования должен быть непрерывным. */
	b->robin += b->step;
	if (b->robin > limit)
		b->robin = limit;

	assert (e >= b->table);
	assert (e < b->robin);
	assert (b->robin <= limit);

	for (; e != b->robin; ++e) {
		ttl = e->time_to_live;
		if (ttl == 0)
			continue;

		e->time_to_live = --ttl;
		if (ttl == 0) {
			/* Clean this entry. */
			/* debug_printf ("bridge: remove entry %02x-%02x-%02x-%02x-%02x-%02x\n",
				e->addr[0], e->addr[1], e->addr[2],
				e->addr[3], e->addr[4], e->addr[5]); */
			next = e->next;
			if (next != 0) {
				*e = *next;
				next->time_to_live = 0;
			}
		}
	}
}

/*
 * Remove all entries from MAC address table.
 */
void
bridge_clear (bridge_t *b)
{
	bridge_entry_t *limit, *e;

	limit = b->table + b->size;
	for (e=b->table; e<limit; ++e)
		e->time_to_live = 0;
}
