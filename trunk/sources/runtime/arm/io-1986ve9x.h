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
#define ARM_USB_BASE		(ARM_PERIPH_BASE + 0x10000)
#define ARM_EEPROM_BASE		(ARM_PERIPH_BASE + 0x18000)
#define ARM_RSTCLK_BASE        	(ARM_PERIPH_BASE + 0x20000)
#define ARM_DMA_BASE		(ARM_PERIPH_BASE + 0x28000)
#define ARM_UART1_BASE		(ARM_PERIPH_BASE + 0x30000)
#define ARM_UART2_BASE		(ARM_PERIPH_BASE + 0x38000)
#define ARM_SSP1_BASE		(ARM_PERIPH_BASE + 0x40000)
#define ARM_TIMER1_BASE		(ARM_PERIPH_BASE + 0x70000)
#define ARM_TIMER2_BASE		(ARM_PERIPH_BASE + 0x78000)
#define ARM_TIMER3_BASE		(ARM_PERIPH_BASE + 0x80000)
#define ARM_SSP2_BASE		(ARM_PERIPH_BASE + 0xA0000)
#define ARM_GPIOA_BASE		(ARM_PERIPH_BASE + 0xA8000)
#define ARM_GPIOB_BASE		(ARM_PERIPH_BASE + 0xB0000)
#define ARM_GPIOC_BASE		(ARM_PERIPH_BASE + 0xB8000)
#define ARM_GPIOD_BASE		(ARM_PERIPH_BASE + 0xC0000)
#define ARM_GPIOE_BASE		(ARM_PERIPH_BASE + 0xC8000)
#define ARM_GPIOF_BASE		(ARM_PERIPH_BASE + 0xE8000)
#define ARM_EXT_BUS_BASE	(ARM_PERIPH_BASE + 0xF0050)

/*------------------------------------------------------
 * General purpose I/O
 */
typedef struct
{
	arm_reg_t DATA;
	arm_reg_t OE;
	arm_reg_t FUNC;
	arm_reg_t ANALOG;
	arm_reg_t PULL;
	arm_reg_t PD;
	arm_reg_t PWR;
	arm_reg_t GFEN;
} GPIO_t;

#define ARM_GPIOA		((GPIO_t*) ARM_GPIOA_BASE)
#define ARM_GPIOB		((GPIO_t*) ARM_GPIOB_BASE)
#define ARM_GPIOC		((GPIO_t*) ARM_GPIOC_BASE)
#define ARM_GPIOD		((GPIO_t*) ARM_GPIOD_BASE)
#define ARM_GPIOE		((GPIO_t*) ARM_GPIOE_BASE)
#define ARM_GPIOF		((GPIO_t*) ARM_GPIOF_BASE)

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

/*------------------------------------------------------
 * UART
 */
typedef struct
{
	arm_reg_t DR;
	arm_reg_t SRCR;
	unsigned reserved0 [4];
	arm_reg_t FR;
	unsigned reserved1;
	arm_reg_t ILPR;
	arm_reg_t IBRD;
	arm_reg_t FBRD;
	arm_reg_t LCR_H;
	arm_reg_t CR;
	arm_reg_t IFLS;
	arm_reg_t IMSC;
	arm_reg_t RIS;
	arm_reg_t MIS;
	arm_reg_t ICR;
	arm_reg_t DMACR;
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
 * Регистр EEPROM_CMD
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

/* End of Milandr 1986BE9x register definitions.
 *----------------------------------------------*/
