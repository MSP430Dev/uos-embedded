# include "htable.h"

struct HashEntry {
	long            value;
	long            key;
	HashEntry       *next;
};

HashTable::HashTable (int n)
{
	register HashEntry **e;

	size = 32;
	while (size < n)
		size <<= 1;
	table = new HashEntry * [size];
	--size;
	for (e = &table[size]; e >= table; --e)
		*e = 0;
}

HashTable::~HashTable ()
{
	delete table;
}

inline HashEntry *HashTable::Probe (long i)
{
	return table [i & size];
}

inline HashEntry **HashTable::ProbeAddr (long i)
{
	return &table [i & size];
}

void HashTable::Insert (long k, long v)
{
	register HashEntry *e;
	register HashEntry **a;

	e = new HashEntry;
	e->key = k;
	e->value = v;
	a = ProbeAddr (k);
	e->next = *a;
	*a = e;
}

long HashTable::Find (long k)
{
	register HashEntry *e;

	for (e = Probe (k); e; e = e->next)
		if (e->key == k)
			return (e->value);
	return (-1);
}
