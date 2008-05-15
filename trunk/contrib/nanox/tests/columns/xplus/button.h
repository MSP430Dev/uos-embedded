class xButton : public xWindow {
public:
	xSensor *input;
	xPen    *pen;

	xPixel  background;
	xPixel  light;
	xPixel  shadow;

	xButton (xWindow *, char *);

	virtual void ComputeSize ();
	virtual void Initialize ();
	virtual void Handle (xEvent &);
	virtual void Redraw (int, int, int, int);

	void SetCallback (void *);

private:
	int     active;
	void    (*callback) ();
};
