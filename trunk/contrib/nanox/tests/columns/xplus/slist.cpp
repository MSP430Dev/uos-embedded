# include "slist.h"

struct SimpleListElement {
	SimpleListElement *next;
	void *value;
};

SimpleList::~SimpleList ()              // destroy list
{
	SimpleListElement *p, *n;

	for (p=head; p; p=n) {
		n = p->next;
		delete p;
	}
}

void SimpleList::Insert (void *val)     // insert new element into list
{
	SimpleListElement *p;

	p = new SimpleListElement;
	p->value = val;
	p->next = 0;
	if (head) {
		tail->next = p;
		tail = p;
	} else
		head = tail = p;
}

void SimpleList::Delete (void *val)     // remove element from list
{
	SimpleListElement *p, *prev;

	prev = 0;
	for (p=head; p; p=p->next) {
		if (p->value == val)
			break;
		prev = p;
	}
	if (p) {
		if (prev)
			prev->next = p->next;
		else
			head = p->next;
		if (p == tail)
			tail = prev;
		delete p;
	}
}

void *SimpleList::First ()              // return first element
{
	ptr = head;
	if (! ptr)
		return (0);
	return (ptr->value);
}

void *SimpleList::Next ()               // return next element
{
	if (! ptr)
		return (0);
	ptr = ptr->next;
	if (! ptr)
		return (0);
	return (ptr->value);
}
