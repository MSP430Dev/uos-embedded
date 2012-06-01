#include "hiddev.h"

#define MIN(x,y)    (((x) < (y)) ? (x) : (y))

static void hiddev_request_handler (void *tag, usb_setup_pkt_t *setup, uint8_t **data, int *size)
{
    hiddev_t *h = (hiddev_t *) tag;
    unsigned index;
    static uint8_t uc;
    
debug_printf ("hiddev_request_handler, bmRequestType = %02X, bRequest = %u, wValue = %04X\n", 
    setup->bmRequestType, setup->bRequest, setup->wValue);
    
    if (USB_REQ_GET_TYPE(setup->bmRequestType) == USB_REQ_TYPE_STANDARD) {
        // Standard request
        switch (setup->bRequest) {
        case USB_SR_GET_DESCRIPTOR:
            index = setup->wValue & 0xFF;
    	        switch (setup->wValue >> 8) {
    	        case USB_DESC_TYPE_REPORT:
    	            if (index < HIDDEV_NB_REPORTS) {
    	                *data = h->rpt_desc[index];
    	                *size = MIN(h->rpt_desc_sz[index], setup->wLength);
    	            }
    	        break;
    	        case USB_DESC_TYPE_PHYSICAL:
    	            if (index < HIDDEV_NB_PHYSICALS) {
    	                *data = h->phys_desc[index];
    	                *size = MIN(h->phys_desc_sz[index], setup->wLength);
    	            }
    	        break;
    	        default:
    	            *size = -1;
    	        }
    	    break;
    	    case USB_SR_SET_DESCRIPTOR:
    	        // Not yet supported
    	        *size = -1;
    	    break;
    	    	}
    } else {
        // HID specific requests
        switch (setup->bRequest) {
        case USB_HID_GET_REPORT:
debug_printf ("USB_HID_GET_REPORT, wValue = %04X, wLength = %d\n", setup->wValue, setup->wLength);
            index = setup->wValue & 0xFF;
            if (index >= HIDDEV_NB_REPORTS) {
                *size = -1;
                break;
            }
            switch (setup->wValue >> 8) {
            case USB_HID_RPT_IN:
                *data = h->in_rpt[index];
                *size = h->in_rpt_sz[index];
debug_printf ("data: %u %u %u\n", h->in_rpt[index][0], h->in_rpt[index][1], h->in_rpt[index][2]);
            break;
            case USB_HID_RPT_OUT:
                *data = h->out_rpt[index];
                *size = h->out_rpt_sz[index];
            break;
            case USB_HID_RPT_FEATURE:
                *data = h->feature_rpt[index];
                *size = h->feature_rpt_sz[index];
            break;
            default:
                *size = -1;
            }
        break;
        case USB_HID_GET_IDLE:
            if (!h->set_idle) {
                *size = -1;
            } else {
                uc = h->idle_rate_ms >> 2;
                *data = &uc;
                *size = 1;
            }
        break;
        case USB_HID_GET_PROTOCOL:
            if (!h->set_prt) {
                *size = -1;
            } else {
                *data = &h->cur_prot;
                *size = 1;
            }
        break;
        case USB_HID_SET_REPORT:
            if (!h->set_rpt) { 
                *size = -1;
            } else {
                if (h->set_rpt (h, *data, *size))
                    *size = 0;
                else *size = -1;
            }
        break;
        case USB_HID_SET_IDLE:
            if (!h->set_idle) {
                *size = -1;
            } else {
                uc = (setup->wValue >> 8);
                uc <<= 2;
                if (h->set_idle (h, uc)) {
                    h->idle_rate_ms = uc;
                    *size = 0;
                } else *size = -1;
            }
        break;
        case USB_HID_SET_PROTOCOL:
            if (!h->set_prt) {
                *size = -1;
            } else {
                if (h->set_prt (h, setup->wValue)) {
                    h->cur_prot = setup->wValue;
                    *size = 0;
                } else *size = -1;
            }
        break;
        default:
            *size = -1;
        }
    }
}

void hiddev_set_idle_rate (hiddev_t *h, unsigned rate)
{
    h->idle_rate_ms = rate;
}

void hiddev_set_protocol (hiddev_t *h, uint8_t protocol)
{
    h->cur_prot = protocol;
}

void hiddev_set_report_handler (hiddev_t *h, hiddev_set_report_handler_t handler)
{
    h->set_rpt = handler;
}

void hiddev_set_idle_handler (hiddev_t *h, hiddev_set_idle_handler_t handler)
{
    h->set_idle = handler;
}

void hiddev_set_protocol_handler (hiddev_t *h, hiddev_set_protocol_handler_t handler)
{
    h->set_prt = handler;
}

void hiddev_set_report_desc (hiddev_t *h, unsigned rpt_id, uint8_t *rpt_desc, unsigned desc_size, unsigned in_rpt_size, unsigned out_rpt_size, unsigned feature_rpt_size)
{
    h->rpt_desc[rpt_id] = rpt_desc;
    h->rpt_desc_sz[rpt_id] = desc_size;
    h->in_rpt_sz[rpt_id] = in_rpt_size;
    h->out_rpt_sz[rpt_id] = out_rpt_size;
    h->feature_rpt_sz[rpt_id] = feature_rpt_size;
    if (in_rpt_size) {
        h->in_rpt[rpt_id] = mem_alloc (h->pool, in_rpt_size);
        assert (h->in_rpt[rpt_id]);
    }
    if (out_rpt_size) {
        h->out_rpt[rpt_id] = mem_alloc (h->pool, out_rpt_size);
        assert (h->out_rpt[rpt_id]);
    }
    if (feature_rpt_size) {
        h->feature_rpt[rpt_id] = mem_alloc (h->pool, feature_rpt_size);
        assert (h->feature_rpt[rpt_id]);
    }
}

void hiddev_set_physical_desc (hiddev_t *h, unsigned rpt_id, uint8_t *phys_desc, unsigned desc_size)
{
    h->phys_desc[rpt_id] = phys_desc;
    h->phys_desc_sz[rpt_id] = desc_size;
}

void hiddev_init (hiddev_t *h, usbdev_t *u, unsigned if_n, mem_pool_t *pool, mutex_t *m)
{
    memset (h, 0, sizeof (hiddev_t));
    
    h->usb = u;
    h->if_n = if_n;
    h->pool = pool;
    h->lock = m;
    
    usbdev_set_iface_specific_handler (u, if_n, hiddev_request_handler, h);
}

void hiddev_output_report (hiddev_t *h, unsigned rpt_id, const uint8_t *report)
{
    assert (rpt_id < HIDDEV_NB_REPORTS);
    
    mutex_lock (h->lock);
    memcpy (h->in_rpt[rpt_id], report, h->in_rpt_sz[rpt_id]);
    h->in_rpt_ready[rpt_id] = 1;
    mutex_unlock (h->lock);
    usbdev_send (h->usb, 1, h->in_rpt[rpt_id], h->in_rpt_sz[rpt_id]);
}

void hiddev_input_report (hiddev_t *h, unsigned rpt_id, uint8_t *report)
{
}

