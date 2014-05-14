#include <usb/usbdev.h>
#include <spi/spi-master-interface.h>
#include "usbdevhal.h"
#include "max3421e.h"

static usbdev_t *usbdev;
static mutex_t *io_lock;
static mem_pool_t *mem;
static spimif_t *spim;
static spi_message_t msg;
static int irqn;
static uint8_t spi_buf[65] __attribute__((aligned(8)));
//static int calibrate_fifo0;
static uint8_t epien;

static ep0_state_t ep0_state;
static ep1out_state_t ep1out_state;
static ep2in_state_t ep2in_state;
static ep3in_state_t ep3in_state;

ARRAY (io_stack, 1500);

static uint8_t read_stat()
{
    msg.word_count = 1;
    spi_buf[0] = MAX3421_REG_NUM(FNADDR) | MAX3421_DIR_RD;
    spim_trx(spim, &msg);
    return spi_buf[0];
}

static void ack_stat()
{
//debug_printf("+++++ ACKSTAT\n");
    msg.word_count = 1;
    spi_buf[0] = MAX3421_REG_NUM(FNADDR) | MAX3421_DIR_RD | MAX3421_ACKSTAT;
    spim_trx(spim, &msg);
}

static void write_reg_ack(uint8_t addr, uint8_t value)
{
    msg.word_count = 2;
    spi_buf[0] = MAX3421_REG_NUM(addr) | MAX3421_DIR_WR | MAX3421_ACKSTAT;
    spi_buf[1] = value;
    spim_trx(spim, &msg);
}

static void write_reg(uint8_t addr, uint8_t value)
{
    msg.word_count = 2;
    spi_buf[0] = MAX3421_REG_NUM(addr) | MAX3421_DIR_WR;
    spi_buf[1] = value;
    spim_trx(spim, &msg);
}

static void write_data(uint8_t addr, uint8_t len_plus_1)
{
    msg.word_count = len_plus_1;
    spi_buf[0] = MAX3421_REG_NUM(addr) | MAX3421_DIR_WR;
    spim_trx(spim, &msg);
}

static uint8_t read_reg(uint8_t addr)
{
    msg.word_count = 2;
    spi_buf[0] = MAX3421_REG_NUM(addr) | MAX3421_DIR_RD;
    spim_trx(spim, &msg);
    return spi_buf[1];
}

static uint8_t *read_data(uint8_t addr, uint8_t len_plus_1)
{
    msg.word_count = len_plus_1;
    spi_buf[0] = MAX3421_REG_NUM(addr) | MAX3421_DIR_RD;
    spim_trx(spim, &msg);
    return &spi_buf[1];
}

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

static void set_addr (unsigned addr)
{
//debug_printf ("set_addr: %d\n", read_reg(FNADDR));
}

static void ep_attr (unsigned ep, int dir, unsigned attr, int max_size, int interval)
{
//debug_printf ("ep_attr, ep = %d, dir = %d, attr = 0x%X, max_size = %d\n", ep, dir, attr, max_size);
    switch (attr & EP_ATTR_TRANSFER_MASK) {
    case EP_ATTR_CONTROL:
        assert (0); // Точки типа CONTROL не поддерживаются микросхемой (кроме EP0)
    break;
    case EP_ATTR_ISOCH:
        // TODO
    break;
    case EP_ATTR_BULK:
    case EP_ATTR_INTR:
        if (dir == USBDEV_DIR_IN) {
            assert(ep == 2 || ep == 3); // Только EP2 и EP3 могут иметь направление IN
            if (ep == 2) ep2in_state.max_size = max_size;
            else ep3in_state.max_size = max_size;
        } else {
            assert(ep == 1); // Только EP1 может иметь направление OUT
            ep1out_state.max_size = max_size;
            epien |= OUT1BAVIE;
            write_reg(EPIEN, epien);
        }
    break;
    }
}

static void ep_wait_out (unsigned ep, int ack)
{
//debug_printf ("ep_wait_out, ep = %d, ack = %d\n", ep, ack);
    if (ack) {
        ack_stat();
        ep0_state.out_ack_pending = 1;
    }
    if (ep == 1)
        write_reg(EPIRQ, OUT1BAVIRQ);
}

static void ep_wait_in (unsigned ep, int pid, const void *data, int size, int last)
{
//debug_printf ("ep_wait_in, ep = %d, pid = %d, data @ %p, size = %d\n", ep, pid, data, size);

    if (ep == 0) {
        if (size == 0) {
            ack_stat();
            ep0_state.in_ack_pending = 1;
        } else {
            while (!(read_stat() & IN0BAVIRQ));
            memcpy(&spi_buf[1], data, size);
            write_data(EP0FIFO, size + 1);
            write_reg(EP0BC, size);
            ep0_state.in_transfers = 1;
            ep0_state.in_bytes = size;
            epien |= IN0BAVIE;
            ep0_state.in_packet_num++;
            if (last) {
                ep0_state.last_in_packet_num = ep0_state.in_packet_num;
                ep0_state.in_packet_num = 0;
            }
        }
        return;
    } else if (ep == 2) {
        while (!(read_stat() & IN2BAVIRQ));
        memcpy(&spi_buf[1], data, size);
        write_data(EP2INFIFO, size + 1);
        write_reg(EP2INBC, size);
        ep2in_state.in_bytes[ep2in_state.in_transfers++] = size;
        epien |= IN2BAVIE;
    } else if (ep == 3) {
        while (!(read_stat() & IN3BAVIRQ));
        memcpy(&spi_buf[1], data, size);
        write_data(EP3INFIFO, size + 1);
        write_reg(EP3INBC, size);
        ep3in_state.in_transfers = 1;
        ep3in_state.in_bytes = size;
        epien |= IN3BAVIE;
    } else {
        return;
    }
    write_reg(EPIEN, epien);
}

static void ep_stall (unsigned ep, int dir)
{
//debug_printf ("STALL EP%d\n", ep);
    if (dir == USBDEV_DIR_IN) {
        if (ep == 0)
            write_reg(EPSTALLS, STLSTAT | STLEP0IN | STLEP0OUT);
        if (ep == 2) write_reg(EPSTALLS, STLEP2IN);
        if (ep == 3) write_reg(EPSTALLS, STLEP3IN);
    } else {
        if (ep == 0)
            write_reg(EPSTALLS, STLSTAT | STLEP0IN | STLEP0OUT);
        if (ep == 1) write_reg(EPSTALLS, STLEP1OUT);
    }
}

static int in_avail_bytes (unsigned ep)
{
    int res = 0;
    if (ep == 0) {
        if (ep0_state.in_transfers < 1)
            res = ep0_state.max_size;
    } else if (ep == 2) {
        if (ep2in_state.in_transfers < 2)
            res = ep2in_state.max_size;
    } else if (ep == 3) {
        if (ep3in_state.in_transfers < 1)
            res = ep3in_state.max_size;
    }
//debug_printf ("in_avail_bytes EP%d returns %d\n", ep, res);
    return res;
}

static usbdev_hal_t hal = {
    .set_addr       = set_addr,
    .ep_attr        = ep_attr,
    .ep_wait_out    = ep_wait_out,
    .ep_wait_in     = ep_wait_in,
    .ep_stall       = ep_stall,
    .in_avail       = in_avail_bytes
};

static void usb_bus_reset ()
{
    ep0_state.max_size = USBDEV_EP0_MAX_SIZE;
    ep0_state.in_transfers = 0;
    ep2in_state.in_transfers = 0;
    ep3in_state.in_transfers = 0;
    
    //URESDNIE doesn't change its value on USB reset
    epien = SUDAVIE | OUT0BAVIE;
    write_reg(USBIEN, URESDNIE | NOVBUSIE);
    write_reg(EPIEN,  epien);
    //calibrate_fifo0 = 1;
    write_data(EP0FIFO, USBDEV_EP0_MAX_SIZE + 1);
}

static void chip_reset ()
{
    write_reg(USBCTL, CHIPRES);
    write_reg(USBCTL, 0);
    while (!(read_reg(USBIRQ) & OSCOKIRQ));
    
    write_reg(USBIEN, URESDNIE | NOVBUSIE);
    write_reg(CPUCTL, IE);
    write_reg(USBCTL, VBGATE | CONNECT);
}

static void usb_interrupt (void *arg)
{
    uint8_t usbirq;
    uint8_t epirq;
    
    write_reg(PINCTL, FDUPSPI);
    chip_reset();
       
    mutex_lock_irq (io_lock, irqn, 0, 0);

    for (;;) {
        mutex_wait (io_lock);

#ifdef ELVEES
        MC_IRQM |= MC_IRQM_NULL(irqn);
#endif

//debug_printf("usb_interrupt: ");

        read_data(EPIRQ, 4);
        epirq = spi_buf[1];
        usbirq = spi_buf[3];
        
//debug_printf("epirq: %02X epstalls: %02X ", epirq, read_reg(EPSTALLS));

        if (usbirq & URESDNIRQ) {
            write_reg(USBIRQ, URESDNIRQ);
//debug_printf("USB bus reset done\n");
            usbdevhal_reset(usbdev);
            usb_bus_reset();
        }
        
        if (usbirq & NOVBUSIRQ) {
            write_reg(USBIRQ, NOVBUSIRQ);
            chip_reset();
        }
        
        if (epirq & epien & SUDAVIRQ) {
//debug_printf("SETUP data\n");
            if (ep0_state.in_ack_pending) {
                usbdevhal_in_done (usbdev, 0, 0);
                ep0_state.in_ack_pending = 0;
            }
            ep0_state.last_in_packet_num = 0;
            ep0_state.in_transfers = 0;
            /*
            if (calibrate_fifo0) {
debug_printf("IGNORE FIRST PACKET\n");
                read_data(SUDFIFO, 9);
                
                int i;
                for (i = 1; i < 9; ++i)
                    if (spi_buf[i] == 0x80) break;
                if (i != 9) {
                    read_data(SUDFIFO, i);
                    calibrate_fifo0 = 0;
                }
                //memset(&spi_buf[1], 0, USBDEV_EP0_MAX_SIZE);
                //write_data(EP0FIFO, USBDEV_EP0_MAX_SIZE);
                //write_reg(EP0BC, USBDEV_EP0_MAX_SIZE);
                //ack_stat();
            }
            */
            //} else {
                read_data(SUDFIFO, 9);
                usbdevhal_out_done (usbdev, 0, USBDEV_TRANSACTION_SETUP, &spi_buf[1], 8);
            //}
            write_reg(EPIRQ, SUDAVIRQ);
        } 
        
        if (epirq & epien & IN0BAVIRQ) {
//debug_printf("IN0BAV\n");
            if (ep0_state.in_transfers) {
                epien &= ~IN0BAVIE;
                ep0_state.in_transfers = 0;
                usbdevhal_in_done (usbdev, 0, ep0_state.in_bytes);
                if ((ep0_state.last_in_packet_num == 1) && ep0_state.out_ack_pending) {
                    usbdevhal_out_done (usbdev, 0, USBDEV_TRANSACTION_OUT, 0, 0);
                    ep0_state.out_ack_pending = 0;
                    ep0_state.last_in_packet_num = 0;
                }
                write_reg(EPIEN, epien);
            }
        }
        
        if (epirq & epien & IN2BAVIRQ) {
//debug_printf("IN2BAV\n");
            if (ep2in_state.in_transfers) {
                ep2in_state.in_transfers--;
                usbdevhal_in_done (usbdev, 2, ep2in_state.in_bytes[0]);
                ep2in_state.in_bytes[0] = ep2in_state.in_bytes[1];
                if (ep2in_state.in_transfers == 0) {
                    epien &= ~IN2BAVIE;
                    write_reg(EPIEN, epien);
                }
            }
        }

        if (epirq & epien & IN3BAVIRQ) {
//debug_printf("IN3BAV\n");
            epien &= ~IN3BAVIE;
            ep3in_state.in_transfers = 0;
            usbdevhal_in_done (usbdev, 3, ep3in_state.in_bytes);
            write_reg(EPIEN, epien);
        }
        
        if (epirq & epien & OUT0BAVIRQ) {
//debug_printf("OUT0BAV\n");
            if ((ep0_state.last_in_packet_num > 1) && ep0_state.out_ack_pending) {
                usbdevhal_out_done (usbdev, 0, USBDEV_TRANSACTION_OUT, 0, 0);
                ep0_state.out_ack_pending = 0;
                ep0_state.last_in_packet_num = 0;
            } else {
                unsigned size = read_reg(EP0BC);
                read_data(EP0FIFO, size + 1);
                usbdevhal_out_done (usbdev, 0, USBDEV_TRANSACTION_OUT, &spi_buf[1], size);
                write_reg(EPIRQ, OUT0BAVIRQ);
                if (ep0_state.in_ack_pending) {
                    usbdevhal_in_done (usbdev, 0, 0);
                    ep0_state.in_ack_pending = 0;
                }
            }
        }
        
        if (epirq & epien & OUT1BAVIRQ) {
//debug_printf("OUT1BAV\n");
            unsigned size = read_reg(EP1OUTBC);
            read_data(EP1OUTFIFO, size + 1);
            usbdevhal_out_done (usbdev, 1, USBDEV_TRANSACTION_OUT, &spi_buf[1], size);
        }
    }
}

void max3421e_usbdev_init (usbdev_t *owner, spimif_t *spi, 
    unsigned spi_freq, unsigned spi_cs_num, int irq, int io_prio, 
    mem_pool_t *pool, mutex_t *m)
{  
    usbdev = owner;
    mem = pool;
    io_lock = m;
    spim = spi;
    irqn = irq;
    
    msg.freq = spi_freq;
    msg.mode = SPI_MODE_CS_NUM(spi_cs_num) | SPI_MODE_NB_BITS(8);
    msg.tx_data = spi_buf;
    msg.rx_data = spi_buf;
    
    usbdevhal_bind (usbdev, &hal, m);

#ifdef ELVEES    
    MC_IRQM |= MC_IRQM_MODE(irqn);
#endif

    task_create (usb_interrupt, 0, "usb_intr", io_prio, io_stack, sizeof (io_stack));
}
