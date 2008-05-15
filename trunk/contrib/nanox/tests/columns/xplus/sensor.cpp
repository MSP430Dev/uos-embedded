# include "event.h"
# include "sensor.h"

xSensor::xSensor ()
{
	mask = 0;
}

xSensor::xSensor (xSensor *sens)
{
	mask = sens->mask;
}

void xSensor::Catch (xEventType m)
{
	switch (m) {
	case xEventDown:
		mask |= xMaskDown;
		break;
	case xEventUp:
		mask |= xMaskUp;
		break;
	case xEventKey:
		mask |= xMaskKey;
		break;
	case xEventEnter:
		mask |= xMaskEnter;
		break;
	case xEventLeave:
		mask |= xMaskLeave;
		break;
	case xEventMotion:
		mask |= xMaskMotion;
		break;
	case xEventButtonMotion:
		mask |= xMaskButtonMotion;
		break;
	case xEventButton1Motion:
		mask |= xMaskButton1Motion;
		break;
	case xEventButton2Motion:
		mask |= xMaskButton2Motion;
		break;
	case xEventButton3Motion:
		mask |= xMaskButton3Motion;
		break;
	}
}

void xSensor::Ignore (xEventType m)
{
	switch (m) {
	case xEventDown:
		mask &= ~xMaskDown;
		break;
	case xEventUp:
		mask &= ~xMaskUp;
		break;
	case xEventKey:
		mask &= ~xMaskKey;
		break;
	case xEventEnter:
		mask &= ~xMaskEnter;
		break;
	case xEventLeave:
		mask &= ~xMaskLeave;
		break;
	case xEventMotion:
		mask &= ~xMaskMotion;
		break;
	case xEventButtonMotion:
		mask &= ~xMaskButtonMotion;
		break;
	case xEventButton1Motion:
		mask &= ~xMaskButton1Motion;
		break;
	case xEventButton2Motion:
		mask &= ~xMaskButton2Motion;
		break;
	case xEventButton3Motion:
		mask &= ~xMaskButton3Motion;
		break;
	}
}
