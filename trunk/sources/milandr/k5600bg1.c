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

/*
 * Адреса регистров и буферов приёма/передачи.
 */
#define ETH_BASE	0x60000000
#define ETH_RXBUF	((eth_reg_t*)  (ETH_BASE + 0x0000))
#define ETH_RXDESC	((eth_desc_t*) (ETH_BASE + 0x2000))
#define ETH_TXBUF	((eth_reg_t*)  (ETH_BASE + 0x4000))
#define ETH_TXDESC	((eth_desc_t*) (ETH_BASE + 0x6000))
#define ETH_REG		((eth_regs_t*) (ETH_BASE + 0x7F00))

#define K5600BG1_IRQ	29		/* pin PB10 - EXT_INT2 */
#define K5600BG1_MTU	1518		/* maximum ethernet frame length */

#define PORTB_NIRQ	(1 << 10)	/* nIRQ on PB10 */
#define PORTB_NRST	(1 << 11)	/* nRST on PB11 */
#define PORTC_NOE	(1 << 1)	/* nOE on PC1 */
#define PORTC_NWE	(1 << 2)	/* nWE on PC2 */
#define PORTE_NCS	(1 << 12)	/* nCS on PE12 */

#define NRD		8		/* number of receive descriptors */
#define RXBUF_BYTES	4096		/* size of hardware receive buffer */

/*
 * Set /RST to 0 for one microsecond.
 */
static void
chip_reset ()
{
	ARM_GPIOB->OE |= PORTB_NRST;
	udelay (1);
	ARM_GPIOB->OE &= ~PORTB_NRST;
}

/*
 * Control /CS for 5600ВГ1 chip.
 */
static void
chip_select (int on)
{
	if (on)
		ARM_GPIOE->DATA &= ~PORTE_NCS;		// Select
	else
		ARM_GPIOE->DATA |= PORTE_NCS;		// Idle
}

/*
 * Set default values to Ethernet controller registers.
 */
static void
chip_init (k5600bg1_t *u)
{
	/* Включение тактовой частоты портов A-C, E, F. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA |
		ARM_PER_CLOCK_GPIOB | ARM_PER_CLOCK_GPIOC |
		ARM_PER_CLOCK_GPIOE | ARM_PER_CLOCK_GPIOF |
		ARM_PER_CLOCK_EXT_BUS;

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

	/* Для nRST включаем режим открытого коллектора.
	 * Чтобы сделать переход от +3.3V к +5V. */
	ARM_GPIOB->PD |= PORTB_NRST;
	ARM_GPIOB->PULL |= PORTB_NRST;

	/* Включаем выходные сигналы в неактивном состоянии.  */
	ARM_GPIOB->DATA &= ~PORTB_NRST;		// nRST on PB11
	ARM_GPIOB->OE &= ~PORTB_NRST;		// открытый сток
	ARM_GPIOE->DATA |= PORTE_NCS;		// nCS on PE12
	ARM_GPIOE->OE |= PORTE_NCS;

	chip_reset ();

	/* Включение внешней шины адрес/данные в режиме RAM.
	 * Длительность цикла должна быть не меньше 112.5 нс.
	 * При частоте процессора 40 МГц (один такт 25 нс)
	 * установка ws=11 или 12 даёт цикл в 150 нс.
	 * Проверено: 125 нс (ws=9) недостаточно. */
	ARM_EXTBUS->CONTROL = ARM_EXTBUS_RAM | ARM_EXTBUS_WS (11);
	chip_select (1);

	/* Режимы параллельного интерфейса к процессору. */
	ETH_REG->GCTRL = GCTRL_GLBL_RST;
	udelay (1);
	ETH_REG->GCTRL = GCTRL_READ_CLR_STAT | GCTRL_SPI_RST |
		GCTRL_ASYNC_MODE | GCTRL_SPI_TX_EDGE | GCTRL_SPI_DIR |
		GCTRL_SPI_FRAME_POL | GCTRL_SPI_DIV(2);

	/* Общие режимы. */
	ETH_REG->MAC_CTRL = MAC_CTRL_PRO_EN |	// Прием всех пакетов
		MAC_CTRL_BCA_EN |		// Прием всех широковещательных пакетов
		MAC_CTRL_SHORT_FRAME_EN;	// Прием коротких пакетов
// MAC_CTRL_DSCR_SCAN_EN	// Режим сканирования дескрипторов

	/* Режимы PHY. */
	ETH_REG->PHY_CTRL = PHY_CTRL_DIR |	// Прямой порядок битов в полубайте
		PHY_CTRL_RXEN | PHY_CTRL_TXEN |	// Включение приёмника и передатчика
		PHY_CTRL_LINK_PERIOD (11);	// Период LINK-импульсов

	/* Свой адрес. */
	ETH_REG->MAC_ADDR[0] = u->netif.ethaddr[0] | (u->netif.ethaddr[1] << 8);
	ETH_REG->MAC_ADDR[1] = u->netif.ethaddr[2] | (u->netif.ethaddr[3] << 8);
	ETH_REG->MAC_ADDR[2] = u->netif.ethaddr[4] | (u->netif.ethaddr[5] << 8);

	ETH_REG->MIN_FRAME = 64;		// Минимальная длина пакета
	ETH_REG->MAX_FRAME = K5600BG1_MTU + 4;	// Максимальная длина пакета
	ETH_REG->COLLCONF = COLLCONF_COLLISION_WINDOW (64) |
		COLLCONF_RETRIES_LIMIT (15);	// Лимит повторов передачи
	ETH_REG->IPGT = 96;			// Межпакетный интервал

	ETH_REG->RXBF_TAIL = 0;
	ETH_REG->RXBF_HEAD = RXBUF_BYTES/2 - 1;

	/* Начальное состояние дескрипторов. */
	unsigned i;
	for (i=0; i<NRD; i++) {
		ETH_RXDESC[i].CTRL = DESC_RX_RDY | DESC_RX_IRQ_EN;
	}
	ETH_RXDESC[NRD-1].CTRL |= DESC_RX_WRAP;
	ETH_TXDESC[0].CTRL = DESC_TX_WRAP;

	/* Ждём прерывания по приёму и передаче. */
	ETH_REG->INT_MASK = INT_TXF | INT_RXF | INT_RXS | INT_RXE;
}

void
k5600bg1_debug (k5600bg1_t *u, struct _stream_t *stream)
{
	unsigned short mac_ctrl, phy_ctrl, phy_stat, gctrl;
	unsigned short rxbf_head, rxbf_tail;

	mutex_lock (&u->netif.lock);
	gctrl = ETH_REG->GCTRL;
	mac_ctrl = ETH_REG->MAC_CTRL;
	phy_ctrl = ETH_REG->PHY_CTRL;
	phy_stat = ETH_REG->PHY_STAT;
	rxbf_head = ETH_REG->RXBF_HEAD;
	rxbf_tail = ETH_REG->RXBF_TAIL;
	mutex_unlock (&u->netif.lock);

	printf (stream, "GCTRL = %b\n", gctrl, GCTRL_BITS);
	printf (stream, "MAC_CTRL = %b\n", mac_ctrl, MAC_CTRL_BITS);
	printf (stream, "PHY_CTRL = %b\n", phy_ctrl, PHY_CTRL_BITS);
	printf (stream, "PHY_STAT = %b\n", phy_stat, PHY_STAT_BITS);
	printf (stream, "INT_SRC = %b\n", u->intr_flags, INT_BITS);
	printf (stream, "RXBF HEAD:TAIL = %04x:%04x\n", rxbf_head, rxbf_tail);
	printf (stream, "RXDESC[%u].CTRL = %b\n", u->rn,
		ETH_RXDESC[u->rn].CTRL, DESC_RX_BITS);
	if (! (ETH_RXDESC[u->rn].CTRL & DESC_RX_RDY))
		printf (stream, "    .LEN = %u, .PTRL = %04x\n",
			ETH_RXDESC[u->rn].LEN, ETH_RXDESC[u->rn].PTRL);
	printf (stream, "   TXDESC.CTRL = %b\n", ETH_TXDESC[0].CTRL, DESC_TX_BITS);
	printf (stream, "NVIC_IABR0 = %08x\n", ARM_NVIC_IABR0);
}

int
k5600bg1_get_carrier (k5600bg1_t *u)
{
	unsigned phy_stat;

	mutex_lock (&u->netif.lock);
	phy_stat = ETH_REG->PHY_STAT;
	mutex_unlock (&u->netif.lock);

	return (phy_stat & PHY_STAT_LINK) != 0;
}

long
k5600bg1_get_speed (k5600bg1_t *u, int *duplex)
{
	unsigned phy_ctrl, phy_stat;

	mutex_lock (&u->netif.lock);
	phy_ctrl = ETH_REG->PHY_CTRL;
	phy_stat = ETH_REG->PHY_STAT;
	mutex_unlock (&u->netif.lock);

	if (! (phy_stat & PHY_STAT_LINK))
		return 0;
	if (duplex)
		*duplex = ! (phy_ctrl & PHY_CTRL_HALFD);
	return u->netif.bps;
}

/*
 * Set PHY loop-back mode.
 */
void
k5600bg1_set_loop (k5600bg1_t *u, int on)
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

void
k5600bg1_set_promisc (k5600bg1_t *u, int station, int group)
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

static void
chip_copyin (unsigned char *data, volatile unsigned *chipbuf, unsigned bytes)
{
	while (bytes >= 2) {
		unsigned word = *chipbuf++;
		*data++ = word;
		*data++ = word >> 8;
		bytes -= 2;
		if (chipbuf >= ETH_RXBUF + RXBUF_BYTES/2)
			chipbuf = ETH_RXBUF;
	}
}

static void
chip_copyout (volatile unsigned *chipbuf, unsigned char *data, unsigned bytes)
{
	while (bytes >= 2) {
		unsigned word = *data++;
		word |= *data++ << 8;
		*chipbuf++ = word;
		bytes -= 2;
	}
	if (bytes > 0)
		*chipbuf = *data;
}

/*
 * Write the packet to chip memory and start transmission.
 * Deallocate the packet.
 */
static void
transmit_packet (k5600bg1_t *u, buf_t *p)
{
	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */
	buf_t *q;
	volatile unsigned *buf = ETH_TXBUF;
	unsigned odd = 0;
	for (q=p; q; q=q->next) {
		/* Copy the packet into the transmit buffer. */
		assert (q->len > 0);
		if (odd) {
			*buf++ |= *q->payload << 8;
			if (q->len > 1) {
				chip_copyout (buf, q->payload + 1, q->len - 1);
				buf += (q->len - 1) >> 1;
				odd ^= (q->len & 1);
			} else
				odd = 0;
		} else {
			chip_copyout (buf, q->payload, q->len);
			buf += (q->len >> 1);
			odd ^= (q->len & 1);
		}
	}

	unsigned len = p->tot_len;
	while (len < 60) {
		*buf++ = 0;
		len += 2;
	}

	ETH_TXDESC[0].PTRL = 0x1000;
	ETH_TXDESC[0].PTRH = 0;
	ETH_TXDESC[0].LEN = len;
	ETH_TXDESC[0].CTRL = DESC_TX_RDY | DESC_TX_IRQ_EN | DESC_TX_WRAP;

	++u->netif.out_packets;
	u->netif.out_bytes += len;

/*debug_printf ("tx %d bytes: ", len);
debug_printf ("%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x\n",
ETH_TXBUF[0], ETH_TXBUF[1], ETH_TXBUF[2], ETH_TXBUF[3],
ETH_TXBUF[4], ETH_TXBUF[5], ETH_TXBUF[6], ETH_TXBUF[7],
ETH_TXBUF[8], ETH_TXBUF[9], ETH_TXBUF[10], ETH_TXBUF[11],
ETH_TXBUF[12], ETH_TXBUF[13]);*/
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
	mutex_lock (&u->netif.lock);

	/* Exit if link has failed */
	if (p->tot_len < 4 || p->tot_len > K5600BG1_MTU /*||
	    ! (phy_read (u, PHY_STS) & PHY_STS_LINK)*/) {
		++u->netif.out_errors;
		mutex_unlock (&u->netif.lock);
/*debug_printf ("output: transmit %d bytes, link failed\n", p->tot_len);*/
		buf_free (p);
		return 0;
	}
/*debug_printf ("output: transmit %d bytes\n", p->tot_len);*/

	if (! (ETH_TXDESC[0].CTRL & DESC_TX_RDY)) {
		/* Смело отсылаем. */
		transmit_packet (u, p);
		mutex_unlock (&u->netif.lock);
		buf_free (p);
		return 1;
	}

	/* Занято, ставим в очередь. */
	if (buf_queue_is_full (&u->outq)) {
		/* Нет места в очереди: теряем пакет. */
		++u->netif.out_discards;
		mutex_unlock (&u->netif.lock);
/*		debug_printf ("k5600bg1_output: overflow\n");*/
		buf_free (p);
		return 0;
	}
	buf_queue_put (&u->outq, p);
	mutex_unlock (&u->netif.lock);
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

	ETH_REG->MAC_ADDR[0] = u->netif.ethaddr[0] | (u->netif.ethaddr[1] << 8);
	ETH_REG->MAC_ADDR[1] = u->netif.ethaddr[2] | (u->netif.ethaddr[3] << 8);
	ETH_REG->MAC_ADDR[2] = u->netif.ethaddr[4] | (u->netif.ethaddr[5] << 8);

	mutex_unlock (&u->netif.lock);
}

/*
 * Fetch the received packet from the network controller.
 * Put it to input queue.
 */
static void
receive_packet (k5600bg1_t *u, unsigned desc_rx, unsigned len, unsigned ptr)
{
	if (desc_rx & DESC_RX_OR) {
		/* Count lost incoming packets. */
		u->netif.in_discards++;
	}
	if (desc_rx & (DESC_RX_EF | DESC_RX_CRC_ERR | DESC_RX_SMB_ERR)) {
		/* Invalid frame */
debug_printf ("receive_data: failed, desc_rx=%b\n", desc_rx, DESC_RX_BITS);
		++u->netif.in_errors;
		return;
	}
	if (len < 4 || len > K5600BG1_MTU) {
		/* Skip this frame */
debug_printf ("receive_data: bad length %d bytes, desc_rx=%b\n", len, desc_rx, DESC_RX_BITS);
		++u->netif.in_errors;
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
/*debug_printf ("receive_data: %d bytes, rn=%u, PTRL=%#04x, CTRL=%b\n", len, u->rn, ptr, desc_rx, DESC_RX_BITS);
debug_printf ("              RXBF HEAD:TAIL=%04x:%04x\n", ETH_REG->RXBF_HEAD, ETH_REG->RXBF_TAIL);*/

	if (buf_queue_is_full (&u->inq)) {
debug_printf ("receive_data: input overflow\n");
		++u->netif.in_discards;
		return;
	}

	/* Allocate a buf chain with total length 'len' */
	buf_t *p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
debug_printf ("receive_data: ignore packet - out of memory\n");
		++u->netif.in_discards;
		return;
	}

	/* Copy the packet data. */
	chip_copyin (p->payload, ETH_RXBUF + ptr, len);
	buf_queue_put (&u->inq, p);
/*debug_printf ("[%d]", p->tot_len); buf_print_ethernet (p);*/
/*unsigned short *rx = (unsigned short*) p->payload;
debug_printf ("              %04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-\n",
rx[0], rx[1], rx[2], rx[3], rx[4], rx[5], rx[6], rx[7]);
debug_printf ("             -%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-\n",
rx[8+0], rx[8+1], rx[8+2], rx[8+3], rx[8+4], rx[8+5], rx[8+6], rx[8+7]);
debug_printf ("             -%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x-\n",
rx[16+0], rx[16+1], rx[16+2], rx[16+3], rx[16+4], rx[16+5], rx[16+6], rx[16+7]);
debug_printf ("             -%04x-%04x-%04x-%04x-%04x-%04x-%04x-%04x\n",
rx[24+0], rx[24+1], rx[24+2], rx[24+3], rx[24+4], rx[24+5], rx[24+6], rx[24+7]);*/
}

/*
 * Process an interrupt.
 * Return nonzero when there was some activity.
 */
static unsigned
handle_interrupt (k5600bg1_t *u)
{
	unsigned active = 0;
	u->intr_flags = ETH_REG->INT_SRC;
//debug_printf ("<%x> ", u->intr_flags);
	for (;;) {
		unsigned desc_rx = ETH_RXDESC[u->rn].CTRL;
		if (desc_rx & DESC_RX_RDY)
			break;
		++active;

		/* Fetch the received packet. */
		unsigned len = ETH_RXDESC[u->rn].LEN;
		unsigned ptr = ETH_RXDESC[u->rn].PTRL;
		receive_packet (u, desc_rx, len-4, ptr);

		/* Корректируем указатель на свободное место буфера. */
		ETH_REG->RXBF_HEAD = ptr + (len >> 1) - 1;
/*debug_printf ("HEAD = %04x (%04x), %x words, PTRL = %04x\n", ETH_REG->RXBF_HEAD, ptr + (len >> 1) - 1, len >> 1, ptr);*/

		/* Освобождаем дескриптор. */
		ETH_RXDESC[u->rn].CTRL = DESC_RX_RDY | DESC_RX_IRQ_EN;
		if (u->rn == NRD-1)
			ETH_RXDESC[NRD-1].CTRL |= DESC_RX_WRAP;
		u->rn++;
		if (u->rn >= NRD)
			u->rn = 0;
	}
	unsigned desc_tx = ETH_TXDESC[0].CTRL;
	if (! (desc_tx & DESC_TX_RDY)) {
		if (desc_tx & DESC_TX_IRQ_EN) {
			/* Закончена передача пакета. */
			ETH_TXDESC[0].CTRL = DESC_TX_WRAP;
			++active;
/*debug_printf ("eth tx finished: desc_tx = %b\n", desc_tx, DESC_TX_BITS);*/

			/* Подсчитываем коллизии. */
			if (desc_tx & (DESC_TX_RL | DESC_TX_LC)) {
				++u->netif.out_collisions;
			}
		}

		/* Извлекаем следующий пакет из очереди. */
		buf_t *p = buf_queue_get (&u->outq);
		if (p) {
			/* Передаём следующий пакет. */
			transmit_packet (u, p);
			buf_free (p);
		}
	}
	return active;
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
interrupt_task (void *arg)
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
	task_create (interrupt_task, u, "eth", prio, u->stack, sizeof (u->stack));
}
