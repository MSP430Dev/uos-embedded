#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/can.h>

#define CAN_IRQ(nchan)	((nchan)==0 ? 1 : 0)	/* interrupt number */

/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
bool_t can_output (can_t *c, can_frame_t fr)
{
	int status;

	mutex_lock (&c->lock);

	if (???) {
		/* Есть место в буфере устройства. */
		transmit_enqueue (c, fr);
		mutex_unlock (&c->lock);
		return 1;
	}
	/* Сразу передать не удается, ставим в очередь. */
	if (can_queue_full (&c->outq)) {
		/* Нет места в очереди. */
		/*debug_printf ("can_output: no free space\n");*/
		++c->tx_qo;
		mutex_unlock (&c->lock);
		return 0;
	}
	buf_queue_put (&c->outq, fr);
	mutex_unlock (&c->lock);
		return 1;
}

/*
 * Fetch received frame.
 */
can_frame_t can_input (can_t *c)
{
	can_frame_t fr;

	mutex_lock (&c->lock);
	if (can_queue_empty (&c->inq))
		fr = {0};
	else
		fr = can_queue_get (&c->inq);
	mutex_unlock (&c->lock);
	return fr;
}

/*
 * Receive interrupt task.
 */
static void
can_task (void *arg)
{
	can_t *c = arg;

	mutex_lock_irq (&c->lock, CAN_IRQ (c->port), 0, 0);
	can_setup (c);

	for (;;) {
		/* Wait for interrupt. */
		mutex_wait (&c->lock);

		/* Process all interrupts. */
		++c->intr;
		can_handle_interrupt (c, CAN_NRBUF / 2);
	}
}

/*
 * Set up the CAN driver.
 */
void can_init (can_t *c, int port, int prio, int kbps)
{
	c->port = port;
	c->kbitsec = kbps;
	can_queue_init (&c->inq);
	can_queue_init (&c->outq);

	/* Create interrupt task. */
	task_create (can_task, c, "can", prio,
		c->stack, sizeof (c->stack));
}

#if 0
#include "..\Headers\mk1986be91t_map.h"
#include "..\Headers\mcb.h"
#include "..\Headers\can_hw.h"
#include "..\Headers\can.h"
#include "..\Headers\pmcb_map.h"
#include "..\Headers\cycbuf.h"
#include <string.h>

int CAN_TX_Buffer_Number[2] = {0, 0};
int CAN_Interrupt_Number[2] = {0, 0};
int CAN_Initialized[2] = {0, 0};
int CAN_Current_Speed[2] = {1000, 1000};
int CAN_Command_Statistics[2] = {0, 0};
int CAN_Status_Statistics[2] = {0, 0};
int CAN_TFrame_Statistics[2] = {0, 0};
int CAN_RFrame_Statistics[2] = {0, 0};
int CAN_RFrame_Statistics2[2] = {0, 0};
int CAN_TError_Statistics[2] = {0, 0};
int CAN_RError_Statistics[2] = {0, 0};
unsigned int CAN_TX_BP[2] = {0, 0};
unsigned int CAN_RX_EP[2] = {0, 0};
can_frame_t CAN_RX_Buffer_0[2] = {{0}, {0}};
unsigned int CAN_check_TX_BP[2] = {0, 0};
unsigned int CAN_check_RX_EP[2] = {0, 0};
can_frame_t		rx_buf0[RX_QUEUE_SIZE];
can_frame_t		rx_buf1[RX_QUEUE_SIZE];
cyclic_buffer_t rx_queue[2];
//======================================================================================
//
//======================================================================================
void CAN_Variables_Initialization(int c)
{
	cyclic_reset(&rx_queue[c]);
	MCB_Write(CAN_STATUS(c), 0);
	MCB_Write(CAN_TXBUF_BP(c), 0);
	MCB_Write(CAN_TXBUF_EP(c), 0);
	MCB_Write(CAN_RXBUF_BP(c), 0);
	MCB_Write(CAN_RXBUF_EP(c), 0);
	MCB_Write(CAN_TXBUF_BP_DUB(c), 0);
	MCB_Write(CAN_RXBUF_EP_DUB(c), 0);
	MCB_Write(CAN_RESULT(c), 0);
	MCB_Write(CAN_RESULT_DUB(c), 0);
	MCB_Write(CAN_ANSWER(c), 0);
	MCB_Write(CAN_ANSWER_DUB(c), 0);
	*PCAN_TXBUF_FULL(c) = 0;
	*PCAN_RXBUF_FULL(c) = 0;
	CAN_TX_BP[c] = 0;
	CAN_RX_EP[c] = 0;
	//memset(&CAN_RX_Buffer_0[c], 0, sizeof(can_frame_t));
	CAN_TX_Buffer_Number[c] = 0;
	CAN_Interrupt_Number[c] = 0;
	CAN_Command_Statistics[c] = 0;
	CAN_Status_Statistics[c] = 0;
	CAN_TFrame_Statistics[c] = 0;
	CAN_RFrame_Statistics[c] = 0;
	CAN_TError_Statistics[c] = 0;
	CAN_RError_Statistics[c] = 0;
	CAN_Initialized[c] = 0;
}
//======================================================================================
//
//======================================================================================
void Initialize_Port_CAN_0(void)
{
	u32 port;

	RST_CLK->PER_CLOCK	|= (1 << 23);				// PORTC Ticks On
	port = GPIOC->FUNC;

	port &= ~(((u32)1 << 19) | (1 << 17));
	port |= (1 << 18) | (1 << 16);
	GPIOC->FUNC = port;

	GPIOC->ANALOG |= (1 << 9) | (1 << 8);

	port = GPIOC->PWR;
	port &= ~((1 << 18) | (1 << 16));
	port |= ((u32)1 << 19) | (1 << 17);
	GPIOC->PWR = port;
}

//======================================================================================
//
//======================================================================================
void Initialize_Port_CAN_1(void)
{
	u32 port;

	RST_CLK->PER_CLOCK	|= (1 << 24);				// PORTD Ticks On

	port = GPIOD->FUNC;
	port &= ~(((u32)1 << 31) | (1 << 19));
	port |= (1 << 30) | (1 << 18);
	GPIOD->FUNC = port;

	GPIOD->ANALOG |= (1 << 15) | (1 << 9);

	port = GPIOD->PWR;
	port &= ~((1 << 30) | (1 << 18));
	port |= ((u32)1 << 31) | (1 << 19);
	GPIOD->PWR = port;
}

//======================================================================================
//
//======================================================================================
void Generic_CAN_Default_Setup(int c)
{
	unsigned int i;
	CAN_TypeDef *pCAN = (c == 0) ? (CAN0) : (CAN1);

	RST_CLK->PER_CLOCK |= (1 << (CAN_PER_CLK + c));		// CAN Ticks On
	RST_CLK->CAN_CLOCK |= (1 << (CAN_CLC_EN + c));		// Enable CAN ticks, CAN_BPG = 0 source CAN_CLK = HCLK

	pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((6 - 1) << 22) | ((7 - 1) << 19) | ((2 - 1) << 16) | 4;

	for (i = 0; i <= 31; i++)
	  {
		pCAN->CAN_BUF_CON[i] = 1 << CAN_BUF_EN;
		pCAN->CAN_BUF[i].BUF_ID = (i << 2) << CAN_SID;
		pCAN->CAN_BUF[i].BUF_DLC = (1 << CAN_SSR) | (1 << CAN_R1) | 8;
		pCAN->CAN_BUF[i].BUF_DATAL = ((i + 1) << 24) | (i << 16) | (i << 8) | (i + 1);
		pCAN->CAN_BUF[i].BUF_DATAH = ((i + 1) << 24) | (i << 16) | (i << 8) | (i + 1);
		if (i == 0) pCAN->CAN_BUF_CON[i] |= (1 << CAN_RX_ON);
		pCAN->CAN_MASK[i].BUF_MASK = 0;
		pCAN->CAN_MASK[i].BUF_FILTER = 0;
	  }

	pCAN->CAN_OVER = 255;
	pCAN->CAN_INT_RX = 0x0000FFFF;
	pCAN->CAN_INT_EN = 3;

	HWREG(0xE000E100) |= (1 << c);						// CAN Interrupts Enable

	pCAN->CAN_CONTROL = 1;
}

//======================================================================================
//
//======================================================================================
__forceinline void Generic_CAN_IRQHandler(int c)
{
	unsigned irq_flags;
	u32 i;
	CAN_TypeDef *pCAN = (c == 0) ? (CAN0) : (CAN1);
	can_frame_t frame;

	CAN_Interrupt_Number[c]++;
	irq_flags = pCAN->CAN_RX;

	if ((irq_flags && 0x00000001) != 0)
	  {
		for (i = 0; i < 1; i++)
		  {
			if(pCAN->CAN_BUF_CON[i] & (1 << CAN_RX_FULL))
			  {
				if (memcmp(&CAN_RX_Buffer_0[c], &pCAN->CAN_BUF[i], sizeof(can_frame_t)) != 0)
				  {
				  	memcpy(&frame, &pCAN->CAN_BUF[i], sizeof(can_frame_t));

					if (i == 0) {
						CAN_RX_Buffer_0[c] = frame;
					}

					if (frame.DLC > 8) {
						CAN_RError_Statistics[c]++;
						pCAN->CAN_BUF_CON[i] &= ~(1 << CAN_RX_FULL);
						continue;
					}
					frame.reserve1 = 0;
					frame.reserve2 = 0;
					frame.reserve3 = 0;
					frame.reserve4 = 0;
					if (!frame.IDE)
						frame.EID = 0;
					memset(frame.data.data1 + frame.DLC, 0, 8 - frame.DLC);
					cyclic_add_message(&rx_queue[c], &frame);
				  }
				pCAN->CAN_BUF_CON[i] &= ~(1 << CAN_RX_FULL);
			  }
		  }
	  }

	CAN_Status_Statistics[c] = pCAN->CAN_STATUS;
	if ((CAN_Status_Statistics[c] && 0x0000FC00) != 0)
        CAN_RError_Statistics[c]++;
}

//======================================================================================
//
//======================================================================================
void __irq CAN0_IRQHandler()
{
	Generic_CAN_IRQHandler(0);
}

//======================================================================================
//
//======================================================================================
void __irq CAN1_IRQHandler()
{
	Generic_CAN_IRQHandler(1);
}

//======================================================================================
//
//======================================================================================
unsigned int CAN_Tx_Message(int c, unsigned int *frame)
{
	int b;
	CAN_TypeDef *pCAN = (c == 0) ? (CAN0) : (CAN1);

	b = CAN_TX_Buffer_Number[c] + 16;
	if (pCAN->CAN_BUF_CON[b] & (1 << CAN_TX_REQ)) {
		CAN_TX_Buffer_Number[c] = (CAN_TX_Buffer_Number[c] + 1) % MAX_NB_OF_TXBUFFERS;
		return(0);
	}

	pCAN->CAN_BUF[b].BUF_DLC = 0x00000000;
	if (frame[1] & (1 << CAN_IDE))
	  {
		pCAN->CAN_BUF[b].BUF_ID = (frame[0] & 0x1fffffff);
		pCAN->CAN_BUF[b].BUF_DLC |= ((1 << CAN_IDE) | (1 << CAN_SSR) | (1 << CAN_R1) | (frame[1] & 0xF));
	  }
	else
	  {
		pCAN->CAN_BUF[b].BUF_ID = (frame[0] & 0x1ffe0000);
		pCAN->CAN_BUF[b].BUF_DLC |= (frame[1] & 0xF);
	  }
	pCAN->CAN_BUF[b].BUF_DATAL = frame[2];
	pCAN->CAN_BUF[b].BUF_DATAH = frame[3];
	pCAN->CAN_BUF_CON[b] |= (1 << CAN_TX_REQ);
	CAN_TFrame_Statistics[c]++;
	CAN_TX_Buffer_Number[c] = (CAN_TX_Buffer_Number[c] + 1) % MAX_NB_OF_TXBUFFERS;

	return(1);
}

//======================================================================================
//
//======================================================================================
void Set_CAN_Speed(int c, int speed)
{
	CAN_TypeDef *pCAN = (c == 0) ? (CAN0) : (CAN1);

	switch (*PCAN_PARAM1(c))
	  {
		case CAN_SPEED_1000:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((6 - 1) << 22) | ((7 - 1) << 19) | ((2 - 1) << 16) | 4;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_800:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 4;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_500:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 7;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_250:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 15;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_125:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 31;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_50:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 79;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_20:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 199;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		case CAN_SPEED_10:
		  {
			pCAN->CAN_CONTROL = 0;
			pCAN->CAN_BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 399;
			pCAN->CAN_CONTROL = 1;
			break;
		  }
		  default:
			*PCAN_RESULT(c) = CAN_BAD_PARAM;
			*PCAN_RESULT_DUB(c) = CAN_BAD_PARAM;
		  	return;
	  }
	*PCAN_RESULT(c) = CAN_RES_OK;
	*PCAN_RESULT_DUB(c) = CAN_RES_OK;
	CAN_Current_Speed[c] = speed;
}

//======================================================================================
//
//======================================================================================
void CAN_Commands(int c)
{
	unsigned int Received_Command, Command_Validity;
	CAN_TypeDef *pCAN = (c == 0) ? (CAN0) : (CAN1);

	Received_Command = *PCAN_COMMAND(c);
	Command_Validity = *PCAN_COMMAND_VALID(c);
	if ((Received_Command == 0) || (Command_Validity != CAN_VALID))
        return;
	switch (Received_Command)
	  {
		case CANCTL_INIT:
		  {
			CAN_Variables_Initialization(c);
			CAN_Initialized[c] = 1;
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_RESET:
		  {
			HWREG(0xE000E100) &= ~(1 << c);
			pCAN->CAN_CONTROL = 0;
			if (c == 0)
				Initialize_Port_CAN_0();
			else
				Initialize_Port_CAN_1();
			Generic_CAN_Default_Setup(c);
			CAN_Variables_Initialization(c);
			CAN_Initialized[c] = 1;
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			CAN_Initialized[c] = 1;
			break;
		  }
		case CANCTL_START:
		  {
			HWREG(0xE000E100) |= (1 << c);
			pCAN->CAN_CONTROL = 1;
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_STOP:
		  {
			HWREG(0xE000E100) &= ~(1 << c);
			pCAN->CAN_CONTROL = 0;
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_SPEED:
		  {
			*PCAN_ANSWER(c) = CAN_Current_Speed[c];
			*PCAN_ANSWER_DUB(c) = CAN_Current_Speed[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_SET_SPEED:
		  {
			Set_CAN_Speed(c, *PCAN_PARAM1(c));
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_CMD_NB:
		  {
			*PCAN_ANSWER(c) = CAN_Command_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_Command_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_STATUS:
		  {
			CAN_Command_Statistics[c]++;
			*PCAN_ANSWER(c) = CAN_Status_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_Status_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			break;
		  }
		case CANCTL_GET_TX_PACK:
		  {
			*PCAN_ANSWER(c) = CAN_TFrame_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_TFrame_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_RX_PACK:
		  {
			*PCAN_ANSWER(c) = CAN_RFrame_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_RFrame_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_TX_ERR:
		  {
			*PCAN_ANSWER(c) = CAN_TError_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_TError_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_GET_RX_ERR:
		  {
			*PCAN_ANSWER(c) = CAN_RError_Statistics[c];
			*PCAN_ANSWER_DUB(c) = CAN_RError_Statistics[c];
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			CAN_Command_Statistics[c]++;
			break;
		  }
		case CANCTL_RESET_STAT:
		  {
			CAN_Command_Statistics[c] = 0;
			CAN_Status_Statistics[c] = 0;
			CAN_TFrame_Statistics[c] = 0;
			CAN_RFrame_Statistics[c] = 0;
			CAN_TError_Statistics[c] = 0;
			CAN_RError_Statistics[c] = 0;
			*PCAN_RESULT(c) = CAN_RES_OK;
			*PCAN_RESULT_DUB(c) = CAN_RES_OK;
			break;
		  }
		default:
			*PCAN_RESULT(c) = CAN_RES_UNKNOWN_CMD;
			*PCAN_RESULT_DUB(c) = CAN_RES_UNKNOWN_CMD;
			*PCAN_COMMAND(c) = 0;
		  	return;
	  }
	*PCAN_COMMAND(c) = 0;
}

int main(void)
{
	int chan, empty;
	can_frame_t frm;

	BKP_REG_0E = BKP_REG_0E | 0x080;

	Peripheral_Initialization();

	*PCAN_COMMAND(0) = 0;
	*PCAN_COMMAND(1) = 0;
	CAN_Initialized[0] = 0;
	CAN_Initialized[1] = 0;
	cyclic_init(&rx_queue[0], RX_QUEUE_SIZE, sizeof(can_frame_t), rx_buf0);
	cyclic_init(&rx_queue[1], RX_QUEUE_SIZE, sizeof(can_frame_t), rx_buf1);
	memset(CAN_RX_Buffer_0, 0, 2 * sizeof(can_frame_t));

	for(;;) {
		for (chan = 0; chan < 2; ++chan) {
			CAN_Commands(chan);
			if (CAN_Initialized[chan]) {
				if (*PCAN_TXBUF_FULL(chan)) {
					if (CAN_Tx_Message(chan, PCAN_TXBUF(chan)))
						*PCAN_TXBUF_FULL(chan) = 0;
				}
				if (! *PCAN_RXBUF_FULL(chan)) {
					__disable_irq();
					empty = cyclic_empty(&rx_queue[chan]);
					if (! empty) {
						CAN_RFrame_Statistics[chan]++;
						memcpy(PCAN_RXBUF(chan), cyclic_avail_start(&rx_queue[chan]),
							sizeof(can_frame_t));
						memcpy(&frm, cyclic_avail_start(&rx_queue[chan]),
							sizeof(can_frame_t));
						*PCAN_RXBUF_FULL(chan) = 1;
						cyclic_advance(&rx_queue[chan], 1);
					}
					__enable_irq();
				}
			}
		}
	}
}
#endif
