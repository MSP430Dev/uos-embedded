# include "ttable.h"

struct TargetEntry {
	void            *value;
	long            key;
	TargetEntry     *next;
};

TargetTable::TargetTable (int n)
{
	register TargetEntry **e;

	size = 32;
	while (size < n)
		size <<= 1;
	table = new TargetEntry * [size];
	--size;
	for (e = &table[size]; e >= table; --e)
		*e = 0;
}

TargetTable::~TargetTable ()
{
	delete table;
}

inline TargetEntry *TargetTable::Probe (long i)
{
	return table [i & size];
}

inline TargetEntry **TargetTable::ProbeAddr (long i)
{
	return &table [i & size];
}

void TargetTable::Insert (long k, void *v)
{
	register TargetEntry *e;
	register TargetEntry **a;

	e = new TargetEntry;
	e->key = k;
	e->value = v;
	a = ProbeAddr (k);
	e->next = *a;
	*a = e;
}

void *TargetTable::Find (long k)
{
	register TargetEntry *e;

	for (e = Probe (k); e; e = e->next)
		if (e->key == k)
			return (e->value);
	return (0);
}

void TargetTable::Remove (long k)
{
	register TargetEntry *e, *prev;
	TargetEntry **a;

	a = ProbeAddr (k);
	e = *a;
	if (e) {
		if (e->key == k) {
			*a = e->next;
			delete e;
		} else {
			do {
				prev = e;
				e = e->next;
			} while (e && e->key != k);
			if (e) {
				prev->next = e->next;
				delete e;
			}
		}
	}
}
