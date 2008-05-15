struct TargetEntry;

class TargetTable {
public:
	TargetTable (int=32);   	// create table of given size
	~TargetTable ();        	// delete table

	void Insert (long, void *);	// insert new key-value pair
	void Remove (long);             // remove value entry
	void *Find (long);      	// find value by key

private:
	int size;               	// size of ttable
	TargetEntry **table;    	// table of key-value pairs

	TargetEntry *Probe (long);
	TargetEntry **ProbeAddr (long);
};
