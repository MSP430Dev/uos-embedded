/*
 * Register definitions for Milandr 1986BE9x.
 */
typedef volatile unsigned int arm_reg_t;

/*
 * Memory map
 */
#define ARM_SRAM_BASE		0x20000000	/* Internal static memory */
#define ARM_PERIPH_BASE		0x40000000	/* Peripheral registers */
#define ARM_EXTBUS0_BASE	0x60000000	/* Access to external bus 0 */
#define ARM_EXTBUS1_BASE	0x90000000	/* Access to external bus 1 */
#define ARM_SYSTEM_BASE		0xE0000000	/* Core registers */

#define ARM_SRAM_SIZE		(32*1024)	/* 32 kbytes */

/*
 * Peripheral memory map
 */
#define ARM_CAN1_BASE		ARM_PERIPH_BASE
#define ARM_CAN2_BASE		(ARM_PERIPH_BASE + 0x08000)
#define ARM_USB_BASE		(ARM_PERIPH_BASE + 0x10000)
#define ARM_EEPROM_BASE		(ARM_PERIPH_BASE + 0x18000)
#define ARM_RSTCLK_BASE        	(ARM_PERIPH_BASE + 0x20000)
#define ARM_DMA_BASE		(ARM_PERIPH_BASE + 0x28000)
#define ARM_UART1_BASE		(ARM_PERIPH_BASE + 0x30000)
#define ARM_UART2_BASE		(ARM_PERIPH_BASE + 0x38000)
#define ARM_SSP1_BASE		(ARM_PERIPH_BASE + 0x40000)
#define ARM_I2C1_BASE		(ARM_PERIPH_BASE + 0x50000)
#define ARM_POWER_BASE		(ARM_PERIPH_BASE + 0x58000)
#define ARM_WWDT_BASE		(ARM_PERIPH_BASE + 0x60000)
#define ARM_IWDT_BASE		(ARM_PERIPH_BASE + 0x68000)
#define ARM_TIMER1_BASE		(ARM_PERIPH_BASE + 0x70000)
#define ARM_TIMER2_BASE		(ARM_PERIPH_BASE + 0x78000)
#define ARM_TIMER3_BASE		(ARM_PERIPH_BASE + 0x80000)
#define ARM_ADC_BASE		(ARM_PERIPH_BASE + 0x88000)
#define ARM_DAC_BASE		(ARM_PERIPH_BASE + 0x90000)
#define ARM_COMP_BASE		(ARM_PERIPH_BASE + 0x98000)
#define ARM_SSP2_BASE		(ARM_PERIPH_BASE + 0xA0000)
#define ARM_GPIOA_BASE		(ARM_PERIPH_BASE + 0xA8000)
#define ARM_GPIOB_BASE		(ARM_PERIPH_BASE + 0xB0000)
#define ARM_GPIOC_BASE		(ARM_PERIPH_BASE + 0xB8000)
#define ARM_GPIOD_BASE		(ARM_PERIPH_BASE + 0xC0000)
#define ARM_GPIOE_BASE		(ARM_PERIPH_BASE + 0xC8000)
#define ARM_BKP_BASE		(ARM_PERIPH_BASE + 0xD8000)
#define ARM_GPIOF_BASE		(ARM_PERIPH_BASE + 0xE8000)
#define ARM_EXT_BUS_BASE	(ARM_PERIPH_BASE + 0xF0050)

/*------------------------------------------------------
 * SysTick timer
 */
typedef struct
{
	arm_reg_t CTRL;		/* Управление и статус */
	arm_reg_t LOAD;		/* Загружаемое значение */
	arm_reg_t VAL;		/* Текущее состояние */
	arm_reg_t CALIB;	/* Подстройка */
} SYSTICK_t;

#define ARM_SYSTICK		((SYSTICK_t*) (ARM_SYSTEM_BASE + 0xE010))

/*
 * Регистр SYSTICK CTRL: управление и статус системного таймера.
 */
#define ARM_SYSTICK_CTRL_COUNTFLAG	(1 << 16) /* досчитал ли таймер до 0 */
#define ARM_SYSTICK_CTRL_HCLK		(1 << 2)  /* источник 0-LSI, 1-HCLK */
#define ARM_SYSTICK_CTRL_TICKINT	(1 << 1)  /* разрешение прерывания */
#define ARM_SYSTICK_CTRL_ENABLE		(1 << 0)  /* пуск таймера */

/*------------------------------------------------------
 * NVIC interrupt controller
 */
#define ARM_NVIC_ISER0	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE100))
					/* Разрешение прерываний */
#define ARM_NVIC_ICER0	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE180))
					/* Запрет прерывания */
#define ARM_NVIC_ISPR0	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE200))
					/* Перевод прерывания в состояние
					 * ожидания обслуживания */
#define ARM_NVIC_ICPR0	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE280))
					/* Сброс состояния ожидания обслуживания */
#define ARM_NVIC_IABR0	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE300))
					/* Активные прерывания */
#define ARM_NVIC_IPR(n)	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xE400 + ((n) << 2)))
					/* Приоритет прерываний */
#define ARM_NVIC_STIR 	(*(arm_reg_t*) (ARM_SYSTEM_BASE + 0xEF00))
					/* Программное формирование прерывания */

/*------------------------------------------------------
 * SCB system control block
 */
typedef struct
{
	arm_reg_t CPUID;	/* Идентификация процессора */
	arm_reg_t ICSR;		/* Управление прерываниями */
	arm_reg_t VTOR;		/* Смещение таблицы векторов прерываний */
	arm_reg_t AIRCR;	/* Управление прерываниями и программного сброса */
	arm_reg_t SCR;		/* Управление системой */
	arm_reg_t CCR;		/* Конфигурация и управление */
	arm_reg_t SHPR1;	/* Приоритет обработчиков memory/bus/usage fault */
	arm_reg_t SHPR2;	/* Приоритет обработчика SVCall */
	arm_reg_t SHPR3;	/* Приоритет обработчиков SysTick, PendSV */
	arm_reg_t SHCSR;	/* Управление и состояние системных обработчиков */
	arm_reg_t CFSR;		/* Состояние отказов с конфигурируемым приоритетом */
	arm_reg_t HFSR;		/* Состояние тяжелого отказа */
	arm_reg_t UNUSED;	/* DFSR */
	arm_reg_t MMAR;		/* Адрес отказа доступа к памяти */
	arm_reg_t BFAR;		/* Адрес отказа доступа к шине */
} SCB_t;

#define ARM_SCB		((SCB_t*) (ARM_SYSTEM_BASE + 0xED00))

/*
 * Регистр SCB AIRCR: управление прерываниями и программный сброс.
 */
#define ARM_AIRCR_VECTKEY	(0x05FA << 16)	/* ключ доступа к регистру */
#define ARM_AIRCR_ENDIANESS	(1 << 15)	/* старший байт идет первым */
#define ARM_AIRCR_PRIGROUP(n)	((n) << 8)	/* группировка приоритетов исключений */
#define ARM_AIRCR_SYSRESETREQ	(1 << 2)	/* запрос сброса системы */

/*
 * Регистр SCB SHCSR: управление и состояние системных обработчиков.
 */
#define ARM_SHCSR_USGFAULTENA	(1 << 18)  /* разрешение обработки отказов,
					    * вызванных ошибками программирования */
#define ARM_SHCSR_BUSFAULTENA	(1 << 17)  /* разрешение обработки отказа доступа к шине */
#define ARM_SHCSR_MEMFAULTENA	(1 << 16)  /* разрешение обработки отказа доступа к памяти */
#define ARM_SHCSR_SVCALLPENDED	(1 << 15)  /* признак ожидания обработки вызова SVC */
#define ARM_SHCSR_BUSFAULTPENDED (1 << 14) /* признак ожидания обработки отказа
					    * доступа к шине */
#define ARM_SHCSR_MEMFAULTPENDED (1 << 13) /* признак ожидания обработки отказа
					    * доступа к памяти */
#define ARM_SHCSR_USGFAULTPENDED (1 << 12) /* признак ожидания обработки отказа,
					    * вызванного ошибками программирования */
#define ARM_SHCSR_SYSTICKACT	(1 << 11)  /* признак активности обработчика
					    * исключения SysTick */
#define ARM_SHCSR_PENDSVACT	(1 << 10)  /* признак активности обработчика
					    * исключения PendSV */
#define ARM_SHCSR_MONITORACT	(1 << 8)   /* признак активности монитора отладчика */
#define ARM_SHCSR_SVCALLACT	(1 << 7)   /* признак активности обработчика
					    * вызова SVC */
#define ARM_SHCSR_USGFAULTACT	(1 << 3)   /* признак активности обработчика
					    * отказа, вызванного ошибкой программирования */
#define ARM_SHCSR_BUSFAULTACT	(1 << 1)   /* признак активности обработчика
					    * отказа доступа к шине */
#define ARM_SHCSR_MEMFAULTACT	(1 << 0)   /* признак активности обработчика
					    * отказа доступа к памяти */

/*
 * Регистры SCB SHPRi: приоритет системных обработчиков.
 */
#define ARM_SHPR1_UFAULT(n)	((n) << 16) /* usage fault */
#define ARM_SHPR1_BFAULT(n)	((n) << 8)  /* bus fault */
#define ARM_SHPR1_MMFAULT(n)	((n) << 0)  /* memory management fault */

#define ARM_SHPR2_SVCALL(n)	((n) << 24) /* SVCall */

#define ARM_SHPR3_SYSTICK(n)	((n) << 24) /* SysTick */
#define ARM_SHPR3_PENDSV(n)	((n) << 16) /* PendSV */

/*------------------------------------------------------
 * General purpose I/O
 */
typedef struct
{
	arm_reg_t DATA;		/* Данные для выдачи и чтения */
	arm_reg_t OE;		/* Направление, 1 - выход */
	arm_reg_t FUNC;		/* Выбор функции, два бита на порт */
	arm_reg_t ANALOG;	/* Режим работы, 1 - цифровой */
	arm_reg_t PULL;		/* Подтяжка вверх [31:16] и
				 * отключение подтяжки вниз [15:0] */
	arm_reg_t PD;		/* Триггер Шмидта входа [31:16] или
				 * открытый сток выхода [15:0]  */
	arm_reg_t PWR;		/* Скорость фронта выхода, два бита на порт */
	arm_reg_t GFEN;		/* Фильтрация входа */
} GPIO_t;

#define ARM_GPIOA		((GPIO_t*) ARM_GPIOA_BASE)
#define ARM_GPIOB		((GPIO_t*) ARM_GPIOB_BASE)
#define ARM_GPIOC		((GPIO_t*) ARM_GPIOC_BASE)
#define ARM_GPIOD		((GPIO_t*) ARM_GPIOD_BASE)
#define ARM_GPIOE		((GPIO_t*) ARM_GPIOE_BASE)
#define ARM_GPIOF		((GPIO_t*) ARM_GPIOF_BASE)

/*
 * Регистр GPIO FUNC: выбор функции порта
 */
#define ARM_FUNC_MASK(n)	(3 << ((n)*2))
#define ARM_FUNC_PORT(n)	(0 << ((n)*2))	/* порт */
#define ARM_FUNC_MAIN(n)	(1 << ((n)*2))	/* основная функция */
#define ARM_FUNC_ALT(n)		(2 << ((n)*2))	/* альтернативная функция */
#define ARM_FUNC_REDEF(n)	(3 << ((n)*2))	/* переопределённая функция */

/*
 * Регистр GPIO PWR: скорость фронта порта вывода
 */
#define ARM_PWR_MASK(n)		(3 << ((n)*2))
#define ARM_PWR_SLOW(n)		(1 << ((n)*2))	/* медленный фронт */
#define ARM_PWR_FAST(n)		(2 << ((n)*2))	/* быстрый фронт */
#define ARM_PWR_FASTEST(n)	(3 << ((n)*2))	/* максимально быстрый фронт */

/*------------------------------------------------------
 * External bus
 */
typedef struct
{
	arm_reg_t NAND_CYCLES;
	arm_reg_t CONTROL;
} EXTBUS_t;

#define ARM_EXTBUS		((EXTBUS_t*) ARM_EXT_BUS_BASE)

/*------------------------------------------------------
 * Clock management
 */
typedef struct
{
	arm_reg_t CLOCK_STATUS;
	arm_reg_t PLL_CONTROL;
	arm_reg_t HS_CONTROL;
	arm_reg_t CPU_CLOCK;
	arm_reg_t USB_CLOCK;
	arm_reg_t ADC_MCO_CLOCK;
	arm_reg_t RTC_CLOCK;
	arm_reg_t PER_CLOCK;
	arm_reg_t CAN_CLOCK;
	arm_reg_t TIM_CLOCK;
	arm_reg_t UART_CLOCK;
	arm_reg_t SSP_CLOCK;
} RSTCLK_t;

#define ARM_RSTCLK		((RSTCLK_t*) ARM_RSTCLK_BASE)

/*
 * Регистр CLOCK_STATUS: состояние блока управления тактовой частотой.
 */
#define ARM_CLOCK_STATUS_PLL_USB_RDY	(1 << 0) /* USB PLL запущена и стабильна */
#define ARM_CLOCK_STATUS_PLL_CPU_RDY	(1 << 1) /* CPU PLL запущена и стабильна */
#define ARM_CLOCK_STATUS_HSE_RDY	(1 << 2) /* осциллятор HSE запущен и стабилен */

/*
 * Регистр PLL_CONTROL: управление блоками умножения частоты.
 */
#define ARM_PLL_CONTROL_USB_ON		(1 << 0)	/* USB PLL включена */
#define ARM_PLL_CONTROL_USB_RLD		(1 << 1)	/* Перезапуск USB PLL */
#define ARM_PLL_CONTROL_CPU_ON		(1 << 2)	/* CPU PLL включена */
#define ARM_PLL_CONTROL_CPU_RLD		(1 << 3)	/* Перезапуск CPU PLL */
#define ARM_PLL_CONTROL_USB_MUL(n)	(((n)-1) << 4)	/* Коэффициент умножения для USB PLL */
#define ARM_PLL_CONTROL_CPU_MUL(n)	(((n)-1) << 8)	/* Коэффициент умножения для CPU PLL */

/*
 * Регистр PER_CLOCK: включение тактирования периферийных блоков.
 */
#define ARM_PER_CLOCK_CAN1	(1 << 0)
#define ARM_PER_CLOCK_CAN2	(1 << 1)
#define ARM_PER_CLOCK_USB	(1 << 2)
#define ARM_PER_CLOCK_EEPROM	(1 << 3)
#define ARM_PER_CLOCK_RSTCLK	(1 << 4)
#define ARM_PER_CLOCK_DMA	(1 << 5)
#define ARM_PER_CLOCK_UART1	(1 << 6)
#define ARM_PER_CLOCK_UART2	(1 << 7)
#define ARM_PER_CLOCK_SSP1	(1 << 8)
#define ARM_PER_CLOCK_I2C1	(1 << 10)
#define ARM_PER_CLOCK_POWER	(1 << 11)
#define ARM_PER_CLOCK_WWDT	(1 << 12)
#define ARM_PER_CLOCK_IWDT	(1 << 13)
#define ARM_PER_CLOCK_TIMER1	(1 << 14)
#define ARM_PER_CLOCK_TIMER2	(1 << 15)
#define ARM_PER_CLOCK_TIMER3	(1 << 16)
#define ARM_PER_CLOCK_ADC	(1 << 17)
#define ARM_PER_CLOCK_DAC	(1 << 18)
#define ARM_PER_CLOCK_COMP	(1 << 19)
#define ARM_PER_CLOCK_SSP2	(1 << 20)
#define ARM_PER_CLOCK_GPIOA	(1 << 21)
#define ARM_PER_CLOCK_GPIOB	(1 << 22)
#define ARM_PER_CLOCK_GPIOC	(1 << 23)
#define ARM_PER_CLOCK_GPIOD	(1 << 24)
#define ARM_PER_CLOCK_GPIOE	(1 << 25)
#define ARM_PER_CLOCK_BKP	(1 << 27)
#define ARM_PER_CLOCK_GPIOF	(1 << 29)
#define ARM_PER_CLOCK_EXT_BUS	(1 << 30)

/*
 * Регистр HS_CONTROL: управление высокочастотным генератором и осциллятором.
 */
#define ARM_HS_CONTROL_HSE_ON	(1 << 0)	/* Осциллятор HSE включён */
#define ARM_HS_CONTROL_HSE_BYP	(1 << 1)	/* Режим внешнего генератора */

/*
 * Регистр CPU_CLOCK: управление тактовой частотой.
 */
#define ARM_CPU_CLOCK_C1_HSI		(0 << 0) /* Выбор источника для CPU C1: HSI */
#define ARM_CPU_CLOCK_C1_HSI_DIV2	(1 << 0) /* HSI/2 */
#define ARM_CPU_CLOCK_C1_HSE		(2 << 0) /* HSE */
#define ARM_CPU_CLOCK_C1_HSE_DIV2	(3 << 0) /* HSE/2 */
#define ARM_CPU_CLOCK_C2_PLLCPUO	(1 << 2) /* Выбор источника для CPU C2: PLLCPUo */
#define ARM_CPU_CLOCK_C3_C2		(0 << 4) /* Выбор источника для CPU C3: CPU C2 */
#define ARM_CPU_CLOCK_C3_C2_DIV2	(8 << 4) /* CPU C2 / 2 */
#define ARM_CPU_CLOCK_C3_C2_DIV4	(8 << 4) /* CPU C2 / 4 */
#define ARM_CPU_CLOCK_C3_C2_DIV8	(8 << 4) /* CPU C2 / 8 */
#define ARM_CPU_CLOCK_C3_C2_DIV16	(8 << 4) /* CPU C2 / 16 */
#define ARM_CPU_CLOCK_C3_C2_DIV32	(8 << 4) /* CPU C2 / 32 */
#define ARM_CPU_CLOCK_C3_C2_DIV64	(8 << 4) /* CPU C2 / 64 */
#define ARM_CPU_CLOCK_C3_C2_DIV128	(8 << 4) /* CPU C2 / 128 */
#define ARM_CPU_CLOCK_C3_C2_DIV256	(8 << 4) /* CPU C2 / 256 */
#define ARM_CPU_CLOCK_HCLK_HSI		(0 << 8) /* Выбор источника для HCLK: HSI */
#define ARM_CPU_CLOCK_HCLK_C3		(1 << 8) /* CPU C3 */
#define ARM_CPU_CLOCK_HCLK_LSE		(2 << 8) /* LSE */
#define ARM_CPU_CLOCK_HCLK_LSI		(3 << 8) /* LSI */

/*
 * Регистр CAN_CLOCK: управление тактовой частотой CAN
 * Делитель тактовой частоты:
 *	0 - HCLK
 *	1 - HCLK/2
 *	2 - HCLK/4
 *	...
 *	7 - HCLK/128
 */
#define ARM_CAN_CLOCK_EN2	(1 << 25)	/* Разрешение тактовой частоты на CAN2 */
#define ARM_CAN_CLOCK_EN1	(1 << 24)	/* Разрешение тактовой частоты на CAN1 */
#define ARM_CAN_CLOCK_BRG2(n)	((n) << 8)	/* Делитель тактовой частоты CAN2 */
#define ARM_CAN_CLOCK_BRG1(n)	(n)		/* Делитель тактовой частоты CAN1 */

/*
 * Регистр UART_CLOCK: управление тактовой частотой UART
 * Делитель тактовой частоты:
 *	0 - HCLK
 *	1 - HCLK/2
 *	2 - HCLK/4
 *	...
 *	7 - HCLK/128
 */
#define ARM_UART_CLOCK_EN2	(1 << 25)	/* Разрешение тактовой частоты на UART2 */
#define ARM_UART_CLOCK_EN1	(1 << 24)	/* Разрешение тактовой частоты на UART1 */
#define ARM_UART_CLOCK_BRG2(n)	((n) << 8)	/* Делитель тактовой частоты UART2 */
#define ARM_UART_CLOCK_BRG1(n)	(n)		/* Делитель тактовой частоты UART1 */

/*------------------------------------------------------
 * UART
 */
typedef struct
{
	arm_reg_t DR;			/* Данные */
	arm_reg_t SRCR;			/* Состояние и сброс ошибки приёмника */
	unsigned reserved0 [4];
	arm_reg_t FR;			/* Флаги */
	unsigned reserved1;
	arm_reg_t ILPR;			/* Управление ИК-обменом */
	arm_reg_t IBRD;			/* Делитель скорости */
	arm_reg_t FBRD;			/* Дробная часть делителя */
	arm_reg_t LCR_H;		/* Управление линией */
	arm_reg_t CR;			/* Управление */
	arm_reg_t IFLS;			/* Порог прерывания FIFO */
	arm_reg_t IMSC;			/* Маска прерывания */
	arm_reg_t RIS;			/* Состояние прерываний */
	arm_reg_t MIS;			/* Состояние прерываний с маскированием */
	arm_reg_t ICR;			/* Сброс прерывания */
	arm_reg_t DMACR;		/* Управление DMA */
} UART_t;

typedef struct
{
	arm_reg_t PERIPHID0;
	arm_reg_t PERIPHID1;
	arm_reg_t PERIPHID2;
	arm_reg_t PERIPHID3;
	arm_reg_t PCELLID0;
	arm_reg_t PCELLID1;
	arm_reg_t PCELLID2;
	arm_reg_t PCELLID3;
} UARTTEST_t;

#define ARM_UART1		((UART_t*) ARM_UART1_BASE)
#define ARM_UART2		((UART_t*) ARM_UART2_BASE)
#define ARM_UART1TEST		((UARTTEST_t*) (ARM_UART1_BASE + 0x0FE0))
#define ARM_UART2TEST		((UARTTEST_t*) (ARM_UART2_BASE + 0x0FE0))

/*
 * Регистр UART DR: данные и флаги
 */
#define ARM_UART_DR_OE		(1 << 11)	/* Переполнение буфера приемника */
#define ARM_UART_DR_BE		(1 << 10)	/* Разрыв линии (break) */
#define ARM_UART_DR_PE		(1 << 9)	/* Ошибка контроля четности */
#define ARM_UART_DR_FE		(1 << 8)	/* Ошибка в структуре кадра */
#define ARM_UART_DR_DATA	0xFF		/* Данные */

/*
 * Регистр UART SRCR: состояние приёмника и сброс ошибки
 */
#define ARM_UART_SRCR_OE	(1 << 3)	/* Переполнение буфера приемника */
#define ARM_UART_SRCR_BE	(1 << 2)	/* Разрыв линии (break) */
#define ARM_UART_SRCR_PE	(1 << 1)	/* Ошибка контроля четности */
#define ARM_UART_SRCR_FE	(1 << 0)	/* Ошибка в структуре кадра */

/*
 * Регистр UART FR: флаги
 */
#define ARM_UART_FR_RI		(1 << 8)	/* Инверсия линии /UARTRI */
#define ARM_UART_FR_TXFE	(1 << 7)	/* Буфер FIFO передатчика пуст */
#define ARM_UART_FR_RXFF	(1 << 6)	/* Буфер FIFO приемника заполнен */
#define ARM_UART_FR_TXFF	(1 << 5)	/* Буфер FIFO передатчика заполнен */
#define ARM_UART_FR_RXFE	(1 << 4)	/* Буфер FIFO приемника пуст */
#define ARM_UART_FR_BUSY	(1 << 3)	/* UART занят */
#define ARM_UART_FR_DCD		(1 << 2)	/* Инверсия линии /UARTDCD */
#define ARM_UART_FR_DSR		(1 << 1)	/* Инверсия линии /UARTDSR */
#define ARM_UART_FR_CTS		(1 << 0)	/* Инверсия линии /UARTCTS */

/*
 * Регистры UART IBRD и FBRD: делитель скорости
 */
#define ARM_UART_IBRD(mhz,baud)	((mhz) / (baud) / 16)
#define ARM_UART_FBRD(mhz,baud)	(((mhz) * 4 / (baud)) & 077)

/*
 * Регистр UART LCR_H: управление линией.
 */
#define ARM_UART_LCRH_SPS	(1 << 7)	/* Фиксация значения бита чётности */
#define ARM_UART_LCRH_WLEN5	(0 << 5)	/* Длина слова 5 бит */
#define ARM_UART_LCRH_WLEN6	(1 << 5)	/* Длина слова 6 бит */
#define ARM_UART_LCRH_WLEN7	(2 << 5)	/* Длина слова 7 бит */
#define ARM_UART_LCRH_WLEN8	(3 << 5)	/* Длина слова 8 бит */
#define ARM_UART_LCRH_FEN	(1 << 4)	/* Разрешение работы FIFO */
#define ARM_UART_LCRH_STP2	(1 << 3)	/* Два стоповых бита */
#define ARM_UART_LCRH_EPS	(1 << 2)	/* Чётность (0) или нечётность (1) */
#define ARM_UART_LCRH_PEN	(1 << 1)	/* Разрешение чётности */
#define ARM_UART_LCRH_BRK	(1 << 0)	/* Разрыв линии (break) */

/*
 * Регистр UART CR: управление.
 */
#define ARM_UART_CR_CTSEN	(1 << 15)	/* Управление потоком данных по CTS */
#define ARM_UART_CR_RTSEN	(1 << 14)	/* Управление потоком данных по RTS */
#define ARM_UART_CR_OUT2	(1 << 13)	/* Инверсия сигнала /UARTOut2 */
#define ARM_UART_CR_OUT1	(1 << 12)	/* Инверсия сигнала /UARTOut1 */
#define ARM_UART_CR_RTS		(1 << 11)	/* Инверсия сигнала /UARTRTS */
#define ARM_UART_CR_DTR		(1 << 10)	/* Инверсия сигнала /UARTDTR */
#define ARM_UART_CR_RXE		(1 << 9)	/* Прием разрешен */
#define ARM_UART_CR_TXE		(1 << 8)	/* Передача разрешена */
#define ARM_UART_CR_LBE		(1 << 7)	/* Шлейф разрешен */
#define ARM_UART_CR_SIRLP	(1 << 2)	/* ИК-обмен с пониженным энергопотреблением */
#define ARM_UART_CR_SIREN	(1 << 1)	/* Разрешение ИК передачи данных IrDA SIR */
#define ARM_UART_CR_UARTEN	(1 << 0)	/* Разрешение работы приемопередатчика */

/*
 * Регистр UART IFLS: пороги FIFO.
 */
#define ARM_UART_IFLS_RX_1_8	(0 << 3)	/* Приём: 1/8 буфера */
#define ARM_UART_IFLS_RX_1_4	(1 << 3)	/* Приём: 1/4 буфера */
#define ARM_UART_IFLS_RX_1_2	(2 << 3)	/* Приём: 1/2 буфера */
#define ARM_UART_IFLS_RX_3_4	(3 << 3)	/* Приём: 3/4 буфера */
#define ARM_UART_IFLS_RX_7_8	(4 << 3)	/* Приём: 7/8 буфера */
#define ARM_UART_IFLS_TX_1_8	(0 << 0)	/* Передача: 1/8 буфера */
#define ARM_UART_IFLS_TX_1_4	(1 << 0)	/* Передача: 1/4 буфера */
#define ARM_UART_IFLS_TX_1_2	(2 << 0)	/* Передача: 1/2 буфера */
#define ARM_UART_IFLS_TX_3_4	(3 << 0)	/* Передача: 3/4 буфера */
#define ARM_UART_IFLS_TX_7_8	(4 << 0)	/* Передача: 7/8 буфера */

/*
 * Регистр UART RIS: состояние прерываний.
 * Регистр UART IMSC: маска прерывания.
 * Регистр UART MIS: состояние прерываний с маскированием.
 * Регистр UART ICR: cброс прерывания.
 */
#define ARM_UART_RIS_OE		(1 << 10)	/* Переполнение буфера */
#define ARM_UART_RIS_BE		(1 << 9)	/* Разрыв линии */
#define ARM_UART_RIS_PE		(1 << 8)	/* Ошибка контроля четности */
#define ARM_UART_RIS_FE		(1 << 7)	/* Ошибка в структуре кадра */
#define ARM_UART_RIS_RT		(1 << 6)	/* Таймаут приема данных */
#define ARM_UART_RIS_TX		(1 << 5)	/* Прерывание от передатчика */
#define ARM_UART_RIS_RX		(1 << 4)	/* Прерывание от приемника */
#define ARM_UART_RIS_DSRM	(1 << 3)	/* Изменение состояния /UARTDSR */
#define ARM_UART_RIS_DCDM	(1 << 2)	/* Изменение состояния /UARTDCD */
#define ARM_UART_RIS_CTSM	(1 << 1)	/* Изменение состояния /UARTCTS */
#define ARM_UART_RIS_RIM	(1 << 0)	/* Изменение состояния /UARTRI */

/*
 * Регистр UART DMACR: управление DMA.
 */
#define ARM_UART_DMACR_ONERR	(1 << 2)	/* Блокирование при ошибке */
#define ARM_UART_DMACR_TXE	(1 << 1)	/* Использование ПДП при передаче */
#define ARM_UART_DMACR_RXE	(1 << 0)	/* Использование ПДП при приеме */

/*------------------------------------------------------
 * Synchronous serial port
 */
typedef struct
{
	arm_reg_t SSPCR0;
	arm_reg_t SSPCR1;
	arm_reg_t SSPDR;
	arm_reg_t SSPSR;
	arm_reg_t SSPCPSR;
	arm_reg_t SSPIMSC;
	arm_reg_t SSPRIS;
	arm_reg_t SSPMIS;
	arm_reg_t SSPICR;
	arm_reg_t SSPDMACR;
} SSP_t;

typedef struct
{
	arm_reg_t PERIPHID0;
	arm_reg_t PERIPHID1;
	arm_reg_t PERIPHID2;
	arm_reg_t PERIPHID3;
	arm_reg_t PCELLID0;
	arm_reg_t PCELLID1;
	arm_reg_t PCELLID2;
	arm_reg_t PCELLID3;
} SSPTEST_t;

#define ARM_SSP1		((SSP_t*) ARM_SSP1_BASE)
#define ARM_SSP2		((SSP_t*) ARM_SSP2_BASE)
#define ARM_SSP1TEST		((SSPTEST_t*) (ARM_SSP1_BASE + 0x0FE0))
#define ARM_SSP2TEST		((SSPTEST_t*) (ARM_SSP2_BASE + 0x0FE0))

/*------------------------------------------------------
 * Timers
 */
typedef struct
{
	arm_reg_t TIM_CNT;
	arm_reg_t TIM_PSG;
	arm_reg_t TIM_ARR;
	arm_reg_t TIM_CNTRL;
	arm_reg_t TIM_CCR1;
	arm_reg_t TIM_CCR2;
	arm_reg_t TIM_CCR3;
	arm_reg_t TIM_CCR4;
	arm_reg_t TIM_CH1_CNTRL;
	arm_reg_t TIM_CH2_CNTRL;
	arm_reg_t TIM_CH3_CNTRL;
	arm_reg_t TIM_CH4_CNTRL;
	arm_reg_t TIM_CH1_CNTRL1;
	arm_reg_t TIM_CH2_CNTRL1;
	arm_reg_t TIM_CH3_CNTRL1;
	arm_reg_t TIM_CH4_CNTRL1;
	arm_reg_t TIM_CH1_DTG;
	arm_reg_t TIM_CH2_DTG;
	arm_reg_t TIM_CH3_DTG;
	arm_reg_t TIM_CH4_DTG;
	arm_reg_t TIM_BRKETR_CNTRL;
	arm_reg_t TIM_STATUS;
	arm_reg_t TIM_IE;
	arm_reg_t TIM_DMA_RE;
	arm_reg_t TIM_CH1_CNTRL2;
	arm_reg_t TIM_CH2_CNTRL2;
	arm_reg_t TIM_CH3_CNTRL2;
	arm_reg_t TIM_CH4_CNTRL2;
	arm_reg_t TIM_CCR11;
	arm_reg_t TIM_CCR21;
	arm_reg_t TIM_CCR31;
	arm_reg_t TIM_CCR41;
} TIMER_t;

#define ARM_TIMER1		((TIMER_t*) ARM_TIMER1_BASE)
#define ARM_TIMER2		((TIMER_t*) ARM_TIMER2_BASE)
#define ARM_TIMER3		((TIMER_t*) ARM_TIMER3_BASE)

/*------------------------------------------------------
 * Battery backup
 */
typedef struct
{
	arm_reg_t BKP_REG_00;		/* Регистр аварийного сохранения 0 */
	unsigned reserved1 [13];
	arm_reg_t BKP_REG_0E;		/* Регистр аварийного сохранения 14 */
	arm_reg_t BKP_REG_0F;		/* Регистр аварийного сохранения 15 и
					 * управления блоками RTC, LSE, LSI и HSI*/
	arm_reg_t RTC_CNT;		/* Регистр основного счетчика часов
					 * реального времени */
	arm_reg_t RTC_DIV;		/* Регистр предварительного делителя
					 * основного счетчика */
	arm_reg_t RTC_PRL;		/* Регистр основания счета
					 * предварительного делителя */
	arm_reg_t RTC_ALRM;		/* Регистр значения для сравнения
					 * основного счетчика и выработки сигнала ALRF */
	arm_reg_t RTC_CS;		/* Регистр управления и состояния
					 * флагов часов реального времени */
} BACKUP_t;

#define ARM_BACKUP		((BACKUP_t*) ARM_BKP_BASE)

/*
 * Регистр BKP_REG_0E.
 */
#define ARM_BKP_REG_0E_LOW		(7 << 0)	/* Выбор режима работы
							 * регулятора 1.8В */
#define ARM_BKP_REG_0E_SELECT_RI	(7 << 3)	/* Выбор дополнительной
							 *  нагрузки для регулятора 1.8В */
#define ARM_BKP_REG_0E_JTAG_A		(1 << 6)	/* Разрешение работы порта JTAG A */
#define ARM_BKP_REG_0E_JTAG_B		(1 << 7)	/* Разрешение работы порта JTAG B */
#define ARM_BKP_REG_0E_TRIM		(7 << 8)	/* Коэффициент настройки опорного
							 * напряжения регулятора */
#define ARM_BKP_REG_0E_FPOR		(1 << 11)	/* Флаг срабатывания POR */

/*------------------------------------------------------
 * Universal Serial Bus
 */
typedef struct
{
	arm_reg_t ENDPOINT_CONTROL_REG;			// [4:0] - R/W
	arm_reg_t ENDPOINT_STATUS_REG;			// [7:0] - R/W
	arm_reg_t ENDPOINT_TRANSTYPE_STATUS_REG;	// [1:0] - R/W
	arm_reg_t ENDPOINT_NAK_TRANSTYPE_STATUS_REG;	// [1:0] - R/W
} EndPointStatusRegs;

typedef struct
{
	arm_reg_t EP_RX_FIFO_DATA;			// [7:0] - R/W
	unsigned reserved1;
	arm_reg_t EP_RX_FIFO_DATA_COUNTL;		// [15:0] - R/W
	arm_reg_t EP_RX_FIFO_DATA_COUNTH;		// [15:0] - R/W
	arm_reg_t EP_RX_FIFO_CONTROL_REG;		// [0:0] - R/W
	unsigned reserved2 [11];
	arm_reg_t EP_TX_FIFO_DATA;			// [7:0] - R/W
	unsigned reserved4 [3];
	arm_reg_t EP_TX_FIFO_CONTROL_REG;		// [0:0] - R/W
	unsigned reserved5 [11];
} EndPointFifoRegs;

typedef struct
{
	// Host Regs
	arm_reg_t HOST_TX_CONTROL_REG;			// [3:0] - R/W
	arm_reg_t HOST_TX_TRANS_TYPE_REG;		// [1:0] - R/W
	arm_reg_t HOST_TX_LINE_CONTROL_REG;		// [4:0] - R/W
	arm_reg_t HOST_TX_SOF_ENABLE_REG;		// [0:0] - R/W
	arm_reg_t HOST_TX_ADDR_REG;			// [6:0] - R/W
	arm_reg_t HOST_TX_ENDP_REG;			// [3:0] - R/W
	arm_reg_t HOST_FRAME_NUM_REGL;			// [10:0]- R/W
	arm_reg_t HOST_FRAME_NUM_REGH;			// [10:0]- R/W
	arm_reg_t HOST_INTERRUPT_STATUS_REG;		// [3:0] - R/O
	arm_reg_t HOST_INTERRUPT_MASK_REG;		// [3:0] - R/W
	arm_reg_t HOST_RX_STATUS_REG;			// [7:0] - R/O
	arm_reg_t HOST_RX_PID_REG;			// [3:0] - R/O
	arm_reg_t HOST_RX_ADDR_REG;			// [6:0] - R/O
	arm_reg_t HOST_RX_ENDP_REG;			// [3:0] - R/O
	arm_reg_t HOST_RX_CONNECT_STATE_REG;		// [1:0] - R/O
	arm_reg_t HOST_SOF_TIMER_MSB_REG;		// [7:0] - R/O
	unsigned reserved1 [16];
	arm_reg_t HOST_RX_FIFO_DATA;			// [7:0] - R/O
	unsigned reserved2;
	arm_reg_t HOST_RX_FIFO_DATA_COUNTL;		// [15:0] - R/O
	arm_reg_t HOST_RX_FIFO_DATA_COUNTH;		// [15:0] - R/O
	arm_reg_t HOST_RX_FIFO_CONTROL_REG;		// [0:0] - R/W
	unsigned reserved3 [11];
	arm_reg_t HOST_TX_FIFO_DATA;			// [7:0] - R/W
	unsigned reserved4 [3];
	arm_reg_t HOST_TX_FIFO_CONTROL_REG;  		// [0:0] - R/W
	unsigned reserved5 [11];

	// Slave Regs
	EndPointStatusRegs EndPointStatusRegs [4];
	arm_reg_t SC_CONTROL_REG; 			// [5:0] - R/W
	arm_reg_t SC_LINE_STATUS_REG;			// [1:0] - R/W
	arm_reg_t SC_INTERRUPT_STATUS_REG;		// [5:0] - R/W
	arm_reg_t SC_INTERRUPT_MASK_REG;		// [5:0] - R/W
	arm_reg_t SC_ADDRESS; 				// [6:0] - R/W
	arm_reg_t SC_FRAME_NUML;			// [10:0] - R/W
	arm_reg_t SC_FRAME_NUMH;			// [10:0] - R/W
	unsigned reserved6 [9];
	EndPointFifoRegs EndPointFifoRegs [4];

	arm_reg_t HOST_SLAVE_CONTROL_REG;    		// [1:0] - R/W
	arm_reg_t HOST_SLAVE_VERSION_REG;		// [7:0] - R/O
} USB_t;

#define ARM_USB			((USB_t*) ARM_USB_BASE)

/*------------------------------------------------------
 * DMA Controller
 */
typedef struct
{
	arm_reg_t STATUS;		// DMA status
	arm_reg_t CONFIG;		// DMA configuration
	arm_reg_t CTRL_BASE_PTR;	// Channel control data base pointer
	arm_reg_t ALT_CTRL_BASE_PTR;	// Channel alternate control data base pointer
	arm_reg_t WAITONREQ_STATUS;	// Channel wait on request status
	arm_reg_t CHNL_SW_REQUEST;	// Channel software request
	arm_reg_t CHNL_USEBURST_SET;	// Channel useburst set
	arm_reg_t CHNL_USEBURST_CLR;	// Channel useburst clear
	arm_reg_t CHNL_REQ_MASK_SET;	// Channel request mask set
	arm_reg_t CHNL_REQ_MASK_CLR;	// Channel request mask clear
	arm_reg_t CHNL_ENABLE_SET;	// Channel enable set
	arm_reg_t CHNL_ENABLE_CLR;	// Channel enable clear
	arm_reg_t CHNL_PRI_ALT_SET;	// Channel primary-alternate set
	arm_reg_t CHNL_PRI_ALT_CLR;	// Channel primary-alternate clear
	arm_reg_t CHNL_PRIORITY_SET;	// Channel priority set
	arm_reg_t CHNL_PRIORITY_CLR;	// Channel priority clear
	unsigned reserved0 [3];
	arm_reg_t ERR_CLR;		// Bus error clear
} DMA_Controller_t;

typedef struct
{
	arm_reg_t INTEGRATION_CFG;
	unsigned reserved0;
	arm_reg_t STALL_STATUS;
	unsigned reserved1;
	arm_reg_t REQ_STATUS;
	unsigned reserved2;
	arm_reg_t SREQ_STATUS;
	unsigned reserved3;
	arm_reg_t DONE_SET;
	arm_reg_t DONE_CLR;
	arm_reg_t ACTIVE_SET;
	arm_reg_t ACTIVE_CLR;
	unsigned reserved4 [5];
	arm_reg_t ERR_SET;
} DMA_Test_t;

typedef struct
{
	arm_reg_t PERIPH_ID4;	// Peripheral identification 4
	unsigned reserved0[3];
	arm_reg_t PERIPH_ID0;
	arm_reg_t PERIPH_ID1;
	arm_reg_t PERIPH_ID2;
	arm_reg_t PERIPH_ID3;
} DMA_Periph_Identification_t;

typedef struct
{
	arm_reg_t PCELL_ID0;	// PrimeCell identification 0
	arm_reg_t PCELL_ID1;
	arm_reg_t PCELL_ID2;
	arm_reg_t PCELL_ID3;
} DMA_PrimeCell_Identification_t;

// Channel control data structure
typedef struct
{
	arm_reg_t PRIMARY_CH0_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH0_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH0_CONTROL;
	arm_reg_t PRIMARY_CH0_UNUSED;

	arm_reg_t PRIMARY_CH1_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH1_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH1_CONTROL;
	arm_reg_t PRIMARY_CH1_UNUSED;

	arm_reg_t PRIMARY_CH2_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH2_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH2_CONTROL;
	arm_reg_t PRIMARY_CH2_UNUSED;

	arm_reg_t PRIMARY_CH3_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH3_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH3_CONTROL;
	arm_reg_t PRIMARY_CH3_UNUSED;

	arm_reg_t PRIMARY_CH4_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH4_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH4_CONTROL;
	arm_reg_t PRIMARY_CH4_UNUSED;

	arm_reg_t PRIMARY_CH5_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH5_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH5_CONTROL;
	arm_reg_t PRIMARY_CH5_UNUSED;

	arm_reg_t PRIMARY_CH6_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH6_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH6_CONTROL;
	arm_reg_t PRIMARY_CH6_UNUSED;

	arm_reg_t PRIMARY_CH7_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH7_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH7_CONTROL;
	arm_reg_t PRIMARY_CH7_UNUSED;

	arm_reg_t PRIMARY_CH8_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH8_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH8_CONTROL;
	arm_reg_t PRIMARY_CH8_UNUSED;

	arm_reg_t PRIMARY_CH9_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH9_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH9_CONTROL;
	arm_reg_t PRIMARY_CH9_UNUSED;

	arm_reg_t PRIMARY_CH10_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH10_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH10_CONTROL;
	arm_reg_t PRIMARY_CH10_UNUSED;

	arm_reg_t PRIMARY_CH11_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH11_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH11_CONTROL;
	arm_reg_t PRIMARY_CH11_UNUSED;

	arm_reg_t PRIMARY_CH12_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH12_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH12_CONTROL;
	arm_reg_t PRIMARY_CH12_UNUSED;

	arm_reg_t PRIMARY_CH13_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH13_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH13_CONTROL;
	arm_reg_t PRIMARY_CH13_UNUSED;

	arm_reg_t PRIMARY_CH14_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH14_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH14_CONTROL;
	arm_reg_t PRIMARY_CH14_UNUSED;

	arm_reg_t PRIMARY_CH15_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH15_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH15_CONTROL;
	arm_reg_t PRIMARY_CH15_UNUSED;

	arm_reg_t PRIMARY_CH16_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH16_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH16_CONTROL;
	arm_reg_t PRIMARY_CH16_UNUSED;

	arm_reg_t PRIMARY_CH17_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH17_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH17_CONTROL;
	arm_reg_t PRIMARY_CH17_UNUSED;

	arm_reg_t PRIMARY_CH18_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH18_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH18_CONTROL;
	arm_reg_t PRIMARY_CH18_UNUSED;

	arm_reg_t PRIMARY_CH19_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH19_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH19_CONTROL;
	arm_reg_t PRIMARY_CH19_UNUSED;

	arm_reg_t PRIMARY_CH20_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH20_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH20_CONTROL;
	arm_reg_t PRIMARY_CH20_UNUSED;

	arm_reg_t PRIMARY_CH21_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH21_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH21_CONTROL;
	arm_reg_t PRIMARY_CH21_UNUSED;

	arm_reg_t PRIMARY_CH22_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH22_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH22_CONTROL;
	arm_reg_t PRIMARY_CH22_UNUSED;

	arm_reg_t PRIMARY_CH23_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH23_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH23_CONTROL;
	arm_reg_t PRIMARY_CH23_UNUSED;

	arm_reg_t PRIMARY_CH24_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH24_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH24_CONTROL;
	arm_reg_t PRIMARY_CH24_UNUSED;

	arm_reg_t PRIMARY_CH25_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH25_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH25_CONTROL;
	arm_reg_t PRIMARY_CH25_UNUSED;

	arm_reg_t PRIMARY_CH26_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH26_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH26_CONTROL;
	arm_reg_t PRIMARY_CH26_UNUSED;

	arm_reg_t PRIMARY_CH27_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH27_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH27_CONTROL;
	arm_reg_t PRIMARY_CH27_UNUSED;

	arm_reg_t PRIMARY_CH28_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH28_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH28_CONTROL;
	arm_reg_t PRIMARY_CH28_UNUSED;

	arm_reg_t PRIMARY_CH29_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH29_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH29_CONTROL;
	arm_reg_t PRIMARY_CH29_UNUSED;

	arm_reg_t PRIMARY_CH30_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH30_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH30_CONTROL;
	arm_reg_t PRIMARY_CH30_UNUSED;

	arm_reg_t PRIMARY_CH31_SOURCE_END_POINTER;
	arm_reg_t PRIMARY_CH31_DEST_END_POINTER;
	arm_reg_t PRIMARY_CH31_CONTROL;
	arm_reg_t PRIMARY_CH31_UNUSED;
} DMA_PrimaryData_t;

typedef struct
{
	arm_reg_t ALT_CH0_SOURCE_END_POINTER;
	arm_reg_t ALT_CH0_DEST_END_POINTER;
	arm_reg_t ALT_CH0_CONTROL;
	arm_reg_t ALT_CH0_UNUSED;

	arm_reg_t ALT_CH1_SOURCE_END_POINTER;
	arm_reg_t ALT_CH1_DEST_END_POINTER;
	arm_reg_t ALT_CH1_CONTROL;
	arm_reg_t ALT_CH1_UNUSED;

	arm_reg_t ALT_CH2_SOURCE_END_POINTER;
	arm_reg_t ALT_CH2_DEST_END_POINTER;
	arm_reg_t ALT_CH2_CONTROL;
	arm_reg_t ALT_CH2_UNUSED;

	arm_reg_t ALT_CH3_SOURCE_END_POINTER;
	arm_reg_t ALT_CH3_DEST_END_POINTER;
	arm_reg_t ALT_CH3_CONTROL;
	arm_reg_t ALT_CH3_UNUSED;

	arm_reg_t ALT_CH4_SOURCE_END_POINTER;
	arm_reg_t ALT_CH4_DEST_END_POINTER;
	arm_reg_t ALT_CH4_CONTROL;
	arm_reg_t ALT_CH4_UNUSED;

	arm_reg_t ALT_CH5_SOURCE_END_POINTER;
	arm_reg_t ALT_CH5_DEST_END_POINTER;
	arm_reg_t ALT_CH5_CONTROL;
	arm_reg_t ALT_CH5_UNUSED;

	arm_reg_t ALT_CH6_SOURCE_END_POINTER;
	arm_reg_t ALT_CH6_DEST_END_POINTER;
	arm_reg_t ALT_CH6_CONTROL;
	arm_reg_t ALT_CH6_UNUSED;

	arm_reg_t ALT_CH7_SOURCE_END_POINTER;
	arm_reg_t ALT_CH7_DEST_END_POINTER;
	arm_reg_t ALT_CH7_CONTROL;
	arm_reg_t ALT_CH7_UNUSED;

	arm_reg_t ALT_CH8_SOURCE_END_POINTER;
	arm_reg_t ALT_CH8_DEST_END_POINTER;
	arm_reg_t ALT_CH8_CONTROL;
	arm_reg_t ALT_CH8_UNUSED;

	arm_reg_t ALT_CH9_SOURCE_END_POINTER;
	arm_reg_t ALT_CH9_DEST_END_POINTER;
	arm_reg_t ALT_CH9_CONTROL;
	arm_reg_t ALT_CH9_UNUSED;

	arm_reg_t ALT_CH10_SOURCE_END_POINTER;
	arm_reg_t ALT_CH10_DEST_END_POINTER;
	arm_reg_t ALT_CH10_CONTROL;
	arm_reg_t ALT_CH10_UNUSED;

	arm_reg_t ALT_CH11_SOURCE_END_POINTER;
	arm_reg_t ALT_CH11_DEST_END_POINTER;
	arm_reg_t ALT_CH11_CONTROL;
	arm_reg_t ALT_CH11_UNUSED;

	arm_reg_t ALT_CH12_SOURCE_END_POINTER;
	arm_reg_t ALT_CH12_DEST_END_POINTER;
	arm_reg_t ALT_CH12_CONTROL;
	arm_reg_t ALT_CH12_UNUSED;

	arm_reg_t ALT_CH13_SOURCE_END_POINTER;
	arm_reg_t ALT_CH13_DEST_END_POINTER;
	arm_reg_t ALT_CH13_CONTROL;
	arm_reg_t ALT_CH13_UNUSED;

	arm_reg_t ALT_CH14_SOURCE_END_POINTER;
	arm_reg_t ALT_CH14_DEST_END_POINTER;
	arm_reg_t ALT_CH14_CONTROL;
	arm_reg_t ALT_CH14_UNUSED;

	arm_reg_t ALT_CH15_SOURCE_END_POINTER;
	arm_reg_t ALT_CH15_DEST_END_POINTER;
	arm_reg_t ALT_CH15_CONTROL;
	arm_reg_t ALT_CH15_UNUSED;

	arm_reg_t ALT_CH16_SOURCE_END_POINTER;
	arm_reg_t ALT_CH16_DEST_END_POINTER;
	arm_reg_t ALT_CH16_CONTROL;
	arm_reg_t ALT_CH16_UNUSED;

	arm_reg_t ALT_CH17_SOURCE_END_POINTER;
	arm_reg_t ALT_CH17_DEST_END_POINTER;
	arm_reg_t ALT_CH17_CONTROL;
	arm_reg_t ALT_CH17_UNUSED;

	arm_reg_t ALT_CH18_SOURCE_END_POINTER;
	arm_reg_t ALT_CH18_DEST_END_POINTER;
	arm_reg_t ALT_CH18_CONTROL;
	arm_reg_t ALT_CH18_UNUSED;

	arm_reg_t ALT_CH19_SOURCE_END_POINTER;
	arm_reg_t ALT_CH19_DEST_END_POINTER;
	arm_reg_t ALT_CH19_CONTROL;
	arm_reg_t ALT_CH19_UNUSED;

	arm_reg_t ALT_CH20_SOURCE_END_POINTER;
	arm_reg_t ALT_CH20_DEST_END_POINTER;
	arm_reg_t ALT_CH20_CONTROL;
	arm_reg_t ALT_CH20_UNUSED;

	arm_reg_t ALT_CH21_SOURCE_END_POINTER;
	arm_reg_t ALT_CH21_DEST_END_POINTER;
	arm_reg_t ALT_CH21_CONTROL;
	arm_reg_t ALT_CH21_UNUSED;

	arm_reg_t ALT_CH22_SOURCE_END_POINTER;
	arm_reg_t ALT_CH22_DEST_END_POINTER;
	arm_reg_t ALT_CH22_CONTROL;
	arm_reg_t ALT_CH22_UNUSED;

	arm_reg_t ALT_CH23_SOURCE_END_POINTER;
	arm_reg_t ALT_CH23_DEST_END_POINTER;
	arm_reg_t ALT_CH23_CONTROL;
	arm_reg_t ALT_CH23_UNUSED;

	arm_reg_t ALT_CH24_SOURCE_END_POINTER;
	arm_reg_t ALT_CH24_DEST_END_POINTER;
	arm_reg_t ALT_CH24_CONTROL;
	arm_reg_t ALT_CH24_UNUSED;

	arm_reg_t ALT_CH25_SOURCE_END_POINTER;
	arm_reg_t ALT_CH25_DEST_END_POINTER;
	arm_reg_t ALT_CH25_CONTROL;
	arm_reg_t ALT_CH25_UNUSED;

	arm_reg_t ALT_CH26_SOURCE_END_POINTER;
	arm_reg_t ALT_CH26_DEST_END_POINTER;
	arm_reg_t ALT_CH26_CONTROL;
	arm_reg_t ALT_CH26_UNUSED;

	arm_reg_t ALT_CH27_SOURCE_END_POINTER;
	arm_reg_t ALT_CH27_DEST_END_POINTER;
	arm_reg_t ALT_CH27_CONTROL;
	arm_reg_t ALT_CH27_UNUSED;

	arm_reg_t ALT_CH28_SOURCE_END_POINTER;
	arm_reg_t ALT_CH28_DEST_END_POINTER;
	arm_reg_t ALT_CH28_CONTROL;
	arm_reg_t ALT_CH28_UNUSED;

	arm_reg_t ALT_CH29_SOURCE_END_POINTER;
	arm_reg_t ALT_CH29_DEST_END_POINTER;
	arm_reg_t ALT_CH29_CONTROL;
	arm_reg_t ALT_CH29_UNUSED;

	arm_reg_t ALT_CH30_SOURCE_END_POINTER;
	arm_reg_t ALT_CH30_DEST_END_POINTER;
	arm_reg_t ALT_CH30_CONTROL;
	arm_reg_t ALT_CH30_UNUSED;

	arm_reg_t ALT_CH31_SOURCE_END_POINTER;
	arm_reg_t ALT_CH31_DEST_END_POINTER;
	arm_reg_t ALT_CH31_CONTROL;
	arm_reg_t ALT_CH31_UNUSED;
} DMA_AltData_t;

#define ARM_DMA                	((DMA_Controller_t*) ARM_DMA_BASE)
#define ARM_DMA_Test		((DMA_Test_t*) (ARM_DMA_BASE + 0x0E00))
#define ARM_DMA_PeriphID       	((DMA_Periph_Identification_t*) (ARM_DMA_BASE + 0x0FD0))
#define ARM_DMA_PrimeCellID   	((DMA_PrimeCell_Identification_t*) (ARM_DMA_BASE + 0x0FF0))

/*------------------------------------------------------
 * Описание регистров контроллера Flash памяти программ.
 */
typedef struct
{
	arm_reg_t CMD;		/* Управление Flash-памятью */
	arm_reg_t ADR;		/* Адрес (словный) */
	arm_reg_t DI;		/* Данные для записи */
	arm_reg_t DO;		/* Считанные данные */
	arm_reg_t KEY;		/* Ключ */
} EEPROM_t;

#define ARM_EEPROM		((EEPROM_t*) ARM_EEPROM_BASE)

/*
 * Регистр EEPROM CMD
 */
#define ARM_EEPROM_CMD_CON	0x00000001
				/*
				 * Переключение контроллера памяти EEPROM на
				 * регистровое управление. Не может производиться
				 * при исполнении программы из области EERPOM.
				 * 0 – управление EERPOM от ядра, рабочий режим
				 * 1 – управление от регистров, режим программирования
				 */

#define ARM_EEPROM_CMD_WR	0x00000002
				/*
				 * Запись в память EERPOM (в режиме программирования)
				 * 0 – нет записи
				 * 1 – есть запись
				 */

#define ARM_EEPROM_CMD_RD	0x00000004
				/*
				 * Чтение из память EERPOM (в режиме программирования)
				 * 0 – нет чтения
				 * 1 – есть чтение
				 */

#define ARM_EEPROM_CMD_DELAY_MASK	0x00000038
				/*
				 * Задержка памяти программ при чтении
				 */
#define ARM_EEPROM_CMD_DELAY_0	0x00000000	/* 0 тактов - до 25 МГц */
#define ARM_EEPROM_CMD_DELAY_1	0x00000008      /* 1 такт - до 50 МГц */
#define ARM_EEPROM_CMD_DELAY_2	0x00000010      /* 2 такта - до 75 МГц */
#define ARM_EEPROM_CMD_DELAY_3	0x00000018      /* 3 такта - до 100 МГц */
#define ARM_EEPROM_CMD_DELAY_4	0x00000020      /* 4 такта - до 125 МГц */
#define ARM_EEPROM_CMD_DELAY_5	0x00000028      /* 5 тактов - до 150 МГц */
#define ARM_EEPROM_CMD_DELAY_6	0x00000030      /* 6 тактов - до 175 МГц */
#define ARM_EEPROM_CMD_DELAY_7	0x00000038      /* 7 тактов - до 200 МГц */

#define ARM_EEPROM_CMD_XE 	0x00000040
				/*
				 * Выдача адреса ADR[16:9]
				 * 0 – не разрешено
				 * 1 - разрешено
				 */

#define ARM_EEPROM_CMD_YE 	0x00000080
				/*
				 * Выдача адреса ADR[8:2]
				 * 0 – не разрешено
				 * 1 – разрешено
				 */

#define ARM_EEPROM_CMD_SE 	0x00000100
				/*
				 * Усилитель считывания
				 * 0 – не включен
				 * 1 – включен
				 */

#define ARM_EEPROM_CMD_IFREN 	0x00000200
				/*
				 * Работа с блоком информации
				 * 0 – основная память
				 * 1 – информационный блок
				 */

#define ARM_EEPROM_CMD_ERASE 	0x00000400
				/*
				 * Стереть строку с адресом ADR[16:9].
				 * ADR[8:0] значения не имеет.
				 * 0 – нет стирания
				 * 1 – стирание
				 */

#define ARM_EEPROM_CMD_MAS1 	0x00000800
				/*
				 * Стереть весь блок, при ERASE=1
				 * 0 – нет стирания
				 * 1 – стирание
				 */

#define ARM_EEPROM_CMD_PROG 	0x00001000
				/*
				 * Записать данные по ADR[16:2] из регистра EERPOM_DI
				 * 0 – нет записи
				 * 1 – есть запись
				 */

#define ARM_EEPROM_CMD_NVSTR	0x00002000
				/*
				 * Операции записи или стирания
				 * 0 – при чтении
				 * 1 - при записи или стирании
				 */

/*------------------------------------------------------
 * Контроллер CAN.
 */
typedef struct
{
	arm_reg_t ID; 		/* Идентификатор сообщения */
	arm_reg_t DLC;		/* Длина и формат сообщения */
	arm_reg_t DATAL; 	/* Младшая часть данных */
	arm_reg_t DATAH;	/* Старшая часть данных */
} CAN_BUF_t;

typedef struct
{
	arm_reg_t MASK;	 	/* Маска для приема сообщения */
	arm_reg_t FILTER; 	/* Фильтр для приема сообщения */
} CAN_MASK_t;

typedef struct
{
	arm_reg_t CONTROL;	/* Управление */
	arm_reg_t STATUS;	/* Состояние */
	arm_reg_t BITTMNG;	/* Задание скорости работы */
	arm_reg_t reserved0;
	arm_reg_t INT_EN;	/* Разрешение прерываний */
	arm_reg_t reserved1;
	arm_reg_t reserved2;
	arm_reg_t OVER;		/* Границы счетчика ошибок */
	arm_reg_t RXID;		/* Идентификатор принятого сообщения */
	arm_reg_t RXDLC;	/* Длина и формат принятого сообщения */
	arm_reg_t RXDATAL; 	/* Младшая часть принятых данных */
	arm_reg_t RXDATAH;	/* Старшая часть принятых данных */
	arm_reg_t TXID;		/* Идентификатор передаваемого сообщения */
	arm_reg_t TXDLC;	/* Длина и формат передаваемого сообщения */
	arm_reg_t DATAL; 	/* Младшая часть передаваемых данных */
	arm_reg_t DATAH;	/* Старшая часть передаваемых данных */
	arm_reg_t BUF_CON [32];	/* Управление буферами 1..32 */
	arm_reg_t INT_RX;	/* Разрешение прерываний от приемных буферов */
	arm_reg_t RX;		/* Флаги RX_FULL от приемных буферов */
	arm_reg_t INT_TX;	/* Разрешение прерываний от передающих буферов */
	arm_reg_t TX;	 	/* Флаги ~TX_REQ от передающих буферов */
	arm_reg_t reserved3 [76];
	CAN_BUF_t BUF [32];	/* Буферы 1..32: ID, DLC, DATA */
	arm_reg_t reserved4 [64];
	CAN_MASK_t MASK [32];	/* Маски и фильтры для приёма в буферы 1..32 */
} CAN_t;

#define ARM_CAN1		((CAN_t*) ARM_CAN1_BASE)
#define ARM_CAN2		((CAN_t*) ARM_CAN2_BASE)

/*
 * Регистр CAN CONTROL - управление контроллером
 */
#define	CAN_CONTROL_EN		(1 << 0)	/* Разрешение работы */
#define	CAN_CONTROL_ROM		(1 << 1)	/* Только на приём */
#define	CAN_CONTROL_STM		(1 << 2)	/* Режим самотестирования */
#define	CAN_CONTROL_SAP		(1 << 3)	/* Подтверждаем приём собственных пакетов */
#define	CAN_CONTROL_ROP		(1 << 4)	/* Принимаем собственные пакеты */

/*
 * Регистр CAN STATUS - состояние контроллера
 */
#define	CAN_STATUS_TEC(s)	((s) >> 24)	/* Счетчик ошибок передатчика */
#define	CAN_STATUS_REC(s)	((s) >> 16 & 0xFF) /* Счетчик ошибок приемника */
#define	CAN_STATUS_TEC8		(1 << 12)	/* TEC более 255 */
#define	CAN_STATUS_REC8		(1 << 11)	/* REC более 255 */
#define	CAN_STATUS_BUS_OFF	(1 << 10)	/* Ожидается восстановление шины */
#define	CAN_STATUS_ERR_ACTIVE	(1 << 9)	/* Отсылается флаг пассивной ошибки */
#define	CAN_STATUS_ID_LOWER	(1 << 8)	/* При передаче был проигран арбитраж */
#define	CAN_STATUS_ERR_ACK	(1 << 7)	/* Ошибка подтверждения приема */
#define	CAN_STATUS_ERR_FRAME	(1 << 6)	/* Ошибка формата пакета */
#define	CAN_STATUS_ERR_CRC	(1 << 5)	/* Ошибка контрольной суммы пакета */
#define	CAN_STATUS_ERR_BITSTUFF	(1 << 4)	/* Ошибка вставленных битов пакета */
#define	CAN_STATUS_ERR_BIT	(1 << 3)	/* Ошибка передаваемых битов пакета */
#define	CAN_STATUS_ERR_OVER	(1 << 2)	/* Превышение заданного уровня TEC и REC */
#define	CAN_STATUS_TX_READY	(1 << 1)	/* Есть буфер готовый для отправки сообщений */
#define	CAN_STATUS_RX_READY	(1 << 0)	/* Есть буфер с принятым сообщением */

/*
 * Регистр CAN BITTMNG - задание скорости работы
 */
#define	CAN_BITTMNG_SB		(1 << 27)	/* Трехкратное семплирование */
#define	CAN_BITTMNG_SJW(n)	((n-1) << 25)	/* Размер фазы SJW */
#define	CAN_BITTMNG_SEG2(n)	((n-1) << 22)	/* Размер фазы SEG2 */
#define	CAN_BITTMNG_SEG1(n)	((n-1) << 19)	/* Размер фазы SEG1 */
#define	CAN_BITTMNG_PSEG(n)	((n-1) << 16)	/* Размер фазы PSEG */
#define	CAN_BITTMNG_BRP(n)	(n)		/* Предделитель системной частоты */

/*
 * Регистр CAN INT_EN - разрешение прерываний
 */
#define	CAN_INT_EN_ERR_OVER	(1 << 4)	/* Счётчик TEC или REC переполнился */
#define	CAN_INT_EN_ERR		(1 << 3)	/* Возникла ошибка */
#define	CAN_INT_EN_TX		(1 << 2)	/* Возможна передача */
#define	CAN_INT_EN_RX		(1 << 1)	/* Принято сообщение */
#define	CAN_INT_EN_GLB		(1 << 0)	/* Общее разрешение прерывания */

/*
 * Регистры CAN BUF_CON[0..31] - управление буферами
 */
#define	CAN_BUF_CON_EN		(1 << 0)	/* Разрешение работы буфера */
#define	CAN_BUF_CON_RX_ON	(1 << 1)	/* Режим работы буфера: на приём */
#define	CAN_BUF_CON_OVER_EN	(1 << 2)	/* Разрешение перезаписи принятого сообщения */
#define	CAN_BUF_CON_RTR_EN	(1 << 3)	/* Ответить при приеме RTR в буфер */
#define	CAN_BUF_CON_PRIOR_0	(1 << 4)	/* Приоритет при отправке */
#define	CAN_BUF_CON_TX_REQ	(1 << 5)	/* Запрос на отправку сообщения */
#define	CAN_BUF_CON_RX_FULL	(1 << 6)	/* Флаг готовности приема */
#define	CAN_BUF_CON_OVER_WR	(1 << 7)	/* Флаг перезаписи принятого сообщения */

/*
 * Регистры CAN BUF[0..31].ID - идентификатор сообщения
 */
#define	CAN_ID_EID_SHIFT	0		/* Сдвиг поля EID */
#define	CAN_ID_EID_MASK		0x3FFFF		/* Маска поля EID */
#define	CAN_ID_SID_SHIFT	18		/* Сдвиг поля SID */
#define	CAN_ID_SID_MASK		(0x7FF << 18)	/* Маска поля SID */

/*
 * Регистры CAN BUF[0..31].DLC - длина и формат сообщения
 */
#define	CAN_DLC_LEN(n)		((n) & 15)	/* Длина данных в байтах */
#define	CAN_DLC_RTR		(1 << 8)	/* Запрос обратного ответа */
#define	CAN_DLC_R1		(1 << 9)	/* Всегда должно быть “1” */
#define	CAN_DLC_R0		(1 << 10)	/* Всегда должно быть “0” */
#define	CAN_DLC_SSR		(1 << 11)	/* Всегда должно быть “1” */
#define	CAN_DLC_IDE		(1 << 12)	/* Расширенный формат */


/* End of Milandr 1986BE9x register definitions.
 *----------------------------------------------*/
