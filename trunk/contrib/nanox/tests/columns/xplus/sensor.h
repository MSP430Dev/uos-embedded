class   xWindow;
enum    xEventType;

class xSensor {
public:
	xSensor ();
	xSensor (xSensor *);            // create copy of xSensor

	void Catch (xEventType);        // catch event type
	void Ignore (xEventType);       // ignore event type

private:
	int             mask;           // mask of events

	friend class xWindow;
};

