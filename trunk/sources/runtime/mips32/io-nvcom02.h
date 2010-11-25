/*
 * TODO
 * Hardware register defines for Elvees NVCom-02 microcontroller.
 */
#ifndef _IO_NVCOM02_H
#define _IO_NVCOM02_H

#define MC_R(a)		*(volatile unsigned*)(0xB82F0000 + (a))

/*
 * Регистры DMA MEM(0-3)
 */
#define MC_CSR_MEM(n)	MC_R (0x0000+(n<<7))	/* Управление и состояние */
#define MC_CP_MEM(n)	MC_R (0x0004+(n<<7))	/* Указатель цепочки */
#define MC_IR0_MEM(n)	MC_R (0x0008+(n<<7))	/* Индекс "0" внешней памяти */
#define MC_IR1_MEM(n)	MC_R (0x000C+(n<<7))	/* Индекс "1" внешней памяти */
#define MC_OR_MEM(n)	MC_R (0x0010+(n<<7))	/* Смещение внешней памяти */
#define MC_Y_MEM(n)	MC_R (0x0014+(n<<7))	/* Параметры направления Y при
						 * двухмерной адресации */
#define MC_RUN_MEM(n)	MC_R (0x0018+(n<<7))	/* Управление состоянием бита RUN */

/*
 * Регистры DMA VPIN
 */
#define MC_DMACSR_VPIN	MC_R (0x8800)   /* Управление и состояние */
#define MC_CP_VPIN	MC_R (0x8804)   /* Указатель цепочки */
#define MC_IR_VPIN	MC_R (0x8808)   /* Индекс */
#define MC_OR_VPIN	MC_R (0x880C)   /* Смещение */
#define MC_Y_VPIN	MC_R (0x8810)   /* Параметры направления Y при двухмерной адресации */
#define MC_RUN_VPIN	MC_R (0x8814)   /* Управления состоянием бита RUN */

/*
 * Регистры DMA VPOUT
 */
#define MC_DMACSR_VPOUT	MC_R (0x9800)   /* Управление и состояние */
#define MC_CP_VPOUT	MC_R (0x9804)   /* Указатель цепочки */
#define MC_IR_VPOUT	MC_R (0x9808)   /* Индекс */
#define MC_OR_VPOUT	MC_R (0x980C)   /* Смещение */
#define MC_Y_VPOUT	MC_R (0x9810)   /* Параметры направления Y при двухмерной адресации */
#define MC_RUN_VPOUT	MC_R (0x9814)   /* Управление состоянием бита RUN */

/*
 * Регистры DMA EMAC(0-1)
 */
#define MC_CSR_EMAC(n)	MC_R (0xE800+(n<<6))	/* Управление и состояние */
#define MC_CP_EMAC(n)	MC_R (0xE804+(n<<6))	/* Указатель цепочки */
#define MC_IR_EMAC(n)	MC_R (0xE808+(n<<6))	/* Индекс */
#define MC_RUN_EMAC(n)	MC_R (0xE80C+(n<<6))	/* Управление состоянием бита RUN */

/*
 * Регистры DMA USB(0-3)
 */
#define MC_CSR_USB(n)	MC_R (0xC800+(n<<6))	/* Управление и состояние */
#define MC_CP_USB(n)	MC_R (0xC804+(n<<6))	/* Указатель цепочки */
#define MC_IR_USB(n)	MC_R (0xC808+(n<<6))	/* Индекс */
#define MC_OR_USB(n)	MC_R (0xC80C+(n<<6))	/* Смещение */
#define MC_Y_USB(n)	MC_R (0xC810+(n<<6))	/* Параметры направления Y при двухмерной адресации */
#define MC_RUN_USB(n)	MC_R (0xC814+(n<<6))	/* Управление состоянием бита RUN */

/*
 * Регистры DMA MFBSP(0-3)
 */
#define MC_CSR_MFBSP(n)	MC_R (0x7800+(n<<6))	/* Управление и состояние */
#define MC_CP_MFBSP(n)	MC_R (0x7804+(n<<6))	/* Указатель цепочки */
#define MC_IR_MFBSP(n)	MC_R (0x7808+(n<<6))	/* Индекс */
#define MC_RUN_MFBSP(n)	MC_R (0x780C+(n<<6))	/* Управления состоянием бита RUN */

/*
 * Регистры I2C
 */
#define MC_I2C_PRER	MC_R (0x2000)	/* Предделитель частоты */
#define MC_I2C_CTR	MC_R (0x2004)	/* Управление */
#define MC_I2C_TXR	MC_R (0x2008)	/* Передача данных */
#define MC_I2C_RXR	MC_R (0x200C)	/* Прием данных */
#define MC_I2C_CR	MC_R (0x2010)	/* Команды */
#define MC_I2C_SR	MC_R (0x2014)	/* Состояние */
#define MC_I2C_PR_CNT	MC_R (0x2018)	/* Счетчик предделителя частоты */

/*
 * Регистры UART0, UART1
 */
#define MC_RBR		MC_R (0x3000)	/* Приемный буферный регистр */
#define MC_THR		MC_R (0x3000)	/* Передающий буферный регистр */
#define MC_IER		MC_R (0x3004)	/* Регистр разрешения прерываний */
#define MC_IIR		MC_R (0x3008)	/* Регистр идентификации прерывания */
#define MC_FCR		MC_R (0x3008)	/* Регистр управления FIFO */
#define MC_LCR		MC_R (0x300C)	/* Регистр управления линией */
#define MC_MCR		MC_R (0x3010)	/* Регистр управления модемом */
#define MC_LSR		MC_R (0x3014)	/* Регистр состояния линии */
#define MC_MSR		MC_R (0x3018)	/* Регистр состояния модемом */
#define MC_SPR		MC_R (0x301C)	/* Регистр Scratch Pad */
#define MC_DLL		MC_R (0x3000)	/* Регистр делителя младший */
#define MC_DLM		MC_R (0x3004)	/* Регистр делителя старший */
#define MC_SCLR		MC_R (0x3014)	/* Регистр предделителя (scaler) */

#define MC_RBR1		MC_R (0x3800)	/* Приемный буферный регистр */
#define MC_THR1		MC_R (0x3800)	/* Передающий буферный регистр */
#define MC_IER1		MC_R (0x3804)	/* Регистр разрешения прерываний */
#define MC_IIR1		MC_R (0x3808)	/* Регистр идентификации прерывания */
#define MC_FCR1		MC_R (0x3808)	/* Регистр управления FIFO */
#define MC_LCR1		MC_R (0x380C)	/* Регистр управления линией */
#define MC_MCR1		MC_R (0x3810)	/* Регистр управления модемом */
#define MC_LSR1		MC_R (0x3814)	/* Регистр состояния линии */
#define MC_MSR1		MC_R (0x3818)	/* Регистр состояния модемом */
#define MC_SPR1		MC_R (0x381C)	/* Регистр Scratch Pad */
#define MC_DLL1		MC_R (0x3800)	/* Регистр делителя младший */
#define MC_DLM1		MC_R (0x3804)	/* Регистр делителя старший */
#define MC_SCLR1	MC_R (0x3814)	/* Регистр предделителя (scaler) */

/*
 * Регистры интервального таймера IT0, IT1
 */
#define MC_ITCSR	MC_R (0xD000)	/* Управление */
#define MC_ITPERIOD	MC_R (0xD004)	/* Период работы таймера */
#define MC_ITCOUNT	MC_R (0xD008)	/* Счетчик */
#define MC_ITSCALE	MC_R (0xD00C)	/* Предделитель */

#define MC_ITCSR1	MC_R (0xD020)	/* Управление */
#define MC_ITPERIOD1	MC_R (0xD024)	/* Период работы таймера */
#define MC_ITCOUNT1	MC_R (0xD028)	/* Счетчик */
#define MC_ITSCALE1	MC_R (0xD02C)	/* Предделитель */

/*
 * Регистры WDT
 */
#define MC_WTCSR	MC_R (0xD010)	/* Управление */
#define MC_WTPERIOD	MC_R (0xD014)	/* Период работы таймера */
#define MC_WTCOUNT	MC_R (0xD018)	/* Счетчик */
#define MC_WTSCALE	MC_R (0xD01C)	/* Предделитель */

/*
 * Регистры MFBSP(0-3)
 */
#define MC_MFBSP_TX(n)		MC_R (0x7000+(n<<8))   /* Буфер передачи данных */
#define MC_MFBSP_RX(n)		MC_R (0x7000+(n<<8))   /* Буфер приема данных */
#define MC_MFBSP_CSR(n)		MC_R (0x7004+(n<<8))   /* Управление и состояние */
#define MC_MFBSP_DIR(n)		MC_R (0x7008+(n<<8))   /* Управление направлением
							* порта ввода-вывода */
#define MC_MFBSP_GPIO_DR(n)	MC_R (0x700C+(n<<8))   /* Данные порта ввода-вывода */
#define MC_MFBSP_TCTR(n)	MC_R (0x7010+(n<<8))   /* Управление передатчиком */
#define MC_MFBSP_RCTR(n)	MC_R (0x7014+(n<<8))   /* Управление приёмником */
#define MC_MFBSP_TSR(n)		MC_R (0x7018+(n<<8))   /* Состояние передатчика */
#define MC_MFBSP_RSR(n)		MC_R (0x701C+(n<<8))   /* Состояние приёмника */
#define MC_MFBSP_TCTR_RATE(n)	MC_R (0x7020+(n<<8))   /* Управление темпом передачи данных */
#define MC_MFBSP_RCTR_RATE(n)	MC_R (0x7024+(n<<8))   /* Управление темпом приёма данных */

/*
 * Регистры VPIN
 */
#define MC_CSR_VPIN		MC_R (0x8000)	/* Управление и состояние */
#define MC_CNT_VPIN		MC_R (0x8004)	/* Счетчик строк / счетчик пикселов */
#define MC_FRAME_CNT_VPIN	MC_R (0x8008)	/* Счетчик кадров */
#define MC_FIFO_VPIN		MC_R (0x800C)	/* Выход FIFO */

/*
 * Регистры VPOUT
 */
#define MC_CSR_VPOUT		MC_R (0x9000)	/* Управление и состояние */
#define MC_DIV			MC_R (0x9004)	/* Период сигнала VCLKO_out */
#define MC_HSTART_HEND		MC_R (0x9008)	/* Начало/конец активной части строки */
#define MC_VSTART_VEND		MC_R (0x900C)	/* Начало/конец активной части кадра */
#define MC_CNT_VPOUT		MC_R (0x9010)	/* Счетчик строк / счетчик пикселов */
#define MC_FRAME_CNT_VPOUT	MC_R (0x9014)	/* Счетчик кадров */
#define MC_FIFO_VPOUT		MC_R (0x901C)	/* Вход FIFO */

/*
 * Регистры USB
 */
#define MC_USB_CSR		MC_R (0xC000)	/* Управление и статус контроллера */
#define MC_USB_INT_CSR		MC_R (0xC004)	/* Управление и статус прерываний */
#define MC_USB_VENDOR_DATA	MC_R (0xC008)   /* Данные для передачи по Vendor-каналу */
#define MC_USB_VENDOR_INDEX	MC_R (0xC00C)   /* Указатель на данные по Vendor-каналу */
#define MC_USB_VENDOR_VALUE	MC_R (0xC010)   /* Принятые данные по Vendor-каналу */
#define MC_USB_CFG_ADDR		MC_R (0xC014)   /* Адрес массива конфигурации */
#define MC_USB_CFG_DATA		MC_R (0xC018)   /* Данные массива конфигурации */
#define MC_USB_REVISION		MC_R (0xC01C)   /* Номер ревизии */
#define MC_USB_CSR_EP1		MC_R (0xC020)   /* Управление и статус ЕР1 */
#define MC_USB_CSR_EP2		MC_R (0xC024)   /* Управление и статус ЕР2 */
#define MC_USB_CSR_EP3		MC_R (0xC028)   /* Управление и статус ЕР3 */
#define MC_USB_CSR_EP4		MC_R (0xC02C)   /* Управление и статус ЕР4 */

/*
 * Регистры Ethernet MAC
 */
#define MC_MAC_CONTROL		MC_R (0xE000)	/* Управление MAC */
#define MC_MAC_ADDR_L		MC_R (0xE004)	/* Младшая часть исходного адреса MAC */
#define MC_MAC_ADDR_H		MC_R (0xE008)	/* Старшая часть исходного адреса MAC */
#define MC_MAC_DADDR_L		MC_R (0xE00C)	/* Младшая часть адреса назначения */
#define MC_MAC_DADDR_H		MC_R (0xE010)	/* Старшая часть адреса назначения */
#define MC_MAC_FCS_CLIENT	MC_R (0xE014)	/* Контрольная сумма кадра */
#define MC_MAC_TYPE		MC_R (0xE018)	/* Тип кадра */
#define MC_MAC_IFS_COLL_MODE	MC_R (0xE01C)	/* IFS и режим обработки коллизии */
#define MC_MAC_TX_FRAME_CONTROL	MC_R (0xE020)	/* Управление передачей кадра */
#define MC_MAC_STATUS_TX	MC_R (0xE024)	/* Статус передачи кадра */
#define MC_MAC_UCADDR_L		MC_R (0xE028)	/* Младшая часть уникального адреса MAC */
#define MC_MAC_UCADDR_H		MC_R (0xE02C)	/* Старшая часть уникального адреса MAC */
#define MC_MAC_MCADDR_L		MC_R (0xE030)	/* Младшая часть группового адреса */
#define MC_MAC_MCADDR_H		MC_R (0xE034)	/* Старшая часть группового адреса */
#define MC_MAC_MCADDR_MASK_L	MC_R (0xE038)	/* Младшая часть маски группового адреса */
#define MC_MAC_MCADDR_MASK_H	MC_R (0xE03C)	/* Старшая часть маски группового адреса */
#define MC_MAC_HASHT_L		MC_R (0xE040)	/* Младшая часть хэш-таблицы */
#define MC_MAC_HASHT_H		MC_R (0xE044)	/* Старшая часть хэш-таблицы */
#define MC_MAC_RX_FRAME_CONTROL	MC_R (0xE048)	/* Управление приемом кадра */
#define MC_MAC_RX_FR_MAXSIZE	MC_R (0xE04C)	/* Максимальный размер принимаемого кадра */
#define MC_MAC_STATUS_RX	MC_R (0xE050)	/* Статус приема кадра */
#define MC_MAC_RX_FRAME_STATUS_FIFO MC_R (0xE054) /* FIFO статусов принятых кадров */
#define MC_MAC_MD_CONTROL	MC_R (0xE058)	/* Управление порта MD */
#define MC_MAC_MD_STATUS	MC_R (0xE05C)	/* Статус порта MD */
#define MC_MAC_MD_MODE		MC_R (0xE060)	/* Режим работы порта MD */
#define MC_MAC_TX_TEST_CSR	MC_R (0xE064)	/* Управление и состояние режима тестирования TX_FIFO */
#define MC_MAC_TX_FIFO		MC_R (0xE068)	/* Передающее TX_FIFO */
#define MC_MAC_RX_TEST_CSR	MC_R (0xE06C)	/* Управление и состояние режима тестирования RX_FIFO */
#define MC_MAC_RX_FIFO		MC_R (0xE070)	/* Принимающее RX_FIFO */

/*
 * Регистры порта внешней памяти MPORT
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100C)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_SDRTMR	MC_R (0x1018)	/* Временные параметры памяти SDRAM */
#define MC_SDRCSR	MC_R (0x101C)	/* Управление режимами памяти SDRAM */

/*
 * Системные регистры
 */
#define MC_CRPLL	MC_R (0x4000)	/* Управление PLL */
#define MC_CLKEN	MC_R (0x4004)	/* Управление отключением частоты от устройств */
#define MC_CSR		MC_R (0x4008)	/* Управление и состояние */
#define MC_MASKR0	MC_R (0x4010)	/* Маска прерываний */
#define MC_QSTR0	MC_R (0x4014)	/* Запросы прерываний от IT, RTT, WDT,
					 * VPOUT, VPIN, ETHERNET MAC, USB,
					 * PMSC, UART, nIRQ[3:0] */
#define MC_MASKR1	MC_R (0x4018)	/* Маска прерываний от DMA MEM */
#define MC_QSTR1	MC_R (0x401C)	/* Запросы прерываний от DMA MEM */
#define MC_MASKR2	MC_R (0x4020)	/* Маска прерываний от MFBSP */
#define MC_QSTR2	MC_R (0x4024)	/* Запросы прерываний от MFBSP */
#define MC_IRQM		MC_R (0x4030)	/* Режимы внешних прерываний nIRQ[3:0] */

/*
 * Регистры многоканального коррелятора
 */
#define MC_BASE_MCC	MC_R (0xF000)	/* Базовый адрес регистров МСС */
#define MC_INT_EN_MCC	MC_R (0xFFF4)	/* Разрешение прерывания от MCC */
#define MC_INT_RST_MCC	MC_R (0xFFF8)	/* Сброс прерывания от MCC */

#endif /* _IO_NVCOM02_H */
