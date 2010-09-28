#ifndef __BRIDGE_H_
#define	__BRIDGE_H_ 1

typedef struct _bridge_entry_t {
	unsigned short	time_to_live;
	unsigned short	addr [3];
	struct _bridge_entry_t	*next;
} bridge_entry_t;

typedef struct _bridge_t {
	unsigned long	size;
	unsigned long	step;
	bridge_entry_t	*robin;
	bridge_entry_t	table [1];
} bridge_t;

bridge_t *bridge_init (struct _mem_pool_t *pool, int size, int msec);
struct _buf_t *bridge_filter (bridge_t *b, struct _buf_t *p);
void bridge_timer (bridge_t *b);
void bridge_clear (bridge_t *b);

#endif /* !__BRIDGE_H_ */
