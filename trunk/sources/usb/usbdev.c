#include "usbdev.h"

//
// Состояния конечных точек
// Endpoint states
//
#define EP_STATE_IDLE               0x01
#define EP_STATE_CTRL_IN            0x02
#define EP_STATE_CTRL_OUT           0x04
#define EP_STATE_CTRL_ADDR          0x08
#define EP_STATE_IN                 0x10
//#define EP_STATE_OUT                0x20

#define MIN(x,y)    (((x) < (y)) ? (x) : (y))

static void set_configuration (usbdev_t *u, int conf_num)
{
    uint8_t *pchar = (uint8_t *) u->conf_desc [conf_num - 1];
    pchar += sizeof (usb_conf_desc_t);
    usb_iface_desc_t *pif = (usb_iface_desc_t *) pchar;
    pchar += sizeof (usb_iface_desc_t);
    usb_ep_desc_t *pep = (usb_ep_desc_t *) pchar;
    while (pep->bDescriptorType != USB_DESC_TYPE_ENDPOINT) {
        pchar += pep->bLength;
        pep = (usb_ep_desc_t *) pchar;
    }
    
    int i;
    for (i = 0; i < pif->bNumEndpoints; ++i) {
        unsigned ep = pep->bEndpointAddress & 0x7F;
        int dir = pep->bEndpointAddress >> 7;
        u->ep_ctrl[ep].attr[dir] = pep->bmAttributes;
        u->ep_ctrl[ep].max_size[dir] = pep->wMaxPacketSize;
        u->ep_ctrl[ep].interval[dir] = pep->bInterval;
        u->ep_ctrl[ep].state = EP_STATE_IDLE;
        if (dir == USBDEV_DIR_IN)
            mem_queue_init (&u->ep_ctrl[ep].rxq, u->pool, u->ep_ctrl[ep].rxq_depth);
        u->hal->ep_attr (ep, dir, pep->bmAttributes, pep->wMaxPacketSize, pep->bInterval);
        pchar += sizeof (usb_ep_desc_t);
        pep = (usb_ep_desc_t *) pchar;
    }
    u->cur_conf = conf_num;
}

static void do_tx (usbdev_t *u, unsigned ep)
{
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
    int in_sz = (epc->in_rest_load < epc->max_size[USBDEV_DIR_IN]) ? 
        (epc->in_rest_load) : (epc->max_size[USBDEV_DIR_IN]);

    u->hal->tx (ep, epc->pid, epc->in_ptr, in_sz);
    epc->in_ptr += in_sz;
    epc->in_rest_load -= in_sz;
    if (epc->in_rest_load == 0 && in_sz != epc->max_size[USBDEV_DIR_IN])
        epc->in_rest_load = -1;
    if (epc->pid == PID_DATA1) epc->pid = PID_DATA0;
    else epc->pid = PID_DATA1;
}

static void start_tx (usbdev_t *u, unsigned ep, int start_pid, const void *data, int size)
{
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
    epc->in_ptr = (uint8_t *) data;
    epc->in_rest_load = size;
    epc->in_rest_ack = size;
    if (start_pid != 0)
        epc->pid = start_pid;
    while (u->hal->tx_avail (ep) > 0 && epc->in_rest_load >= 0)
        do_tx (u, ep);
}

static void process_std_req (usbdev_t *u, unsigned ep, usb_setup_pkt_t *setup)
{
    switch (setup->bRequest) {
    case USB_SR_GET_STATUS:
        //u->hal->tx (ep, u->conf_desc [setup.wValue & 0xFF], setup.wLength);
    break;
        // Доделать все стандартные типы запросов
    case USB_SR_SET_ADDRESS:
//debug_printf ("USB_SR_SET_ADDRESS\n");
        u->ep_ctrl[ep].state = EP_STATE_CTRL_ADDR;
        u->usb_addr = setup->wValue;
        start_tx (u, ep, PID_DATA1, 0, 0);
    break;
    case USB_SR_GET_DESCRIPTOR:
//debug_printf ("descriptor type: %d\n", setup->wValue >> 8);
        switch (setup->wValue >> 8) {
            // Доделать все необходимые типы дескрипторов
        case USB_DESC_TYPE_DEVICE:
//debug_printf ("USB_DESC_TYPE_DEVICE, wLength = %d\n", setup->wLength);
            u->ep_ctrl[ep].state = EP_STATE_CTRL_IN;
            start_tx (u, ep, PID_DATA1, u->dev_desc, sizeof(usb_dev_desc_t));
        break;
        case USB_DESC_TYPE_CONFIG:
        {
            int conf_n = setup->wValue & 0xFF;
//debug_printf ("USB_DESC_TYPE_CONFIG, wLength = %d, wValue = %X\n", setup->wLength, conf_n);
            if (conf_n >= USBDEV_NB_CONF)
                u->hal->stall (ep, USBDEV_DIR_IN);
            u->ep_ctrl[ep].state = EP_STATE_CTRL_IN;
            usb_conf_desc_t *conf = u->conf_desc[conf_n];
            start_tx (u, ep, PID_DATA1, conf, MIN(setup->wLength, conf->wTotalLength));
        }
        break;
        case USB_DESC_TYPE_STRING:
//debug_printf ("USB_DESC_TYPE_STRING, wLength = %d, wValue = %04X\n", setup->wLength, setup->wValue);
            u->ep_ctrl[ep].state = EP_STATE_CTRL_IN;
            unsigned char *plen = (unsigned char *) u->strings[setup->wValue & 0xFF];
            start_tx (u, ep, PID_DATA1, u->strings[setup->wValue & 0xFF], MIN(setup->wLength, *plen));
        break;
        default:
            u->hal->stall (ep, USBDEV_DIR_IN);
        }
    break;
    case USB_SR_GET_CONFIGURATION:
//debug_printf ("USB_SR_GET_CONFIGURATION, wLength = %d\n", setup->wLength);
        if (setup->wLength != 1) {
            u->rx_bad_req++;
            u->hal->stall (ep, USBDEV_DIR_IN);
        }
        else start_tx (u, ep, PID_DATA1, &u->cur_conf, 1);
    break;
    case USB_SR_SET_CONFIGURATION:
//debug_printf ("USB_SR_SET_CONFIGURATION, wValue = %X\n", setup->wValue);
        if (setup->wValue > USBDEV_NB_CONF) {
            u->rx_bad_req++;
            u->hal->stall (ep, USBDEV_DIR_IN);
        } else {
            set_configuration (u, setup->wValue);
            start_tx (u, ep, PID_DATA1, 0, 0);
        }      
    break;
    }
}

static void process_setup (usbdev_t *u, unsigned ep, void *data)
{
    usb_setup_pkt_t *setup = data;
    unsigned char *res_data;
    int res_size;
    unsigned num;
    
    u->rx_req++;
    
//debug_printf ("process_setup, ep = %d, bmRequestType = %02X, bRequest = %02X\n", 
//    ep, setup->bmRequestType, setup->bRequest);

    switch (USB_REQ_GET_RCV(setup->bmRequestType)) {
    case USB_REQ_RCV_DEVICE:
        if (USB_REQ_GET_TYPE(setup->bmRequestType) == USB_REQ_TYPE_STANDARD) {
            process_std_req (u, ep, setup);
            return;
        } else {
            if (u->dev_specific_handler)
                u->dev_specific_handler (u->dev_specific_tag, setup, &res_data, &res_size);
        }
        break;
    case USB_REQ_RCV_IFACE:
        num = setup->wIndex & 0xFF;
        if (num >= USBDEV_NB_INTERFACES) break;
        if (u->iface_ctrl[num].specific_handler)
            u->iface_ctrl[num].specific_handler (
                u->iface_ctrl[num].specific_tag, setup, &res_data, &res_size);
        break;
    case USB_REQ_RCV_ENDPOINT:
        num = setup->wIndex & 0xF;
        if (num >= USBDEV_NB_ENDPOINTS) break;
        if (setup->wIndex & EP_ATTR_IN) {
            if (u->ep_ctrl[num].in_specific_handler)
                u->ep_ctrl[num].in_specific_handler (
                    u->ep_ctrl[num].in_specific_tag, setup, &res_data, &res_size);
        } else {
            if (u->ep_ctrl[num].out_specific_handler)
                u->ep_ctrl[num].out_specific_handler (
                    u->ep_ctrl[num].out_specific_tag, setup, &res_data, &res_size);
        }
    }
    
    if (res_size >= 0)
        start_tx (u, ep, PID_DATA1, res_data, res_size);
    else
        u->hal->stall (ep, USBDEV_DIR_IN);
}

void usbdevhal_bind (usbdev_t *u, usbdev_hal_t *hal)
{
    assert (hal);
    assert (hal->set_addr);
    assert (hal->ep_attr);
    assert (hal->tx);
    assert (hal->tx_avail);
    assert (hal->stall);
    
    u->hal = hal;
    //u->hal->ep_attr (0, EP_ATTR_CONTROL, 8);
}

void usbdevhal_reset (usbdev_t *u)
{
    int i;
    for (i = 0; i < USBDEV_NB_ENDPOINTS; ++i) {
        u->ep_ctrl[i].state = EP_STATE_IDLE;
        u->ep_ctrl[i].pid = 0;
        mem_queue_free (&u->ep_ctrl[i].rxq);
    }
}

void usbdevhal_suspend (usbdev_t *u)
{
}

void usbdevhal_tx_done (usbdev_t *u, unsigned ep, int size)
{
//debug_printf ("usbdevhal_tx_done, ep = %d, size = %d\n", ep, size);
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
    epc->in_rest_ack -= size;
    if (epc->in_rest_load >= 0) 
        do_tx (u, ep);
    if (epc->in_rest_ack == 0) {
        if (epc->state == EP_STATE_CTRL_IN)
            u->ctrl_failed++;
        else if (epc->state == EP_STATE_CTRL_ADDR)
            u->hal->set_addr (u->usb_addr);
        else {
            epc->state = EP_STATE_IDLE;
            mutex_signal (&epc->lock, (void *) USBDEV_DIR_IN);
        }
    }
}

void usbdevhal_rx_done (usbdev_t *u, unsigned ep, int pid, void *data, int size)
{
//debug_printf ("usbdevhal_rx_done, ep = %d, pid = %04x, size = %d\n", ep, pid, size);
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
    
    switch (pid) {
    case PID_SETUP:
        if ((epc->attr[USBDEV_DIR_OUT] & EP_ATTR_TRANSFER_MASK) == EP_ATTR_CONTROL) {
            if (size == 8) process_setup (u, ep, data);
            else u->rx_bad_len++;
        }
        else u->rx_bad_pid++;
    break;
    case PID_OUT:
        if (epc->state == EP_STATE_CTRL_IN) {
            if (size == 0) epc->state = EP_STATE_IDLE;
            else u->rx_bad_len++;
        } else {
            if (mem_queue_is_full (&epc->rxq)) {
                u->rx_discards++;
                break;
            } else {
                void *buf = mem_alloc_dirty (u->pool, size);
                if (! buf) {
                    u->out_of_memory++;
                    u->rx_discards++;
                    break;
                }
                memcpy (buf, data, size);
                mem_queue_put (&epc->rxq, buf);
                mutex_signal (&epc->lock, (void *) USBDEV_DIR_OUT);
            }
        }
    break;
    default:
        u->rx_bad_pid++;
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
        u->ep_ctrl[i].rxq_depth = USBDEV_DEFAULT_RXQ_DEPTH;
    u->ep_ctrl[0].max_size[USBDEV_DIR_IN] = u->ep_ctrl[0].max_size[USBDEV_DIR_OUT] = USBDEV_EP0_MAX_SIZE;
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
    if (dir == USBDEV_DIR_IN) {
        u->ep_ctrl[ep_n].in_specific_handler = handler;
        u->ep_ctrl[ep_n].in_specific_tag     = tag;
    } else {
        u->ep_ctrl[ep_n].out_specific_handler = handler;
        u->ep_ctrl[ep_n].out_specific_tag     = tag;
    }
}

void usbdev_set_rx_queue_depth (usbdev_t *u, unsigned ep, int depth)
{
    u->ep_ctrl[ep].rxq_depth = depth;
}

void usbdev_send (usbdev_t *u, unsigned ep, const void *data, int size)
{
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
//debug_printf ("usbdev_send, epc->state = %d, tx_avail = %d\n", epc->state, u->hal->tx_avail(ep));
    mutex_lock (&epc->lock);
    while (epc->state != EP_STATE_IDLE || u->hal->tx_avail(ep) == 0)
        mutex_wait (&epc->lock);
    epc->state = EP_STATE_IN;
    char *d = (char *) data;
debug_printf ("before start_tx, data = %d %d %d\n", d[0], d[1], d[2]);
    start_tx (u, ep, 0, data, size);
    //while (epc->state != EP_STATE_IDLE)
    //    mutex_wait (&epc->lock);
    mutex_unlock (&epc->lock);
}

int usbdev_recv (usbdev_t *u, unsigned ep, void *data, int size)
{
    void *q_item = 0;
    int min_sz;
    ep_ctrl_t *epc = &u->ep_ctrl[ep];
    
    mutex_lock (&epc->lock);
    while (mem_queue_is_empty (&epc->rxq)) 
        mutex_wait (&epc->lock);
    mem_queue_get (&epc->rxq, &q_item);
    mutex_unlock (&epc->lock);
    
    min_sz = mem_size (q_item);
    if (size < min_sz) min_sz = size;
    memcpy (data, q_item, min_sz);
    mem_free (q_item);

    return min_sz;
}

