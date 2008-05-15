/*
 * Register definitions for Samsung S3C4530A.
 */

/*
 * Internal SRAM address and size.
 */
#define ARM_SRAM_BASE		0x00ff0000
#define ARM_SRAM_SIZE		0x00001000

/*
 * Base address of special registers (26-bit value).
 * Non-caching by accessing addr | 0x04000000.
 */
#define ARM_REGISTER(x)		(*(volatile unsigned long*) (0x07ff0000 | (x)))

#define ARM_NONCACHED(x)	((unsigned long) (x) | 0x04000000)
#define ARM_CACHED(x)		((unsigned long) (x) & ~0x04000000)
#define ARM_ISNONCACHED(x)	((unsigned long) (x) & 0x04000000)

/*----------------------------------------------
 * System Manager
 */
/*
 * System configuration register, R/W, default 0x4FFFFF91
 * Register bases and caching.
 */
#define ARM_SYSCFG		ARM_REGISTER (0x0000)

#define ARM_SYSCFG_SDM		0x80000000
#define ARM_SYSCFG_PD_ID_MASK	0x3c000000
#define ARM_SYSCFG_SRBBP_MASK	0x03ff0000	/* address/64k */
#define ARM_SYSCFG_ISBBP_MASK	0x0000ffc0	/* a25-a16 */
#define ARM_SYSCFG_CM_MASK	0x00000030
#define ARM_SYSCFG_CM_4R_4C	0x00000000
#define ARM_SYSCFG_CM_0R_8C	0x00000010
#define ARM_SYSCFG_CM_8R_0C	0x00000020
#define ARM_SYSCFG_FP		0x00000008	/* LY: fixed priority on system bus, not round-robin */
#define ARM_SYSCFG_WE		0x00000004	/* only KS32C50100? */
#define ARM_SYSCFG_CE		0x00000002
#define ARM_SYSCFG_SE		0x00000001

/*
 * Clock control register, R/W, default 0x00000000
 */
#define ARM_CLKCON		ARM_REGISTER (0x3000)

/*
 * External I/O timing registers 1-2, R/W, default 0x00000000
 */
#define ARM_EXTACON0		ARM_REGISTER (0x3008)
#define ARM_EXTACON1		ARM_REGISTER (0x300c)

/*
 * Data bus width for each memory bank, R/W, default 0x00000000
 */
#define ARM_EXTDBWTH		ARM_REGISTER (0x3010)

#define ARM_EXTDBWTH_MASK	3
#define ARM_EXTDBWTH_8BIT	1
#define ARM_EXTDBWTH_16BIT	2
#define ARM_EXTDBWTH_32BIT	3

#define ARM_EXTDBWTH_DSR0_shift 0
#define ARM_EXTDBWTH_DSR1_shift 2
#define ARM_EXTDBWTH_DSR2_shift 4
#define ARM_EXTDBWTH_DSR3_shift 6
#define ARM_EXTDBWTH_DSR4_shift 8
#define ARM_EXTDBWTH_DSR5_shift 10
#define ARM_EXTDBWTH_DSD0_shift 12
#define ARM_EXTDBWTH_DSD1_shift 14
#define ARM_EXTDBWTH_DSD2_shift 16
#define ARM_EXTDBWTH_DSD3_shift 18
#define ARM_EXTDBWTH_DSX0_shift 20
#define ARM_EXTDBWTH_DSX1_shift 22
#define ARM_EXTDBWTH_DSX2_shift 24
#define ARM_EXTDBWTH_DSX3_shift 26

/*
 * ROM/SRAM/Flash bank 0 control register, R/W, default 0x20000060
 */
#define ARM_ROMCON0		ARM_REGISTER (0x3014)

/*
 * ROM/SRAM/Flash bank 1-5 control registers, R/W, default 0x00000060
 */
#define ARM_ROMCON1		ARM_REGISTER (0x3018)
#define ARM_ROMCON2		ARM_REGISTER (0x301c)
#define ARM_ROMCON3		ARM_REGISTER (0x3020)
#define ARM_ROMCON4		ARM_REGISTER (0x3024)
#define ARM_ROMCON5		ARM_REGISTER (0x3028)

#define ARM_ROMCON_PMC_MASK		0x00000003
#define ARM_ROMCON_PMC_ROM		0x00000000
#define ARM_ROMCON_PMC_4W_PAGE		0x00000001
#define ARM_ROMCON_PMC_8W_PAGE		0x00000002
#define ARM_ROMCON_PMC_16W_PAGE		0x00000003

#define ARM_ROMCON_TPA_MASK		0x0000000c
#define ARM_ROMCON_TPA_5C		0x00000000
#define ARM_ROMCON_TPA_2C		0x00000004
#define ARM_ROMCON_TPA_3C		0x00000008
#define ARM_ROMCON_TPA_4C		0x0000000c

#define ARM_ROMCON_TACC_MASK		0x00000070
#define ARM_ROMCON_TACC_DISABLE		0x00000000
#define ARM_ROMCON_TACC_2C		0x00000010
#define ARM_ROMCON_TACC_3C		0x00000020
#define ARM_ROMCON_TACC_4C		0x00000030
#define ARM_ROMCON_TACC_5C		0x00000040
#define ARM_ROMCON_TACC_6C		0x00000050
#define ARM_ROMCON_TACC_7C		0x00000060

#define ARM_ROMCON_BASE_MASK		0x000f3000
#define ARM_ROMCON_BASE(p)		((p) >> 20 << 14)

#define ARM_ROMCON_NEXT_MASK		0x3f000000
#define ARM_ROMCON_NEXT(p)		((p) >> 20 << 24)

/*
 * DRAM bank 0-3 control registers, R/W, default 0x00000000
 */
#define ARM_DRAMCON0		ARM_REGISTER (0x302c)
#define ARM_DRAMCON1		ARM_REGISTER (0x3030)
#define ARM_DRAMCON2		ARM_REGISTER (0x3034)
#define ARM_DRAMCON3		ARM_REGISTER (0x3038)

#define ARM_DRAMCON_EDO		0x00000001	/* EDO DRAM */

#define ARM_DRAMCON_TCS_MASK	0x00000006	/* CAS strobe time */
#define ARM_DRAMCON_TCS_1C	0x00000000
#define ARM_DRAMCON_TCS_2C	0x00000002
#define ARM_DRAMCON_TCS_3C	0x00000004
#define ARM_DRAMCON_TCS_4C	0x00000006

#define ARM_DRAMCON_TCP_2C	0x00000008	/* CAS pre-charge time */
#define ARM_DRAMCON_TRC_2C	0x00000080	/* RAS to CAS delay */

#define ARM_DRAMCON_TRP_MASK	0x00000300	/* RAS pre-charge time */
#define ARM_DRAMCON_TRP_1C	0x00000000
#define ARM_DRAMCON_TRP_2C	0x00000100
#define ARM_DRAMCON_TRP_3C	0x00000200
#define ARM_DRAMCON_TRP_4C	0x00000300

#define ARM_DRAMCON_BASE_MASK	0x000f3000	/* base address */
#define ARM_DRAMCON_BASE(p)	((p) >> 20 << 14)

#define ARM_DRAMCON_NEXT_MASK	0x3f000000	/* address limit */
#define ARM_DRAMCON_NEXT(p)	((p) >> 20 << 24)

#define ARM_DRAMCON_CAN_MASK	0xc0000000	/* column address bits */
#define ARM_DRAMCON_CAN_8B	0x00000000
#define ARM_DRAMCON_CAN_9B	0x40000000
#define ARM_DRAMCON_CAN_10B	0x80000000
#define ARM_DRAMCON_CAN_11B	0xc0000000

/*
 * Refresh and external I/O control register, R/W, default 0x000083ED
 */
#define ARM_REFEXTCON		ARM_REGISTER (0x303c)

#define ARM_REFEXTCON_BASE_MASK 0x000003f0	/* base address */
#define ARM_REFEXTCON_BASE(p)	((p) >> 20 << 4)

#define ARM_REFEXTCON_VSF	0x00008000	/* validity of special regs */
#define ARM_REFEXTCON_REN	0x00010000	/* refresh enable */

#define ARM_REFEXTCON_TCHR_MASK 0x000e0000	/* CAS hold time */
#define ARM_REFEXTCON_TCHR_1C	0x00000000
#define ARM_REFEXTCON_TCHR_2C	0x00020000
#define ARM_REFEXTCON_TCHR_3C	0x00040000
#define ARM_REFEXTCON_TCHR_4C	0x00060000
#define ARM_REFEXTCON_TCHR_5C	0x00080000

#define ARM_REFEXTCON_TCSR_2C	0x00100000	/* CAS setup time */

#define ARM_REFEXTCON_RFR_MASK		0xffe00000	/* refresh duration */
#define ARM_REFEXTCON_RFR(usec,khz)	((2049 - (usec)*(khz)/1000) << 21)

/*
 * Set ROM/RAM configuration registers.
 * Samsung documentation says that all system manager special registers
 * must be set all at once, using the single STMIA instruction.
 * Here, "ptr" must point to an array of 12 values for:
 * EXTDBWTH, ROMCON0, ROMCON1, ROMCON2, ROMCON3, ROMCON4, ROMCON5,
 * DRAMCON0, DRAMCON1, DRAMCON2, DRAMCON3, REFEXTCON.
 */
typedef struct _arm_memory_regs_t {
	unsigned long savearea [12];
	unsigned long extdbwth;
	unsigned long romcon0;
	unsigned long romcon1;
	unsigned long romcon2;
	unsigned long romcon3;
	unsigned long romcon4;
	unsigned long romcon5;
	unsigned long dramcon0;
	unsigned long dramcon1;
	unsigned long dramcon2;
	unsigned long dramcon3;
	unsigned long refextcon;
} arm_memory_regs_t;

static void inline __attribute__ ((always_inline))
arm_get_memory_regs (arm_memory_regs_t *reg)
{
	reg->extdbwth = ARM_EXTDBWTH;
	reg->romcon0 = ARM_ROMCON0;
	reg->romcon1 = ARM_ROMCON1;
	reg->romcon2 = ARM_ROMCON2;
	reg->romcon3 = ARM_ROMCON3;
	reg->romcon4 = ARM_ROMCON4;
	reg->romcon5 = ARM_ROMCON5;
	reg->dramcon0 = ARM_DRAMCON0;
	reg->dramcon1 = ARM_DRAMCON1;
	reg->dramcon2 = ARM_DRAMCON2;
	reg->dramcon3 = ARM_DRAMCON3;
	reg->refextcon = ARM_REFEXTCON;
}

static void inline __attribute__ ((always_inline))
arm_set_memory_regs (arm_memory_regs_t *ptr)
{
	asm volatile (
#if __thumb__
	".balignw 4, 0x46c0 \n"
"	bx	pc \n"			/* switch to ARM mode */
"	nop \n"
"	.code	32 \n\t"
#endif
	"mov	r0, %0 \n"
"	mov	lr, %1 \n"
"	stmia	r0!, {r1-r12} \n"
"	ldmia	r0, {r1-r12} \n"
"	stmia	lr, {r1-r12} \n"
"	ldmdb	r0!, {r1-r12}"
#if __thumb__
"\n	ldr	lr, [pc] \n"		/* switch back to Thumb mode */
"	bx	lr \n"
"	.word	.+5"
#endif
	: : "r" (ptr), "r" (&ARM_EXTDBWTH) : "r0", "lr", "memory");
}

/*----------------------------------------------
 * Interrupt Controller
 */
/*
 * Interrupt mode register, R/W, default undefined
 */
#define ARM_INTMOD		ARM_REGISTER (0x4000)

/*
 * Interrupt pending register, R/W, default 0x00000000
 */
#define ARM_INTPND		ARM_REGISTER (0x4004)

/*
 * Interrupt mask register, R/W, default 0x00000000
 */
#define ARM_INTMSK		ARM_REGISTER (0x4008)

#define ARM_INTMSK_GLOBAL	(1 << 21)

/*
 * Interrupt priority register 0-5, R/W, default:
 * 0x003FFFFF, 0x07060504, 0x0B0A0908, 0x0F0E0D0C, 0x13121110, 0x00000014
 */
#define ARM_INTPRI0		ARM_REGISTER (0x400c)
#define ARM_INTPRI1		ARM_REGISTER (0x4010)
#define ARM_INTPRI2		ARM_REGISTER (0x4014)
#define ARM_INTPRI3		ARM_REGISTER (0x4018)
#define ARM_INTPRI4		ARM_REGISTER (0x401c)
#define ARM_INTPRI5		ARM_REGISTER (0x4020)

/*
 * Interrupt offset address register, R, default 0x00000054
 */
#define ARM_INTOFFSET		ARM_REGISTER (0x4024)

/*
 * Interrupt pending test register, W
 */
#define ARM_INTPNDTST		ARM_REGISTER (0x402c)

/*
 * FIQ interrupt offset register, R, default 0x00000054
 */
#define ARM_INTOFFSET_FIQ	ARM_REGISTER (0x4030)

/*
 * IRQ interrupt offset register, R, default 0x00000054
 */
#define ARM_INTOFFSET_IRQ	ARM_REGISTER (0x4034)

/*----------------------------------------------
 * I/O Ports
 */
/*
 * I/O port mode register, R/W, default 0x00000000
 */
#define ARM_IOPMOD		ARM_REGISTER (0x5000)

/*
 * I/O port control register, R/W, default 0x00000000
 */
#define ARM_IOPCON0		ARM_REGISTER (0x5004)

/*
 * Input port data register, R/W, default 0x00000000
 */
#define ARM_IOPDATA		ARM_REGISTER (0x5008)

/*
 * I/O port control register, R/W, default 0x00000000
 */
#define ARM_IOPCON1		ARM_REGISTER (0x500c)

/*----------------------------------------------
 * Timers
 */
/*
 * Timer mode register, R/W, default 0x00000000
 */
#define ARM_TMOD		ARM_REGISTER (0x6000)

#define ARM_TMOD_TE0		0x00000001	/* timer 0 enable */
#define ARM_TMOD_TMD0		0x00000002	/* timer 0 toggle mode */
#define ARM_TMOD_TCLR0		0x00000004	/* timer 0 initial pin value */
#define ARM_TMOD_TE1		0x00000008
#define ARM_TMOD_TMD1		0x00000010
#define ARM_TMOD_TCLR1		0x00000020

/*
 * Timer 0-1 data registers, R/W, default 0x00000000
 */
#define ARM_TDATA(n)		ARM_REGISTER ((n) ? 0x6008 :0x6004)

/*
 * Timer 0-1 count registers, R/W, default 0xffffffff
 */
#define ARM_TCNT(n)		ARM_REGISTER ((n) ? 0x6010 : 0x600c)

/*----------------------------------------------
 * UART
 */
/*
 * UART channel 0-1 control registers, R/W, default 0x00
 */
#define ARM_UCON(n)		ARM_REGISTER ((n) ? 0xE000 : 0xD000)

#define ARM_UCON_TMODE_MASK	0x00000003
#define ARM_UCON_TMODE_DISABLE	0x00000000
#define ARM_UCON_TMODE_IRQ	0x00000001
#define ARM_UCON_TMODE_GDMA0	0x00000002
#define ARM_UCON_TMODE_GDMA1	0x00000003

#define ARM_UCON_RMODE_MASK	0x0000000c
#define ARM_UCON_RMODE_DISABLE	0x00000000
#define ARM_UCON_RMODE_IRQ	0x00000004
#define ARM_UCON_RMODE_GDMA0	0x00000008
#define ARM_UCON_RMODE_GDMA1	0x0000000c

#define ARM_UCON_SBR		0x00000010
#define ARM_UCON_CKSL		0x00000020
#define ARM_UCON_AUBD		0x00000040
#define ARM_UCON_LOOB		0x00000080

#define ARM_UCON_PMD_MASK	0x00000700
#define ARM_UCON_PMD_NOPARITY	0x00000000
#define ARM_UCON_PMD_ODD	0x00000400
#define ARM_UCON_PMD_EVEN	0x00000500
#define ARM_UCON_PMD_FORCE1	0x00000600
#define ARM_UCON_PMD_FORCE0	0x00000700

#define ARM_UCON_STB		0x00000800

#define ARM_UCON_WL_MASK	0x00003000
#define ARM_UCON_WL_5		0x00000000
#define ARM_UCON_WL_6		0x00001000
#define ARM_UCON_WL_7		0x00002000
#define ARM_UCON_WL_8		0x00003000

#define ARM_UCON_IR		0x00004000
#define ARM_UCON_TFEN		0x00010000
#define ARM_UCON_RFEN		0x00020000
#define ARM_UCON_TFRST		0x00040000
#define ARM_UCON_RFRST		0x00080000

#define ARM_UCON_TFTL_MASK	0x00300000
#define ARM_UCON_TFTL_30	0x00000000
#define ARM_UCON_TFTL_24	0x00100000
#define ARM_UCON_TFTL_16	0x00200000
#define ARM_UCON_TFTL_8		0x00300000

#define ARM_UCON_RFTL_MASK	0x00c00000
#define ARM_UCON_RFTL_1		0x00000000
#define ARM_UCON_RFTL_8		0x00400000
#define ARM_UCON_RFTL_18	0x00800000
#define ARM_UCON_RFTL_28	0x00c00000

#define ARM_UCON_DTR		0x01000000
#define ARM_UCON_RTS		0x02000000
#define ARM_UCON_HFEN		0x10000000
#define ARM_UCON_SFEN		0x20000000

/*
 * UART channel 0-1 status registers, R/W, default 0xE0240
 */
#define ARM_USTAT(n)		ARM_REGISTER ((n) ? 0xE004 : 0xD004)

#define ARM_USTAT_RDV		0x00000001
#define ARM_USTAT_BKD		0x00000002
#define ARM_USTAT_FER		0x00000004
#define ARM_USTAT_PER		0x00000008
#define ARM_USTAT_OER		0x00000010
#define ARM_USTAT_CCD		0x00000020
#define ARM_USTAT_DCDL		0x00000040
#define ARM_USTAT_RFREA		0x00000080
#define ARM_USTAT_RFEMT		0x00000100
#define ARM_USTAT_RFFUL		0x00000200
#define ARM_USTAT_ROVFF		0x00000400
#define ARM_USTAT_RIDLE		0x00000800
#define ARM_USTAT_ERXTO		0x00001000
#define ARM_USTAT_DSR		0x00004000
#define ARM_USTAT_CTS		0x00008000
#define ARM_USTAT_ECTS		0x00010000
#define ARM_USTAT_TC		0x00020000
#define ARM_USTAT_THE		0x00040000
#define ARM_USTAT_TFEMT		0x00080000
#define ARM_USTAT_TFFUL		0x00100000

/*
 * UART channel 0-1 interrupt enable registers, R/W, default 0x00000000
 */
#define ARM_UINTEN(n)		ARM_REGISTER ((n) ? 0xE008 : 0xD008)

#define ARM_UINTEN_RDVIE	0x00000001
#define ARM_UINTEN_BSDIE	0x00000002
#define ARM_UINTEN_FERIE	0x00000004
#define ARM_UINTEN_PERIE	0x00000008
#define ARM_UINTEN_OERIE	0x00000010
#define ARM_UINTEN_CCDIE	0x00000020
#define ARM_UINTEN_DCDLIE	0x00000040
#define ARM_UINTEN_RFREAIE	0x00000080
#define ARM_UINTEN_OVFF		0x00000400
#define ARM_UINTEN_ERXTOIE	0x00001000
#define ARM_UINTEN_ECTSIE	0x00010000
#define ARM_UINTEN_THEIE	0x00040000

/*
 * UART channel 0-1 transmit holding registers, W, default undefined
 */
#define ARM_UTXBUF(n)		ARM_REGISTER ((n) ? 0xE00C : 0xD00C)

/*
 * UART channel 0-1 receive buffer registers, R, default undefined
 */
#define ARM_URXBUF(n)		ARM_REGISTER ((n) ? 0xE010 : 0xD010)

/*
 * Baud rate divisor registers 0-1, R/W, default 0x00
 */
#define ARM_UBRDIV(n)		ARM_REGISTER ((n) ? 0xE014 : 0xD014)

/*
 * UART0-1 Control Character Registers 1-2, R/W, default 0x00000000
 */
#define ARM_UCC1(n)		ARM_REGISTER ((n) ? 0xE018 : 0xD018)
#define ARM_UCC2(n)		ARM_REGISTER ((n) ? 0xE01C : 0xD01C)

/*----------------------------------------------
 * Cache
 */
/*
 * 4-kbyte set 0 RAM area
 */
#define ARM_CACHE_SET0_ADDR	0x10000000

/*
 * 4-kbyte set 1 RAM area
 */
#define ARM_CACHE_SET1_ADDR	0x10800000

/*
 * 1-kbyte Tag RAM area
 */
#define ARM_CACHE_TAG_ADDR	0x11000000

/*----------------------------------------------
 * HDLC Channels A, B
 */
/*
 * HDLC mode, R/W, default 0x00000000
 */
#define ARM_HMODE(n)		ARM_REGISTER ((n) ? 0x8000 : 0x7000)

#define ARM_HMODE_MFF		0x00000001	/* Multi-Frame in HTxFIFO */
#define ARM_HMODE_RXCINV	0x00000004	/* sample on falling edge */
#define ARM_HMODE_TXCINV	0x00000008	/* shift on rising edge */
#define ARM_HMODE_RXLITTLE	0x00000010	/* little-endian HRxFIFO */
#define ARM_HMODE_TXLITTLE	0x00000020	/* little-endian HTxFIFO */
#define ARM_HMODE_RXTRANS	0x00000040	/* transparent receive */
#define ARM_HMODE_TXTRANS	0x00000080	/* transparent transmit */
#define ARM_HMODE_TXPL_MASK	0x00000700	/* Tx preamble length */

#define ARM_HMODE_DF_MASK	0x00007000	/* data format */
#define ARM_HMODE_DF_NRZ		0x00000000
#define ARM_HMODE_DF_NRZI		0x00001000
#define ARM_HMODE_DF_FM0		0x00002000
#define ARM_HMODE_DF_FM1		0x00003000
#define ARM_HMODE_DF_MANCHESTER		0x00004000

#define ARM_HMODE_DPLLCLK	0x00070000	/* DPLL clock select */
#define ARM_HMODE_DPLLCLK_TXC		0x00000000
#define ARM_HMODE_DPLLCLK_RXC		0x00010000
#define ARM_HMODE_DPLLCLK_MCLK		0x00020000
#define ARM_HMODE_DPLLCLK_BRGOUT1	0x00030000
#define ARM_HMODE_DPLLCLK_BRGOUT2	0x00040000

#define ARM_HMODE_BRGCLK	0x00080000	/* use MCLK2 as BRG source */

#define ARM_HMODE_TXCLK		0x00700000	/* Tx clock select */
#define ARM_HMODE_TXCLK_TXC		0x00000000
#define ARM_HMODE_TXCLK_RXC		0x00100000
#define ARM_HMODE_TXCLK_DPLLOUTT	0x00200000
#define ARM_HMODE_TXCLK_BRGOUT1		0x00300000
#define ARM_HMODE_TXCLK_BRGOUT2		0x00400000

#define ARM_HMODE_RXCLK		0x07000000	/* Rx clock select */
#define ARM_HMODE_RXCLK_TXC		0x00000000
#define ARM_HMODE_RXCLK_RXC		0x01000000
#define ARM_HMODE_RXCLK_DPLLOUTR	0x02000000
#define ARM_HMODE_RXCLK_BRGOUT1		0x03000000
#define ARM_HMODE_RXCLK_BRGOUT2		0x04000000

#define ARM_HMODE_TXCOPS	0x70000000	/* TXC output pin select */
#define ARM_HMODE_TXCOPS_TXC		0x00000000
#define ARM_HMODE_TXCOPS_RXC		0x10000000
#define ARM_HMODE_TXCOPS_BRGOUT1	0x20000000
#define ARM_HMODE_TXCOPS_BRGOUT2	0x30000000
#define ARM_HMODE_TXCOPS_DPLLOUTT	0x40000000
#define ARM_HMODE_TXCOPS_DPLLOUTR	0x50000000

/*
 * HDLC control, R/W, default 0x00000000
 */
#define ARM_HCON(n)		ARM_REGISTER ((n) ? 0x8004 : 0x7004)

#define ARM_HCON_TXRS		0x00000001	/* reset HTxFIFO and Tx block */
#define ARM_HCON_RXRS		0x00000002	/* reset HRxFIFO and Rx block */
#define ARM_HCON_DTXRS		0x00000004	/* reset DMA Tx block */
#define ARM_HCON_DRXRS		0x00000008	/* reset DMA Rx block */
#define ARM_HCON_TXEN		0x00000010	/* enable transmitter */
#define ARM_HCON_RXEN		0x00000020	/* enable receiver */
#define ARM_HCON_DTXEN		0x00000040	/* DMA Tx enable */
#define ARM_HCON_DRXEN		0x00000080	/* DMA Rx enable */
#define ARM_HCON_DPLLEN		0x00000100	/* DPLL enable */
#define ARM_HCON_BRGEN		0x00000200	/* baud rate gen. enable */
#define ARM_HCON_TX4WD		0x00000400	/* Tx 4 word mode */
#define ARM_HCON_RX4WD		0x00000800	/* Rx 4 word mode */
#define ARM_HCON_RXWA_MASK	0x00003000	/* Rx widget alignment */
#define ARM_HCON_DTXSTSK	0x00004000	/* DMA Tx stop or skip */
#define ARM_HCON_DRXSTSK	0x00008000	/* DMA Rx stop or skip */
#define ARM_HCON_DRXMADEC	0x00010000	/* DMA Rx memory address decr */
#define ARM_HCON_TXFLAG		0x00020000	/* Tx flag idle */
#define ARM_HCON_TXSFLAG	0x00040000	/* Tx single flag */
#define ARM_HCON_TXLOOP		0x00080000	/* Tx loop-back mode */
#define ARM_HCON_RXECHO		0x00100000	/* Rx echo mode */
#define ARM_HCON_TXABTEXT	0x00200000	/* Tx abort extension */
#define ARM_HCON_TXABT		0x00400000	/* Tx abort */
#define ARM_HCON_TXPRMB		0x00800000	/* Tx preamble */
#define ARM_HCON_TXDTR		0x01000000	/* Tx data terminal ready */
#define ARM_HCON_RXDISCON	0x02000000	/* Rx frame discontinue */
#define ARM_HCON_TXNOCRC	0x04000000	/* Tx no CRC */
#define ARM_HCON_RXNOCRC	0x08000000	/* Rx no CRC */
#define ARM_HCON_AUTOEN		0x10000000	/* Auto-enable on DCD and CTS */
#define ARM_HCON_TRXSTOP	0x20000000	/* Transparent Rx stop */
#define ARM_HCON_TXREV		0x40000000	/* Transmit reverse */
#define ARM_HCON_RXREV		0x80000000	/* Receive reverse */

#define ARM_HCON_BITS "\20"\
"\1txrs\2rxrs\3dtxrs\4drxrs\5txen\6rxen\7dtxen\10drxen"\
"\11dpllen\12brgen\13tx4wd\14rx4wd\15rxwa0\16rxwa1\17dtxstsk\20drxstsk"\
"\21drxmadec\22txflag\23txsflag\24txloop\25rxecho\26txabtext\27txabt\30txprmb"\
"\31txdtr\32rxdiscon\33txnocrc\34rxnocrc\35autoen\36trxstop\37txrev\40rxrev"

/*
 * HDLC status, R/W, default 0x00000000
 */
#define ARM_HSTAT(n)		ARM_REGISTER ((n) ? 0x8008 : 0x7008)

#define ARM_HSTAT_RXRB		0x0000000f	/* Rx remaining bytes */
#define ARM_HSTAT_TXFC		0x00000010	/* Tx frame complete */
#define ARM_HSTAT_TXFA		0x00000020	/* Tx FIFO available */
#define ARM_HSTAT_TXCTS		0x00000040	/* Tx clear-to-send */
#define ARM_HSTAT_TXSCTS	0x00000080	/* Tx stored CTS */
#define ARM_HSTAT_TXU		0x00000100	/* Tx underrun */
#define ARM_HSTAT_RXFA		0x00000200	/* Rx FIFO available */
#define ARM_HSTAT_TXFG		0x00000400	/* Tx frame good */
#define ARM_HSTAT_RXFD		0x00000800	/* Rx flag detected */
#define ARM_HSTAT_RXDCD		0x00001000	/* Rx data carrier */
#define ARM_HSTAT_RXSDCD	0x00002000	/* Rx stored DCD */
#define ARM_HSTAT_RXFV		0x00004000	/* Rx frame valid */
#define ARM_HSTAT_RXIDLE	0x00008000	/* Rx idle */
#define ARM_HSTAT_RXABT		0x00010000	/* Rx abort */
#define ARM_HSTAT_RXCRCE	0x00020000	/* Rx CRC error */
#define ARM_HSTAT_RXNO		0x00040000	/* Rx non-octet align */
#define ARM_HSTAT_RXOV		0x00080000	/* Rx overrun */
#define ARM_HSTAT_RXMOV		0x00100000	/* DMA Rx memory overflow */
#define ARM_HSTAT_DTXABT	0x00400000	/* DMA Tx abort */
#define ARM_HSTAT_RXIERR	0x00800000	/* Rx internal error */
#define ARM_HSTAT_DRXFD		0x01000000	/* DMA Rx frame done */
#define ARM_HSTAT_DRXNL		0x02000000	/* DMA Rx null list */
#define ARM_HSTAT_DRXNO		0x04000000	/* DMA Rx not owner */
#define ARM_HSTAT_DTXFD		0x08000000	/* DMA Tx frame done */
#define ARM_HSTAT_DTXNL		0x10000000	/* DMA Tx null list */
#define ARM_HSTAT_DTXNO		0x20000000	/* DMA Tx not owner */
#define ARM_HSTAT_DPLLOM	0x40000000	/* DPLL one clock missing */
#define ARM_HSTAT_DPLLTM	0x80000000	/* DPLL two clocks missing */

#define ARM_HSTAT_BITS "\20"\
"\5txfc\6txfa\7txcts\10txscts"\
"\11txu\12rxfa\13txfg\14rxfd\15rxdcd\16rxsdcd\17rxfv\20rxidle"\
"\21rxabt\22rxcrce\23rxno\24rxov\25rxmov\27dtxabt\30rxierr"\
"\31drxfd\32drxnl\33drxno\34dtxfd\35dtxnl\36dtxno\37dpllom\40dplltm"

/*
 * HDLC interrupt enable, R/W, default 0x00000000
 */
#define ARM_HINTEN(n)		ARM_REGISTER ((n) ? 0x800c : 0x700c)

#define ARM_HINTEN_TXFCIE	0x00000010	/* Tx frame complete */
#define ARM_HINTEN_TXFAIE	0x00000020	/* Tx FIFO available */
#define ARM_HINTEN_TXSCTSIE	0x00000080	/* CTS transition */
#define ARM_HINTEN_TXUIE	0x00000100	/* Tx under-run */
#define ARM_HINTEN_RXFAIE	0x00000200	/* Rx FIFO available */
#define ARM_HINTEN_TXFGIE	0x00000400	/* Tx frame good */
#define ARM_HINTEN_RXFDIE	0x00000800	/* Rx flag detected */
#define ARM_HINTEN_RXSDCDIE	0x00002000	/* DCD transition */
#define ARM_HINTEN_RXFVIE	0x00004000	/* Rx frame valid */
#define ARM_HINTEN_RXIDLEIE	0x00008000	/* Idle detected */
#define ARM_HINTEN_RXABTIE	0x00010000	/* Abort detected */
#define ARM_HINTEN_RXCRCEIE	0x00020000	/* CRC error frame */
#define ARM_HINTEN_RXNOIE	0x00040000	/* Non-octet aligned frame */
#define ARM_HINTEN_RXOVIE	0x00080000	/* Rx overrun */
#define ARM_HINTEN_RXMOVIE	0x00100000	/* Rx memory overflow */
#define ARM_HINTEN_DTXABTIE	0x00400000	/* DMA Tx abort */
#define ARM_HINTEN_RXIERRIE	0x00800000	/* Rx internal error */
#define ARM_HINTEN_DRXFDIE	0x01000000	/* DMA Rx frame done */
#define ARM_HINTEN_DRXNLIE	0x02000000	/* DMA Rx null list */
#define ARM_HINTEN_DRXNOIE	0x04000000	/* DMA Rx not owner */
#define ARM_HINTEN_DTXFDIE	0x08000000	/* DMA Tx frame done */
#define ARM_HINTEN_DTXNLIE	0x10000000	/* DMA Tx null list */
#define ARM_HINTEN_DTXNOIE	0x20000000	/* DMA Tx not owner */
#define ARM_HINTEN_DPLLOMIE	0x40000000	/* DPLL one clock missing */
#define ARM_HINTEN_DPLLTMIE	0x80000000	/* DPLL two clocks missing */

/*
 * HTxFIFO frame continue, W
 */
#define ARM_HTXFIFOC(n)		ARM_REGISTER ((n) ? 0x8010 : 0x7010)

/*
 * HTxFIFO frame terminate, W
 */
#define ARM_HTXFIFOT(n)		ARM_REGISTER ((n) ? 0x8014 : 0x7014)

/*
 * HRxFIFO entry, R, default 0x00000000
 */
#define ARM_HRXFIFO(n)		ARM_REGISTER ((n) ? 0x8018 : 0x7018)

/*
 * HDLC BRG time constant, R/W, default 0x00000000
 */
#define ARM_HBRGTC(n)		ARM_REGISTER ((n) ? 0x801c : 0x701c)

#define ARM_HBRGTC_DIV_MASK	0x0000000f	/* CNT1, CNT2 */
#define ARM_HBRGTC_DIV_16	0x00000001	/* divide by 16 */
#define ARM_HBRGTC_DIV_32	0x00000002	/* divide by 32 */
#define ARM_HBRGTC_DIV_256	0x00000005	/* divide by 256 */
#define ARM_HBRGTC_DIV_512	0x00000006	/* divide by 512 */

#define ARM_HBRGTC_CNT0_MASK	0x0000fff0	/* time constant 0 */
#define ARM_HBRGTC_CNT0(p)	((p) << 4)

/*
 * HDLC preamble, R/W, default 0x00000000
 */
#define ARM_HPRMB(n)		ARM_REGISTER ((n) ? 0x8020 : 0x7020)

/*
 * HDLC station address 0, R/W, default 0x00000000
 */
#define ARM_HSAR0(n)		ARM_REGISTER ((n) ? 0x8024 : 0x7024)

/*
 * HDLC station address 1, R/W, default 0x00000000
 */
#define ARM_HSAR1(n)		ARM_REGISTER ((n) ? 0x8028 : 0x7028)

/*
 * HDLC station address 2, R/W, default 0x00000000
 */
#define ARM_HSAR2(n)		ARM_REGISTER ((n) ? 0x802c : 0x702c)

/*
 * HDLC station address 3, R/W, default 0x00000000
 */
#define ARM_HSAR3(n)		ARM_REGISTER ((n) ? 0x8030 : 0x7030)

/*
 * HDLC mask, R/W, default 0x00000000
 */
#define ARM_HMASK(n)		ARM_REGISTER ((n) ? 0x8034 : 0x7034)

/*
 * DMA Tx buffer descr.ptr, R/W, default 0xFFFFFFFF
 */
#define ARM_HDMATXPTR(n)	ARM_REGISTER ((n) ? 0x8038 : 0x7038)

/*
 * DMA Rx buffer descr.ptr, R/W, default 0xFFFFFFFF
 */
#define ARM_HDMARXPTR(n)	ARM_REGISTER ((n) ? 0x803c : 0x703c)

/*
 * Maximum frame length, R/W, default 0xXXXX0000
 */
#define ARM_HMFLR(n)		ARM_REGISTER ((n) ? 0x8040 : 0x7040)

/*
 * Receive buffer size, R/W, default 0xXXXX0000
 */
#define ARM_HRBSR(n)		ARM_REGISTER ((n) ? 0x8044 : 0x7044)

/*
 * HDLC Sync Register, R/W, default 0x7E
 */
#define ARM_HSYNC(n)		ARM_REGISTER ((n) ? 0x8048 : 0x7048)

/*
 * Transparent Control, R/W, default 0x00000000
 */
#define ARM_TCON(n)		ARM_REGISTER ((n) ? 0x804c : 0x704c)

#define ARM_TCON_RTS		0x00000010	/* RTS pin control */

/*
 * TSA configuration, W, default 0x00000000
 */
#define ARM_TSACFG		ARM_REGISTER (0x7800)

/*
 * TSA A time slot, W, default 0x00000000
 */
#define ARM_TSAASLOT		ARM_REGISTER (0x7804)

/*
 * TSA B time slot, W, default 0x00000000
 */
#define ARM_TSABSLOT		ARM_REGISTER (0x7808)

/*----------------------------------------------
 * Ethernet (BDMA)
 */
/*
 * Buffered DMA receive control, R/W, default 0x00000000
 */
#define ARM_BDMATXCON		ARM_REGISTER (0x9000)

#define ARM_BDMATXCON_BRST	0x0000001f	/* BDMA Tx burst size */
#define ARM_BDMATXCON_BRST_1		0x00000000	/* 1 word */
#define ARM_BDMATXCON_BRST_4		0x00000003	/* 4 words */
#define ARM_BDMATXCON_BRST_8		0x00000007	/* 8 words */
#define ARM_BDMATXCON_BRST_12		0x0000000b	/* 12 words */
#define ARM_BDMATXCON_BRST_16		0x0000000f	/* 16 words */
#define ARM_BDMATXCON_BRST_20		0x00000013	/* 20 words */
#define ARM_BDMATXCON_BRST_24		0x00000017	/* 24 words */
#define ARM_BDMATXCON_BRST_28		0x0000001b	/* 28 words */
#define ARM_BDMATXCON_BRST_32		0x0000001f	/* 32 words */

#define ARM_BDMATXCON_STSKO	0x00000020	/* stop/skip by owner bit */
#define ARM_BDMATXCON_CCPIE	0x00000080	/* ctl pkt intr enable */
#define ARM_BDMATXCON_NLIE	0x00000100	/* null list intr enable */
#define ARM_BDMATXCON_NOIE	0x00000200	/* not owner intr enable */
#define ARM_BDMATXCON_EMPTY	0x00000400	/* buffer empty intr enable */

#define ARM_BDMATXCON_MSL	0x00003800	/* transmit to MAC start level */
#define ARM_BDMATXCON_MSL_NOWAIT	0x00000000	/* no wait */
#define ARM_BDMATXCON_MSL_1_8		0x00000800	/* wait 1/8 of buffer */
#define ARM_BDMATXCON_MSL_2_8		0x00001000	/* wait 2/8 of buffer */
#define ARM_BDMATXCON_MSL_3_8		0x00001800	/* wait 3/8 of buffer */
#define ARM_BDMATXCON_MSL_4_8		0x00002000	/* wait 4/8 of buffer */
#define ARM_BDMATXCON_MSL_5_8		0x00002800	/* wait 5/8 of buffer */
#define ARM_BDMATXCON_MSL_6_8		0x00003000	/* wait 6/8 of buffer */
#define ARM_BDMATXCON_MSL_7_8		0x00003800	/* wait 7/8 of buffer */

#define ARM_BDMATXCON_EN	0x00004000	/* BDMA Tx enable */
#define ARM_BDMATXCON_RS	0x00008000	/* reset BDMA Tx block */

/*
 * Buffered DMA transmit control, R/W, default 0x00000000
 */
#define ARM_BDMARXCON		ARM_REGISTER (0x9004)

#define ARM_BDMARXCON_BRST	0x0000001f	/* BDMA Rx burst size */
#define ARM_BDMARXCON_BRST_1		0x00000000	/* 1 word */
#define ARM_BDMARXCON_BRST_4		0x00000003	/* 4 words */
#define ARM_BDMARXCON_BRST_8		0x00000007	/* 8 words */
#define ARM_BDMARXCON_BRST_12		0x0000000b	/* 12 words */
#define ARM_BDMARXCON_BRST_16		0x0000000f	/* 16 words */
#define ARM_BDMARXCON_BRST_20		0x00000013	/* 20 words */
#define ARM_BDMARXCON_BRST_24		0x00000017	/* 24 words */
#define ARM_BDMARXCON_BRST_28		0x0000001b	/* 28 words */
#define ARM_BDMARXCON_BRST_32		0x0000001f	/* 32 words */

#define ARM_BDMARXCON_STSKO	0x00000020	/* stop/skip by owner bit */
#define ARM_BDMARXCON_MAINC	0x00000040	/* memory address increment */
#define ARM_BDMARXCON_DIE	0x00000080	/* every pkt intr enable */
#define ARM_BDMARXCON_NLIE	0x00000100	/* null list intr enable */
#define ARM_BDMARXCON_NOIE	0x00000200	/* not owner intr enable */
#define ARM_BDMARXCON_MSOIE	0x00000400	/* max size over intr enable */
#define ARM_BDMARXCON_LITTLE	0x00000800	/* little endian */

#define ARM_BDMARXCON_WA	0x00003000	/* word alignment */
#define ARM_BDMARXCON_WA_1		0x00001000	/* 1 invalid byte */
#define ARM_BDMARXCON_WA_2		0x00002000	/* 2 invalid bytes */
#define ARM_BDMARXCON_WA_3		0x00003000	/* 3 invalid bytes */

#define ARM_BDMARXCON_EN	0x00004000	/* BDMA Rx enable */
#define ARM_BDMARXCON_RS	0x00008000	/* reset BDMA Rx block */
#define ARM_BDMARXCON_EMPTY	0x00010000	/* buffer empty intr enable */
#define ARM_BDMARXCON_EARLY	0x00020000	/* early notify intr enable */

/*
 * Transmit frame descriptor start address, R/W, default 0x00000000
 */
#define ARM_BDMATXPTR		ARM_REGISTER (0x9008)

/*
 * Receive frame descriptor start address, R/W, default 0x00000000
 */
#define ARM_BDMARXPTR		ARM_REGISTER (0x900C)

/*
 * Receive frame maximum size, R/W, default Undefined
 */
#define ARM_BDMARXLSZ		ARM_REGISTER (0x9010)

/*
 * Buffered DMA status, R/W, default 0x00000000
 */
#define ARM_BDMASTAT		ARM_REGISTER (0x9014)

#define ARM_BDMASTAT_RX_RDF	0x00000001	/* Rx done frame */
#define ARM_BDMASTAT_RX_NL	0x00000002	/* Rx null list */
#define ARM_BDMASTAT_RX_NO	0x00000004	/* Rx not owner */
#define ARM_BDMASTAT_RX_MSO	0x00000008	/* Rx max size over */
#define ARM_BDMASTAT_RX_EMPTY	0x00000010	/* Rx buffer empty */
#define ARM_BDMASTAT_RX_SEARLY	0x00000020	/* Rx early notification */
#define ARM_BDMASTAT_RX_FRF	0x00000080	/* one more frame in Rx buf */
#define ARM_BDMASTAT_RX_NFR	0x0000ff00	/* number of frames in Rx buf */

#define ARM_BDMASTAT_TX_CCP	0x00010000	/* Tx complete ctl pkt */
#define ARM_BDMASTAT_TX_NL	0x00020000	/* Tx null list */
#define ARM_BDMASTAT_TX_NO	0x00040000	/* Tx not owner */
#define ARM_BDMASTAT_TX_EMPTY	0x00100000	/* Tx buffer empty */

#define ARM_BDMASTAT_BITS "\20"\
"\1rx-rdf\2rx-nl\3rx-no\4rx-mso\5rx-empty\6rx-searly\10rx-frf"\
"\21tx-ccp\22tx-nl\23tx-no\25tx-empty"

/*
 * CAM content (32 words), W,   default Undefined
 */
#define ARM_CAM(n)		ARM_REGISTER (0x9100 + (n)*4)

/*
 * BDMA Tx buffer (64 words), R/W, default Undefined
 */
#define ARM_BDMATXBUF		ARM_REGISTER (0x9200)

/*
 * BDMA Rx buffer (64 words), R/W, default Undefined
 */
#define ARM_BDMARXBUF		ARM_REGISTER (0x9800)

/*----------------------------------------------
 * Ethernet (MAC)
 */
/*
 * Ethernet MAC control, R/W, default 0x00000000
 */
#define ARM_MACON		ARM_REGISTER (0xA000)

#define ARM_MACON_HALTREQ	0x00000001	/* halt request */
#define ARM_MACON_HALTIMM	0x00000002	/* halt immediate */
#define ARM_MACON_RESET		0x00000004	/* MAC software reset */
#define ARM_MACON_FULLDUP	0x00000008	/* enable full duplex */
#define ARM_MACON_MACLOOP	0x00000010	/* enable internal loopback */
#define ARM_MACON_MIIOFF	0x00000040	/* select 10 Mbps interface */
#define ARM_MACON_LOOP10	0x00000080	/* set Loop_10 signal */
#define ARM_MACON_MISSROLL	0x00000400	/* missed error counter rolled
						   over (readonly) */
#define ARM_MACON_MDCOFF	0x00001000	/* disable MDC clock */
#define ARM_MACON_ENMISSROLL	0x00002000	/* enable irq on MISSROLL */
#define ARM_MACON_LINK10	0x00008000	/* read Link_10 pin */

/*
 * CAM control, R/W, default 0x00000000
 */
#define ARM_CAMCON		ARM_REGISTER (0xA004)

#define ARM_CAMCON_STATIONACC	0x00000001	/* accept any unicast */
#define ARM_CAMCON_GROUPACC	0x00000002	/* accept any multicast */
#define ARM_CAMCON_BROADACC	0x00000004	/* accept broadcasts */
#define ARM_CAMCON_NEGCAM	0x00000008	/* negative CAM */
#define ARM_CAMCON_COMPEN	0x00000010	/* enable compare mode */

/*
 * MAC transmit control, R/W, default 0x00000000
 */
#define ARM_MACTXCON		ARM_REGISTER (0xA008)

#define ARM_MACTXCON_TXEN	0x00000001	/* enable transmittion */
#define ARM_MACTXCON_TXHALT	0x00000002	/* halt transmittion */
#define ARM_MACTXCON_NOPAD	0x00000004	/* no padding to 64 bytes */
#define ARM_MACTXCON_NOCRC	0x00000008	/* suppress addition of CRC */
#define ARM_MACTXCON_FBACK	0x00000010	/* faster back-off time */
#define ARM_MACTXCON_NODEF	0x00000020	/* disable defer counting */
#define ARM_MACTXCON_SDPAUSE	0x00000040	/* send pause command */
#define ARM_MACTXCON_SQEN	0x00000080	/* enable 10 Mbps SQE testing */
#define ARM_MACTXCON_ENUNDER	0x00000100	/* en.interrupt on underruns */
#define ARM_MACTXCON_ENDEFER	0x00000200	/* en.irq on max defer time */
#define ARM_MACTXCON_ENNCARR	0x00000400	/* en.irq on carrier loss */
#define ARM_MACTXCON_ENEXCOLL	0x00000800	/* en.irq on 16 collisions */
#define ARM_MACTXCON_ENLATECOLL	0x00001000	/* en.irq on collision
						   after 512 bits */
#define ARM_MACTXCON_ENTXPAR	0x00002000	/* en.irq on parity error
						   in Tx FIFO */
#define ARM_MACTXCON_ENCOMP	0x00004000	/* en.irq on frame completion */

/*
 * MAC transmit status, R/W, default 0x00000000
 */
#define ARM_MACTXSTAT		ARM_REGISTER (0xA00C)

#define ARM_MACTXSTAT_TXCOLL	0x0000000f	/* collision count */
#define ARM_MACTXSTAT_EXCOLL	0x00000010	/* excessive collisions (16) */
#define ARM_MACTXSTAT_TXDEFER	0x00000020	/* delay during transmission */
#define ARM_MACTXSTAT_PAUSED	0x00000040	/* `Pause' received */
#define ARM_MACTXSTAT_INTTX	0x00000080	/* transmission interrupt */
#define ARM_MACTXSTAT_UNDER	0x00000100	/* data underrun */
#define ARM_MACTXSTAT_DEFER	0x00000200	/* transfer deferred */
#define ARM_MACTXSTAT_NCARR	0x00000400	/* no carrier */
#define ARM_MACTXSTAT_SQE	0x00000800	/* SQE not present */
#define ARM_MACTXSTAT_LATECOLL	0x00001000	/* collision after 512 bits */
#define ARM_MACTXSTAT_TXPAR	0x00002000	/* transmit parity error */
#define ARM_MACTXSTAT_COMP	0x00004000	/* packet completed */
#define ARM_MACTXSTAT_TXHALTED	0x00008000	/* transmission halted */

#define ARM_MACTXSTAT_BITS "\20"\
"\5excoll\6txdefer\7paused\10inttx\11under\12defer\13ncarr"\
"\14sqe\15latecoll\16txpar\17comp\20txhalted"

/*
 * MAC receive control, R/W, default 0x00000000
 */
#define ARM_MACRXCON		ARM_REGISTER (0xA010)

#define ARM_MACRXCON_RXEN	0x00000001	/* receive enable */
#define ARM_MACRXCON_RXHALT	0x00000002	/* receive halt request */
#define ARM_MACRXCON_LONGEN	0x00000004	/* long enable (>1518 bytes) */
#define ARM_MACRXCON_SHORTEN	0x00000008	/* short enable (<64 bytes) */
#define ARM_MACRXCON_STRIPCRC	0x00000010	/* strip CRC from packet */
#define ARM_MACRXCON_PASSCTL	0x00000020	/* receive ctl packets */
#define ARM_MACRXCON_IGNORECRC	0x00000040	/* disable CRC checking */
#define ARM_MACRXCON_ENALIGN	0x00000100	/* en.irq on alignment errs */
#define ARM_MACRXCON_ENCRCERR	0x00000200	/* en.irq on CRC errors */
#define ARM_MACRXCON_ENOVER	0x00000400	/* en.irq on overflows */
#define ARM_MACRXCON_ENLONGERR	0x00000800	/* en.irq on frames >1518 bytes */
#define ARM_MACRXCON_ENRXPAR	0x00002000	/* en.irq on parity error
						   in Rx FIFO */
#define ARM_MACRXCON_ENGOOD	0x00004000	/* en.irq on good packet */

/*
 * MAC receive status, R/W, default 0x00000000
 */
#define ARM_MACRXSTAT		ARM_REGISTER (0xA014)

#define ARM_MACRXSTAT_CTLRECD	0x00000020	/* ctl frame received */
#define ARM_MACRXSTAT_INTRX	0x00000040	/* packet received */
#define ARM_MACRXSTAT_RX10STAT	0x00000080	/* 1 for 7-wire interface */
#define ARM_MACRXSTAT_ALIGNERR	0x00000100	/* alignment error */
#define ARM_MACRXSTAT_CRCERR	0x00000200	/* CRC error */
#define ARM_MACRXSTAT_OVERFLOW	0x00000400	/* overflow error */
#define ARM_MACRXSTAT_LONGERR	0x00000800	/* frame longer 1518 bytes */
#define ARM_MACRXSTAT_RXPAR	0x00002000	/* parity error in FIFO */
#define ARM_MACRXSTAT_GOOD	0x00004000	/* good packet received */
#define ARM_MACRXSTAT_RXHALTED	0x00008000	/* reception halted */

#define ARM_MACRXSTAT_BITS "\20"\
"\6ctlrecd\7intrx\10rx10stat\11alignerr\12crcerr\13overflow"\
"\14longerr\16rxpar\17good\20rxhalted"

/*
 * Station management data, R/W, default 0x00000000
 */
#define ARM_STADATA		ARM_REGISTER (0xA018)

/*
 * Station management control and address, R/W, default 0x00006000
 */
#define ARM_STACON		ARM_REGISTER (0xA01C)

#define ARM_STACON_ADDR		0x0000001f	/* PHY register address */
#define ARM_STACON_PHY		0x000003e0	/* PHY address */
#define ARM_STACON_WR		0x00000400	/* write operation */
#define ARM_STACON_BUSY		0x00000800	/* busy bit */
#define ARM_STACON_PRESUP	0x00001000	/* preamble suppress */

#define ARM_STACON_MDCCLK	0x0000e000	/* MDC clock */
#define ARM_STACON_MDCCLK_16		0x00000000	/* 16/MCLK */
#define ARM_STACON_MDCCLK_18		0x00002000	/* 18/MCLK */
#define ARM_STACON_MDCCLK_20		0x00004000	/* 20/MCLK */
#define ARM_STACON_MDCCLK_22		0x00006000	/* 22/MCLK */
#define ARM_STACON_MDCCLK_24		0x00008000	/* 24/MCLK */
#define ARM_STACON_MDCCLK_26		0x0000a000	/* 26/MCLK */
#define ARM_STACON_MDCCLK_28		0x0000c000	/* 28/MCLK */
#define ARM_STACON_MDCCLK_30		0x0000e000	/* 30/MCLK */

/*
 * CAM enable, R/W, default 0x00000000
 */
#define ARM_CAMEN		ARM_REGISTER (0xA028)

/*
 * Missed error count, R/W, default 0x00000000
 */
#define ARM_EMISSCNT		ARM_REGISTER (0xA03C)

/*
 * Pause count, R, default 0x00000000
 */
#define ARM_EPZCNT		ARM_REGISTER (0xA040)

/*
 * Remote pause count, R, default 0x00000000
 */
#define ARM_ERMPZCNT		ARM_REGISTER (0xA044)

/*
 * Transmit control frame status, R, default 0x00000000
 */
#define ARM_ETXSTAT		ARM_REGISTER (0x9040)

/*----------------------------------------------
 * I2C Bus
 */
/*
 * I2C bus control status, R/W, default 0x00000054
 */
#define ARM_IICCON		ARM_REGISTER (0xf000)

/*
 * I2C bus shift buffer, R/W, default undefined
 */
#define ARM_IICBUF		ARM_REGISTER (0xf004)

/*
 * I2C bus prescaler, R/W, default 0x00000000
 */
#define ARM_IICPS		ARM_REGISTER (0xf008)

/*
 * I2C bus prescaler counter, R, default 0x00000000
 */
#define ARM_IICCOUNT		ARM_REGISTER (0xf00c)

/*----------------------------------------------
 * GDMA
 */
/*
 * GDMA control, R/W, default 0x00000000
 */
#define ARM_GDMACON(n)		ARM_REGISTER ((n) ? 0xc000 : 0xb000)

/*
 * GDMA source address, R/W, default undefined
 */
#define ARM_GDMASRC(n)		ARM_REGISTER ((n) ? 0xc004 : 0xb004)

/*
 * GDMA destination address, R/W, default undefined
 */
#define ARM_GDMADST(n)		ARM_REGISTER ((n) ? 0xc008 : 0xb008)

/*
 * GDMA transfer count, R/W, default undefined
 */
#define ARM_GDMACNT(n)		ARM_REGISTER ((n) ? 0xc00c : 0xb00c)

/* End of Samsung S3C4530A register definitions.
 *----------------------------------------------*/
