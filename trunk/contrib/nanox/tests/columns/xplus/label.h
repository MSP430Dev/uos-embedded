class xLabel : public xWindow {
public:
	xPen    *pen;

	xLabel (xWindow *, char *, char *);

	virtual void ComputeSize ();
	virtual void Redraw (int, int, int, int);

private:
	char    *label;
};
