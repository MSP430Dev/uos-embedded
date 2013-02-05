#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include <milandr/mil-std-1553_setup.h>

#define MILSTD_DEBUG 0

int mil_std_1553_setup(int _port, int _mode, unsigned int _addr)
{
#if MILSTD_DEBUG
    if (_mode == MIL_STD_MODE_RT)
        debug_printf("mil_std_1553_setup, rt, port = %d\n", _port);
    else
        debug_printf("mil_std_1553_setup, bc, port = %d\n", _port);
#endif

    MIL_STD_1553B_t *mil_std_channel = ARM_MIL_STD_1553B1;

    if (_port == 0)
    {
        mil_std_channel = ARM_MIL_STD_1553B1;

        // Подача синхроимпульсов для периферийного блока MIL-STD-1553 (канал 1)
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_MIL_STD_1553B1;

        milandr_init_pin(ARM_GPIOC, PORT_C, 13, FUNC_MAIN);  // PRMA+
        milandr_init_pin(ARM_GPIOC, PORT_C, 14, FUNC_MAIN);  // PRMA-
        milandr_init_pin(ARM_GPIOD, PORT_D,  1, FUNC_MAIN);  // PRDA+
        milandr_init_pin(ARM_GPIOD, PORT_D,  2, FUNC_MAIN);  // PRDA-
        milandr_init_pin(ARM_GPIOD, PORT_D,  5, FUNC_MAIN);  // PRD_PRM_A

        milandr_init_pin(ARM_GPIOC, PORT_C, 15, FUNC_MAIN);  // PRMB+
        milandr_init_pin(ARM_GPIOD, PORT_D,  0, FUNC_MAIN);  // PRMB-
        milandr_init_pin(ARM_GPIOD, PORT_D,  3, FUNC_MAIN);  // PRDB+
        milandr_init_pin(ARM_GPIOD, PORT_D,  4, FUNC_MAIN);  // PRDB-
        milandr_init_pin(ARM_GPIOD, PORT_D,  6, FUNC_MAIN);  // PRD_PRM_B
    }
    else if (_port == 1)
    {
        mil_std_channel = ARM_MIL_STD_1553B2;

        // Подача синхроимпульсов для периферийного блока MIL-STD-1553 (канал 2)
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_MIL_STD_1553B2;

        milandr_init_pin(ARM_GPIOF, PORT_F,  3, FUNC_MAIN);  // PRMC+
        milandr_init_pin(ARM_GPIOF, PORT_F,  4, FUNC_MAIN);  // PRMC-
        milandr_init_pin(ARM_GPIOF, PORT_F,  7, FUNC_MAIN);  // PRDC+
        milandr_init_pin(ARM_GPIOF, PORT_F,  8, FUNC_MAIN);  // PRDC-
        milandr_init_pin(ARM_GPIOF, PORT_F, 11, FUNC_MAIN);  // PRD_PRM_C

        milandr_init_pin(ARM_GPIOF, PORT_F,  5, FUNC_MAIN);  // PRMD+
        milandr_init_pin(ARM_GPIOF, PORT_F,  6, FUNC_MAIN);  // PRMD-
        milandr_init_pin(ARM_GPIOF, PORT_F,  9, FUNC_MAIN);  // PRDD+
        milandr_init_pin(ARM_GPIOF, PORT_F, 10, FUNC_MAIN);  // PRDD-
        milandr_init_pin(ARM_GPIOF, PORT_F, 12, FUNC_MAIN);  // PRD_PRM_D
    }
    else
    {
        return -1;
    }

    // Разрешение тактовой частоты на контроллер ГОСТ Р52070-2003
    ARM_RSTCLK->ETH_CLOCK |= ARM_ETH_CLOCK_MAN_EN;

    unsigned int locControl = 0;
    locControl |= MIL_STD_CONTROL_DIV(KHZ/1000);
    locControl |= MIL_STD_CONTROL_MODE(_mode);
    locControl |= MIL_STD_CONTROL_TRA;  // (!) Здесь можно включить также и резервный канал (может быть вынести в параметры функции)
    if (_mode == MIL_STD_MODE_RT)
    {
        // режим ОУ
        locControl |= MIL_STD_CONTROL_ADDR(_addr) | MIL_STD_CONTROL_TRB;
        mil_std_channel->StatusWord1 = MIL_STD_STATUS_ADDR_OU(_addr);
    }
    mil_std_channel->CONTROL = MIL_STD_CONTROL_MR;
    mil_std_channel->CONTROL = locControl;

    return 0;
}

#endif

