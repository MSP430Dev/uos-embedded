#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include "arinc.h"

int arinc_init_tx(int out_channel, unsigned int flags)
{
    if (out_channel < 1 || out_channel >= ARINC_OUT_CHANNELS_CNT)
        return -1;

    // Подача синхроимпульсов
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_ARINC429T;

    switch (out_channel)
    {
    case 1:
        milandr_init_pin(ARM_GPIOD, PORT(PD13), PIN(PD13), FUNC_ALT);
        milandr_init_pin(ARM_GPIOD, PORT(PD14), PIN(PD14), FUNC_ALT);
        break;
    case 2:
        milandr_init_pin(ARM_GPIOF, PORT(PF13), PIN(PF13), FUNC_MAIN);
        milandr_init_pin(ARM_GPIOF, PORT(PF14), PIN(PF14), FUNC_MAIN);
        break;
    case 3:
        milandr_init_pin(ARM_GPIOD, PORT(PD15), PIN(PD15), FUNC_MAIN);
        milandr_init_pin(ARM_GPIOE, PORT(PE1), PIN(PE1), FUNC_MAIN);
        break;
    case 4:
        milandr_init_pin(ARM_GPIOE, PORT(PE0), PIN(PE0), FUNC_MAIN);
        milandr_init_pin(ARM_GPIOE, PORT(PE2), PIN(PE2), FUNC_MAIN);
        break;
    }

    // Настройка передатчика ARINC
    ARM_ARINC429T->CONTROL1 = (ARM_ARINC429T->CONTROL1 & ~ARM_ARINC429T_CONTROL_DIV(0x7f)) | ARM_ARINC429T_CONTROL_DIV(KHZ/1000);

    ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_CH_EN(out_channel);

    if ((flags & ARINC_FLAG_PAR_EN) != 0)
        ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_EN_PAR(out_channel);
    else
        ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_EN_PAR(out_channel);

    if ((flags & ARINC_FLAG_PAR_ODD) != 0)
        ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_ODD(out_channel);
    else
        ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_ODD(out_channel);

    if ((flags & ARINC_FLAG_CLK_12_5kHz) != 0)
        ARM_ARINC429T->CONTROL1 |= ARM_ARINC429T_CONTROL_CLK(out_channel);
    else
        ARM_ARINC429T->CONTROL1 &= ~ARM_ARINC429T_CONTROL_CLK(out_channel);

    return 0;
}

int arinc_init_rx(int in_channel)
{
    if (in_channel < 1 || in_channel >= ARINC_IN_CHANNELS_CNT)
        return -1;

    // Подача синхроимпульсов
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_ARINC429R;

    switch (in_channel)
    {
    case 1:
        milandr_init_pin(ARM_GPIOB, PORT(PB0), PIN(PB0), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB1), PIN(PB1), FUNC_ALT);
        break;
    case 2:
        milandr_init_pin(ARM_GPIOB, PORT(PB2), PIN(PB2), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB3), PIN(PB3), FUNC_ALT);
        break;
    case 3:
        milandr_init_pin(ARM_GPIOB, PORT(PB4), PIN(PB4), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB5), PIN(PB5), FUNC_ALT);
        break;
    case 4:
        milandr_init_pin(ARM_GPIOB, PORT(PB6), PIN(PB6), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB7), PIN(PB7), FUNC_ALT);
        break;
    case 5:
        milandr_init_pin(ARM_GPIOB, PORT(PB8), PIN(PB8), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB9), PIN(PB9), FUNC_ALT);
        break;
    case 6:
        milandr_init_pin(ARM_GPIOB, PORT(PB10), PIN(PB10), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB11), PIN(PB11), FUNC_ALT);
        break;
    case 7:
        milandr_init_pin(ARM_GPIOB, PORT(PB12), PIN(PB12), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB13), PIN(PB13), FUNC_ALT);
        break;
    case 8:
        milandr_init_pin(ARM_GPIOB, PORT(PB14), PIN(PB14), FUNC_ALT);
        milandr_init_pin(ARM_GPIOB, PORT(PB15), PIN(PB15), FUNC_ALT);
        break;
    }

    // Настройка приёмника
    ARM_ARINC429R->CONTROL1 = (ARM_ARINC429R->CONTROL1 & ~ARM_ARINC429R_CONTROL1_DIV_LO(0xf)) | ARM_ARINC429R_CONTROL1_DIV_LO(KHZ/1000);
    ARM_ARINC429R->CONTROL1 |= ARM_ARINC429R_CONTROL1_CH_EN(in_channel);
    ARM_ARINC429R->CONTROL2 = (ARM_ARINC429R->CONTROL2 & ~ARM_ARINC429R_CONTROL2_DIV_HI(7)) | ARM_ARINC429R_CONTROL2_DIV_HI(KHZ/1000);

    return 0;
}

int arinc_write(int out_channel, ARINC_msg_t msg)
{
    if (out_channel < 1 || out_channel >= ARINC_OUT_CHANNELS_CNT)
        return -1;

    arm_reg_t *data = &ARM_ARINC429T->DATA1_T;

    switch (out_channel)
    {
    case 2:
        data = &ARM_ARINC429T->DATA2_T;
        break;
    case 3:
        data = &ARM_ARINC429T->DATA3_T;
        break;
    case 4:
        data = &ARM_ARINC429T->DATA4_T;
        break;
    }

    *data = *(unsigned int *)&msg;

    return 0;
}

int arinc_read(int in_channel, ARINC_msg_t *msg)
{
    if (in_channel < 1 || in_channel >= ARINC_IN_CHANNELS_CNT)
        return -1;

    // Выбор текущего канала для приёма
    ARM_ARINC429R->CHANNEL = ARM_ARINC429R_CHANNEL(in_channel);

    if ((ARM_ARINC429R->STATUS1 & ARM_ARINC429R_STATUS1_DR(in_channel)) != 0)
    {
        *msg = *(ARINC_msg_t *)&ARM_ARINC429R->DATA_R;
        return 0;
    }

    return 1;
}

#endif
