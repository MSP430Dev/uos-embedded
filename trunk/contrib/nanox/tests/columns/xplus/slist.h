class SimpleList {
public:
	SimpleList ()           // create empty list
	    { head = ptr = 0; }

	~SimpleList ();         // destroy list

	void Insert (void *);   // insert new element into list
	void Delete (void *);   // remove element from list

	void *First ();         // return first element
	void *Next ();          // return next element

	int Empty ()            // check if list is empty
	    { return (head == 0); }

private:
	struct SimpleListElement *head;
	struct SimpleListElement *tail;
	struct SimpleListElement *ptr;
};
