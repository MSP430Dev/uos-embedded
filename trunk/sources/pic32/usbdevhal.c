#include <usb/usbdev.h>
#include "usbdevhal.h"

#define EP_DIR_IN   0
#define EP_DIR_OUT  1

static usbdev_t *usbdev;
static volatile ep_bd_t bdt [USBDEV_MAX_EP_NB] __attribute__ ((aligned (512)));
static mutex_t *io_lock;
static ep_state_t ep_state [16];
static mem_pool_t *mem;

#define EVEN 0
#define ODD  1
static volatile uint8_t ep0_buf[4][USBDEV_EP0_MAX_SIZE];

ARRAY (io_stack, 1000);

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

static void pic32_set_addr (unsigned addr)
{
    U1ADDR = addr;
}

static void pic32_ep_attr (unsigned ep, int dir, unsigned attr, int max_size, int interval)
{
    ep_state_t *eps = &ep_state[ep];
    
debug_printf ("pic32_ep_attr, ep = %d, attr = 0x%X, max_size = %d\n", ep, attr, max_size);
    switch (attr & EP_ATTR_TRANSFER_MASK) {
    case EP_ATTR_CONTROL:
    
#if 0
        // TODO: if the endpoint was used before we'd better free allocated buffer correctly
        memset ((void *)&bdt[ep], 0, sizeof (bdt[ep]));
        bdt[ep].rx_even.address = mips_virtual_addr_to_physical ((unsigned) usbdevhal_alloc_buffer (usbdev, max_size));
        //bdt[ep].rx_odd.address  = mips_virtual_addr_to_physical ((unsigned) usbdevhal_alloc_buffer (max_size));
        bdt[ep].tx_even.address = mips_virtual_addr_to_physical ((unsigned) usbdevhal_alloc_buffer (usbdev, max_size));
        //bdt[ep].tx_odd.address  = mips_virtual_addr_to_physical ((unsigned) usbdevhal_alloc_buffer (max_size));
        bdt[ep].tx_even.status = PIC32_DB_SET_COUNT(max_size) | PIC32_DB_UOWN | PIC32_DB_DTS/* | PIC32_DB_BSTALL*/;
#endif

    break;
    case EP_ATTR_ISOCH:
        // TODO
    break;
    case EP_ATTR_BULK:
        // TODO
    break;
    case EP_ATTR_INTR:
        if (dir == USBDEV_DIR_IN) {
            eps->cur_inbuf = 0;
            eps->max_size[USBDEV_DIR_IN] = max_size;
            //if (bdt[ep].in[0].address) mem_free ((void *) bdt[ep].in[0].address);
            //if (bdt[ep].in[1].address) mem_free ((void *) bdt[ep].in[1].address);
            bdt[ep].in[0].address = (unsigned) mem_alloc_dirty (mem, max_size);
            assert (bdt[ep].in[0].address);
            bdt[ep].in[1].address = (unsigned) mem_alloc_dirty (mem, max_size);
            assert (bdt[ep].in[1].address);
            U1EP(ep) |= PIC32_U1EP_EPRXEN | PIC32_U1EP_EPTXEN | PIC32_U1EP_EPHSHK;
        } else {
            eps->cur_outbuf = 0;
            eps->max_size[USBDEV_DIR_OUT] = max_size;
            //if (bdt[ep].out[0].address) mem_free ((void *) bdt[ep].out[0].address);
            //if (bdt[ep].out[1].address) mem_free ((void *) bdt[ep].out[1].address);
            bdt[ep].out[0].address = (unsigned) mem_alloc_dirty (mem, max_size);
            assert (bdt[ep].out[0].address);
            bdt[ep].out[1].address = (unsigned) mem_alloc_dirty (mem, max_size);
            assert (bdt[ep].out[1].address);
            bdt[ep].out[0].status = PIC32_DB_SET_COUNT(USBDEV_EP0_MAX_SIZE) | PIC32_DB_UOWN;
            bdt[ep].out[1].status = PIC32_DB_SET_COUNT(USBDEV_EP0_MAX_SIZE) | PIC32_DB_UOWN;
            U1EP(ep) |= PIC32_U1EP_EPRXEN | PIC32_U1EP_EPTXEN | PIC32_U1EP_EPHSHK;
        }
    break;
    }
}

static void pic32_tx (unsigned ep, int pid, const void *data, int size)
{
debug_printf ("pic32_tx (%d, %X, %08X, %d)\n", ep, pid, data, size);
    ep_state_t *eps = &ep_state[ep];
    bdt[ep].in[eps->cur_inbuf].address = mips_virtual_addr_to_physical ((unsigned) data);
    unsigned status = PIC32_DB_SET_COUNT(size) | PIC32_DB_DTS | PIC32_DB_UOWN;
    if (pid == PID_DATA1) status |= PIC32_DB_DATA1;
    bdt[ep].in[eps->cur_inbuf].status = status;
    ep_state[ep].in_busy[eps->cur_inbuf] = 1;
    eps->cur_inbuf = !eps->cur_inbuf;
}

static int pic32_tx_avail_bytes (unsigned ep)
{
    ep_state_t *eps = &ep_state[ep];
    if (!eps->in_busy[0] || !eps->in_busy[1]) 
        return eps->max_size[USBDEV_DIR_IN];
//debug_printf ("pic32_tx_avail_bytes returns %d\n", avail_bytes);
    return 0;
}

static void pic32_stall (unsigned ep, int dir)
{
    if (dir == USBDEV_DIR_IN)
        bdt[ep].in[ep_state[ep].cur_inbuf].status = PIC32_DB_BSTALL | PIC32_DB_UOWN;
    //bdt[ep].out[endpoint[ep][EP_DIR_OUT].cur_outbuf].status = PIC32_DB_BSTALL | PIC32_DB_DTS | PIC32_DB_UOWN;
}

static usbdev_hal_t hal = {
    .set_addr   = pic32_set_addr,
    .ep_attr    = pic32_ep_attr,
    .tx         = pic32_tx,
    .tx_avail   = pic32_tx_avail_bytes,
    .stall      = pic32_stall
};

static void pic32_usb_reset ()
{
debug_printf ("pic32_usb_reset\n");
    int i;
    
    U1EIR = 0xFF;           // Clear all errors
    U1IR  = 0xFF;           
    
    //U1EIE = 0x9F;                 // Unmask all USB error interrupts
    //U1IE = 0xFB;
    
    // Reset all of the Ping Pong buffers
    U1CON |= PIC32_U1CON_PPBRST;
    U1CON &= ~PIC32_U1CON_PPBRST;
    U1ADDR = 0; // Reset to default address
    // Clear all of the endpoint control registers
    for (i = 0; i < USBDEV_MAX_EP_NB; ++i) U1EP(i) = 0;
    for (i = 1; i < USBDEV_MAX_EP_NB; ++i) {
        mem_free ((void *) bdt[i].in[0].address);
        mem_free ((void *) bdt[i].in[1].address);
        mem_free ((void *) bdt[i].out[0].address);
        mem_free ((void *) bdt[i].out[1].address);
    }
    //Clear all of the BDT entries
    memset ((void *)bdt, 0, sizeof (bdt));
        
    // Flush any pending transactions
    while (U1IR & PIC32_U1I_TRN) {
        (void) U1STAT;
        U1IR = PIC32_U1I_TRN;
    }
    
    U1IE = PIC32_U1I_URST |	// Unmask RESET interrupt
           PIC32_U1I_IDLE | // Unmask IDLE interrupt
           PIC32_U1I_TRN;	
           
    // Make sure packet processing is enabled
    U1CON &= ~PIC32_U1CON_PKTDIS;

    U1CNFG1 = 0;
    
	memset (ep_state, 0, sizeof (ep_state));
    ep_state[0].max_size[USBDEV_DIR_IN] = ep_state[0].max_size[USBDEV_DIR_OUT] = USBDEV_EP0_MAX_SIZE;    
    
    bdt[0].out[0].address = mips_virtual_addr_to_physical ((unsigned) &ep0_buf[EVEN]);
    bdt[0].out[1].address = mips_virtual_addr_to_physical ((unsigned) &ep0_buf[ODD]);
    bdt[0].out[0].status = PIC32_DB_SET_COUNT(USBDEV_EP0_MAX_SIZE) | PIC32_DB_UOWN;
    bdt[0].out[1].status = PIC32_DB_SET_COUNT(USBDEV_EP0_MAX_SIZE) | PIC32_DB_UOWN;
    bdt[0].in[0].address = 0;
    bdt[0].in[1].address = 0;
    bdt[0].in[0].status = 0;
    bdt[0].in[1].status = 0;
    U1EP(0) = PIC32_U1EP_EPRXEN | PIC32_U1EP_EPTXEN | PIC32_U1EP_EPHSHK;    
}

static void usb_interrupt (void *arg)
{
//debug_printf ("usb_interrupt task started!\n");
    mutex_lock_irq (io_lock, PIC32_VECT_USB, 0, 0);
    
    for (;;) {
        mutex_wait (io_lock);
//debug_printf ("usb_interrupt, U1IR = %02X, U1EIR = %02X, U1OTGIR = %02X!\n", U1IR, U1EIR, U1OTGIR);
        if (U1IR & U1IE & PIC32_U1I_URST) {
//debug_printf ("usb_interrupt: device reset\n");
            U1IR = PIC32_U1I_URST;
            usbdevhal_reset (usbdev);
            pic32_usb_reset ();            
        }
        if (U1IR & PIC32_U1I_IDLE) {
//debug_printf ("usb_interrupt: bus idle\n");
            U1IR = PIC32_U1I_IDLE;
            usbdevhal_suspend (usbdev);
        }
        if (U1IR & PIC32_U1I_SOF) {
//debug_printf ("usb_interrupt: start of frame\n");
            U1IR = PIC32_U1I_SOF;
        }
        while (U1IR & PIC32_U1I_TRN) {
//debug_printf ("usb_interrupt: token interrupt\n");
            unsigned pid;
            unsigned bytes_done;
            unsigned u1stat = U1STAT;
            U1IR = PIC32_U1I_TRN;
//debug_printf ("U1STAT = %08X, U1CON = %08X\n", u1stat, U1CON);
            int curbuf;
            unsigned ep = PIC32_U1STAT_ENDPT(u1stat);
            if (u1stat & PIC32_U1STAT_TX) {
u1stat_error: 
                if (u1stat & PIC32_U1STAT_PPBI) {
//debug_printf ("last in was odd\n");
                    pid = PIC32_DB_GET_PID(bdt[ep].in[1].status);
                    
                    bytes_done = PIC32_DB_GET_COUNT(bdt[ep].in[1].status);
                    //ep_desc->cur_inbuf = EVEN;
                    ep_state[ep].in_busy[1] = 0;
                } else {
//debug_printf ("last in was even\n");
                    pid = PIC32_DB_GET_PID(bdt[ep].in[0].status);
                    bytes_done = PIC32_DB_GET_COUNT(bdt[ep].in[0].status);
                    //ep_desc->cur_inbuf = ODD;
                    ep_state[ep].in_busy[0] = 0;
                }
                //U1CON &= ~PIC32_U1CON_PKTDIS;
                /*
                curbuf = ep_state[ep].cur_txbuf;
                pid = PIC32_DB_GET_PID(bdt[ep].in[curbuf].status);
                bytes_done = PIC32_DB_GET_COUNT(bdt[ep].in[curbuf].status);
                ep_state[ep].in_busy[curbuf] = 0;
                ep_state[ep].cur_txbuf = !curbuf;
                */
//debug_printf ("IN PID: 0x%X, bytes done: %d\n", pid, bytes_done);
                usbdevhal_tx_done (usbdev, ep, bytes_done);
            } else {
                curbuf = (u1stat & PIC32_U1STAT_PPBI) >> 2;
                pid = PIC32_DB_GET_PID(bdt[ep].out[curbuf].status);
                if (pid == 0) {
debug_printf ("---------------- U1STAT ERROR\n");
                    goto u1stat_error;
                }
//                debug_printf ("OUT PID: 0x%X\n", pid);
                if (PIC32_U1STAT_ENDPT(u1stat) == 0)
                    usbdevhal_rx_done (usbdev, 0, pid, (void *) ep0_buf[curbuf], 
                        PIC32_DB_GET_COUNT(bdt[0].out[curbuf].status));
                else
                    debug_printf ("PACKET FOR ENDPOINT %d - NOT SUPPORTED YET!\n", ep);
                bdt[ep].out[curbuf].status = PIC32_DB_SET_COUNT(USBDEV_EP0_MAX_SIZE) | PIC32_DB_UOWN;
                U1CON &= ~PIC32_U1CON_PKTDIS;
            }
            
        }
    }
}

void pic32_usbdev_init (usbdev_t *owner, int io_prio, mem_pool_t *pool, mutex_t *m)
{
    usbdev = owner;
    mem = pool;
    io_lock = m;
    
    usbdevhal_bind (usbdev, &hal);
    
    //power up the module
    U1PWRC |= PIC32_U1PWRC_USBPWR;    

    U1BDTP1 = (unsigned) bdt >> 8;  //set the address of the BDT
    
    memset ((void *)bdt, 0, sizeof (bdt));
        
    pic32_usb_reset ();
    
	// Disable module & detach from bus
    U1CON = 0;

    // Enable module & attach to bus
    while (! (U1CON & PIC32_U1CON_USBEN)) {
		U1CON |= PIC32_U1CON_USBEN;
	}
	
    task_create (usb_interrupt, 0, "usb_intr", io_prio, io_stack, sizeof (io_stack));
}


