#include <usb/usbdev.h>
#include "usbdevhal.h"

#define EP_DIR_IN   0
#define EP_DIR_OUT  1

static usbdev_t *usbdev;
static mutex_t *io_lock;
static mem_pool_t *mem;

#define EVEN 0
#define ODD  1
static uint8_t ep_data[USBDEV_EP0_MAX_SIZE];
static ep_state_t ep_state [USBDEV_MAX_EP_NB];

ARRAY (io_stack, 1500);

#if 0
static void dump (const void *addr, int size)
{
    const unsigned char *p = addr;
    int i;
    for (i = 0; i < size; ++i)
        debug_printf ("%02X ", p[i]);
    debug_printf ("\n");
}
#endif

static void mldr_set_addr (unsigned addr)
{
    ARM_USB->SA = addr;
//debug_printf ("set_addr: %d\n", ARM_USB->SA);
}

static void mldr_ep_attr (unsigned ep, int dir, unsigned attr, int max_size, int interval)
{
    ep_state_t *eps = &ep_state[ep];

//debug_printf ("mldr_ep_attr, ep = %d, dir = %d, attr = 0x%X, max_size = %d\n", ep, dir, attr, max_size);
    switch (attr & EP_ATTR_TRANSFER_MASK) {
    case EP_ATTR_CONTROL:

    break;
    case EP_ATTR_ISOCH:
        // TODO
    break;
    case EP_ATTR_BULK:
    case EP_ATTR_INTR:
        if (dir == USBDEV_DIR_IN) {
            eps->max_size[USBDEV_DIR_IN] = max_size;
            ARM_USB->SEPS[ep].CTRL = ARM_USB_EPEN | ARM_USB_EPRDY | ARM_USB_EPDATASEQ;
        } else {
            eps->max_size[USBDEV_DIR_OUT] = max_size;
            ARM_USB->SEPS[ep].CTRL = ARM_USB_EPEN | ARM_USB_EPRDY | ARM_USB_EPDATASEQ;
        }
    break;
    }
}

static void mldr_ep_wait_out (unsigned ep, int ack)
{
//debug_printf ("ep_wait_out, ep = %d\n", ep);
    ARM_USB->SEPS[ep].CTRL = ARM_USB_EPEN | ARM_USB_EPRDY;
}

static void mldr_ep_wait_in (unsigned ep, int pid, const void *data, int size, int last)
{
//debug_printf ("ep_wait_in, ep = %d, pid = %d, data @ %p, size = %d\n", ep, pid, data, size);
    const unsigned char *p = data;
    ep_state[ep].busy = 1;
    ep_state[ep].last_in_bytes = size;
    ARM_USB->SEPF[ep].TXFC = 1;
    while (size--)
        ARM_USB->SEPF[ep].TXFD = *p++;
    ARM_USB->SEPS[ep].CTRL = ARM_USB_EPEN | ARM_USB_EPRDY | 
        ((pid == PID_DATA1) ? ARM_USB_EPDATASEQ : 0);
//debug_printf ("EP%d CTRL = %X\n", ep, ARM_USB->SEPS[ep].CTRL);
}

static void mldr_ep_stall (unsigned ep, int dir)
{
//debug_printf ("STALL EP%d\n", ep);
    ARM_USB->SEPF[ep].TXFC = 1;
    ARM_USB->SEPS[ep].CTRL |= ARM_USB_EPRDY | ARM_USB_EPSSTALL;
}

static int mldr_in_avail_bytes (unsigned ep)
{
    if (ep_state[ep].busy)
        return 0;
    else
        return ep_state[ep].max_size[EP_DIR_OUT];
}

static usbdev_hal_t hal = {
    .set_addr       = mldr_set_addr,
    .ep_attr        = mldr_ep_attr,
    .ep_wait_out    = mldr_ep_wait_out,
    .ep_wait_in     = mldr_ep_wait_in,
    .ep_stall       = mldr_ep_stall,
    .in_avail       = mldr_in_avail_bytes
};

static void mldr_usb_reset ()
{
    int i;
//debug_printf ("mldr_usb_reset, ctrl_failed = %d\n", usbdev->ctrl_failed);

    memset (ep_state, 0, sizeof (ep_state));
#ifdef MILANDR_USB_LOW_SPEED
    ARM_USB->SC = ARM_USB_SCGEN;
#else
    ARM_USB->SC = ARM_USB_SCFSR | ARM_USB_SCFSP | ARM_USB_SCGEN;
#endif
    ARM_USB->SA = 0;
    ep_state[0].max_size[EP_DIR_IN] = ep_state[0].max_size[EP_DIR_OUT] = USBDEV_EP0_MAX_SIZE;
    for (i = 0; i < USBDEV_MAX_EP_NB; ++i) {
        ARM_USB->SEPS[i].CTRL = 0;
        ARM_USB->SEPF[i].TXFC = 1;
        ARM_USB->SEPF[i].RXFC = 1;
    }
    ARM_USB->SEPS[0].CTRL = ARM_USB_EPEN | ARM_USB_EPRDY;
    ARM_USB->SIM = ARM_USB_SC_TDONE /*| ARM_USB_SC_RESUME*/ | ARM_USB_SC_RESET_EV /*| ARM_USB_SC_SOF_REC*/ | ARM_USB_SC_NAK_SENT;
    ARM_USB->HSCR |= ARM_USB_EN_TX | ARM_USB_EN_RX;
}

static void do_usbdev (int ep)
{
    static uint8_t *p;
    static int size;
    
    ep_state[ep].busy = 0;
    if (ARM_USB->SEPS[ep].TS == ARM_USB_IN_TRANS) {
        ARM_USB->SEPS[ep].STS = 0;
        ARM_USB->SEPF[ep].TXFC = 1;
        usbdevhal_in_done (usbdev, ep, ep_state[ep].last_in_bytes);
    } else {
        size = ARM_USB->SEPF[ep].RXFDC_H;
        p = ep_data;
        while (ARM_USB->SEPF[ep].RXFDC_H)
            *p++ = ARM_USB->SEPF[ep].RXFD;
        ARM_USB->SEPF[ep].RXFC = 1;
        if (ARM_USB->SEPS[ep].TS == ARM_USB_SETUP_TRANS) {
            usbdevhal_out_done (usbdev, ep, USBDEV_TRANSACTION_SETUP, ep_data, size);
        } else {
            usbdevhal_out_done (usbdev, ep, USBDEV_TRANSACTION_OUT, ep_data, size);
        }
    }
}

static void usb_interrupt (void *arg)
{
    static int ep;
    
    mutex_lock_irq (io_lock, USB_IRQn, 0, 0);

    for (;;) {
        mutex_wait (io_lock);
        
        if (ARM_USB->SIS & ARM_USB->SIM & ARM_USB_SC_RESET_EV) {
//debug_printf ("usb_interrupt: device reset\n");
            ARM_USB->SIS = ARM_USB_SC_RESET_EV;
            ARM_USB->SIM &= ~ARM_USB_SC_RESET_EV;
            usbdevhal_reset (usbdev);
            mldr_usb_reset ();
        }
        if (ARM_USB->SIS & ARM_USB->SIM & ARM_USB_SC_SOF_REC) {
//debug_printf ("usb_interrupt: start of frame, SIS = %02X, SIM = %02X, SFN = %d\n", ARM_USB->SIS, ARM_USB->SIM, ARM_USB->SFN_L);
            ARM_USB->SIS = ARM_USB_SC_SOF_REC;
        }
        if (ARM_USB->SIS & ARM_USB->SIM & ARM_USB_SC_TDONE) {
//debug_printf ("usb_interrupt: tx done, RXFDC = %d, CTRL = %X, STS = %X, TS = %X\n", ARM_USB->SEPF[0].RXFDC_H, ARM_USB->SEPS[0].CTRL, ARM_USB->SEPS[0].STS, ARM_USB->SEPS[0].TS);
            for (ep = 0; ep < USBDEV_MAX_EP_NB; ++ep) {
                if (ARM_USB->SEPS[ep].STS & ARM_USB_SC_STALL_SENT) {
                    ARM_USB->SEPS[ep].STS &= ~ARM_USB_SC_STALL_SENT;
//debug_printf ("STALL sent\n");
                    ARM_USB->SEPS[ep].CTRL = (ARM_USB->SEPS[ep].CTRL & ~ARM_USB_EPSSTALL) | ARM_USB_EPRDY;
                } else if ((ARM_USB->SEPS[ep].CTRL & (ARM_USB_EPEN | ARM_USB_EPRDY)) == ARM_USB_EPEN) {
                    do_usbdev (ep);
                }
            }
            ARM_USB->SIS = ARM_USB_SC_TDONE;
        }
        if (ARM_USB->SIS & ARM_USB->SIM & ARM_USB_SC_RESUME) {
//debug_printf ("usb_interrupt: resume\n");
            ARM_USB->SIS = ARM_USB_SC_RESUME;
        }
        if (ARM_USB->SIS & ARM_USB->SIM & ARM_USB_SC_NAK_SENT) {
            for (ep = 0; ep < USBDEV_MAX_EP_NB; ++ep) {
                if ((ARM_USB->SEPS[ep].CTRL & (ARM_USB_EPEN | ARM_USB_EPRDY)) == ARM_USB_EPEN) {
//debug_printf ("NAK sent, EP%d CTRL = %x, STS = %x\n", ep, ARM_USB->SEPS[ep].CTRL, ARM_USB->SEPS[ep].STS);
                    if (ARM_USB->SEPS[ep].STS & ARM_USB_SC_ACK_RXED ||
                        ARM_USB->SEPS[ep].STS == 0x90)
                        do_usbdev (ep);
                        
                    if (ARM_USB->SEPS[ep].CTRL & ARM_USB_EPSSTALL)
                        //ARM_USB->SEPS[ep].CTRL |= ARM_USB_EPRDY;
                        ARM_USB->SEPS[ep].CTRL = (ARM_USB->SEPS[ep].CTRL & ~ARM_USB_EPSSTALL) | ARM_USB_EPRDY;
                }
            }
            ARM_USB->SIS = ARM_USB_SC_NAK_SENT;
        }
    }
}

void mldr_usbdev_init (usbdev_t *owner, int io_prio, mem_pool_t *pool, mutex_t *m)
{  
    usbdev = owner;
    mem = pool;
    io_lock = m;
    
    usbdevhal_bind (usbdev, &hal, m);

    //power on the module
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_USB;
    ARM_RSTCLK->USB_CLOCK = ARM_USB_C1_SEL(ARM_USB_HSE_DIV2) | 
        ARM_USB_C2_SEL(ARM_USB_PLLUSBo) | ARM_USB_C3_SEL(ARM_USB_C2) | 
        ARM_USB_CLK_EN;
    ARM_RSTCLK->PLL_CONTROL |= ARM_PLL_CONTROL_USB_ON | ARM_PLL_CONTROL_USB_MUL(48000/KHZ_CLKIN*2);
    while (! (ARM_RSTCLK->CLOCK_STATUS & ARM_CLOCK_STATUS_PLL_USB_RDY));
    
    ARM_USB->HSCR |= ARM_USB_RESET_CORE;
    volatile unsigned cnt;
    for (cnt = 0; cnt < 1000; cnt++);
    
#ifdef MILANDR_USB_LOW_SPEED
    ARM_USB->HSCR = ARM_USB_D_MINUS_PULLUP;
#else
    ARM_USB->HSCR = ARM_USB_D_PLUS_PULLUP;
#endif

    mldr_usb_reset ();

    task_create (usb_interrupt, 0, "usb_intr", io_prio, io_stack, sizeof (io_stack));
}
