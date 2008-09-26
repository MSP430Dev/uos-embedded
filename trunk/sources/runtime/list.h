#ifndef __LIST_H_
#define __LIST_H_ 1

/*
 * LY: some ideas comming from Linux's source code, but not the code.
 */

typedef struct _list_t {
	struct _list_t	*f /* forward */,
			*b /* back */;
} list_t;

static inline void list_init (list_t *l)
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

static inline void list_ahead (list_t *l, list_t *i)
{
	__list_ins (i, l, l->f);
}

static inline void list_append (list_t *l, list_t *i)
{
	__list_ins (i, l->b, l);
}

static inline void list_del (list_t *i)
{
	__list_del (i->b, i->f);
	list_init (i);
}

static inline void list_replace (list_t *o, list_t *n)
{
	n->f = o->f; n->f->b = n;
	n->b = o->b; n->b->f = n;
	list_init (o);
}

static inline void list_move_ahead (list_t *l, list_t *i)
{
	__list_del (i->b, i->f);
	list_ahead (l, i);
}

static inline void list_move_append (list_t *l, list_t *i)
{
	__list_del (i->b, i->f);
	list_append (l, i);
}

static inline bool_t list_is_last (const list_t *l, const list_t *i)
{
	return i->f == l;
}

static inline bool_t list_is_empty (const list_t *l)
{
	return l->f == l;
}

static inline bool_t list_is_linked (const list_t *l)
{
	return l->f != l;
}

/*
 * LY: append all items from src to dst, emptied src.
 */
static inline void list_join (list_t *dst, list_t *src)
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

static inline void list_eject (list_t *src, list_t *dst)
{
	list_init (dst);
	list_join (dst, src);
}

#define list_entry(ptr, type, member) ({					\
		const typeof (((type *)0)->member) *__mptr = (ptr);		\
		(type *)((char *)__mptr - __builtin_offsetof (type, member));	\
	})

#define list_first_entry(ptr, type, member) \
	list_entry ((ptr)->f, type, member)

#define list_iterate(i, head) \
	for (i = (head)->f; i != (head); i = i->f)

#define list_iterate_backward(i, head) \
	for (i = (head)->b; i != (head); i = i->b)

#define list_iterate_entry(i, head, member)				\
	for (i = list_entry ((head)->f, typeof (*i), member);		\
	     &i->member != (head); 					\
	     i = list_entry (i->member.f, typeof (*i), member))

#define list_iterate_entry_backward(i, head, member)			\
	for (i = list_entry ((head)->b, typeof (*i), member);		\
	     &i->member != (head); 					\
	     i = list_entry (i->member.b, typeof (*i), member))

#endif /* __LIST_H_ */
