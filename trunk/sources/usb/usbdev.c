#include "usbdev.h"

#define MIN(x,y)    (((x) < (y)) ? (x) : (y))


static void set_configuration (usbdev_t *u, int conf_num)
{
    uint8_t *pchar = (uint8_t *) u->conf_desc [conf_num - 1];
    pchar += sizeof (usb_conf_desc_t);
    usb_iface_desc_t *pif = (usb_iface_desc_t *) pchar;
    pchar += sizeof (usb_iface_desc_t);
    if (pif->bNumEndpoints != 0) {
        usb_ep_desc_t *pep = (usb_ep_desc_t *) pchar;
        while (pep->bDescriptorType != USB_DESC_TYPE_ENDPOINT) {
            pchar += pep->bLength;
            pep = (usb_ep_desc_t *) pchar;
        }
        
        int i;
        for (i = 0; i < pif->bNumEndpoints; ++i) {
            unsigned ep = pep->bEndpointAddress & 0x7F;
            int dir = pep->bEndpointAddress >> 7;
            if (dir == USBDEV_DIR_OUT) {
                u->ep_out[ep].attr = pep->bmAttributes;
                u->ep_out[ep].max_size = pep->wMaxPacketSize;
                u->ep_out[ep].interval = pep->bInterval;
                mem_queue_init (&u->ep_out[ep].rxq, u->pool, u->ep_out[ep].rxq_depth);
                if ((pep->bmAttributes & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL) {
                    u->ep_out[ep].state = EP_STATE_WAIT_SETUP;
                } else {
                    u->ep_out[ep].state = EP_STATE_WAIT_OUT;
                }
            } else { // dir == USBDEV_DIR_IN
                u->ep_in[ep].attr = pep->bmAttributes;
                u->ep_in[ep].max_size = pep->wMaxPacketSize;
                u->ep_in[ep].interval = pep->bInterval;
                u->ep_in[ep].state = EP_STATE_WAIT_IN;
            }
            u->hal->ep_attr (ep, dir, pep->bmAttributes, pep->wMaxPacketSize, pep->bInterval);
            pchar += sizeof (usb_ep_desc_t);
            pep = (usb_ep_desc_t *) pchar;
        }
    }
    u->cur_conf = conf_num;
    //u->state = USBDEV_STATE_CONFIGURED;
}

static void do_in (usbdev_t *u, unsigned ep)
{
    ep_in_t *epi = &u->ep_in[ep];
    int in_sz = (epi->rest_load < epi->max_size) ? 
        (epi->rest_load) : (epi->max_size);
    u->hal->ep_wait_in (ep, epi->pid, epi->ptr, in_sz);
    if (epi->state == EP_STATE_WAIT_IN) {
        if (epi->rest_load < epi->max_size)
            epi->state = EP_STATE_WAIT_IN_LAST;
        else if (epi->rest_load == epi->max_size)
            if (!epi->shorter_len)
                epi->state = EP_STATE_WAIT_IN_LAST;
    }
    epi->ptr += in_sz;
    epi->rest_load -= in_sz;
    if (epi->pid == PID_DATA1) epi->pid = PID_DATA0;
    else epi->pid = PID_DATA1;
}

static void start_in (usbdev_t *u, unsigned ep, int start_pid, const void *data, int req_size, int real_size)
{
//debug_printf ("start_in, ep = %d, pid = %d, data @ %p, req_size = %d, real_size = %d\n", ep, start_pid, data, req_size, real_size);
    ep_in_t *epi = &u->ep_in[ep];
    epi->shorter_len = (real_size < req_size);
    if (real_size == 0)
        epi->state = EP_STATE_WAIT_IN_ACK;
    else if ((real_size < epi->max_size) || 
        ((real_size == epi->max_size) && !epi->shorter_len))
            epi->state = EP_STATE_WAIT_IN_LAST;
    else
        epi->state = EP_STATE_WAIT_IN;
    epi->ptr = (uint8_t *) data;
    epi->rest_ack = epi->rest_load = MIN(req_size, real_size);
    if (start_pid != 0)
        epi->pid = start_pid;
    while (u->hal->in_avail (ep) > 0 && epi->rest_load >= 0)
        do_in (u, ep);
}

static void set_address (usbdev_t *u, unsigned ep, int dir, void *tag)
{
    unsigned addr = (unsigned) tag;
    u->hal->set_addr (addr);
    usbdev_remove_ack_handler (u, ep, dir);
}

static void process_std_req (usbdev_t *u, unsigned ep, usb_setup_pkt_t *setup)
{
    uint16_t status;
    
    switch (setup->bRequest) {
    case USB_SR_GET_STATUS:
//debug_printf ("USB_SR_GET_STATUS\n");
        status = (USBDEV_SELF_POWERED << 0) | (USBDEV_REMOTE_WAKEUP << 1);
        start_in (u, ep, PID_DATA1, &status, setup->wLength, 2);
        break;
        // Доделать все стандартные типы запросов
    case USB_SR_SET_ADDRESS:
//debug_printf ("USB_SR_SET_ADDRESS\n");
        //u->state = USBDEV_STATE_ADDRESS;
        u->usb_addr = setup->wValue;
        usbdev_set_ack_handler (u, ep, USBDEV_DIR_OUT, set_address, (void *)(int)setup->wValue);
        start_in (u, ep, PID_DATA1, 0, 0, 0);
        break;
    case USB_SR_GET_DESCRIPTOR:
//debug_printf ("descriptor type: %d\n", setup->wValue >> 8);
        switch (setup->wValue >> 8) {
            // !!! Доделать все необходимые типы дескрипторов
        case USB_DESC_TYPE_DEVICE:
//debug_printf ("USB_DESC_TYPE_DEVICE, wLength = %d\n", setup->wLength);
            start_in (u, ep, PID_DATA1, u->dev_desc, setup->wLength, sizeof(usb_dev_desc_t));
            break;
        case USB_DESC_TYPE_QUALIFIER:       // Это нужно для HIGH SPEED
//debug_printf ("USB_DESC_TYPE_QUALIFIER\n");
            if (u->qualif_desc)
                start_in (u, ep, PID_DATA1, u->qualif_desc, setup->wLength, u->qualif_desc->bLength);
            else
                u->hal->ep_stall (ep, USBDEV_DIR_IN);
            break;
        case USB_DESC_TYPE_CONFIG:
        {
            int conf_n = setup->wValue & 0xFF;
//debug_printf ("USB_DESC_TYPE_CONFIG, wLength = %d, wValue = %X\n", setup->wLength, conf_n);
            if (conf_n >= USBDEV_NB_CONF) {
                u->hal->ep_stall (ep, USBDEV_DIR_IN);
            }
            usb_conf_desc_t *conf = u->conf_desc[conf_n];
            start_in (u, ep, PID_DATA1, conf, setup->wLength, conf->wTotalLength);
            break;
        }
        case USB_DESC_TYPE_STRING:
        {
//debug_printf ("USB_DESC_TYPE_STRING, wLength = %d, wValue = %04X\n", setup->wLength, setup->wValue);
            unsigned char *plen = (unsigned char *) u->strings[setup->wValue & 0xFF];
            start_in (u, ep, PID_DATA1, u->strings[setup->wValue & 0xFF], setup->wLength, *plen);
            break;
        }
        default:
//debug_printf ("stall process_std_req default\n");
            u->hal->ep_stall (ep, USBDEV_DIR_IN);
            break;
        }
        break;
    case USB_SR_GET_CONFIGURATION:
//debug_printf ("USB_SR_GET_CONFIGURATION, wLength = %d\n", setup->wLength);
        if (setup->wLength != 1) {
            u->rx_bad_req++;
            u->hal->ep_stall (ep, USBDEV_DIR_IN);
        }
        else start_in (u, ep, PID_DATA1, &u->cur_conf, setup->wLength, 1);
        break;
    case USB_SR_SET_CONFIGURATION:
//debug_printf ("USB_SR_SET_CONFIGURATION, wValue = %X\n", setup->wValue);
        if (setup->wValue > USBDEV_NB_CONF) {
            u->rx_bad_req++;
            u->hal->ep_stall (ep, USBDEV_DIR_IN);
        } else {
            set_configuration (u, setup->wValue);
            start_in (u, ep, PID_DATA1, 0, 0, 0);
        }
        break;
    default:
        start_in (u, ep, PID_DATA1, 0, 0, 0);
        break;
    }
}

static void process_setup (usbdev_t *u, unsigned ep, usb_setup_pkt_t *setup, void *data)
{
    unsigned char *res_data = data;
    int res_size = setup->wLength;
    unsigned num;
    
    u->rx_req++;
    
//debug_printf ("process_setup, ep = %d, bmRequestType = %02X, bRequest = %02X, wLength = %d\n", 
//    ep, setup->bmRequestType, setup->bRequest, setup->wLength);

    switch (USB_REQ_GET_RCV(setup->bmRequestType)) {
    case USB_REQ_RCV_DEVICE:
        if (USB_REQ_GET_TYPE(setup->bmRequestType) == USB_REQ_TYPE_STANDARD) {
            process_std_req (u, ep, setup);
            return;
        } else {
            if (u->dev_specific_handler)
                u->dev_specific_handler (u, u->dev_specific_tag, setup, &res_data, &res_size);
        }
        break;
    case USB_REQ_RCV_IFACE:
        num = setup->wIndex & 0xFF;
        if (num >= USBDEV_NB_INTERFACES) break;
        if (u->iface_ctrl[num].specific_handler)
            u->iface_ctrl[num].specific_handler (
                u, u->iface_ctrl[num].specific_tag, setup, &res_data, &res_size);
        break;
    case USB_REQ_RCV_ENDPOINT:
        num = setup->wIndex & 0xF;
        if (num >= USBDEV_NB_ENDPOINTS) break;
        if (setup->wIndex & EP_ATTR_IN) {
            if (u->ep_in[num].specific_handler)
                u->ep_in[num].specific_handler (
                    u, u->ep_in[num].specific_tag, setup, &res_data, &res_size);
        } else {
            if (u->ep_out[num].specific_handler)
                u->ep_out[num].specific_handler (
                    u, u->ep_out[num].specific_tag, setup, &res_data, &res_size);
        }
    }
 
    if (res_size >= 0) {
        start_in (u, ep, PID_DATA1, res_data, setup->wLength, res_size);
    } else {
        u->hal->ep_stall (ep, USBDEV_DIR_IN);
    }
}

void usbdevhal_bind (usbdev_t *u, usbdev_hal_t *hal)
{
    assert (hal);
    assert (hal->set_addr);
    assert (hal->ep_attr);
    assert (hal->ep_wait_out);
    assert (hal->ep_wait_in);
    assert (hal->ep_stall);
    assert (hal->in_avail);
    
    u->hal = hal;
}

void usbdevhal_reset (usbdev_t *u)
{
    int i;
    for (i = 0; i < USBDEV_NB_ENDPOINTS; ++i) {
        u->ep_out[i].state = EP_STATE_DISABLED;
        u->ep_in[i].state = EP_STATE_DISABLED;
        u->ep_in[i].pid = 0;
        mem_queue_free (&u->ep_out[i].rxq);
    }
    //u->state = USBDEV_STATE_DEFAULT;
    u->ep_out[0].state = EP_STATE_WAIT_SETUP;
    u->hal->ep_wait_out(0);
}

void usbdevhal_suspend (usbdev_t *u)
{
}

void usbdevhal_in_done (usbdev_t *u, unsigned ep, int size)
{
    ep_in_t *epi = &u->ep_in[ep];
//debug_printf ("usbdevhal_in_done, ep = %d, size = %d, EP0 state = 0x%03X\n", ep, size, epi->state);

    switch (epi->state) {
    case EP_STATE_WAIT_IN:
        epi->rest_ack -= size;
        do_in (u, ep);
        break;
        
    case EP_STATE_WAIT_IN_LAST:
        mutex_signal (&epi->lock, (void *) USBDEV_DIR_IN);
        epi->state = EP_STATE_NACK;
        if ((epi->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL) {
            u->ep_out[ep].state = EP_STATE_WAIT_OUT_ACK;
            u->hal->ep_wait_out (ep);
        }
        break;
        
    case EP_STATE_WAIT_IN_ACK:
        assert ((epi->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL);
        //if (u->state == USBDEV_STATE_ADDRESS)
        //    u->hal->set_addr (u->usb_addr);
        u->ep_out[ep].state = EP_STATE_WAIT_SETUP;
        epi->state = EP_STATE_NACK;
        if (u->ack_handlers[ep][USBDEV_DIR_OUT])
            u->ack_handlers[ep][USBDEV_DIR_OUT](u, ep, USBDEV_DIR_OUT,
                u->ack_handler_tags[ep][USBDEV_DIR_OUT]);
        u->hal->ep_wait_out (ep);
        break;
        
    default:
        u->ctrl_failed++;
        u->hal->ep_stall (ep, USBDEV_DIR_IN);
        break;
    }
}

void usbdevhal_out_done (usbdev_t *u, unsigned ep, int trans_type, void *data, int size)
{
    usb_setup_pkt_t *psetup;
    static usb_setup_pkt_t setup;
    static void *setup_data = 0;
    static unsigned setup_data_cnt;
    ep_out_t *epo = &u->ep_out[ep];
//debug_printf ("usbdevhal_out_done, ep = %d, trans = %d, size = %d, EP0 state = 0x%03X\n", ep, trans_type, size, epo->state);

    if ((trans_type == USBDEV_TRANSACTION_SETUP) && 
        ((epo->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL) &&
        epo->state != EP_STATE_WAIT_SETUP) {
        u->rx_bad_trans++;
        epo->state = EP_STATE_WAIT_SETUP;
    }
    
    switch (epo->state) {
    case EP_STATE_WAIT_SETUP:
        assert ((epo->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL);
        if (trans_type != USBDEV_TRANSACTION_SETUP) {
//debug_printf ("bad_trans 1: %d\n", trans_type);
            u->rx_bad_trans++;
            u->hal->ep_wait_out (ep);
            break;
        }
        if (size != 8) {
            u->rx_bad_len++;
            u->hal->ep_stall (ep, USBDEV_DIR_OUT);
            break;
        }
        psetup = data;
        if ((psetup->bmRequestType & EP_ATTR_IN) || (psetup->wLength == 0)) {
            process_setup (u, ep, psetup, 0);
        } else {
            memcpy (&setup, data, size);
            if (setup_data != 0)
                mem_free (setup_data);
            setup_data = mem_alloc_dirty (u->pool, size);
//debug_printf ("============= setup_data @ %p\n", setup_data);
            if (! setup_data) {
                u->out_of_memory++;
                u->rx_discards++;
                u->hal->ep_stall (ep, USBDEV_DIR_OUT);
                break;
            }
            setup_data_cnt = 0;
            epo->state = EP_STATE_SETUP_DATA_OUT;
            u->hal->ep_wait_out (ep);
        }
        break;
        
    case EP_STATE_SETUP_DATA_OUT:
        assert ((epo->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL);
        if (trans_type != USBDEV_TRANSACTION_OUT) {
//debug_printf ("bad_trans 2\n");
            u->rx_bad_trans++;
            epo->state = EP_STATE_WAIT_SETUP;
            u->hal->ep_stall (ep, USBDEV_DIR_OUT);
            break;
        }
        memcpy (setup_data + setup_data_cnt, data, size);
        setup_data_cnt += size;
        if (setup_data_cnt >= setup.wLength) {
            process_setup (u, ep, &setup, setup_data);
            mem_free (setup_data);
            setup_data = 0;
        } else {
            u->hal->ep_wait_out (ep);
        }
        break;
        
    case EP_STATE_WAIT_OUT:
        assert ((epo->attr & EP_ATTR_TRANSFER_MASK) != EP_ATTR_CONTROL);
        if (trans_type != USBDEV_TRANSACTION_OUT) {
//debug_printf ("bad_trans 3\n");
            u->rx_bad_trans++;
            u->hal->ep_stall (ep, USBDEV_DIR_OUT);
            break;
        }
        void *buf = mem_alloc_dirty (u->pool, size);
        if (! buf) {
            u->out_of_memory++;
            u->rx_discards++;
            u->hal->ep_stall (ep, USBDEV_DIR_OUT);
            break;
        }
        memcpy (buf, data, size);
        mem_queue_put (&epo->rxq, buf);
        if (mem_queue_is_full (&epo->rxq)) {
            // Намеренно не разрешаем приём от хоста, пусть конечная точка
            // отвечает NACK до освобождения места в очереди
            epo->state = EP_STATE_NACK;
            break;
        } 
        mutex_signal (&epo->lock, (void *) USBDEV_DIR_OUT);
        u->hal->ep_wait_out (ep);
        break;
        
    case EP_STATE_WAIT_OUT_ACK:
        assert ((epo->attr & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL);
        epo->state = EP_STATE_WAIT_SETUP;
        if (u->ack_handlers[ep][USBDEV_DIR_IN])
            u->ack_handlers[ep][USBDEV_DIR_IN](u, ep, USBDEV_DIR_IN,
                u->ack_handler_tags[ep][USBDEV_DIR_IN]);
        u->hal->ep_wait_out (ep);
        break;
        
    default:
//debug_printf ("bad_trans 4\n");
        u->rx_bad_trans++;
        break;
    }
}

void *usbdevhal_alloc_buffer (usbdev_t *u, int size)
{
    void *res = mem_alloc_dirty (u->pool, size);
    assert (res);
    return res;
}

void usbdev_init (usbdev_t *u, mem_pool_t *pool, const usb_dev_desc_t *dd)
{
    memset (u, 0, sizeof (usbdev_t));
    u->pool = pool;
    u->dev_desc = (usb_dev_desc_t *) dd;
    int i;
    for (i = 0; i < USBDEV_NB_ENDPOINTS; ++i)
        u->ep_out[i].rxq_depth = USBDEV_DEFAULT_RXQ_DEPTH;
    u->ep_in[0].max_size = u->ep_out[0].max_size = USBDEV_EP0_MAX_SIZE;
}

void usbdev_add_config_desc (usbdev_t *u, const void *cd)
{
    usb_conf_desc_t *cdesc = (usb_conf_desc_t *) cd;
    u->conf_desc[cdesc->bConfigurationValue - 1] = cdesc;
}

void usbdev_set_string_table (usbdev_t *u, const void *st[])
{
    u->strings = (void **) st;
}

void usbdev_set_dev_specific_handler (usbdev_t *u, usbdev_specific_t handler, void *tag)
{
    u->dev_specific_handler = handler;
    u->dev_specific_tag     = tag;
}

void usbdev_set_iface_specific_handler (usbdev_t *u, unsigned if_n, usbdev_specific_t handler, void *tag)
{
    assert (if_n < USBDEV_NB_INTERFACES);
    u->iface_ctrl[if_n].specific_handler = handler;
    u->iface_ctrl[if_n].specific_tag     = tag;
}

void usbdev_set_ep_specific_handler (usbdev_t *u, unsigned ep_n, int dir, usbdev_specific_t handler, void *tag)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    assert (dir < 2);
    if (dir == USBDEV_DIR_IN) {
        u->ep_in[ep_n].specific_handler = handler;
        u->ep_in[ep_n].specific_tag     = tag;
    } else {
        u->ep_out[ep_n].specific_handler = handler;
        u->ep_out[ep_n].specific_tag     = tag;
    }
}

void usbdev_set_ack_handler (usbdev_t *u, unsigned ep_n, int dir, usbdev_ack_t handler, void *tag)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    assert (dir < 2);
    u->ack_handlers[ep_n][dir] = handler;
    u->ack_handler_tags[ep_n][dir] = tag;
}

void usbdev_remove_ack_handler (usbdev_t *u, unsigned ep_n, int dir)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    assert (dir < 2);
    u->ack_handlers[ep_n][dir] = 0;
}

void usbdev_set_rx_queue_depth (usbdev_t *u, unsigned ep_n, int depth)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    u->ep_out[ep_n].rxq_depth = depth;
}

void usbdev_send (usbdev_t *u, unsigned ep_n, const void *data, int size)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    
    ep_in_t *epi = &u->ep_in[ep_n];
//debug_printf ("usbdev_send, epi->state = %d, in_avail = %d\n", epi->state, u->hal->in_avail(ep));
    mutex_lock (&epi->lock);
    while (epi->state != EP_STATE_NACK || u->hal->in_avail(ep_n) == 0)
        mutex_wait (&epi->lock);
//char *d = (char *) data;
//debug_printf ("before start_in, data = %d %d %d\n", d[0], d[1], d[2]);
    start_in (u, ep_n, 0, data, size, size);
    mutex_unlock (&epi->lock);
}

int usbdev_recv (usbdev_t *u, unsigned ep_n, void *data, int size)
{
    assert (ep_n < USBDEV_NB_ENDPOINTS);
    
    void *q_item = 0;
    int min_sz;
    ep_out_t *epo = &u->ep_out[ep_n];
    
    mutex_lock (&epo->lock);
    while (mem_queue_is_empty (&epo->rxq)) 
        mutex_wait (&epo->lock);
    mem_queue_get (&epo->rxq, &q_item);
    if (epo->state == EP_STATE_NACK) {
        epo->state = EP_STATE_WAIT_OUT;
        u->hal->ep_wait_out (ep_n);
    }
    mutex_unlock (&epo->lock);
    
    min_sz = mem_size (q_item);
    if (size < min_sz) min_sz = size;
    memcpy (data, q_item, min_sz);
    mem_free (q_item);

    return min_sz;
}

