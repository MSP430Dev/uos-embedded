struct NameTableEntry;

class NameTable {
public:
	NameTable (int=32);             // create table of given size
	~NameTable ();                  // delete table

	void Insert (char *, long);     // insert new key-value pair
	long Find (char *);             // find value by key

private:
	int size;                       // size of table
	NameTableEntry **table;    	// table of key-value pairs

	NameTableEntry *Probe (char *name);
	NameTableEntry **ProbeAddr (char *name);
	int Hash (char *);
};
