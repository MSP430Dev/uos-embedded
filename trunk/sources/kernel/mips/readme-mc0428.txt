
IRQ numbers for Elvees MC-0428 microcontroller:
// QSTR0  
   0  - Внешнее прерывание /IRQ0
   1  - Внешнее прерывание /IRQ1
   2  - Внешнее прерывание /IRQ2
   3  - Внешнее прерывание /IRQ3
   4  - Прерывание от UART
   5  - Прерывание от PMSC_MBR
   6  - Прерывание от PMSC_MASTER
   7  - Прерывание от USB
   8  - От приёмника USB end point 1
   9  - От передатчика USB end point 2
   10 - От приёмника USB end point 3
   11 - От передатчика USB end point 4
   12 - От приёмника Ethernet
   13 - От передатчика Ethernet
   14 - От DMA приёма Ethernet
   15 - От DMA передачи Ethernet
   16 - От контроллера VPIN
   17 - От канала DMA VPIN
   18 - От контроллера VPOUT
   19 - От канала DMA VPOUT
   20 - От таймера WDT
   21 - От таймера IT0
   22 - От таймера IT1
   23 - Прерывание от I2C
   24 - Прерывание COMPARE
// QSTR1
   25 - От канала DMA MEM_CH00
   26 - От канала DMA MEM_CH01
   27 - От канала DMA MEM_CH02
   28 - От канала DMA MEM_CH03
   29 - От канала DMA MEM_CH04
   30 - От канала DMA MEM_CH05
   31 - От канала DMA MEM_CH06
   32 - От канала DMA MEM_CH07
   33 - От канала DMA MEM_CH10
   34 - От канала DMA MEM_CH11
   35 - От канала DMA MEM_CH12
   36 - От канала DMA MEM_CH13
   37 - От канала DMA MEM_CH14
   38 - От канала DMA MEM_CH15
   39 - От канала DMA MEM_CH16
   40 - От канала DMA MEM_CH17
// QSTR2   
   41 - SRIO0_PWRITE			В SRIO0 поступил пакет типа PORT_WRITE
   42 - SRIO0_DOORBELL			В SRIO0 поступил пакет типа DOORBELL 
   43 - SRIO0_LSU				Прерывание от LSU SRIO0
   44 - SRIO0_MPU_RX			Прерывание от MPU_RX SRIO0
   45 - SRIO0_MPU_TX			Прерывание от MPU_TX SRIO0
   46 - SRIO0_PORT_ERROR		LPU SRIO0 находится в нерабочем состоянии из-за 
								обнаружения невосстанавливаемой ошибки. 
								Повторяет состояние бита PORT_ERROR регистра ERROR_STATUS_CSR
   47 - SRIO0_RESET_DEVICE_CMD	В SRIO0 поступили 4 команды Reset-Device Command. 
								Повторяет состояние бита RESET_DEVICE_CMD регистра LPU_CSR
   48 - SRIO0_MCE_DEC			В SRIO0 принял символ Multicast-Event. 
								Повторяет состояние бита MCE_DEC регистра LPU_CSR

   49 - SRIO1_PWRITE			В SRIO1 поступил пакет типа PORT_WRITE
   50 - SRIO1_DOORBELL			В SRIO1 поступил пакет типа DOORBELL 
   51 - SRIO1_LSU				Прерывание от LSU SRIO1
   52 - SRIO1_MPU_RX			Прерывание от MPU_RX SRIO1
   53 - SRIO1_MPU_TX			Прерывание от MPU_TX SRIO1
   54 - SRIO1_PORT_ERROR		LPU SRIO1 находится в нерабочем состоянии из-за 
								обнаружения невосстанавливаемой ошибки. 
								Повторяет состояние бита PORT_ERROR регистра ERROR_STATUS_CSR
   55 - SRIO1_RESET_DEVICE_CMD	В SRIO1 поступили 4 команды Reset-Device Command. 
								Повторяет состояние бита RESET_DEVICE_CMD регистра LPU_CSR
   56 - SRIO1_MCE_DEC			В SRIO1 принял символ Multicast-Event. 
								Повторяет состояние бита MCE_DEC регистра LPU_CSR
// QSTR3   
   57 - SRQ0			Запрос обслуживания от порта MFBSP0.
						Формируется, если порт выключен (LEN=0, SPI_I2S_EN=0), 
						а на выводах LACK или LCLK присутствует сигнал высокого уровня
   58 - MFBSP_TXBUF0	Формируется, если порт MFBSP0 включен на передачу данных (в одном из режимов), 
						а число 64-х разрядных слов, находящихся в буфере  передачи меньше, 
						либо равно TLEV (TLEV устанавливается в регистре состояния передатчика TSR)
   59 - MFBSP_RXBUF0	Формируется, если порт MFBSP0 включен на прием данных (в одном из режимов), 
						а число 64-х разрядных слов в буфере приёма больше чем RLEV 
						(RLEV устанавливается в регистре состояния приёмника RSR)
   60 - Unused
   61 - MFBSP_TX_CH0	Прерывание от канала DMA порта MFBSP0 при передаче данных
   62 - MFBSP_RX_CH0	Прерывание от канала DMA порта MFBSP0 при приеме данных
   63 - Unused
   64 - Unused
   
   65 - SRQ1			Запрос обслуживания от порта MFBSP1.
						Формируется, если порт выключен (LEN=0, SPI_I2S_EN=0), 
						а на выводах LACK или LCLK присутствует сигнал высокого уровня
   66 - MFBSP_TXBUF1	Формируется, если порт MFBSP1 включен на передачу данных (в одном из режимов), 
						а число 64-х разрядных слов, находящихся в буфере  передачи меньше, 
						либо равно TLEV (TLEV устанавливается в регистре состояния передатчика TSR)
   67 - MFBSP_RXBUF1	Формируется, если порт MFBSP1 включен на прием данных (в одном из режимов), 
						а число 64-х разрядных слов в буфере приёма больше чем RLEV 
						(RLEV устанавливается в регистре состояния приёмника RSR)
   68 - Unused
   69 - MFBSP_TX_CH1	Прерывание от канала DMA порта MFBSP1 при передаче данных
   70 - MFBSP_RX_CH1	Прерывание от канала DMA порта MFBSP1 при приеме данных
   71 - Unused
   72 - Unused

   73 - SW_LINK0		Прерывание от SWIC0 при установлении соединения
   74 - SW_ERR0			Прерывание от SWIC0 при ошибке в канале
   75 - SW_TIME0		Прерывание от SWIC0 при получении маркер времени 
						или распределенного прерывания
   76 - Unused
   77 - SW_RX_DES_CH0	Прерывание от канала DMA SW_RX_DES_CH0
   78 - SW_RX_DAT_CH0	Прерывание от канала DMA SW_RX_DAT_CH0
   79 - SW_TX_DES_CH0	Прерывание от канала DMA SW_TX_DES_CH0            
   80 - SW_TX_DAT_CH0	Прерывание от канала DMA SW_TX_DAT_CH0
   
   81 - SW_LINK1		Прерывание от SWIC1 при установлении соединения
   82 - SW_ERR1		Прерывание от SWIC1 при ошибке в канале
   83 - SW_TIME1		Прерывание от SWIC1 при получении маркер времени 
						или распределенного прерывания
   84 - Unused
   85 - SW_RX_DES_CH1	Прерывание от канала DMA SW_RX_DES_CH1
   86 - SW_RX_DAT_CH1	Прерывание от канала DMA SW_RX_DAT_CH1
   87 - SW_TX_DES_CH1	Прерывание от канала DMA SW_TX_DES_CH1            
   88 - SW_TX_DAT_CH1	Прерывание от канала DMA SW_TX_DAT_CH1
