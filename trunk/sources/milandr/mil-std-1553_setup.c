#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include <milandr/mil-std-1553_setup.h>

void mil_std_1553_init_pins(int port)
{
    if (port == 1)
    {
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
}

MIL_STD_1553B_t *mil_std_1553_port_setup(int port)
{
    MIL_STD_1553B_t *mil_std_channel = ARM_MIL_STD_1553B1;
    if (port == 1)
    {
        mil_std_channel = ARM_MIL_STD_1553B2;

        // Подача синхроимпульсов для периферийного блока
        // MIL-STD-1553 (контроллер 2)
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_MIL_STD_1553B2;
    }
    else
    {
        mil_std_channel = ARM_MIL_STD_1553B1;

        // Подача синхроимпульсов для периферийного блока
        // MIL-STD-1553 (контроллер 1)
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_MIL_STD_1553B1;
    }

    // Разрешение тактовой частоты на контроллер ГОСТ Р52070-2003
    ARM_RSTCLK->ETH_CLOCK |= ARM_ETH_CLOCK_MAN_EN;

    return mil_std_channel;
}

#endif
