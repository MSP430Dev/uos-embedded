# include "ntable.h"
# include "extern.h"

struct NameTableEntry {
	long            value;
	char            *key;
	NameTableEntry  *next;
};

NameTable::NameTable (int n)
{
	register NameTableEntry **e;

	size = 32;
	while (size < n)
		size <<= 1;
	table = new NameTableEntry * [size];
	--size;
	for (e = &table[size]; e >= table; --e)
		*e = 0;
}

NameTable::~NameTable ()
{
	delete table;
}

int NameTable::Hash (char *name)
{
	int sum = 0;
	for (char *p=name; *p; ++p)
		sum = (sum << 1) + *p;
	return ((sum * 353) & size);
}

inline NameTableEntry *NameTable::Probe (char *name)
{
	return (table [Hash (name)]);
}

inline NameTableEntry **NameTable::ProbeAddr (char *name)
{
	return (&table [Hash (name)]);
}

void NameTable::Insert (char *k, long v)
{
	register NameTableEntry *e;
	register NameTableEntry **a;

	e = new NameTableEntry;
	e->key = strdup (k);
	e->value = v;
	a = ProbeAddr (k);
	e->next = *a;
	*a = e;
}

long NameTable::Find (char *k)
{
	register NameTableEntry *e;

	for (e = Probe (k); e; e = e->next)
		if (! strcmp (e->key, k))
			return (e->value);
	return (-1);
}
