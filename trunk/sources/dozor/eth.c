/*
 * Ethernet controller driver for Milandr 5600ВГ1.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <dozor/eth.h>
#include <dozor/eth-regs.h>

//========================================================================================================================
/*
 * Адреса регистров и буферов приёма/передачи.
 */
#define ETH_RXBUF	((eth_reg_t*)  (0x0000))
#define ETH_RXDESC	((eth_desc_t*) (0x0800))
#define ETH_TXBUF	((eth_reg_t*)  (0x1000))
#define ETH_TXDESC	((eth_desc_t*) (0x1800))
#define ETH_REG		((eth_regs_t*) (0x1FC0))

#define K5600BG1_IRQ	16		/* pin PC10 - Timer 3 ETR */
#define K5600BG1_MTU	1518		/* maximum ethernet frame length */

#define PORTB_NIRQ	(1 << 10)	/* nIRQ on PB10 */
#define PORTB_NRST	(1 << 11)	/* nRST on PB11 */
#define PORTC_NOE	(1 << 1)	/* nOE on PC1 */
#define PORTC_NWE	(1 << 2)	/* nWE on PC2 */
#define PORTE_NCS	(1 << 12)	/* nCS on PE12 */

#define NRD		8		/* number of receive descriptors */
#define RXBUF_BYTES	2048		/* size of hardware receive buffer */

//========================================================================================================================
#define HWREG(x)				(*((volatile unsigned long *)(x)))

extern int Read_Ethernet_Status(void);
extern void Read_Ethernet_Register(unsigned short r, unsigned short *v);
extern void Read_Ethernet_Descriptor(unsigned short d, unsigned short *v);
extern void Read_Ethernet_Buffer(unsigned short b, unsigned short *v);
extern void Write_Ethernet_Register(unsigned short r, unsigned short *v);
extern void Write_Ethernet_Descriptor(unsigned short d, unsigned short *v);
extern void Write_Ethernet_Buffer(unsigned short b, unsigned short *v);

unsigned short mac_ctrl, min_frame, max_frame, collconf;
unsigned short ipgt, mac_addr_t, mac_addr_m, mac_addr_h;
unsigned short hash0, hash1, hash2, hash3;
unsigned short int_mask, int_src, phy_ctrl, phy_stat;
unsigned short rxbf_head, rxbf_tail, reserved0, reserved1;
unsigned short stat_rx_all, stat_rx_ok, stat_rx_ovf, stat_rx_lost;
unsigned short stat_tx_all, stat_tx_ok, base_rxbf, base_txbf;
unsigned short base_rxbd, base_txbd, base_reg, gctrl;

unsigned short rx_ctrl, rx_len, rx_ptrl, rx_ptrh;
unsigned short tx_ctrl, tx_len, tx_ptrl, tx_ptrh;
//========================================================================================================================

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

//========================================================================================================================
void Initialize_Timer_3(void)
{
	ARM_RSTCLK->PER_CLOCK |= 0x00010000;			// Timer 3 Ticks On
	ARM_RSTCLK->TIM_CLOCK |= 0x04000000;
	ARM_TIMER3->TIM_CNTRL	= 0x00000000;
	ARM_TIMER3->TIM_CNT = 0;
	ARM_TIMER3->TIM_PSG = 0;
	ARM_TIMER3->TIM_ARR = 0;
	ARM_TIMER3->TIM_IE = 0x00000008;				// Timer 3 Interrupts Enable
	HWREG(0xE000E100) |= 0x00010000;				// Timer 3 Interrupts Enable
}

/*
 * Set default values to Ethernet controller registers.
 */
static void
chip_init (k5600bg1_t *u)
{
	/* Включение тактовой частоты портов A-C, E, F. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA | ARM_PER_CLOCK_GPIOB
						   | ARM_PER_CLOCK_GPIOC | ARM_PER_CLOCK_GPIOE
						   | ARM_PER_CLOCK_GPIOF | ARM_PER_CLOCK_EXT_BUS;

	ARM_GPIOA->ANALOG = 0x0000FFFF;
	ARM_GPIOA->FUNC = 0x55555555;
	ARM_GPIOA->DATA = 0x00000000;
	ARM_GPIOA->OE = 0x0000FFFF;
	ARM_GPIOA->PULL = 0x00000000;
	ARM_GPIOA->PD = 0x00000000;
	ARM_GPIOA->PWR = 0xBBBBBBBB;
	ARM_GPIOA->GFEN = 0x00000000;

	ARM_GPIOB->ANALOG = 0x0000FFFF;
	ARM_GPIOB->FUNC = 0x55555555;
	ARM_GPIOB->DATA = 0x00000000;
	ARM_GPIOB->OE = 0x0000FFFF;
	ARM_GPIOB->PULL = 0x00000000;
	ARM_GPIOB->PD = 0x00000000;
	ARM_GPIOB->PWR = 0xBBBBBBBB;
	ARM_GPIOB->GFEN = 0x00000000;

	ARM_GPIOC->ANALOG = 0x000004FF;
	ARM_GPIOC->FUNC = 0x00205554;
	ARM_GPIOC->DATA = 0x00000001;
	ARM_GPIOC->OE = 0x000000FF;
	ARM_GPIOC->PULL = 0x00000000;
	ARM_GPIOC->PD = 0x00000000;
	ARM_GPIOC->PWR = 0x0030FFFF;
	ARM_GPIOC->GFEN = 0x00000000;

	ARM_GPIOD->ANALOG = 0x00001400;
	ARM_GPIOD->FUNC = 0x01200000;
	ARM_GPIOD->DATA = 0x00000000;
	ARM_GPIOD->OE = 0x00001400;
	ARM_GPIOD->PULL = 0x00000000;
	ARM_GPIOD->PD = 0x00000000;
	ARM_GPIOD->PWR = 0x03300000;
	ARM_GPIOD->GFEN = 0x00000000;

	ARM_GPIOE->ANALOG = 0x0000FFFF;
	ARM_GPIOE->FUNC = 0x55555555;
	ARM_GPIOE->DATA = 0x00000000;
	ARM_GPIOE->OE = 0x0000FFFF;
	ARM_GPIOE->PULL = 0x0000FFFF;
	ARM_GPIOE->PD = 0x00000000;
	ARM_GPIOE->PWR = 0xFFFFFFFF;
	ARM_GPIOE->GFEN = 0x00000000;

	ARM_GPIOF->ANALOG = 0x0000FFFF;
	ARM_GPIOF->FUNC = 0x5555555F;
	ARM_GPIOF->DATA = 0x00000000;
	ARM_GPIOF->OE = 0x0000FFFE;
	ARM_GPIOF->PULL = 0x0000FFFC;
	ARM_GPIOF->PD = 0x00000000;
	ARM_GPIOF->PWR = 0xFFFFFFF5;
	ARM_GPIOF->GFEN = 0x00000000;

	/* Включение внешней шины адрес/данные в режиме RAM.
	 * Длительность цикла должна быть не меньше 112.5 нс.
	 * При частоте процессора 40 МГц (один такт 25 нс)
	 * установка ws=11 или 12 даёт цикл в 150 нс.
	 * Проверено: 125 нс (ws=9) недостаточно. */
	ARM_EXTBUS->CONTROL = ARM_EXTBUS_RAM | ARM_EXTBUS_WS (15);
//========================================================================================================================
//	chip_select (1);
	/* Режимы параллельного интерфейса к процессору. */
	gctrl = GCTRL_GLBL_RST;
	Write_Ethernet_Register(ETH_GCTRL,&gctrl);
	udelay (1);
	gctrl = GCTRL_READ_CLR_STAT | GCTRL_SPI_RST |
			GCTRL_ASYNC_MODE | GCTRL_SPI_TX_EDGE | GCTRL_SPI_DIR |
			GCTRL_SPI_FRAME_POL | GCTRL_SPI_DIV(2);
	Write_Ethernet_Register(ETH_GCTRL,&gctrl);

	/* Общие режимы. */
	mac_ctrl = MAC_CTRL_PRO_EN |	// Прием всех пакетов
			   MAC_CTRL_BCA_EN |		// Прием всех широковещательных пакетов
			   MAC_CTRL_HALFD_EN |		// Полудуплексный режим
			   MAC_CTRL_SHORT_FRAME_EN;	// Прием коротких пакетов
	Write_Ethernet_Register(ETH_MAC_CTRL,&mac_ctrl);

	/* Режимы PHY. */
	phy_ctrl = PHY_CTRL_DIR |	// Прямой порядок битов в полубайте
			   PHY_CTRL_RXEN | PHY_CTRL_TXEN |	// Включение приёмника и передатчика
			   PHY_CTRL_HALFD |		// Полудуплесный режим
			   PHY_CTRL_LINK_PERIOD (11);	// Период LINK-импульсов
	Write_Ethernet_Register(ETH_PHY_CTRL,&phy_ctrl);

	/* Свой адрес. */
	mac_addr_t = u->netif.ethaddr[0] | (u->netif.ethaddr[1] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_T,&mac_addr_t);
	mac_addr_m = u->netif.ethaddr[2] | (u->netif.ethaddr[3] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_M,&mac_addr_m);
	mac_addr_h = u->netif.ethaddr[4] | (u->netif.ethaddr[5] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_H,&mac_addr_h);

	min_frame = 64;																// Минимальная длина пакета
	Write_Ethernet_Register(ETH_MIN_FRAME,&min_frame);
	max_frame = K5600BG1_MTU + 4;												// Максимальная длина пакета
	Write_Ethernet_Register(ETH_MAX_FRAME,&max_frame);
	collconf = COLLCONF_COLLISION_WINDOW (64) | COLLCONF_RETRIES_LIMIT (15);	// Лимит повторов передачи
	Write_Ethernet_Register(ETH_COLLCONF,&collconf);
	ipgt = 96;																	// Межпакетный интервал
	Write_Ethernet_Register(ETH_IPGT,&ipgt);

	rxbf_tail = 0;
	Write_Ethernet_Register(ETH_RXBF_TAIL,&rxbf_tail);
	rxbf_head = RXBUF_BYTES/2 - 1;
	Write_Ethernet_Register(ETH_RXBF_HEAD,&rxbf_head);

	/* Начальное состояние дескрипторов. */
	unsigned int i;
	unsigned short v;
	for (i=0; i<(NRD*4); i++)
	  {
		if ((i % 4) == 0) v = DESC_RX_RDY | DESC_RX_IRQ_EN; else v = 0x0000;
		Write_Ethernet_Descriptor(Rx_Descriptors_Base + i, &v);
		v = 0x0000;
		Write_Ethernet_Descriptor(Tx_Descriptors_Base + i, &v);
	  }
	v = DESC_RX_RDY | DESC_RX_WRAP | DESC_RX_IRQ_EN;
	Write_Ethernet_Descriptor(Rx_Descriptors_Base + ((NRD - 1) * 4), &v);
	v = DESC_TX_WRAP;
	Write_Ethernet_Descriptor(Tx_Descriptors_Base, &v);
	/* Ждём прерывания по приёму и передаче. */
	int_mask = INT_TXF | INT_RXF | INT_RXS | INT_RXE;
	Write_Ethernet_Register(ETH_INT_MASK,&int_mask);
	Initialize_Timer_3();
}

void
k5600bg1_debug (k5600bg1_t *u, struct _stream_t *stream)
{
//	unsigned short mac_ctrl, phy_ctrl, phy_stat, gctrl;
//	unsigned short rxbf_head, rxbf_tail;
//	unsigned short tx_ctrl, rx_ctrl, rx_len, rx_ptr;
//	unsigned short stat_rx_all, stat_rx_ok, stat_rx_ovf;
//	unsigned short stat_rx_lost, stat_tx_all, stat_tx_ok;

	mutex_lock (&u->netif.lock);

	Read_Ethernet_Register(ETH_GCTRL, &gctrl);
	Read_Ethernet_Register(ETH_MAC_CTRL, &mac_ctrl);
	Read_Ethernet_Register(ETH_PHY_CTRL, &phy_ctrl);
	Read_Ethernet_Register(ETH_PHY_STAT, &phy_stat);
	Read_Ethernet_Register(ETH_RXBF_HEAD, &rxbf_head);
	Read_Ethernet_Register(ETH_RXBF_TAIL, &rxbf_tail);
	Read_Ethernet_Descriptor(Rx_Descriptors_Base + u->rn + 0, &rx_ctrl);
	Read_Ethernet_Descriptor(Rx_Descriptors_Base + u->rn + 1, &rx_len);
	Read_Ethernet_Descriptor(Rx_Descriptors_Base + u->rn + 2, &rx_ptrl);
	Read_Ethernet_Descriptor(Tx_Descriptors_Base + u->rn + 0, &tx_ctrl);
	Read_Ethernet_Register(ETH_STAT_RX_ALL,&stat_rx_all);
	Read_Ethernet_Register(ETH_STAT_RX_OK,&stat_rx_ok);
	Read_Ethernet_Register(ETH_STAT_RX_OVF,&stat_rx_ovf);
	Read_Ethernet_Register(ETH_STAT_RX_LOST,&stat_rx_lost);
	Read_Ethernet_Register(ETH_STAT_TX_ALL,&stat_tx_all);
	Read_Ethernet_Register(ETH_STAT_TX_OK,&stat_tx_ok);

	mutex_unlock (&u->netif.lock);

	printf (stream, "GCTRL = %b\n", gctrl, GCTRL_BITS);
	printf (stream, "MAC_CTRL = %b\n", mac_ctrl, MAC_CTRL_BITS);
	printf (stream, "PHY_CTRL = %b\n", phy_ctrl, PHY_CTRL_BITS);
	printf (stream, "PHY_STAT = %b\n", phy_stat, PHY_STAT_BITS);
	printf (stream, "INT_SRC = %b\n", u->intr_flags, INT_BITS);
	printf (stream, "RXBF HEAD:TAIL = %04x:%04x\n", rxbf_head, rxbf_tail);
	printf (stream, "RXDESC[%u].CTRL = %b\n", u->rn, rx_ctrl, DESC_RX_BITS);
	if (! (rx_ctrl & DESC_RX_RDY)) printf (stream, "    .LEN = %u, .PTRL = %04x\n", rx_len, rx_ptrl);
	printf (stream, "   TXDESC.CTRL = %b\n", tx_ctrl, DESC_TX_BITS);
	printf (stream, "STAT_RX_ALL = %u\n", stat_rx_all);
	printf (stream, "STAT_RX_OK = %u\n", stat_rx_ok);
	printf (stream, "STAT_RX_OVF = %u\n", stat_rx_ovf);
	printf (stream, "STAT_RX_LOST = %u\n", stat_rx_lost);
	printf (stream, "STAT_TX_ALL = %u\n", stat_tx_all);
	printf (stream, "STAT_TX_OK = %u\n", stat_tx_ok);
}

int
k5600bg1_get_carrier (k5600bg1_t *u)
{
	mutex_lock (&u->netif.lock);
	Read_Ethernet_Register(ETH_PHY_STAT,&phy_stat);
	mutex_unlock (&u->netif.lock);
	return (phy_stat & PHY_STAT_LINK) != 0;
}

long
k5600bg1_get_speed (k5600bg1_t *u, int *duplex)
{
	mutex_lock (&u->netif.lock);
	Read_Ethernet_Register(ETH_PHY_STAT,&phy_ctrl);
	Read_Ethernet_Register(ETH_PHY_STAT,&phy_stat);
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
	mutex_lock (&u->netif.lock);
	Read_Ethernet_Register(ETH_PHY_CTRL,&phy_ctrl);
	if (on) {
		phy_ctrl |= PHY_CTRL_LB;
	} else {
		phy_ctrl &= ~PHY_CTRL_LB;
	}
	Write_Ethernet_Register(ETH_PHY_CTRL,&phy_ctrl);
	mutex_unlock (&u->netif.lock);
}

void
k5600bg1_set_promisc (k5600bg1_t *u, int station, int group)
{
	mutex_lock (&u->netif.lock);
	Read_Ethernet_Register(ETH_MAC_CTRL,&mac_ctrl);
	mac_ctrl = mac_ctrl & ~MAC_CTRL_PRO_EN;
	if (station) {
		/* Accept any unicast. */
		mac_ctrl |= MAC_CTRL_PRO_EN;
	}
	/* TODO: multicasts. */
	Write_Ethernet_Register(ETH_MAC_CTRL,&mac_ctrl);
	mutex_unlock (&u->netif.lock);
}
static void
chip_copyin (unsigned char *data, unsigned chipaddr, unsigned bytes)
{
	unsigned short word;
	unsigned short *pshort = (unsigned short *) data;

	while (bytes >= 2) {
		Read_Ethernet_Buffer(chipaddr,&word);
		chipaddr++;
		*pshort++ = word;
		bytes -= 2;
		if (chipaddr >= (unsigned)(ETH_RXBUF + RXBUF_BYTES))
			chipaddr = (unsigned)ETH_RXBUF;
	}
	if (bytes > 0) {
		data = (unsigned char *) pshort;
		Read_Ethernet_Buffer(chipaddr,&word);
		*data = word;
	}
}

static void
chip_copyout (unsigned chipaddr, unsigned char *data, unsigned bytes)
{
	unsigned short word;
	unsigned short *pshort = (unsigned short *) data;

	while (bytes >= 2) {
		word = *data++;
		word |= (*data++ << 8);
		Write_Ethernet_Buffer(chipaddr,&word);
		chipaddr++;
		bytes -= 2;
	}
	if (bytes > 0) {
		word = *data;
		Write_Ethernet_Buffer(chipaddr,&word);
	}
}

/*
 * Write the packet to chip memory and start transmission.
 * Deallocate the packet.
 */
static void
transmit_packet (k5600bg1_t *u, buf_t *p)
{
	unsigned short ptrl, ptrh, lngth, ctrl;

	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */
	buf_t *q;

	unsigned addr = Tx_Buffers_Base;

	unsigned odd = 0;
	unsigned short last_word;

	for (q=p; q; q=q->next) {
		/* Copy the packet into the transmit buffer. */
		assert (q->len > 0);
		if (odd) {
			last_word |= (*q->payload << 8);
			Write_Ethernet_Buffer(addr++,&last_word);
			if (q->len > 1) {
				chip_copyout (addr, q->payload + 1, q->len - 1);
				addr += ((q->len - 1) >> 1);
				odd ^= (q->len & 1);
			} else
				odd = 0;
		} else {
			chip_copyout (addr, q->payload, q->len);
			addr += (q->len >> 1);
			odd ^= (q->len & 1);
			last_word = *(q->payload + q->len - 1);
		}
	}

	unsigned len = p->tot_len;
	if (odd) {
		addr++;
		len++;
	}
	last_word = 0;
	while (len < 60) {
		Write_Ethernet_Buffer(addr++,&last_word);
		len += 2;
	}

	ptrl = 0x1000;
	Write_Ethernet_Descriptor(Tx_Descriptors_Base + 3,&ptrl);
	ptrh = 0x0000;
	Write_Ethernet_Descriptor(Tx_Descriptors_Base + 2,&ptrh);
	lngth = len;
	Write_Ethernet_Descriptor(Tx_Descriptors_Base + 1,&lngth);
	ctrl = DESC_TX_RDY | DESC_TX_IRQ_EN | DESC_TX_WRAP;
	Write_Ethernet_Descriptor(Tx_Descriptors_Base + 0,&ctrl);

	++u->netif.out_packets;
	u->netif.out_bytes += len;
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
	if (p->tot_len < 4 || p->tot_len > K5600BG1_MTU) {
		++u->netif.out_errors;
		mutex_unlock (&u->netif.lock);
		buf_free (p);
		return 0;
	}

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

	mac_addr_t = u->netif.ethaddr[0] | (u->netif.ethaddr[1] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_T,&mac_addr_t);
	mac_addr_m = u->netif.ethaddr[2] | (u->netif.ethaddr[3] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_M,&mac_addr_m);
	mac_addr_h = u->netif.ethaddr[4] | (u->netif.ethaddr[5] << 8);
	Write_Ethernet_Register(ETH_MAC_ADDR_H,&mac_addr_h);

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
		++u->netif.in_errors;
		return;
	}
	if (len < 4 || len > K5600BG1_MTU) {
		/* Skip this frame */
		++u->netif.in_errors;
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;

	if (buf_queue_is_full (&u->inq)) {
		++u->netif.in_discards;
		return;
	}

	/* Allocate a buf chain with total length 'len' */
	buf_t *p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
		++u->netif.in_discards;
		return;
	}

	/* Copy the packet data. */
	chip_copyin (p->payload, Rx_Buffers_Base + ptr, len);

	buf_queue_put (&u->inq, p);

}

/*
 * Process an interrupt.
 * Return nonzero when there was some activity.
 */
static unsigned
handle_interrupt (k5600bg1_t *u)
{
	unsigned active = 0;
	Read_Ethernet_Register(ETH_INT_SRC, &u->intr_flags);
	for (;;) {
		unsigned short desc_rx;
		Read_Ethernet_Descriptor((Rx_Descriptors_Base + (u->rn * 4) + 0), &desc_rx);

		if (desc_rx & DESC_RX_RDY)
			break;
		++active;

		/* Fetch the received packet. */
		unsigned short len;
		unsigned short ptr;
		Read_Ethernet_Descriptor((Rx_Descriptors_Base + (u->rn * 4) + 1), &len);
		Read_Ethernet_Descriptor((Rx_Descriptors_Base + (u->rn * 4) + 3), &ptr);

		receive_packet (u, desc_rx, len-4, ptr);

		/* Корректируем указатель на свободное место буфера. */
		rxbf_head = ptr + (len >> 1) - 1;
		Write_Ethernet_Register(ETH_RXBF_HEAD,&rxbf_head);

		/* Освобождаем дескриптор. */
		desc_rx = DESC_RX_RDY | DESC_RX_IRQ_EN;
		Write_Ethernet_Descriptor((Rx_Descriptors_Base + (u->rn * 4) + 0), &desc_rx);
		if (u->rn == NRD-1) {
			desc_rx = DESC_RX_RDY | DESC_RX_WRAP | DESC_RX_IRQ_EN;
			Write_Ethernet_Descriptor((Rx_Descriptors_Base + ((NRD-1) * 4) + 0), &desc_rx);
		}
		u->rn++;
		if (u->rn >= NRD)
			u->rn = 0;
	}

	unsigned short desc_tx;
	Read_Ethernet_Descriptor((Tx_Descriptors_Base + 0), &desc_tx);

	if (! (desc_tx & DESC_TX_RDY)) {
		if (desc_tx & DESC_TX_IRQ_EN) {
			/* Закончена передача пакета. */
			desc_tx = DESC_TX_WRAP;
			Write_Ethernet_Descriptor((Tx_Descriptors_Base + 0), &desc_tx);
			++active;

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
	ARM_TIMER3->TIM_STATUS = 0x00000000;
	HWREG(0xE000E280) |= 0x00010000;
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
	struct _arp_t *arp, const unsigned char *macaddr)
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
	//task_create (interrupt_task, u, "eth", prio, u->stack, sizeof (u->stack));
}
