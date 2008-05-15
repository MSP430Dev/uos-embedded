struct HashEntry;

class HashTable {
public:
	HashTable (int=32);             // create table of given size
	~HashTable ();                  // delete table

	void Insert (long, long);       // insert new key-value pair
	long Find (long);               // find value by key

private:
	int size;               	// size of ttable
	HashEntry **table;              // table of key-value pairs

	HashEntry *Probe (long);
	HashEntry **ProbeAddr (long);
};
