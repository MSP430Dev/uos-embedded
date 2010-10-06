/*
 * Ethernet controller driver for Milandr 5600ВГ1.
 * Copyright (c) 2010 Serge Vakulenko.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <milandr/k5600bg1.h>
#include <milandr/k5600bg1-regs.h>

#define K5600BG1_IRQ		12	/* interrupt from pin PB10 */

#define PORTB_NIRQ	(1 << 10)	// nIRQ on PB10
#define PORTB_NRST	(1 << 11)	// nRST on PB11
#define PORTC_NOE	(1 << 1)	// nOE on PC1
#define PORTC_NWE	(1 << 2)	// nWE on PC2
#define PORTE_NCS	(1 << 12)	// nCS on PE12

/*
 * Set /RST to 0 for one microsecond.
 */
static void chip_reset ()
{
	ARM_GPIOB->DATA &= ~PORTB_NRST;
	udelay (1);
	ARM_GPIOB->DATA |= PORTB_NRST;
}

/*
 * Set default values to Ethernet controller registers.
 */
static void chip_init (k5600bg1_t *u)
{
	/* Включение тактовой частоты портов A-C, E, F. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA |
		ARM_PER_CLOCK_GPIOB | ARM_PER_CLOCK_GPIOC |
		ARM_PER_CLOCK_GPIOE | ARM_PER_CLOCK_GPIOF;

	/* Цифровые сигналы. */
	ARM_GPIOA->ANALOG = 0xFFFF;			// Data 0-15
	ARM_GPIOB->ANALOG |= PORTB_NIRQ | PORTB_NRST;	// nIRQ on PB10, nRST on PB11
	ARM_GPIOC->ANALOG |= PORTC_NOE | PORTC_NWE;	// nOE on PC1, nWE on PC2
	ARM_GPIOE->ANALOG |= PORTE_NCS;			// nCS on PE12
	ARM_GPIOF->ANALOG |= 0x7FFC;			// Addr 2-14

	/* Быстрый фронт. */
	ARM_GPIOA->PWR = 0xFFFFFFFF;
	ARM_GPIOB->PWR |= ARM_PWR_FASTEST(10) | ARM_PWR_FASTEST(11);
	ARM_GPIOC->PWR |= ARM_PWR_FASTEST(1) | ARM_PWR_FASTEST(2);
	ARM_GPIOE->PWR |= ARM_PWR_FASTEST(12);
	ARM_GPIOF->PWR |= ARM_PWR_FASTEST(2) | ARM_PWR_FASTEST(3) |
		ARM_PWR_FASTEST(4) | ARM_PWR_FAST(5) | ARM_PWR_FASTEST(6) |
		ARM_PWR_FASTEST(7) | ARM_PWR_FAST(8) | ARM_PWR_FASTEST(9) |
		ARM_PWR_FASTEST(10) | ARM_PWR_FAST(11) | ARM_PWR_FASTEST(12) |
		ARM_PWR_FASTEST(13) | ARM_PWR_FAST(14);

	/* Основная функция для PA0-15(DATA), PC1(OE), PC2(WE), PF2-14(ADDR).
	 * Альтернативная функция для PB10 (EXT_INT2).
	 * Функция по умолчанию для PB11 и PE12. */
	ARM_GPIOA->FUNC = 0x55555555;			// Data 0-15
	ARM_GPIOB->FUNC = (ARM_GPIOB->FUNC &		// nIRQ on PB10, nRST on PB11
		~(ARM_FUNC_MASK(10) | ARM_FUNC_MASK(11))) |
		ARM_FUNC_ALT(10);
	ARM_GPIOC->FUNC = (ARM_GPIOC->FUNC &		// nOE on PC1, nWE on PC2
		~(ARM_FUNC_MASK(1) | ARM_FUNC_MASK(2))) |
		ARM_FUNC_MAIN(1) | ARM_FUNC_MAIN(2);
	ARM_GPIOE->FUNC &= ~ARM_FUNC_MASK(12);		// nCS on PE12
	ARM_GPIOF->FUNC = (ARM_GPIOF->FUNC &		// Addr 2-14
		~(ARM_FUNC_MASK(2) | ARM_FUNC_MASK(3) |
		ARM_FUNC_MASK(4) | ARM_FUNC_MASK(5) | ARM_FUNC_MASK(6) |
		ARM_FUNC_MASK(7) | ARM_FUNC_MASK(8) | ARM_FUNC_MASK(9) |
		ARM_FUNC_MASK(10) | ARM_FUNC_MASK(11) | ARM_FUNC_MASK(12) |
		ARM_FUNC_MASK(13) | ARM_FUNC_MASK(14))) |
		ARM_FUNC_MAIN(2) | ARM_FUNC_MAIN(3) |
		ARM_FUNC_MAIN(4) | ARM_FUNC_MAIN(5) | ARM_FUNC_MAIN(6) |
		ARM_FUNC_MAIN(7) | ARM_FUNC_MAIN(8) | ARM_FUNC_MAIN(9) |
		ARM_FUNC_MAIN(10) | ARM_FUNC_MAIN(11) | ARM_FUNC_MAIN(12) |
		ARM_FUNC_MAIN(13) | ARM_FUNC_MAIN(14);

	/* Для nOE и nWE отключаем режим открытого коллектора. */
	ARM_GPIOC->PD &= ~(PORTC_NOE | PORTC_NWE);

	/* Включаем выходные сигналы.  */
	ARM_GPIOB->DATA |= PORTB_NRST;		// nRST on PB11
	ARM_GPIOB->OE |= PORTB_NRST;
	ARM_GPIOE->DATA |= PORTE_NCS;		// nCS on PE12
	ARM_GPIOE->OE |= PORTE_NCS;

	chip_reset ();

	/* Включение внешней шины адрес/данные в режиме RAM.
	 * Длительность цикла на шине равна пяти тактам (2 wait states). */
	ARM_EXTBUS->CONTROL = ARM_EXTBUS_RAM | ARM_EXTBUS_WS(2);

	ETH_REG->GCTRL = 0x5382;

#if 0
	/* Reset transceiver. */
	phy_write (u, PHY_CTL, PHY_CTL_RST);
	int count;
	for (count=10000; count>0; count--)
		if (! (phy_read (u, PHY_CTL) & PHY_CTL_RST))
			break;
	if (count == 0)
		debug_printf ("k5600bg1_init: PHY reset failed\n");
	phy_write (u, PHY_EXTCTL, PHY_EXTCTL_JABBER);

	/* Perform auto-negotiation. */
	phy_write (u, PHY_ADVRT, PHY_ADVRT_CSMA | PHY_ADVRT_10_HDX |
		PHY_ADVRT_10_FDX | PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX);
	phy_write (u, PHY_CTL, PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST);

	/* Reset TX and RX blocks and pointers */
	MC_MAC_CONTROL = MAC_CONTROL_CP_TX | MAC_CONTROL_RST_TX |
			 MAC_CONTROL_CP_RX | MAC_CONTROL_RST_RX;
	udelay (10);
	/*debug_printf ("MAC_CONTROL: 0x%08x\n", MC_MAC_CONTROL);*/

	/* Общие режимы. */
	MC_MAC_CONTROL =
		MAC_CONTROL_FULLD |		/* дуплексный режим */
		MAC_CONTROL_EN_TX |		/* разрешение передачи */
		MAC_CONTROL_EN_TX_DMA |		/* разрешение передающего DMА */
		MAC_CONTROL_EN_RX |		/* разрешение приема */
		MAC_CONTROL_IRQ_TX_DONE | 	/* прерывание от передачи */
		MAC_CONTROL_IRQ_RX_DONE | 	/* прерывание по приёму */
		MAC_CONTROL_IRQ_RX_OVF; 	/* прерывание по переполнению */
	/*debug_printf ("MAC_CONTROL: 0x%08x\n", MC_MAC_CONTROL);*/

	/* Режимы приёма. */
	MC_MAC_RX_FRAME_CONTROL =
		RX_FRAME_CONTROL_DIS_RCV_FCS | 	/* не сохранять контрольную сумму */
		RX_FRAME_CONTROL_ACC_TOOSHORT |	/* прием коротких кадров */
		RX_FRAME_CONTROL_DIS_TOOLONG | 	/* отбрасывание слишком длинных кадров */
		RX_FRAME_CONTROL_DIS_FCSCHERR |	/* отбрасывание кадров с ошибкой контрольной суммы */
		RX_FRAME_CONTROL_DIS_LENGTHERR;	/* отбрасывание кадров с ошибкой длины */
	/*debug_printf ("RX_FRAME_CONTROL: 0x%08x\n", MC_MAC_RX_FRAME_CONTROL);*/

	/* Режимы передачи:
	 * запрет формирования кадра в блоке передачи. */
	MC_MAC_TX_FRAME_CONTROL = TX_FRAME_CONTROL_DISENCAPFR;
	/*debug_printf ("TX_FRAME_CONTROL: 0x%08x\n", MC_MAC_TX_FRAME_CONTROL);*/

	/* Режимы обработки коллизии. */
	MC_MAC_IFS_COLL_MODE = IFS_COLL_MODE_ATTEMPT_NUM(15) |
		IFS_COLL_MODE_EN_CW |
		IFS_COLL_MODE_COLL_WIN(64) |
		IFS_COLL_MODE_JAMB(0xC3) |
		IFS_COLL_MODE_IFS(24);

	/* Тактовый сигнал MDC не должен превышать 2.5 МГц. */
	MC_MAC_MD_MODE = MD_MODE_DIVIDER (KHZ / 2000);

	/* Свой адрес. */
	MC_MAC_UCADDR_L = u->netif.ethaddr[0] |
			 (u->netif.ethaddr[1] << 8) |
			 (u->netif.ethaddr[2] << 16)|
			 (u->netif.ethaddr[3] << 24);
	MC_MAC_UCADDR_H = u->netif.ethaddr[4] |
			 (u->netif.ethaddr[5] << 8);
/*debug_printf ("UCADDR=%02x:%08x\n", MC_MAC_UCADDR_H, MC_MAC_UCADDR_L);*/

	/* Максимальный размер кадра. */
	MC_MAC_RX_FR_MAXSIZE = K5600BG1_MTU;
#endif
}

void
k5600bg1_debug (k5600bg1_t *u, struct _stream_t *stream)
{
	unsigned short phy_stat, gctrl;

	mutex_lock (&u->netif.lock);
	phy_stat = ETH_REG->PHY_STAT;
	gctrl = ETH_REG->GCTRL;
	mutex_unlock (&u->netif.lock);

	printf (stream, "PHY_STAT=%b\n", phy_stat, PHY_STAT_BITS);
	printf (stream, "GCTRL=%04x\n", gctrl);
}

void k5600bg1_start_negotiation (k5600bg1_t *u)
{
	mutex_lock (&u->netif.lock);
	ETH_REG->PHY_CTRL = PHY_CTRL_RST | PHY_CTRL_TXEN | PHY_CTRL_RXEN |
		PHY_CTRL_DIR | PHY_CTRL_LINK_PERIOD (11);
	mutex_unlock (&u->netif.lock);
}

int k5600bg1_get_carrier (k5600bg1_t *u)
{
	unsigned phy_stat;

	mutex_lock (&u->netif.lock);
	phy_stat = ETH_REG->PHY_STAT;
	mutex_unlock (&u->netif.lock);

	return (phy_stat & PHY_STAT_LINK) != 0;
}

long k5600bg1_get_speed (k5600bg1_t *u, int *duplex)
{
	unsigned phy_ctrl;

	mutex_lock (&u->netif.lock);
	phy_ctrl = ETH_REG->PHY_CTRL;
	mutex_unlock (&u->netif.lock);

	if (phy_ctrl & PHY_CTRL_HALFD) {
		if (duplex)
			*duplex = 0;
	} else {
		if (duplex)
			*duplex = 1;
	}
	return u->netif.bps;
}

/*
 * Set PHY loop-back mode.
 */
void k5600bg1_set_loop (k5600bg1_t *u, int on)
{
	unsigned phy_ctrl;

	mutex_lock (&u->netif.lock);
	phy_ctrl = ETH_REG->PHY_CTRL;
	if (on) {
		phy_ctrl |= PHY_CTRL_LB;
	} else {
		phy_ctrl &= ~PHY_CTRL_LB;
	}
	ETH_REG->PHY_CTRL = phy_ctrl;
	mutex_unlock (&u->netif.lock);
}

void k5600bg1_set_promisc (k5600bg1_t *u, int station, int group)
{
	mutex_lock (&u->netif.lock);
	unsigned mac_ctrl = ETH_REG->MAC_CTRL & ~MAC_CTRL_PRO_EN;
	if (station) {
		/* Accept any unicast. */
		mac_ctrl |= MAC_CTRL_PRO_EN;
	}
	/* TODO: multicasts. */
	ETH_REG->MAC_CTRL = mac_ctrl;
	mutex_unlock (&u->netif.lock);
}

/*
 * Put data to transmit FIFO from dma buffer.
 */
static void
chip_write_txfifo (unsigned physaddr, unsigned nbytes)
{
#if 0
/*debug_printf ("write_txfifo %08x, %d bytes\n", physaddr, nbytes);*/
	/* Set the address and length for DMA. */
	unsigned csr = MC_DMA_CSR_WN(15) |
		MC_DMA_CSR_WCX (((nbytes + 7) >> 3) - 1);
	MC_IR_EMAC(1) = physaddr;
	MC_CP_EMAC(1) = 0;
	MC_CSR_EMAC(1) = csr;
/*debug_printf ("<t%d> ", nbytes);*/

	/* Run the DMA. */
	MC_CSR_EMAC(1) = csr | MC_DMA_CSR_RUN;

	unsigned count;
	for (count=100000; count>0; count--) {
		csr = MC_CSR_EMAC(1);
		if (! (csr & MC_DMA_CSR_RUN))
			break;
/*debug_printf ("~");*/
	}
	if (count == 0) {
		debug_printf ("eth: TX DMA failed, CSR=%08x\n", csr);
		MC_CSR_EMAC(1) = 0;
	}
#endif
}

/*
 * Fetch data from receive FIFO to dma buffer.
 */
static void
chip_read_rxfifo (unsigned physaddr, unsigned nbytes)
{
#if 0
	/* Set the address and length for DMA. */
	unsigned csr = MC_DMA_CSR_WN(15) |
		MC_DMA_CSR_WCX (((nbytes + 7) >> 3) - 1);
	MC_CSR_EMAC(0) = csr;
	MC_IR_EMAC(0) = physaddr;
	MC_CP_EMAC(0) = 0;
/*debug_printf ("(r%d) ", nbytes);*/

	/* Run the DMA. */
	MC_CSR_EMAC(0) = csr | MC_DMA_CSR_RUN;

	unsigned count;
	for (count=100000; count>0; count--) {
		csr = MC_CSR_EMAC(0);
		if (! (csr & MC_DMA_CSR_RUN))
			break;
	}
	if (count == 0) {
		debug_printf ("eth: RX DMA failed, CSR=%08x\n", csr);
		MC_CSR_EMAC(0) = 0;
	}
#endif
}

/*
 * Write the packet to chip memory and start transmission.
 * Deallocate the packet.
 */
static void
chip_transmit_packet (k5600bg1_t *u, buf_t *p)
{
	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */
	buf_t *q;
	unsigned char *buf = (unsigned char*) u->txbuf;
	for (q=p; q; q=q->next) {
		/* Copy the packet into the transmit buffer. */
		assert (q->len > 0);
/*debug_printf ("txcpy %08x <- %08x, %d bytes\n", buf, q->payload, q->len);*/
		memcpy (buf, q->payload, q->len);
		buf += q->len;
	}

	unsigned len = p->tot_len;
	if (len < 60) {
		len = 60;
/*debug_printf ("txzero %08x, %d bytes\n", u->txbuf + p->tot_len, len - p->tot_len);*/
		memset (u->txbuf + p->tot_len, 0, len - p->tot_len);
	}
#if 0
	MC_MAC_TX_FRAME_CONTROL = TX_FRAME_CONTROL_DISENCAPFR |
		TX_FRAME_CONTROL_DISPAD |
		TX_FRAME_CONTROL_LENGTH (len);
	chip_write_txfifo (u->txbuf, len);
/*debug_printf ("!");*/
	MC_MAC_TX_FRAME_CONTROL |= TX_FRAME_CONTROL_TX_REQ;
/*debug_printf ("@");*/
#endif
	++u->netif.out_packets;
	u->netif.out_bytes += len;

/*debug_printf ("tx%d", len); buf_print_data (u->txbuf, p->tot_len);*/
}

/*
 * Do the actual transmission of the packet. The packet is contained
 * in the pbuf that is passed to the function. This pbuf might be chained.
 * Return 1 when the packet is succesfully queued for transmission.
 * Or return 0 if the packet is lost.
 */
static bool_t
k5600bg1_output (k5600bg1_t *u, buf_t *p, small_uint_t prio)
{
	mutex_lock (&u->tx_lock);

	/* Exit if link has failed */
	if (p->tot_len < 4 || p->tot_len > K5600BG1_MTU /*||
	    ! (phy_read (u, PHY_STS) & PHY_STS_LINK)*/) {
		++u->netif.out_errors;
		mutex_unlock (&u->tx_lock);
/*debug_printf ("k5600bg1_output: transmit %d bytes, link failed\n", p->tot_len);*/
		buf_free (p);
		return 0;
	}
/*debug_printf ("k5600bg1_output: transmit %d bytes\n", p->tot_len);*/

#if 0
	if (! (MC_MAC_STATUS_TX & STATUS_TX_ONTX_REQ)) {
		/* Смело отсылаем. */
		chip_transmit_packet (u, p);
		mutex_unlock (&u->tx_lock);
		buf_free (p);
		return 1;
	}
#endif
	/* Занято, ставим в очередь. */
	if (buf_queue_is_full (&u->outq)) {
		/* Нет места в очереди: теряем пакет. */
		++u->netif.out_discards;
		mutex_unlock (&u->tx_lock);
		debug_printf ("k5600bg1_output: overflow\n");
		buf_free (p);
		return 0;
	}
	buf_queue_put (&u->outq, p);
	mutex_unlock (&u->tx_lock);
	return 1;
}

/*
 * Get a packet from input queue.
 */
static buf_t *
k5600bg1_input (k5600bg1_t *u)
{
	buf_t *p;

	mutex_lock (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	mutex_unlock (&u->netif.lock);
	return p;
}

/*
 * Setup MAC address.
 */
static void
k5600bg1_set_address (k5600bg1_t *u, unsigned char *addr)
{
	mutex_lock (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);

#if 0
	MC_MAC_UCADDR_L = u->netif.ethaddr[0] |
			 (u->netif.ethaddr[1] << 8) |
			 (u->netif.ethaddr[2] << 16)|
			 (u->netif.ethaddr[3] << 24);
	MC_MAC_UCADDR_H = u->netif.ethaddr[4] |
			 (u->netif.ethaddr[5] << 8);
/*debug_printf ("UCADDR=%02x:%08x\n", MC_MAC_UCADDR_H, MC_MAC_UCADDR_L);*/
#endif
	mutex_unlock (&u->netif.lock);
}

/*
 * Fetch the received packet from the network controller.
 * Put it to input queue.
 */
static void
k5600bg1_receive_frame (k5600bg1_t *u)
{
#if 0
	unsigned frame_status = MC_MAC_RX_FRAME_STATUS_FIFO;
	if (! (frame_status & RX_FRAME_STATUS_OK)) {
		/* Invalid frame */
debug_printf ("k5600bg1_receive_data: failed, frame_status=%#08x\n", frame_status);
		++u->netif.in_errors;
		return;
	}
	/* Extract data from RX FIFO. */
	unsigned len = RX_FRAME_STATUS_LEN (frame_status);
	chip_read_rxfifo (u->rxbuf, len);

	if (len < 4 || len > K5600BG1_MTU) {
		/* Skip this frame */
debug_printf ("k5600bg1_receive_data: bad length %d bytes, frame_status=%#08x\n", len, frame_status);
		++u->netif.in_errors;
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
/*debug_printf ("k5600bg1_receive_data: ok, frame_status=%#08x, length %d bytes\n", frame_status, len);*/
/*debug_printf ("k5600bg1_receive_data: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
((unsigned char*)u->rxbuf)[0], ((unsigned char*)u->rxbuf)[1],
((unsigned char*)u->rxbuf)[2], ((unsigned char*)u->rxbuf)[3],
((unsigned char*)u->rxbuf)[4], ((unsigned char*)u->rxbuf)[5],
((unsigned char*)u->rxbuf)[6], ((unsigned char*)u->rxbuf)[7],
((unsigned char*)u->rxbuf)[8], ((unsigned char*)u->rxbuf)[9],
((unsigned char*)u->rxbuf)[10], ((unsigned char*)u->rxbuf)[11],
((unsigned char*)u->rxbuf)[12], ((unsigned char*)u->rxbuf)[13]);*/

	if (buf_queue_is_full (&u->inq)) {
/*debug_printf ("k5600bg1_receive_data: input overflow\n");*/
		++u->netif.in_discards;
		return;
	}

	/* Allocate a buf chain with total length 'len' */
	buf_t *p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
debug_printf ("k5600bg1_receive_data: ignore packet - out of memory\n");
		++u->netif.in_discards;
		return;
	}

	/* Copy the packet data. */
/*debug_printf ("receive %08x <- %08x, %d bytes\n", p->payload, u->rxbuf, len);*/
	memcpy (p->payload, u->rxbuf, len);
	buf_queue_put (&u->inq, p);
/*debug_printf ("[%d]", p->tot_len); buf_print_ethernet (p);*/
#endif
}

/*
 * Process an interrupt.
 * Return nonzero when there was some activity.
 */
static unsigned
handle_interrupt (k5600bg1_t *u)
{
/*	unsigned active = 0;*/
	for (;;) {
#if 1
		return 0;
#else
		unsigned status_rx = MC_MAC_STATUS_RX;
/*debug_printf ("eth rx irq: STATUS_RX = %08x\n", status_rx);*/
		if (status_rx & (STATUS_RX_STATUS_OVF | STATUS_RX_FIFO_OVF)) {
			/* Count lost incoming packets. */
			if (STATUS_RX_NUM_MISSED (status_rx))
				u->netif.in_discards += STATUS_RX_NUM_MISSED (status_rx);
			else
				u->netif.in_discards++;
			MC_MAC_STATUS_RX = 0;
		}
		/* Check if a packet has been received and buffered. */
		if (! (status_rx & STATUS_RX_DONE)) {
			/* All interrupts processed. */
			return active;
		}
		++active;
		MC_MAC_STATUS_RX = 0;

		/* Fetch all received packets. */
		unsigned nframes = STATUS_RX_NUM_FR (status_rx);
		while (nframes-- > 0)
			k5600bg1_receive_frame (u);

		unsigned status_tx = MC_MAC_STATUS_TX;
		if (status_tx & STATUS_TX_DONE) {
			MC_MAC_STATUS_TX = 0;

			/* Подсчитываем коллизии. */
			if (status_tx & (STATUS_TX_ONCOL | STATUS_TX_LATE_COLL)) {
				++u->netif.out_collisions;
			}

			/* Извлекаем следующий пакет из очереди. */
			buf_t *p = buf_queue_get (&u->outq);
			if (p) {
				/* Передаём следующий пакет. */
				chip_transmit_packet (u, p);
				buf_free (p);
			}
		}
#endif
	}
}

/*
 * Poll for interrupts.
 * Must be called by user in case there is a chance to lose an interrupt.
 */
void
k5600bg1_poll (k5600bg1_t *u)
{
	mutex_lock (&u->netif.lock);
	if (handle_interrupt (u))
		mutex_signal (&u->netif.lock, 0);
	mutex_unlock (&u->netif.lock);
}

/*
 * Interrupt task.
 */
static void
k5600bg1_task (void *arg)
{
	k5600bg1_t *u = arg;

	/* Register the interrupt. */
	mutex_lock_irq (&u->netif.lock, K5600BG1_IRQ, 0, 0);

	for (;;) {
		/* Wait for the interrupt. */
		mutex_wait (&u->netif.lock);
		++u->intr;
		handle_interrupt (u);
	}
}

static netif_interface_t k5600bg1_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						k5600bg1_output,
	(buf_t *(*) (netif_t*))			k5600bg1_input,
	(void (*) (netif_t*, unsigned char*))	k5600bg1_set_address,
};

/*
 * Set up the network interface.
 */
void
k5600bg1_init (k5600bg1_t *u, const char *name, int prio, mem_pool_t *pool,
	arp_t *arp, const unsigned char *macaddr)
{
	u->netif.interface = &k5600bg1_interface;
	u->netif.name = name;
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 1000000;
	memcpy (&u->netif.ethaddr, macaddr, 6);

	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));
	buf_queue_init (&u->outq, u->outqdata, sizeof (u->outqdata));

	/* Initialize hardware. */
	chip_init (u);

	/* Create interrupt task. */
	task_create (k5600bg1_task, u, "eth-rx", prio, u->stack, sizeof (u->stack));
}
