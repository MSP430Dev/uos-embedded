class QuarkList {
	XrmQuark        *list;
	short           length;         // actually length of list is length+2

	QuarkList (char *nam)
	{
		length = 1;
		list = new XrmQuark [length + 2];
		list [0] = XrmStringToQuark (nam);
		list [1] = 0;
		list [2] = 0;
	}

	QuarkList (QuarkList *q, char *nam)
	{
		length = q->length + 1;
		list = new XrmQuark [length + 2];
		for (int i=0; i<q->length; ++i)
			list [i] = q->list [i];
		list [length - 1] = XrmStringToQuark (nam);
		list [length] = 0;
		list [length + 1] = 0;
	}

	~QuarkList ()
		{ delete list; }

	void Append (char *nam)         // append name to quark list
		{ list [length] = XrmStringToQuark (nam); }

	char *LastString ()
		{ return (XrmQuarkToString (list [length - 1])); }

	friend class xWindow;
	friend class xScreen;
};
