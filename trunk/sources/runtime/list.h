#ifndef __LIST_H_
#define __LIST_H_ 1

/*
 * LY: some ideas comming from Linux's source code, but not the code.
 */

typedef struct _list_t {
	struct _list_t	*f /* forward */,
			*b /* back */;
} list_t;

inline extern void list_init (list_t *l)
{
	l->f = l; l->b = l;
}

static inline void __list_ins (list_t *n, list_t *b, list_t *f)
{
	f->b = n; n->f = f;
	n->b = b; b->f = n;
}

static inline void __list_del (list_t *b, list_t *f)
{
	f->b = b; b->f = f;
}

inline extern void list_ahead (list_t *l, list_t *i)
{
	__list_ins (i, l, l->f);
}

inline extern void list_append (list_t *l, list_t *i)
{
	__list_ins (i, l->b, l);
}

inline extern void list_del (list_t *i)
{
	__list_del (i->b, i->f);
	list_init (i);
}

inline extern void list_replace (list_t *o, list_t *n)
{
	n->f = o->f; n->f->b = n;
	n->b = o->b; n->b->f = n;
	list_init (o);
}

inline extern void list_move_ahead (list_t *l, list_t *i)
{
	__list_del (i->b, i->f);
	list_ahead (l, i);
}

inline extern void list_move_append (list_t *l, list_t *i)
{
	__list_del (i->b, i->f);
	list_append (l, i);
}

inline extern bool_t list_is_last (const list_t *l, const list_t *i)
{
	return i->f == l;
}

inline extern bool_t list_is_empty (const list_t *l)
{
	return l->f == l;
}

inline extern bool_t list_is_linked (const list_t *l)
{
	return l->f != l;
}

/*
 * LY: append all items from src to dst, emptied src.
 */
inline extern void list_join (list_t *dst, list_t *src)
{
	if (! list_is_empty (src)) {
		list_t *sf = src->f;
		list_t *sl = src->b;
		list_t *df = dst->f;

		sf->b = dst;
		sl->f = df;

		dst->f = sf;
		df->b = sl;

		list_init (src);
	}
}

inline extern void list_eject (list_t *src, list_t *dst)
{
	list_init (dst);
	list_join (dst, src);
}

#define list_entry(ptr, type, member) \
	container_of (ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry ((ptr)->f, type, member)

#define list_iterate(pos, head) \
	for (pos = (head)->f; pos != (head); pos = pos->f)

#define list_iterate_backward(pos, head) \
	for (pos = (head)->b; pos != (head); pos = pos->b)

#define list_iterate_entry(pos, head, member)				\
	for (pos = list_entry ((head)->f, typeof (*pos), member);	\
	     &pos->member != (head); 					\
	     pos = list_entry (pos->member.f, typeof (*pos), member))

#define list_iterate_entry_backward(pos, head, member)			\
	for (pos = list_entry ((head)->b, typeof (*pos), member);	\
	     &pos->member != (head); 					\
	     pos = list_entry (pos->member.b, typeof (*pos), member))

#endif /* __LIST_H_ */
