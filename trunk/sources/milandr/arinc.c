#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include "arinc.h"


void arinc_init_pins(arinc_t *u)
{
	if (u->flags & ARINC_FLAG_RX) {
		assert((u->chan >= 0) && (u->chan < ARINC_IN_CHANNELS_CNT));
		
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOB;
		
		switch (u->chan)
		{
		case 0:
			milandr_init_pin(ARM_GPIOB, PORT(PB0), PIN(PB0), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB1), PIN(PB1), FUNC_ALT);
			break;
		case 1:
			milandr_init_pin(ARM_GPIOB, PORT(PB2), PIN(PB2), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB3), PIN(PB3), FUNC_ALT);
			break;
		case 2:
			milandr_init_pin(ARM_GPIOB, PORT(PB4), PIN(PB4), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB5), PIN(PB5), FUNC_ALT);
			break;
		case 3:
			milandr_init_pin(ARM_GPIOB, PORT(PB6), PIN(PB6), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB7), PIN(PB7), FUNC_ALT);
			break;
		case 4:
			milandr_init_pin(ARM_GPIOB, PORT(PB8), PIN(PB8), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB9), PIN(PB9), FUNC_ALT);
			break;
		case 5:
			milandr_init_pin(ARM_GPIOB, PORT(PB10), PIN(PB10), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB11), PIN(PB11), FUNC_ALT);
			break;
		case 6:
			milandr_init_pin(ARM_GPIOB, PORT(PB12), PIN(PB12), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB13), PIN(PB13), FUNC_ALT);
			break;
		case 7:
			milandr_init_pin(ARM_GPIOB, PORT(PB14), PIN(PB14), FUNC_ALT);
			milandr_init_pin(ARM_GPIOB, PORT(PB15), PIN(PB15), FUNC_ALT);
			break;
		}
	} else {
		assert((u->chan >= 0) && (u->chan < ARINC_OUT_CHANNELS_CNT));
		
		switch (u->chan)
		{
		case 0:
			ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;
			milandr_init_pin(ARM_GPIOD, PORT(PD13), PIN(PD13), FUNC_ALT);
			milandr_init_pin(ARM_GPIOD, PORT(PD14), PIN(PD14), FUNC_ALT);
			break;
		case 1:
			ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOF;
			milandr_init_pin(ARM_GPIOF, PORT(PF13), PIN(PF13), FUNC_MAIN);
			milandr_init_pin(ARM_GPIOF, PORT(PF14), PIN(PF14), FUNC_MAIN);
			break;
		case 2:
			ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD | ARM_PER_CLOCK_GPIOE;
			milandr_init_pin(ARM_GPIOD, PORT(PD15), PIN(PD15), FUNC_MAIN);
			milandr_init_pin(ARM_GPIOE, PORT(PE1), PIN(PE1), FUNC_MAIN);
			break;
		case 3:
			ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOE;
			milandr_init_pin(ARM_GPIOE, PORT(PE0), PIN(PE0), FUNC_MAIN);
			milandr_init_pin(ARM_GPIOE, PORT(PE2), PIN(PE2), FUNC_MAIN);
			break;
		}
	}
}

void arinc_init(arinc_t *u, int channel, void * buf, unsigned bytes, unsigned flags)
{
	u->chan = channel;
	u->buf = buf;
	u->size = bytes;
	u->flags = flags;
	
	if (flags & ARINC_FLAG_RX) {
		// Подача синхроимпульсов
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_ARINC429R;

		// Настройка приёмника
		ARM_ARINC429R->CONTROL1 = (ARM_ARINC429R->CONTROL1 & ~ARM_ARINC429R_CONTROL1_DIV_LO(0xf)) | ARM_ARINC429R_CONTROL1_DIV_LO(KHZ/1000);
		ARM_ARINC429R->CONTROL1 |= ARM_ARINC429R_CONTROL1_CH_EN(channel);
		ARM_ARINC429R->CONTROL2 = (ARM_ARINC429R->CONTROL2 & ~ARM_ARINC429R_CONTROL2_DIV_HI(7)) | ARM_ARINC429R_CONTROL2_DIV_HI(KHZ/1000);
	} else {
		// Подача синхроимпульсов
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_ARINC429T;

		// Настройка передатчика ARINC
		ARM_ARINC429T->CONTROL1 = (ARM_ARINC429T->CONTROL1 & ~ARM_ARINC429T_CONTROL_DIV(0x7f)) | ARM_ARINC429T_CONTROL_DIV(KHZ/1000);

		ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_CH_EN(channel);

		if ((flags & ARINC_FLAG_PAR_EN) != 0)
			ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_EN_PAR(channel);
		else
			ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_EN_PAR(channel);

		if ((flags & ARINC_FLAG_PAR_ODD) != 0)
			ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_ODD(channel);
		else
			ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_ODD(channel);

		if ((flags & ARINC_FLAG_CLK_12_5kHz) != 0)
			ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_CLK(channel);
		else
			ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_CLK(channel);
	}
}

int arinc_write(arinc_t *u, ARINC_msg_t msg)
{
	assert(!(u->flags & ARINC_FLAG_RX));
	assert((u->chan >= 0) && (u->chan < ARINC_OUT_CHANNELS_CNT));
	
	if (ARM_ARINC429T->STATUS & ARM_ARINC429T_STATUS_FFT(u->chan))
		return 0;
		
	ARM_ARINC429T->DATA_T[u->chan] = *((unsigned *) &msg);
	
	u->labels++;

    return 1;
}

int arinc_read(arinc_t *u, ARINC_msg_t *msg)
{
	assert(u->flags & ARINC_FLAG_RX);
	assert((u->chan >= 0) && (u->chan < ARINC_IN_CHANNELS_CNT));

    // Выбор текущего канала для приёма
    ARM_ARINC429R->CHANNEL = ARM_ARINC429R_CHANNEL(u->chan);
	
    if (!(ARM_ARINC429R->STATUS1 & ARM_ARINC429R_STATUS1_DR(u->chan)))
		return 0;

	*msg = *(ARINC_msg_t *)&ARM_ARINC429R->DATA_R;
	
	u->labels++;

    return 1;
}

void arinc_poll(arinc_t *u)
{
	ARINC_msg_t msg;
	unsigned *p;
	
	assert(u->buf);
	
	mutex_lock(&u->lock);
	if (u->flags & ARINC_FLAG_RX) {
		// Выбор текущего канала для приёма
		ARM_ARINC429R->CHANNEL = ARM_ARINC429R_CHANNEL(u->chan);
		
		if (ARM_ARINC429R->STATUS1 & ARM_ARINC429R_STATUS1_ERR(u->chan))
			u->rx_errors++;

		// Приём и "раскладка" принятых меток по адресам
		while ((ARM_ARINC429R->STATUS1 & ARM_ARINC429R_STATUS1_DR(u->chan)) != 0) {
			msg = *(ARINC_msg_t *)&ARM_ARINC429R->DATA_R;
			//!!! Метки хранятся в перевёрнутом виде?
			if (msg.label >= (u->size >> 2))
				u->rx_errors++;
			else {
				u->buf[msg.label] = *((unsigned *) &msg);
				u->labels++;
			}
		}
	} else {
		// Выдаём в FIFO сообщения в буфере, либо до ячейки, содержащей ноль,
		// либо до конца буфера
		if (ARM_ARINC429T->STATUS & ARM_ARINC429T_STATUS_TX_R(u->chan)) {
				p = u->buf;
				while ((*p != 0) && (p - u->buf < (u->size >> 2))) {
					ARM_ARINC429T->DATA_T[u->chan] = *p;
					p++;
					u->labels++;
				}		
		}
	}
	mutex_unlock(&u->lock);
}



#endif
