#include "runtime/lib.h"
#include "kernel/uos.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "cs8900/cs8900.h"

extern volatile unsigned char device_cs8900[];

#define ETH_MTU		1514
#define ETH_MIN		(64-4)	/* LY: Минимальна длина пакета без CRC. */
#define CS8900_RUNT	0	/* LY: Допускает передачу коротких фреймов, т.е. меньше 60+4 байт. */
#define CS8900_PAD64	1	/* LY: Включает софтверное дополнение фреймов до 60+4 байт.
				 *     Имеет смысл только при CS8900_RUNT=0.
				 *     Если выключено и CS8900_RUNT=0, то чип дополняет
				 *     пакеты мусором (похоже размножает последний байт).
				 */
#define POLL_MODE
#define DATAREG		0x00
#define TXCMD		0x04
#define TXLENGTH	0x06
#define ISQ		0x08
#define PACKETPP	0x0A
#define PPDATA		0x0C

/*
 * Output low byte first, then high byte.
 */
void inline __attribute__((always_inline))
cs_out (small_uint_t reg, unsigned val) {
#ifdef __AVR__
	__asm __volatile (
		"sts %1, %A0"		"\n\t"
		"sts (%1)+1, %B0"
		:: "r" (val), "o" (device_cs8900[reg]));
#else
	device_cs8900[reg] = val;
	device_cs8900[reg + 1] = val >> 8;
#endif
}

/*
 * Input high byte first, then low byte.
 */
unsigned inline __attribute__((always_inline))
cs_in (small_uint_t reg) {
#ifdef __AVR__
	unsigned v;
	__asm __volatile (
		"lds %B0, (%1)+1"	"\n\t"
		"lds %A0, %1"
		: "=r" (v) : "o" (device_cs8900[reg]));
	return v;
#else
	small_uint_t l, h;
	h = device_cs8900[reg + 1];
	l = device_cs8900[reg];
	return ((unsigned) h << 8) | l;
#endif
}

/*
 * Self Control Register - Read/write
 */
#define PP_SELFCTL		0x0114
#define OUT_SELFCTL(v)		cs_out (PPDATA, 0x15 | (v))

#define POWER_ON_RESET		0x0040
#define SW_STOP			0x0100
#define SLEEP_ON		0x0200
#define AUTO_WAKEUP		0x0400
#define HCB0_ENBL		0x1000
#define HCB1_ENBL		0x2000
#define HCB0			0x4000
#define HCB1			0x8000

/*
 * Self Status Register - Read only
 */
#define PP_SELFST		0x0136

#define ACTIVE_33V		0x0040
#define INIT_DONE		0x0080
#define SI_BUSY			0x0100
#define EEPROM_PRESENT		0x0200
#define EEPROM_OK		0x0400
#define EL_PRESENT		0x0800
#define EE_SIZE_64		0x1000

/*
 * Individual Address Registers - Read/write
 */
#define PP_IA			0x0158

/*
 * Receive Control Register - Read/write
 */
#define PP_RXCTL		0x0104
#define OUT_RXCTL(v)		cs_out (PPDATA, 0x05 | (v))

#define RX_IA_HASH_ACCEPT	0x0040
#define RX_PROM_ACCEPT		0x0080
#define RX_OK_ACCEPT		0x0100
#define RX_MULTCAST_ACCEPT	0x0200
#define RX_IA_ACCEPT		0x0400
#define RX_BROADCAST_ACCEPT	0x0800
#define RX_BAD_CRC_ACCEPT	0x1000
#define RX_RUNT_ACCEPT		0x2000
#define RX_EXTRA_DATA_ACCEPT	0x4000

/*
 * Receive Configuration and Interrupt Mask Register - Read/write
 */
#define PP_RXCFG		0x0102
#define OUT_RXCFG(v)		cs_out (PPDATA, 0x03 | (v))

#define SKIP_1			0x0040
#define RX_STREAM_ENBL		0x0080
#define RX_OK_ENBL		0x0100
#define RX_DMA_ONLY		0x0200
#define AUTO_RX_DMA		0x0400
#define BUFFER_CRC		0x0800
#define RX_CRC_ERROR_ENBL	0x1000
#define RX_RUNT_ENBL		0x2000
#define RX_EXTRA_DATA_ENBL	0x4000

/*
 * Transmit Configuration Interrupt Mask Register - Read/write
 */
#define PP_TXCFG		0x0106
#define OUT_TXCFG(v)		cs_out (PPDATA, 0x07 | (v))

#define TX_LOST_CRS_ENBL	0x0040
#define TX_SQE_ERROR_ENBL	0x0080
#define TX_OK_ENBL		0x0100
#define TX_LATE_COL_ENBL	0x0200
#define TX_JBR_ENBL		0x0400
#define TX_ANY_COL_ENBL		0x0800
#define TX_16_COL_ENBL		0x8000

/*
 * Interrupt Number Register - Read/write
 */
#define PP_INTNUM		0x0022	/* Interrupt number (0,1,2, or 3) */

/*
 * Buffer Configuration Interrupt Mask Register - Read/write
 */
#define PP_BUFCFG		0x010A
#define OUT_BUFCFG(v)		cs_out (PPDATA, 0x0b | (v))

#define GENERATE_SW_INTERRUPT	0x0040
#define RX_DMA_ENBL		0x0080
#define READY_FOR_TX_ENBL	0x0100
#define TX_UNDERRUN_ENBL	0x0200
#define RX_MISS_ENBL		0x0400
#define RX_128_BYTE_ENBL	0x0800
#define TX_COL_COUNT_OVRFLOW_ENBL 0x1000
#define RX_MISS_COUNT_OVRFLOW_ENBL 0x2000
#define RX_DEST_MATCH_ENBL	0x8000

/*
 * ISA Bus Control Register - Read/write
 */
#define PP_BUSCTL		0x0116
#define OUT_BUSCTL(v)		cs_out (PPDATA, 0x17 | (v))

#define RESET_RX_DMA		0x0040
#define MEMORY_ON		0x0400
#define DMA_BURST_MODE		0x0800
#define IO_CHANNEL_READY_ON	0x1000
#define RX_DMA_SIZE_64Ks	0x2000
#define ENABLE_IRQ		0x8000

/*
 * Line Control Register - Read/write
 */
#define PP_LINECTL		0x0112
#define OUT_LINECTL(v)		cs_out (PPDATA, 0x13 | (v))

#define SERIAL_RX_ON		0x0040
#define SERIAL_TX_ON		0x0080
#define AUI_ONLY		0x0100
#define AUTO_AUI_10BASET	0x0200
#define MODIFIED_BACKOFF	0x0800
#define NO_AUTO_POLARITY	0x1000
#define TWO_PART_DEFDIS		0x2000
#define LOW_RX_SQUELCH		0x4000

/*
 * Receive Event Register - Read-only
 */
#define PP_RXEVENT		0x0124

#define RXE_IA_HASHED		0x0040
#define RXE_DRIBBLE		0x0080
#define RXE_OK			0x0100
#define RXE_HASHED		0x0200
#define RXE_IA			0x0400
#define RXE_BROADCAST		0x0800
#define RXE_CRC_ERROR		0x1000
#define RXE_RUNT		0x2000
#define RXE_EXTRA_DATA		0x4000
#define RXE_ALL_BITS		0x7FC0

/*
 * Buffer Event Register - Read-only
 */
#define PP_BUFEVENT		0x012C

#define BUF_SWINT		0x0040
#define BUF_RXDMA		0x0080
#define BUF_RDY4TX		0x0100
#define BUF_TXUNDERRUN		0x0200
#define BUF_RXMISS		0x0400
#define BUF_RX128		0x0800
#define BUF_RXDEST		0x8000

/*
 * Interrupt Status Queue Register
 */
#define ISQ_EVENT_MASK		0x003F
#define ISQ_RECEIVER_EVENT	0x0004
#define ISQ_TRANSMITTER_EVENT	0x0008
#define ISQ_BUFFER_EVENT	0x000c
#define ISQ_RX_MISS_EVENT	0x0010
#define ISQ_TX_COL_EVENT	0x0012

/*
 * Line Status Register - Read-only
 */
#define PP_LINEST		0x0134

#define LINK_OK			0x0080
#define AUI_ON			0x0100
#define TENBASET_ON		0x0200
#define POLARITY_OK		0x1000
#define CRS_OK			0x4000

/*
 * Bus Status Register
 */
#define PP_BUSST		0x0138

#define TX_BID_ERROR		0x0080
#define READY_FOR_TX_NOW	0x0100

/*
 * Receive Miss Count
 */
#define PP_RXMISS		0x0130

/*
 * Transmit Collision Count
 */
#define PP_TXCOL		0x0132

/*
 * Transmit Command Register - Write only
 */
#define TX_START_4_BYTES	0x0000
#define TX_START_64_BYTES	0x0040
#define TX_START_128_BYTES	0x0080
#define TX_START_ALL_BYTES	0x00C0
#define TX_FORCE		0x0100
#define TX_ONE_COL		0x0200
#define TX_TWO_PART_DEFF_DISABLE 0x0400
#define TX_NO_CRC		0x1000
#define TX_RUNT			0x2000

static buf_t *
cs8900_input (cs8900_t *u)
{
	buf_t *p;

	lock_take (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	lock_release (&u->netif.lock);
	return p;
}

static void
cs8900_set_address (cs8900_t *u, unsigned char *addr)
{
	lock_take (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);

	/* Set MAC address. */
	cs_out (PACKETPP, PP_IA);
	cs_out (PPDATA, u->netif.ethaddr[0] | u->netif.ethaddr[1] << 8);
	cs_out (PACKETPP, PP_IA + 2);
	cs_out (PPDATA, u->netif.ethaddr[2] | u->netif.ethaddr[3] << 8);
	cs_out (PACKETPP, PP_IA + 4);
	cs_out (PPDATA, u->netif.ethaddr[4] | u->netif.ethaddr[5] << 8);

	lock_release (&u->netif.lock);
}

/*
 * Fetch and process the received packet from the network controller.
 */
static void
cs8900_receive_data (cs8900_t *u)
{
	unsigned len, event;
	unsigned char *b, h;
	buf_t *p;

	/* Read and check RxEvent, expecting correctly received frame,
	 * either broadcast or individual address */
	event = cs_in (DATAREG);
	len = cs_in (DATAREG);
	if (! (event & RXE_OK) || len < 4 || len > ETH_MTU) {
		/* Skip this frame */
		/* debug_printf ("cs8900_receive_data: failed, event=%#04x, length %d bytes\n", event, len); */
		++u->netif.in_errors;
skip:
		/* Throw away the last committed received frame */
		cs_out (PACKETPP, PP_RXCFG);
		OUT_RXCFG (SKIP_1 | RX_OK_ENBL | RX_RUNT_ENBL);
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
	/* debug_printf ("cs8900_receive_data: ok, event=%#04x, length %d bytes\n", event, len); */

	if (buf_queue_is_full (&u->inq)) {
		/* debug_printf ("cs8900_receive_data: input overflow\n"); */
		++u->netif.in_discards;
		goto skip;
	}

	/* Allocate a buf chain with total length 'len' */
	p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
		/* debug_printf ("cs8900_receive_data: ignore packet - out of memory\n"); */
		++u->netif.in_discards;
		goto skip;
	}

	/* Get received packet. */
	for (b = p->payload; ;) {
		b[0] = device_cs8900[DATAREG];
		h = device_cs8900[DATAREG + 1];
		if (len == 1)
			break;
		b[1] = h;
		if ((len -= 2) == 0)
			break;
		b += 2;
	}

	/* debug_dump ("cs8900-RX", p->payload, p->len); */
	buf_queue_put (&u->inq, p);
}

/*
 * Process a pending interrupt.
 */
static void
cs8900_interrupt (cs8900_t *u)
{
	for (;;) {
		cs_out (PACKETPP, PP_RXEVENT);
		if (0 == (cs_in (PPDATA) & RXE_ALL_BITS))
			break;

		cs8900_receive_data (u);
	}

	/* Read RxMiss Counter (zeroes itself upon read) */
	cs_out (PACKETPP, PP_RXMISS);
	u->netif.in_discards += cs_in (PPDATA) >> 6;

	/* Read RxCol Counter (zeroes itself upon read) */
	cs_out (PACKETPP, PP_TXCOL);
	u->netif.out_collisions += cs_in (PPDATA) >> 6;
}

#ifdef POLL_MODE
void
cs8900_poll (cs8900_t *u)
{
	lock_take (&u->netif.lock);
	cs8900_interrupt (u);
	if (u->inq.count != 0) {
		/* debug_printf ("cs8900_poll: received data\n"); */
		lock_signal (&u->netif.lock, 0);
	}
	lock_release (&u->netif.lock);
}
#else
/*
 * Receive interrupt task.
 */
static void
cs8900_receiver (void *arg)
{
	cs8900_t *u = arg;

	for (;;) {
		/* Wait for the receive interrupt. */
		lock_wait (&u->netif.lock);

		/* Process all pending interrupts. */
		cs8900_interrupt (u);
	}
}
#endif /* POLL_MODE */

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
bool_t
cs8900_output (cs8900_t *u, buf_t *p, small_uint_t prio)
{
	buf_t *q;
	small_uint_t hilo;
	unsigned char *b;
	unsigned len;

	/*debug_printf ("cs8900_output: transmit %d bytes\n", p->tot_len);*/
	lock_take (&u->netif.lock);

	/* Exit if link has failed */
	cs_out (PACKETPP, PP_LINEST);
	if (! (cs_in (PPDATA) & LINK_OK) || p->tot_len < 4 || p->tot_len > ETH_MTU) {
		/* debug_printf ("cs8900_output: transmit %d bytes, link failed\n", p->tot_len); */
failed: 	++u->netif.out_errors;
		lock_release (&u->netif.lock);
		buf_free (p);
		return 0;
	}

	/* Transmit command */
	cs_out (TXCMD, TX_START_ALL_BYTES | (CS8900_RUNT ? TX_RUNT : 0));
	len = p->tot_len;
	if (! CS8900_RUNT && CS8900_PAD64 && len < ETH_MIN)
		len = ETH_MIN;
	cs_out (TXLENGTH, len);

#if 1	/* LY: более "умный" вариант, если нет места в tx-буфере, то:
	 *	- пробуем вычитать приемный буфер;
	 *	- считаем сбой если нет несущей;
	 *	- ждем готовности бесконечно, при аварии сработает watchdog;
	 * 	Проблем на момент commit'а не замечено.
	*/
	for (;;) {
		cs_out (PACKETPP, PP_BUSST);
		if (cs_in (PPDATA) & READY_FOR_TX_NOW)
			break;
		task_yield ();
		cs8900_interrupt (u);
		cs_out (PACKETPP, PP_LINEST);
		if (! (cs_in (PPDATA) & LINK_OK))
			goto failed;
	}
	if (u->inq.count != 0)
		lock_signal (&u->netif.lock, 0);
#else	/* LY: А так было раньше: */
	cs_out (PACKETPP, PP_BUSST);
	tries = 0;
	/* Wait until ready for transmission, up to 100 retries */
	while (! (cs_in (PPDATA) & READY_FOR_TX_NOW) && (tries++ < 100)) {
		/* Throw away the last committed received frame */
		cs_out (PACKETPP, PP_RXCFG);
		OUT_RXCFG (SKIP_1 | RX_OK_ENBL | RX_RUNT_ENBL);
		cs_out (PACKETPP, PP_BUSST);
	}

	/* Ready to transmit? */
	if (! (cs_in (PPDATA) & READY_FOR_TX_NOW))
		goto failed;
#endif

	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */

	hilo = DATAREG;
	q = p; do {
		/* LY: писать данные в чип нужно всегда словам.
		 * Делаем здесь все правильно, с учетом возможной фрагментации пакета.
		 */
		assert (q->len > 0);
		b = q->payload;
		for (len = q->len; len; --len) {
			device_cs8900[hilo] = *b++;
			hilo ^= 1;
		}
		if (hilo != DATAREG)
			device_cs8900[DATAREG + 1] = 0;
		q = q->next;
	} while (q);
	/* debug_dump ("cs8900-TX", p->payload, p->len); */

	/* LY: pad eth-frame tail, else cs8900 just repead last byte. */
	if (! CS8900_RUNT && CS8900_PAD64 && p->tot_len < ETH_MIN - 1) {
		small_uint_t pad = p->tot_len;
		pad = (ETH_MIN - pad) >> 1;
		do {
			cs_out (DATAREG, 0);
		} while (--pad);
	}

	++u->netif.out_packets;
	u->netif.out_bytes += p->tot_len;

	lock_release (&u->netif.lock);
	buf_free (p);
	return 1;
}

/*
 * Detect CS8900 chip.
 * Read the Product identification code: must be 0x630E.
 * Return 1 on success; 0 on failure.
 */
bool_t
cs8900_probe ()
{
	unsigned id;

	cs_out (PACKETPP, 0);
	id = cs_in (PPDATA);
	/*debug_printf ("\nmanufacturer = %#04x\n", id);*/
	return id == 0x630E;
}

static netif_interface_t cs8900_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						cs8900_output,
	(buf_t *(*) (netif_t*))			cs8900_input,
	(void (*) (netif_t*, unsigned char*))	cs8900_set_address,
};

#include <timer/timer.h>
extern timer_t timer;

/*
 * Set up the network interface.
 */
void
cs8900_init (cs8900_t *u, const char *name, int prio, mem_pool_t *pool, arp_t *arp)
{
	u->netif.interface = &cs8900_interface;
	u->netif.name = name;
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));

	/* Obtain MAC address from network interface.
	 * We just fake an address... */
	u->netif.ethaddr[0] = 0x00;
	u->netif.ethaddr[1] = 0x09;
	u->netif.ethaddr[2] = 0x94;
	u->netif.ethaddr[3] = 0xff;
	u->netif.ethaddr[4] = 0xff;
	u->netif.ethaddr[5] = 0xff;
#if 0
	{
		unsigned x;
		cs_out (PACKETPP, 0x55aa);
		x = cs_in (PACKETPP);
		if (x != 0x55aa)
			debug_printf ("written 0x55aa read %#04x\n", x);
	}
#endif
	lock_take (&u->netif.lock);

	/*
	 * Initialize hardware.
	 */
	/* Set RESET bit. */
	cs_out (PACKETPP, PP_SELFCTL);
	OUT_SELFCTL (POWER_ON_RESET);

	/* The RESET bit will be cleared by the cs8900a
	 * as a result of the reset.
	 * Wait for RESET bit cleared. */
	for (;;) {
		unsigned selfctl = cs_in (PPDATA);

		if ((selfctl & POWER_ON_RESET) == 0)
			break;
		/*debug_printf ("selfctl=0x%04x ", selfctl);*/
	}

	/* After full initialization of the cs8900a
	 * the INITD bit will be set.
	 * Wait for INITD bit set. */
	cs_out (PACKETPP, PP_SELFST);
	for (;;) {
		unsigned selfst = cs_in (PPDATA);

		if (selfst & INIT_DONE)
			break;
		/*debug_printf ("selfst=0x%04x ", selfst);*/
		continue;
	}

	/* Set MAC address. */
	cs_out (PACKETPP, PP_IA);
	cs_out (PPDATA, u->netif.ethaddr[0] | u->netif.ethaddr[1] << 8);
	cs_out (PACKETPP, PP_IA + 2);
	cs_out (PPDATA, u->netif.ethaddr[2] | u->netif.ethaddr[3] << 8);
	cs_out (PACKETPP, PP_IA + 4);
	cs_out (PPDATA, u->netif.ethaddr[4] | u->netif.ethaddr[5] << 8);

	/* Accept valid unicast or broadcast frames. */
	cs_out (PACKETPP, PP_RXCTL);
	OUT_RXCTL (RX_OK_ACCEPT | RX_IA_ACCEPT | RX_BROADCAST_ACCEPT |
		RX_RUNT_ACCEPT);

	/* Enable receive interrupt */
	cs_out (PACKETPP, PP_RXCFG);
	OUT_RXCFG (RX_OK_ENBL | RX_RUNT_ENBL);

	/* Disable transmit interrupt (is default). */
	cs_out (PACKETPP, PP_TXCFG);
	OUT_TXCFG (0);

	/* Use interrupt number 0. */
	cs_out (PACKETPP, PP_INTNUM);
	cs_out (PPDATA, 0);

	/* Disable buffer events */
	cs_out (PACKETPP, PP_BUFCFG);
	OUT_BUFCFG (0);

	/* Enable interrupt generation */
	cs_out (PACKETPP, PP_BUSCTL);
	OUT_BUSCTL (0 /*ENABLE_IRQ*/);

	/* Enable receiver and transmitter */
	cs_out (PACKETPP, PP_LINECTL);
	OUT_LINECTL (SERIAL_RX_ON | SERIAL_TX_ON);

	lock_release (&u->netif.lock);

#ifndef POLL_MODE
	/* Create cs8900 receive task. */
	task_create (cs8900_receiver, u, "cs8900", prio,
		u->stack, sizeof (u->stack));
#endif
}
