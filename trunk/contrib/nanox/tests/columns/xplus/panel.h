class xPanel : public xWindow {
public:
	xPanel (xWindow *, char *nam, int = 0, int = 2, int = 2);

	virtual void ComputeSize ();
	virtual void Resize ();

private:
	int     vertical;
	int     nchilds;
	int     border;
	int     gap;
};
