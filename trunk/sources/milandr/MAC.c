//
// Определения функций и внутренних переменных.
//
// Project:	Ethernet MAC
// Author:	Stanislav V. Afanas'ev
// Company:	Milandr
// File:	MAC.c
// Version:	1.2
// Date:	22.11.07
//
#include "MAC.h"

#define	irq_INT0	0x10
#define	irq_INT1	0x11
#define	irq_INT2	0x12
#define	irq_INT3	0x13

t_PACK_state stat_TX_PKG;
t_PACK_state stat_RX_PKG;

unsigned int free_size_TX = BUFF_SIZE;

static unsigned int *ptr_TX_BUFF;
t_INT_SOURCE int_SRC;

unsigned TMP;
unsigned *dst;
unsigned *src;
unsigned err;
unsigned err_count;

void MAC_reset (void)
{
	MAC_RG->GCTRL.bit.GLBL_RST = 1;

	RX_Descriptors[0].RX_ctrl.all = 0;
	TX_Descriptors[0].TX_ctrl.all = 0;
	ptr_TX_BUFF = (void *) dflt_addr_Tx_BF;

	MAC_MEM_clear();
	MAC_MEM_test();
	MAC_MEM_clear();
}

void MAC_MEM_clear (void)
{
	unsigned i;

	for (i = 0; i < sizeof(TX_buff); i++)
		TX_buff[i] = 0;

	for (i = 0; i < sizeof(RX_buff); i++)
		RX_buff[i] = 0;
}

void MAC_MEM_test (void)
{
	unsigned int i;
	err = 0;
	err_count = 0;

	for (i = 0; i < sizeof(TX_buff); i++)
		TX_buff[i] = 0xFFFF;
	for (i = 0; i < sizeof(RX_buff); i++)
		RX_buff[i] = 0xFFFF;
	for (i = 0; i < sizeof(TX_buff); i++)
		if (TX_buff[i] != 0xFFFF)
			err_count = err + 1;
	for (i = 0; i < sizeof(RX_buff); i++)
		if (RX_buff[i] != 0xFFFF)
			err_count = err + 1;

	MAC_MEM_clear();

	for (i = 0; i < sizeof(TX_buff); i++)
		MAC_MEM_CELL_test (&TX_buff[i], i);
	for (i = 0; i < sizeof(RX_buff); i++)
		MAC_MEM_CELL_test (&RX_buff[i], i);
}

void MAC_MEM_CELL_test (unsigned *addr, unsigned value)
{
	*addr = value;
	if (*addr ^ value)
		err_count = err + 1;
	//printf("RX_buff init error at %X (%x)\n",addr,*addr);
/*
	*addr ^= value;
	if (*addr ^ value)
		err_count = err + 1;
	//printf("RX_buff init error at %X (%x)\n",addr,*addr);

	*addr ^= value;
	if (*addr)
		err_count = err + 1;
	//printf("RX_buff init error at %X (%x)\n",addr,*addr);
*/
}

void MAC_init (void)
{
	MAC_RG->GCTRL.all	= dflt_GCTRL;
	MAC_RG->base_MAC_RG	= dflt_addr_RG;
	MAC_RG->base_MAC_RxBF	= dflt_addr_Rx_BF;
	MAC_RG->base_MAC_TxBF	= dflt_addr_Tx_BF;
	MAC_RG->base_MAC_RxBD	= dflt_addr_Rx_BD;
	MAC_RG->base_MAC_TxBD	= dflt_addr_Tx_BD;

	MAC_RG->MAC_CTRL.all	= dflt_MAC_CTRL;
//BIT_TX_RST | BIT_RX_RST | BIT_READ_CLR_STAT | BIT_BIG_ENDIAN
//BIT_PRO_EN | BIT_BCA_EN | BIT_MCA_EN | BIT_CTRL_FRAME_EN | BIT_LONG_FRAME_EN | BIT_SHORT_FRAME_EN | BIT_ERR_FRAME_EN
//BIT_HALFD_EN | BIT_BCKOF_DIS | BIT_LB_EN

	MAC_RG->PACKETLEN.MIN_FRAME	= dflt_MinFrame;
	MAC_RG->PACKETLEN.MAX_FRAME	= dflt_MaxFrame;
	MAC_RG->COLLCONF.all		= dflt_CollConfig;
	MAC_RG->IPGT			= dflt_IPGTx;

	MAC_RG->MAC_ADDR[0]	= dflt_MAC_ADDR_H;
	MAC_RG->MAC_ADDR[1]	= dflt_MAC_ADDR_M;
	MAC_RG->MAC_ADDR[2]	= dflt_MAC_ADDR_T;
	MAC_RG->INT_MASK.all	= dflt_INT_MASK;
	MAC_RG->HASH[0]		= 0;
	MAC_RG->HASH[1]		= 0;
	MAC_RG->HASH[2]		= 0;
	MAC_RG->HASH[3]		= 0x8000;

	MAC_RG->PHY_CTRL.all	= dflt_PHY_CTRL;
//BIT_PHY_RST | BIT_PHY_EXT_EN | BIT_PHY_ERLY_DV | BIT_PHY_HALFD | BIT_PHY_DLB | BIT_PHY_LB
	MAC_RG->RXBF_HEAD	= dflt_addr_Rx_BF + sizeof_RxBF - 1;
	MAC_RG->RXBF_TAIL	= dflt_addr_Rx_BF;
}

int chk_Send_OK (unsigned *Dscr_Num)
{
	if (! TX_Descriptors[*Dscr_Num].TX_ctrl.bit.RDY) {
		stat_TX_PKG.num	 = *Dscr_Num;
		stat_TX_PKG.stat = TX_Descriptors[*Dscr_Num].TX_ctrl.all;
		free_size_TX	+= TX_Descriptors[*Dscr_Num].len;
		if (free_size_TX > BUFF_SIZE)
			free_size_TX = BUFF_SIZE;
		return 0;
	} else
		return -1;
}

int chk_Receive_Ready (unsigned *Dscr_Num)
{
	if (! RX_Descriptors[*Dscr_Num].RX_ctrl.bit.EMPTY) {
		stat_RX_PKG.num	 = *Dscr_Num;
		stat_RX_PKG.stat = RX_Descriptors[*Dscr_Num].RX_ctrl.all;
		return 0;
	} else
		return -1;
}

int Send_ETH_Pack (unsigned *Dscr_Num, t_ETH_Pack *PKG, unsigned PARAM)
{
	int i;

	if (PKG->len > free_size_TX)
		return -1;
	if (TX_Descriptors[*Dscr_Num].TX_ctrl.bit.RDY)
		return -2;

	TX_Descriptors[*Dscr_Num].data = ptr_TX_BUFF;
	TX_Descriptors[*Dscr_Num].len = PKG->len +14;

	dst = ptr_TX_BUFF;

	src = (void*) PKG->DA;
	for (i = 3; i > 0; i--) {
		*dst++ = *src++;
		dst = (void*) (MAC_TX_BUFF_BASE_ADDR | ((unsigned) dst & (BUFF_SIZE-1)));
	}

	src = (void*) PKG->SA;
	for (i = 3; i > 0; i--) {
		*dst++ = *src++;
		dst = (void*) (MAC_TX_BUFF_BASE_ADDR | ((unsigned) dst & (BUFF_SIZE-1)));
	}

	*dst++ = PKG->len;
	dst = (void*) (MAC_TX_BUFF_BASE_ADDR | ((unsigned) dst & (BUFF_SIZE-1)));

	src = (void*) PKG->data;
	for (i = (PKG->len/2)+1; i > 0; i--) {
		*dst++ = *src++;
		dst = (void*) (MAC_TX_BUFF_BASE_ADDR | ((unsigned) dst & (BUFF_SIZE-1)));
	}

	ptr_TX_BUFF = dst;
	TX_Descriptors[(*Dscr_Num)++].TX_ctrl.all = PARAM;

	if (PARAM & BIT_TX_DSCR_WRAP)
		*Dscr_Num = 0;
	else
		*Dscr_Num &= ((DSCR_CNT*DSCR_SIZE)-1);
	free_size_TX -= PKG->len;
	return 0;
}

int Receive_ETH_Pack (unsigned *Dscr_Num, t_ETH_Pack *PKG, unsigned PARAM)
{
	int i;
	unsigned len;

	len = RX_Descriptors[*Dscr_Num].len;
	if (len < 16)
		return -1;

	src = (void*) RX_Descriptors[*Dscr_Num].data;
	dst = (void*) PKG->DA;
	for (i = 3; i > 0; i--) {
		*dst++ = *src++;
		src = (void*) (MAC_RX_BUFF_BASE_ADDR | ((unsigned) src & (BUFF_SIZE-1)));
	}

	dst = (void*) PKG->SA;
	for (i = 3; i > 0; i--) {
		*dst++ = *src++;
		src = (void*) (MAC_RX_BUFF_BASE_ADDR | ((unsigned) src & (BUFF_SIZE-1)));
	}

	PKG->len = *src++;
	src = (void*) (MAC_RX_BUFF_BASE_ADDR | ((unsigned) src & (BUFF_SIZE-1)));

	dst = (void*) PKG->data;
	for (i = ((PKG->len / 2)+(PKG->len & 0x1)); i > 0; i--) {
		*dst++ = *src++;
		src = (void*) (MAC_RX_BUFF_BASE_ADDR | ((unsigned) src & (BUFF_SIZE-1)));
	}

	MAC_RG.RXBF_HEAD = (unsigned int) src -1;

	RX_Descriptors[(*Dscr_Num)++].RX_ctrl.all = PARAM;

	if (PARAM & BIT_RX_DSCR_WRAP)
		*Dscr_Num = 0;
	else
		*Dscr_Num &= ((DSCR_CNT*DSCR_SIZE)-1);
	return 0;
}
