/*
 * Register definitions for Netsilicon Net+50.
 */

/*
 * Internal SRAM address and size.
 */
#define ARM_SRAM_BASE		0xfff00000
#define ARM_SRAM_SIZE		0x00004000

/*
 * Base address of special registers.
 */
#define ARM_REGISTER(x)		(*(volatile unsigned long*) (x))

/*----------------------------------------------
 * GEN module
 */
/*
 * General System Control Register
 */
#define ARM_GSCR		ARM_REGISTER (0xffb00000)

#define ARM_GSCR_LENDIAN	0x80000000	/* little endian, A27 */

#define ARM_GSCR_BSPEED_MASK	0x60000000	/* system bus clock speed */
#define ARM_GSCR_BSPEED_4	0x00000000	/* 1/4 of SYSCLK */
#define ARM_GSCR_BSPEED_2	0x20000000	/* 1/2 of SYSCLK */
#define ARM_GSCR_BSPEED_FULL	0x40000000	/* full speed */

#define ARM_GSCR_SWE		0x01000000	/* watchdog enable */

#define ARM_GSCR_SWRI_MASK	0x00c00000	/* wdog reset/intr select */
#define ARM_GSCR_SWRI_IRQ	0x00000000	/* wdog causes IRQ */
#define ARM_GSCR_SWRI_FIRQ	0x00400000	/* wdog causes FIRQ */
#define ARM_GSCR_SWRI_RESET	0x00800000	/* wdog causes reset */

#define ARM_GSCR_SWT_MASK	0x00300000	/* wdog timeout period */
#define ARM_GSCR_SWT_2_20	0x00000000	/* 2^20 / Fxtal */
#define ARM_GSCR_SWT_2_22	0x00100000	/* 2^22 / Fxtal */
#define ARM_GSCR_SWT_2_24	0x00200000	/* 2^24 / Fxtal */
#define ARM_GSCR_SWT_2_25	0x00300000	/* 2^25 / Fxtal */

#define ARM_GSCR_BME		0x00040000	/* bus monitor enable */

#define ARM_GSCR_BMT_MASK	0x00030000	/* bus monitor timer */
#define ARM_GSCR_BMT_128	0x00000000	/* 128 BCLKs */
#define ARM_GSCR_BMT_64		0x00010000	/* 64 BCLKs */
#define ARM_GSCR_BMT_32		0x00020000	/* 32 BCLKs */
#define ARM_GSCR_BMT_16		0x00030000	/* 16 BCLKs */

#define ARM_GSCR_USER		0x00008000	/* user access to registers */
#define ARM_GSCR_BUSER		0x00004000	/* must be 0 */
#define ARM_GSCR_IARB		0x00002000	/* must be 1 */
#define ARM_GSCR_DMATST		0x00001000	/* DMA module test mode */
#define ARM_GSCR_TEALAST	0x00000800	/* use /TEA for burst */
#define ARM_GSCR_MISALIGN	0x00000400	/* bus abort on misalign */
#define ARM_GSCR_CACHE		0x00000200	/* cache enable */
#define ARM_GSCR_DMARST		0x00000040	/* DMA module reset */

#define ARM_GSCR_BSYNC_MASK	0x00000030	/* /TA input synchronizer */
#define ARM_GSCR_BSYNC_1	0x00000000	/* 1 stage synchronizer */
#define ARM_GSCR_BSYNC_2	0x00000020	/* 2 stage synchronizer */
#define ARM_GSCR_BSYNC_OFF	0x00000030	/* no synchronizer */

/*
 * General System Status Register
 */
#define ARM_GSSR		ARM_REGISTER (0xffb00004)

#define ARM_GSSR_REV_MASK	0xff000000	/* chip revision id */
#define ARM_GSSR_REV_NET_10	0x00000000	/* Net+5, Net+10 */
#define ARM_GSSR_REV_NET_12_1	0x01000000
#define ARM_GSSR_REV_NET_15_0	0x04000000
#define ARM_GSSR_REV_NET_15_1	0x05000000
#define ARM_GSSR_REV_NET_15_2	0x06000000
#define ARM_GSSR_REV_NET_15_3	0x07000000
#define ARM_GSSR_REV_NET_40_0	0x08000000
#define ARM_GSSR_REV_NET_40_1	0x09000000
#define ARM_GSSR_REV_NET_40_2	0x0a000000
#define ARM_GSSR_REV_NET_40_3	0x0b000000
#define ARM_GSSR_REV_NET_50_1	0x19000000
#define ARM_GSSR_REV_NET_50_3	0x1b000000

#define ARM_GSSR_EXT		0x00800000	/* last reset was /RESET */
#define ARM_GSSR_POW		0x00400000	/* last reset was powerup */
#define ARM_GSSR_SW		0x00200000	/* last reset was wdog */
#define ARM_GSSR_RSTIO		0x00100000	/* last reset was RSTIO */
#define ARM_GSSR_LOCK		0x00010000	/* system PLL in lock state */
#define ARM_GSSR_GEN_ID		0x000003ff	/* product id, A19:09 */

/*
 * General Software Service Register
 */
#define ARM_GSWSR		ARM_REGISTER (0xffb0000c)

#define ARM_GSWSR_WDOG_ACK1	0x0000005a	/* acknowledge watch-dog */
#define ARM_GSWSR_WDOG_ACK2	0x000000a5
#define ARM_GSWSR_RESET1	0x00000123	/* software reset */
#define ARM_GSWSR_RESET2	0x00000321

/*----------------------------------------------
 * Timers
 */
/*
 * Timer Control registers
 */
#define ARM_TCR1		ARM_REGISTER (0xffb00010)
#define ARM_TCR2		ARM_REGISTER (0xffb00018)

#define ARM_TCR_TE		0x80000000	/* timer enable */
#define ARM_TCR_TIE		0x40000000	/* timer interrupt enable */
#define ARM_TCR_TIRQ		0x20000000	/* timer interrupt mode=FIRQ */
#define ARM_TCR_TPRE		0x10000000	/* timer prescaler 1:512 */

#define ARM_TCR_TCLK_MASK	0x08000000	/* timer clock source */
#define ARM_TCR_TCLK_FXTAL	0x00000000	/* Fxtal = 3.6864 MHz */
#define ARM_TCR_TCLK_FSYSCLK	0x08000000	/* Fsysclk = 44.2368 MHz */

#define ARM_TCR_ITC_MASK	0x07ffffff	/* initial timer count */

/*
 * Timer Status registers
 */
#define ARM_TSR1		ARM_REGISTER (0xffb00014)
#define ARM_TSR2		ARM_REGISTER (0xffb0001c)

#define ARM_TSR_TIP		0x40000000	/* timer interrupt pending */
#define ARM_TSR_CTC_MASK	0x07ffffff	/* current timer count */

/*----------------------------------------------
 * I/O ports
 */
#define ARM_PORTA		ARM_REGISTER (0xffb00020)
#define ARM_PORTB		ARM_REGISTER (0xffb00024)
#define ARM_PORTC		ARM_REGISTER (0xffb00028)
#define ARM_PORTD1		ARM_REGISTER (0xffa00040)
#define ARM_PORTD2		ARM_REGISTER (0xffa00044)
#define ARM_PORTF		ARM_REGISTER (0xffa00048)
#define ARM_PORTG		ARM_REGISTER (0xffa0004c)
#define ARM_PORTH		ARM_REGISTER (0xffa00050)

#define ARM_PORTX_MODE_SHIFT	24
#define ARM_PORTX_DIR_SHIFT	16

/*----------------------------------------------
 * Interrupt controller
 */
/*
 * Interrupt Enable Register
 */
#define ARM_IER			ARM_REGISTER (0xffb00030)

/*
 * Interrupt Enable Register - Set/Enabled
 */
#define ARM_IER_SET		ARM_REGISTER (0xffb00034)

/*
 * Interrupt Enable Register - Clear/Raw
 */
#define ARM_IER_CLEAR		ARM_REGISTER (0xffb00038)

#define ARM_IER_DMA1		0x80000000	/* DMA channel 1 */
#define ARM_IER_DMA2		0x40000000	/* DMA channel 2 */
#define ARM_IER_DMA3		0x20000000	/* DMA channel 3 */
#define ARM_IER_DMA4		0x10000000	/* DMA channel 4 */
#define ARM_IER_DMA5		0x08000000	/* DMA channel 5 */
#define ARM_IER_DMA6		0x04000000	/* DMA channel 6 */
#define ARM_IER_DMA7		0x02000000	/* DMA channel 7 */
#define ARM_IER_DMA8		0x01000000	/* DMA channel 8 */
#define ARM_IER_DMA9		0x00800000	/* DMA channel 9 */
#define ARM_IER_DMA10		0x00400000	/* DMA channel 10 */
#define ARM_IER_ENETRX		0x00020000	/* ethernet receiver */
#define ARM_IER_ENETTX		0x00010000	/* ethernet transmitter */
#define ARM_IER_SER1RX		0x00008000	/* serial A receiver */
#define ARM_IER_SER1TX		0x00004000	/* serial A transmitter */
#define ARM_IER_SER2RX		0x00002000	/* serial B receiver */
#define ARM_IER_SER2TX		0x00001000	/* serial B transmitter */
#define ARM_IER_WATCHDOG	0x00000040	/* watch-dog timer */
#define ARM_IER_TIMER1		0x00000020	/* timer 1 */
#define ARM_IER_TIMER2		0x00000010	/* timer 2 */
#define ARM_IER_PC3		0x00000008	/* port c3  */
#define ARM_IER_PC2		0x00000004	/* port c2 */
#define ARM_IER_PC1		0x00000002	/* port c1 */
#define ARM_IER_PC0		0x00000001	/* port c0 */

/*----------------------------------------------
 * Cache
 */
/*
 * Cache Control Registers
 */
#define ARM_GCCR0		ARM_REGISTER (0xffb00040)

#define ARM_GCCR_BASE		0xff000000	/* a31:a24 of cached area */
#define ARM_GCCR_MASK		0x00ff0000	/* a31:a24 mask */
#define ARM_GCCR_ENABLE		0x00008000	/* enable this CCR */
#define ARM_GCCR_SUPV		0x00004000	/* cache only supervisor */
#define ARM_GCCR_WP		0x00001000	/* do now allow writes here */
#define ARM_GCCR_FRC32		0x00008000	/* force 32-bit fetches */
#define ARM_GCCR_SET1		0x00000008	/* enable set1 */
#define ARM_GCCR_SET2		0x00000004	/* enable set2 */
#define ARM_GCCR_SET3		0x00000002	/* enable set3 */
#define ARM_GCCR_SET4		0x00000001	/* enable set4 */

/*----------------------------------------------
 * Memory controller module
 */
/*
 * Memory Module Configuration Register
 */
#define ARM_MMCR		ARM_REGISTER (0xffc00000)

#define ARM_MMCR_RFCNT		0xff000000	/* DRAM refresh period */
#define ARM_MMCR_REFEN		0x00800000	/* enable DRAM refresh */

#define ARM_MMCR_RCYC_MASK	0x00600000	/* refresh cycle count */
#define ARM_MMCR_RCYC_8		0x00000000	/* 8 BCLKs */
#define ARM_MMCR_RCYC_6		0x00200000	/* 6 BCLKs */
#define ARM_MMCR_RCYC_5		0x00400000	/* 5 BCLKs */
#define ARM_MMCR_RCYC_4		0x00600000	/* 4 BCLKs */

#define ARM_MMCR_AMUX		0x00100000	/* external addr.multiplexing */
#define ARM_MMCR_A27		0x00080000	/* 0 - use A27 as /CS0OE */
#define ARM_MMCR_A26		0x00040000	/* 0 - use A27 as /CS0WE */
#define ARM_MMCR_A25		0x00020000	/* always set to 0 */
#define ARM_MMCR_AMUX2		0x00010000	/* always set to 0 */

/*
 * Chip Select Base Address Register
 */
#define ARM_CSBAR0		ARM_REGISTER (0xffc00010)
#define ARM_CSBAR1		ARM_REGISTER (0xffc00020)
#define ARM_CSBAR2		ARM_REGISTER (0xffc00030)
#define ARM_CSBAR3		ARM_REGISTER (0xffc00040)
#define ARM_CSBAR4		ARM_REGISTER (0xffc00050)

#define ARM_CSBAR_BASE		0xfffff000	/* a31:a12 of address */

#define ARM_CSBAR_PGSIZE	0x00000c00	/* page size */
#define ARM_CSBAR_PGSIZE_64	0x00000000	/* 64 bytes */
#define ARM_CSBAR_PGSIZE_32	0x00000400	/* 32 bytes */
#define ARM_CSBAR_PGSIZE_16	0x00000800	/* 16 bytes */
#define ARM_CSBAR_PGSIZE_8	0x00000c00	/* 8 bytes */

#define ARM_CSBAR_DMODE_MASK	0x00000300	/* DRAM mode */
#define ARM_CSBAR_DMODE_FP	0x00000000	/* Fast Page DRAM */
#define ARM_CSBAR_DMODE_EDO	0x00000100	/* EDO DRAM */
#define ARM_CSBAR_DMODE_SDRAM	0x00000200	/* SDRAM */

#define ARM_CSBAR_DMUXS		0x00000080	/* external DRAM multiplexer */
#define ARM_CSBAR_EXTTA		0x00000040	/* enable external transfers */
#define ARM_CSBAR_DMUXM		0x00000020	/* mode-1 8 CAS */
#define ARM_CSBAR_IDLE		0x00000010	/* add 1 null cycle */
#define ARM_CSBAR_DRSEL		0x00000008	/* DRAM mode */
#define ARM_CSBAR_BURST		0x00000004	/* burst cycles allowed */
#define ARM_CSBAR_WP		0x00000002	/* do not allow writes */
#define ARM_CSBAR_V		0x00000001	/* enable chip select */

/*
 * Chip Select Option Register
 */
#define ARM_CSOR0		ARM_REGISTER (0xffc00014)
#define ARM_CSOR1		ARM_REGISTER (0xffc00024)
#define ARM_CSOR2		ARM_REGISTER (0xffc00034)
#define ARM_CSOR3		ARM_REGISTER (0xffc00044)
#define ARM_CSOR4		ARM_REGISTER (0xffc00054)

#define ARM_CSOR_MASK		0xfffff000	/* a31:a12 address mask */
#define ARM_CSOR_WAIT_MASK	0x00000f00	/* number of wait states */

#define ARM_CSOR_BCYC_MASK	0x000000c0	/* burst clock cycles */
#define ARM_CSOR_BCYC_1		0x00000000	/* 1 BCLK */
#define ARM_CSOR_BCYC_2		0x00000040	/* 2 BCLKs */
#define ARM_CSOR_BCYC_3		0x00000080	/* 3 BCLKs */
#define ARM_CSOR_BCYC_4		0x000000c0	/* 4 BCLKs */

#define ARM_CSOR_BSIZE		0x00000030	/* burst size */
#define ARM_CSOR_BSIZE_2	0x00000000	/* 2 cycles in burst */
#define ARM_CSOR_BSIZE_4	0x00000010	/* 4 cycles in burst */
#define ARM_CSOR_BSIZE_8	0x00000020	/* 8 cycles in burst */
#define ARM_CSOR_BSIZE_16	0x00000030	/* 16 cycles in burst */

#define ARM_CSOR_PS		0x0000000c	/* port size */
#define ARM_CSOR_PS_32		0x00000000	/* 32-bit */
#define ARM_CSOR_PS_16		0x00000004	/* 16-bit */
#define ARM_CSOR_PS_8		0x00000008	/* 8-bit */
#define ARM_CSOR_PS_32_EXT	0x0000000c	/* 32-bit with ext. data ack */

#define ARM_CSOR_RSYNC		0x00000002	/* read sync mode */
#define ARM_CSOR_WSYNC		0x00000001	/* write sync mode */

/*
 * Chip Select Option Register B
 */
#define ARM_CSORB0		ARM_REGISTER (0xffc00018)
#define ARM_CSORB1		ARM_REGISTER (0xffc00028)
#define ARM_CSORB2		ARM_REGISTER (0xffc00038)
#define ARM_CSORB3		ARM_REGISTER (0xffc00048)
#define ARM_CSORB4		ARM_REGISTER (0xffc00058)

#define ARM_CSORB_SYNC		0x00000003	/* /TA input synchronizer */
#define ARM_CSORB_SYNC_OFF	0x00000000	/* no synchronizer */
#define ARM_CSORB_SYNC_1	0x00000001	/* 1-stage synchronizer */
#define ARM_CSORB_SYNC_2	0x00000002	/* 2-stage synchronizer */

/*----------------------------------------------
 * DMA controller module
 */
/*
 * DMA Buffer Descriptor Pointer
 */
#define ARM_DBDP1A		ARM_REGISTER (0xff900000)
#define ARM_DBDP1B		ARM_REGISTER (0xff900020)
#define ARM_DBDP1C		ARM_REGISTER (0xff900040)
#define ARM_DBDP1D		ARM_REGISTER (0xff900060)
#define ARM_DBDP2		ARM_REGISTER (0xff900080)
#define ARM_DBDP3		ARM_REGISTER (0xff9000a0)
#define ARM_DBDP4		ARM_REGISTER (0xff9000c0)
#define ARM_DBDP5		ARM_REGISTER (0xff9000e0)
#define ARM_DBDP6		ARM_REGISTER (0xff900100)
#define ARM_DBDP7		ARM_REGISTER (0xff900120)
#define ARM_DBDP8		ARM_REGISTER (0xff900140)
#define ARM_DBDP9		ARM_REGISTER (0xff900160)
#define ARM_DBDP10		ARM_REGISTER (0xff900180)

#define ARM_DBDP_WRAP		0x80000000	/* last buffer descriptor */
#define ARM_DBDP_INT		0x40000000	/* generate interrupt */
#define ARM_DBDP_LAST		0x20000000	/* last buffer in frame */

#define ARM_DBDL_FULL		0x00008000	/* buffer is full */

/*
 * DMA Control Register
 */
#define ARM_DCR1A		ARM_REGISTER (0xff900010)
#define ARM_DCR1B		ARM_REGISTER (0xff900030)
#define ARM_DCR1C		ARM_REGISTER (0xff900050)
#define ARM_DCR1D		ARM_REGISTER (0xff900070)
#define ARM_DCR2		ARM_REGISTER (0xff900090)
#define ARM_DCR3		ARM_REGISTER (0xff9000b0)
#define ARM_DCR4		ARM_REGISTER (0xff9000d0)
#define ARM_DCR5		ARM_REGISTER (0xff9000f0)
#define ARM_DCR6		ARM_REGISTER (0xff900110)
#define ARM_DCR7		ARM_REGISTER (0xff900130)
#define ARM_DCR8		ARM_REGISTER (0xff900150)
#define ARM_DCR9		ARM_REGISTER (0xff900170)
#define ARM_DCR10		ARM_REGISTER (0xff900190)

#define ARM_DCR_CE		0x80000000	/* DMA channel enable */
#define ARM_DCR_CA		0x40000000	/* channel abort request */

#define ARM_DCR_BB_MASK		0x30000000	/* bus bandwidth */
#define ARM_DCR_BB_100		0x00000000	/* 100 % */
#define ARM_DCR_BB_75		0x10000000	/* 75 % */
#define ARM_DCR_BB_50		0x20000000	/* 50 % */
#define ARM_DCR_BB_25		0x30000000	/* 25 % */

#define ARM_DCR_MODE_MASK	0x0c000000	/* DMA operation mode */
#define ARM_DCR_MODE_WRITE	0x00000000	/* from device to memory */
#define ARM_DCR_MODE_READ	0x04000000	/* memory to device */
#define ARM_DCR_MODE_MEM	0x08000000	/* memory to memory */

#define ARM_DCR_BTE_MASK	0x03000000	/* burst transfer enable */
#define ARM_DCR_BTE_1		0x00000000      /* 1 operand - no burst*/
#define ARM_DCR_BTE_2		0x01000000      /* 2 operands - 8 bytes */
#define ARM_DCR_BTE_4		0x02000000      /* 4 operands - 16 bytes */

#define ARM_DCR_REQ		0x00800000	/* request ext.peripheral */
#define ARM_DCR_SINC		0x00200000	/* do not incr src address */
#define ARM_DCR_DINC		0x00100000	/* do not incr dst address */

#define ARM_DCR_SIZE_MASK	0x00030000	/* data operand size */
#define ARM_DCR_SIZE_32		0x00000000	/* 32-bit */
#define ARM_DCR_SIZE_16		0x00010000	/* 16-bit */
#define ARM_DCR_SIZE_8		0x00020000	/* 8-bit */

#define ARM_DCR_STATE_MASK	0x0000fc00	/* current DMA state */
#define ARM_DCR_STATE_IDLE	0x00000000      /* idle */
#define ARM_DCR_STATE_LSBA	0x00000400      /* load src buffer address */
#define ARM_DCR_STATE_LDBA	0x00001000      /* load dst buffer address */
#define ARM_DCR_STATE_OP1	0x00002000      /* first operand */
#define ARM_DCR_STATE_OP2	0x00004000      /* second operand */
#define ARM_DCR_STATE_UBD	0x00008000      /* update buffer descriptor */

#define ARM_DCR_INDEX		0x000003ff	/* current descriptor offset */

/*
 * DMA Status/Interrupt Enable Register
 */
#define ARM_DSIER1A		ARM_REGISTER (0xff900014)
#define ARM_DSIER1B		ARM_REGISTER (0xff900034)
#define ARM_DSIER1C		ARM_REGISTER (0xff900054)
#define ARM_DSIER1D		ARM_REGISTER (0xff900074)
#define ARM_DSIER2		ARM_REGISTER (0xff900094)
#define ARM_DSIER3		ARM_REGISTER (0xff9000b4)
#define ARM_DSIER4		ARM_REGISTER (0xff9000d4)
#define ARM_DSIER5		ARM_REGISTER (0xff9000f4)
#define ARM_DSIER6		ARM_REGISTER (0xff900114)
#define ARM_DSIER7		ARM_REGISTER (0xff900134)
#define ARM_DSIER8		ARM_REGISTER (0xff900154)
#define ARM_DSIER9		ARM_REGISTER (0xff900174)
#define ARM_DSIER10		ARM_REGISTER (0xff900194)

#define ARM_DSIER_NCIP		0x80000000	/* normal completion intr */
#define ARM_DSIER_ECIP		0x40000000	/* error completion intr */
#define ARM_DSIER_NRIP		0x20000000	/* buffer not ready intr */
#define ARM_DSIER_CAIP		0x10000000	/* channel abort intr */
#define ARM_DSIER_NCIE		0x00800000	/* normal completion enable */
#define ARM_DSIER_ECIE		0x00400000      /* error completion enable */
#define ARM_DSIER_NRIE		0x00200000      /* buffer not ready enable */
#define ARM_DSIER_CAIE		0x00100000      /* channel abort enable */
#define ARM_DSIER_WRAP		0x00080000
#define ARM_DSIER_IDONE		0x00040000
#define ARM_DSIER_LAST		0x00020000
#define ARM_DSIER_FULL		0x00010000
#define ARM_DSIER_BLEN		0x00007fff	/* remaining byte count */

/*----------------------------------------------
 * Ethernet module
 */
/*
 * Ethernet General Control Register
 */
#define ARM_EGCR		ARM_REGISTER (0xff800000)

#define ARM_EGCR_ERX		0x80000000	/* enable RX FIFO */
#define ARM_EGCR_ERXDMA		0x40000000	/* enable receive DMA */
#define ARM_EGCR_ERXLNG		0x20000000	/* accept long rcv packets */
#define ARM_EGCR_ERXSHT		0x10000000	/* accept short rcv packets */
#define ARM_EGCR_ERXREG		0x08000000	/* enable rcv register irq */
#define ARM_EGCR_ERFIFOH	0x04000000	/* enable rcv FIFO half irq */
#define ARM_EGCR_ERXBR		0x02000000	/* enable rcv buf ready irq */
#define ARM_EGCR_ERXBAD		0x01000000	/* accept bad rcv packets */
#define ARM_EGCR_ETX		0x00800000	/* enable transmit FIFO */
#define ARM_EGCR_ETXDMA		0x00400000	/* enable transmit DMA */

#define ARM_EGCR_ETXWM_MASK	0x00300000	/* transmit FIFO watermark */
#define ARM_EGCR_ETXWM_25	0x00000000	/* 25% FIFO full */
#define ARM_EGCR_ETXWM_50	0x00100000	/* 50% FIFO full */
#define ARM_EGCR_ETXWM_75	0x00200000	/* 75% FIFO full */
#define ARM_EGCR_ETXWM_100	0x00300000	/* 100% FIFO full */

#define ARM_EGCR_ETXREG		0x00080000	/* enable tx register irq */
#define ARM_EGCR_ETFIFOH	0x00040000	/* enable tx FIFO half irq */
#define ARM_EGCR_ETXBC		0x00020000	/* enable tx buf complete irq */
#define ARM_EGCR_EFULLD		0x00010000	/* enable full duplex */

#define ARM_EGCR_MODE_MASK	0x0000c000	/* interface mode */
#define ARM_EGCR_MODE_MII	0x00000000	/* 10/100 Mbit MII */
#define ARM_EGCR_MODE_LEVEL1	0x00008000	/* 10 Mbit level 1 ENDEC */
#define ARM_EGCR_MODE_SEEQ	0x0000c000	/* 10 Mbit SEEQ ENDEC */

#define ARM_EGCR_LB		0x00002000	/* MAC internal loopback mode */
#define ARM_EGCR_RXCINV		0x00001000	/* invert rcv clock input */
#define ARM_EGCR_TXCINV		0x00000800	/* invert xmit clock input */
#define ARM_EGCR_PNA		0x00000400	/* pSOS pNA buffer format */

/* Only for external ENDEC */
#define ARM_EGCR_PDN		0x00000080	/* external PHY power down */

#define ARM_EGCR_CTYPE_MASK	0x00000060	/* AUI/TP selection */
#define ARM_EGCR_CTYPE_AUTO	0x00000000	/* auto select */
#define ARM_EGCR_CTYPE_AUI	0x00000020	/* AUI port */
#define ARM_EGCR_CTYPE_TP	0x00000040	/* TP port */

#define ARM_EGCR_LNK		0x00000010	/* enable link pulse detect */
#define ARM_EGCR_LPBK		0x00000008	/* external loopback mode */
#define ARM_EGCR_UTP		0x00000004	/* UTP wiring */

/*
 * Ethernet General Status Register
 */
#define ARM_EGSR		ARM_REGISTER (0xff800004)

#define ARM_EGSR_RXFDB		0x30000000	/* rx FIFO bytes available */
#define ARM_EGSR_RXREGR		0x08000000	/* rx register ready */
#define ARM_EGSR_RXFIFOH	0x04000000	/* rx FIFO half full */
#define ARM_EGSR_RXBR		0x02000000	/* rx buffer ready */
#define ARM_EGSR_RXSKIP		0x01000000	/* rx buffer skip */
#define ARM_EGSR_TXREGE		0x00080000	/* tx register empty */
#define ARM_EGSR_TXFIFOH	0x00040000	/* tx FIFO half empty */
#define ARM_EGSR_TXBC		0x00020000	/* tx buffer complete */
#define ARM_EGSR_TXFIFOE	0x00010000	/* tx FIFO empty */

/* Only for external ENDEC */
#define ARM_EGSR_FULLD		0x00008000	/* half duplex mode */
#define ARM_EGSR_MANSENSE	0x00002000	/* jumper sense out */
#define ARM_EGSR_CTYPE		0x00001000	/* AUI port */
#define ARM_EGSR_LINKPUL	0x00000800	/* enable link pulse detect */
#define ARM_EGSR_AUTOMAN	0x00000400	/* AUTOMAN jumper out */

/*
 * Ethernet Transmit Status Register
 */
#define ARM_ETSR		ARM_REGISTER (0xff800010)

#define ARM_ETSR_TXOK		0x00008000	/* xmit packet OK */
#define ARM_ETSR_TXBR		0x00004000	/* xmit broadcast packet */
#define ARM_ETSR_TXMC		0x00002000	/* xmit multicast packet */
#define ARM_ETSR_TXAL		0x00001000	/* abort late collision */
#define ARM_ETSR_TXAED		0x00000800	/* abort excessive deferral */
#define ARM_ETSR_TXAEC		0x00000400	/* abort excessive collisions */
#define ARM_ETSR_TXAUR		0x00000200	/* abort underrun */
#define ARM_ETSR_TXAJ		0x00000100	/* abort jumbo */
#define ARM_ETSR_TXDEF		0x00000040	/* xmit packet deferred */
#define ARM_ETSR_TXCRC		0x00000020	/* xmit packet CRC error */
#define ARM_ETSR_TXLCOL		0x00000010	/* packet late collision */
#define ARM_ETSR_TXCOLC		0x0000000f	/* packet collision count */

/*
 * Ethernet Receive Status Register
 */
#define ARM_ERSR		ARM_REGISTER (0xff800014)

#define ARM_ERSR_SIZE		0xffff0000	/* rcv carrier event */
#define ARM_ERSR_RXCE		0x00008000	/* rcv carrier event */
#define ARM_ERSR_RXDV		0x00004000	/* rcv data event */
#define ARM_ERSR_RXOK		0x00002000	/* rcv packet OK */
#define ARM_ERSR_RXBR		0x00001000	/* rcv broadcast packet */
#define ARM_ERSR_RXMC		0x00000800	/* rcv multicast packet */
#define ARM_ERSR_RXCRC		0x00000400	/* rcv CRC error */
#define ARM_ERSR_RXDR		0x00000200	/* rcv dribble nibble */
#define ARM_ERSR_RXCV		0x00000100	/* rcv code violation */
#define ARM_ERSR_RXLNG		0x00000080	/* rcv long packet */
#define ARM_ERSR_RXSHT		0x00000040	/* rcv short packet */
#define ARM_ERSR_ROVER		0x00000020	/* rcv overrun */

/*
 * Ethernet MAC Configuration Register
 */
#define ARM_EMACCFR		ARM_REGISTER (0xff800400)

#define ARM_EMACCFR_SRST	0x00008000	/* MAC soft reset */
#define ARM_EMACCFR_INTLB	0x00004000	/* internal loopback */

#define ARM_EMACCFR_TSTMD_MASK	0x00003000	/* test mode */
#define ARM_EMACCFR_TSTMD_NORM	0x00000000	/* normal operation */
#define ARM_EMACCFR_TSTMD_TXTST	0x00002000	/* xmit test mode */
#define ARM_EMACCFR_TSTMD_RXTST	0x00003000	/* rcv test mode */

#define ARM_EMACCFR_NOBO	0x00000020	/* no backoff */
#define ARM_EMACCFR_RTRYL	0x00000010	/* retry late collision */
#define ARM_EMACCFR_CRCEN	0x00000008	/* append CRC */
#define ARM_EMACCFR_PADEN	0x00000004	/* auto pad enable */
#define ARM_EMACCFR_FULLD	0x00000002	/* full duplex enable */
#define ARM_EMACCFR_HUGEN	0x00000001	/* enable huge frames */

/*
 * Ethernet Physical Coding Sublayer Configuration Register
 */
#define ARM_EPCSCR		ARM_REGISTER (0xff800408)

#define ARM_EPCSCR_SRST		0x00008000	/* soft reset */
#define ARM_EPCSCR_INTLB	0x00004000	/* internal loopback */

#define ARM_EPCSCR_TSTMD_MASK	0x00003000	/* test mode */
#define ARM_EPCSCR_TSTMD_NORM	0x00000000	/* normal operation */
#define ARM_EPCSCR_TSTMD_PE100X	0x00002000	/* PE100X test mode */
#define ARM_EPCSCR_TSTMD_PE10T	0x00003000	/* PE10T test mode */

#define ARM_EPCSCR_EXINT_MASK	0x00000300	/* external interface mode */
#define ARM_EPCSCR_EXINT_MII	0x00000000	/* MII normal operation */
#define ARM_EPCSCR_EXINT_TPPMD	0x00000100	/* TP-PMD mode */
#define ARM_EPCSCR_EXINT_ENDEC	0x00000200	/* 10 Mbit ENDEC mode */

#define ARM_EPCSCR_CLKS_MASK	0x00000004	/* clock select */
#define ARM_EPCSCR_CLKS_25	0x00000000	/* 25 MHz */
#define ARM_EPCSCR_CLKS_44	0x00000004	/* 44 MHz */

#define ARM_EPCSCR_JABBER	0x00000002	/* enable jabber protection */
#define ARM_EPCSCR_NOCFR	0x00000001	/* disable ciphering */

/*
 * Ethernet Station Logic Configuration Register
 *
 */
#define ARM_ESTLCR		ARM_REGISTER (0xff800410)

#define ARM_ESTLCR_SRST		0x00008000	/* soft reset */

#define ARM_ESTLCR_TSTMD_MASK	0x00003000	/* test mode */
#define ARM_ESTLCR_TSTMD_NORM	0x00000000	/* normal operation */
#define ARM_ESTLCR_TSTMD_STATS	0x00002000	/* statistics module test */
#define ARM_ESTLCR_TSTMD_ADDR	0x00003000	/* station address logic test */

#define ARM_ESTLCR_AUTOZ	0x00000004	/* auto zero statistics */
#define ARM_ESTLCR_RXEN		0x00000002	/* receiver enable */
#define ARM_ESTLCR_ITXA		0x00000001	/* insert tx source address */

/*
 * Ethernet Back-to-Back Inter-Packet Gap Timer Register
 */
#define ARM_EB2BIPGTR		ARM_REGISTER (0xff800440)

/*
 * Ethernet Non Back-to-Back Inter-Packet Gap Timer Register
 */
#define ARM_ENB2BIPGTR		ARM_REGISTER (0xff800444)

#define ARM_ENB2BIPGTR_IPGR1_MASK	0x00007f80
#define ARM_ENB2BIPGTR_IPGR1_SHIFT	7
#define ARM_ENB2BIPGTR_IPGR2_MASK	0x0000007f

/*
 * Ethernet Collision Window / Collision Retry Register
 */
#define ARM_ECWCR		ARM_REGISTER (0xff800448)
#define ARM_ECWCR_LCOL_MASK	0x00003f00	/* late collision window */
#define ARM_ECWCR_LCOL_SHIFT	8
#define ARM_ECWCR_RETRY_MASK	0x0000000f	/* max retries on collision */

/*
 * Ethernet MII Command
 */
#define ARM_EMIC		ARM_REGISTER (0xff800540)

#define ARM_EMIC_SCAN		0x00000002	/* auto scan for read data */
#define ARM_EMIC_RSTAT		0x00000001	/* single scan */

/*
 * Ethernet MII Address Register
 */
#define ARM_EMAR		ARM_REGISTER (0xff800544)

/*
 * Ethernet MII Write Data Register
 */
#define ARM_EMWT		ARM_REGISTER (0xff800548)

/*
 * Ethernet MII Read Data Register
 */
#define ARM_EMRD		ARM_REGISTER (0xff80054c)

/*
 * Ethernet MII Indicators Register
 */
#define ARM_EMIR		ARM_REGISTER (0xff800550)

#define ARM_EMIR_SCAN		0x00000002	/* auto scan in progress */
#define ARM_EMIR_BUSY		0x00000001	/* MII interface busy */

/*
 * Error Statistics Counter Registers
 */
#define ARM_ECRCEC		ARM_REGISTER (0xff800580)
#define ARM_EAEC		ARM_REGISTER (0xff800584)
#define ARM_ECEC		ARM_REGISTER (0xff800588)
#define ARM_ELNGFC		ARM_REGISTER (0xff80058c)
#define ARM_ESHRTFC		ARM_REGISTER (0xff800590)
#define ARM_ELCC		ARM_REGISTER (0xff800594)
#define ARM_EEDC		ARM_REGISTER (0xff800598)
#define ARM_EMCC		ARM_REGISTER (0xff80059c)

/*
 * Ethernet Station Address Filter Register
 */
#define ARM_ESAFR		ARM_REGISTER (0xff8005c0)

#define ARM_ESAFR_PRO		0x00000008	/* receive all (promisc) */
#define ARM_ESAFR_PRM		0x00000004	/* accept all multicast */
#define ARM_ESAFR_PRA		0x00000002	/* accept multicast via hash */
#define ARM_ESAFR_BROAD		0x00000001	/* accept all broadcast */

/*
 * Ethernet Station Address Registers
 */
#define ARM_ESA1R		ARM_REGISTER (0xff8005c4)
#define ARM_ESA2R		ARM_REGISTER (0xff8005c8)
#define ARM_ESA3R		ARM_REGISTER (0xff8005cc)

/*
 * Ethernet Multicast Hash Table
 */
#define ARM_EHT1		ARM_REGISTER (0xff8005d0)
#define ARM_EHT2		ARM_REGISTER (0xff8005d4)
#define ARM_EHT3		ARM_REGISTER (0xff8005d8)
#define ARM_EHT4		ARM_REGISTER (0xff8005dc)

/*----------------------------------------------
 * Serial Channel module
 */
/*
 * Serial Channel Control Register A
 */
#define ARM_SCRA(n)		ARM_REGISTER ((n) ? 0xffd00040 : 0xffd00000)

#define ARM_SCRA_CE		0x80000000	/* channel enable */
#define ARM_SCRA_BRK		0x40000000	/* UART send break */
#define ARM_SCRA_STICKP		0x20000000	/* UART stick parity */
#define ARM_SCRA_EPS		0x10000000	/* UART even parity select */
#define ARM_SCRA_PE		0x08000000	/* UART parity enable */

#define ARM_SCRA_STOP_MASK	0x04000000	/* UART stop bits */
#define ARM_SCRA_STOP_1		0x00000000	/* 1 stop bit */
#define ARM_SCRA_STOP_2		0x04000000	/* 2 stop bits */

#define ARM_SCRA_WLS_MASK	0x03000000	/* UART word length select */
#define ARM_SCRA_WLS_5		0x00000000	/* 5 bits */
#define ARM_SCRA_WLS_6		0x01000000	/* 6 bits */
#define ARM_SCRA_WLS_7		0x02000000	/* 7 bits */
#define ARM_SCRA_WLS_8		0x03000000	/* 8 bits */

#define ARM_SCRA_CTSTX		0x00800000	/* CTS transmit handshake */
#define ARM_SCRA_RTSRX		0x00400000	/* RTS receive handshake */
#define ARM_SCRA_RL		0x00200000	/* remote loopback */
#define ARM_SCRA_LL		0x00100000	/* local loopback */
#define ARM_SCRA_OUT2		0x00080000	/* OUT2 extra signal */
#define ARM_SCRA_OUT1		0x00040000	/* OUT1 extra signal */
#define ARM_SCRA_DTR		0x00020000	/* DTR control */
#define ARM_SCRA_RTS		0x00010000	/* RTS control */

/* Receiver interrupt enable bits */
#define ARM_SCRA_ERXBRK		0x00008000	/* break */
#define ARM_SCRA_ERXFE		0x00004000	/* framing error */
#define ARM_SCRA_ERXPE		0x00002000	/* parity error */
#define ARM_SCRA_ERXORUN	0x00001000	/* overrun */
#define ARM_SCRA_ERXDRDY	0x00000800	/* register ready */
#define ARM_SCRA_ERXHALF	0x00000400	/* FIFO half-full */
#define ARM_SCRA_ERXBC		0x00000200	/* buffer closed */
#define ARM_SCRA_ERXDMA		0x00000100	/* enable rx DMA */
#define ARM_SCRA_ERXDCD		0x00000080	/* change in DCD */
#define ARM_SCRA_ERXRI		0x00000040	/* change in RI */
#define ARM_SCRA_ERXDSR		0x00000020	/* change in DSR */

/* Transmitter interrupt enable bits */
#define ARM_SCRA_ETXCTS		0x00000010	/* change in CTS */
#define ARM_SCRA_ETXOBE		0x00000008	/* register empty */
#define ARM_SCRA_ETXHALF	0x00000004	/* FIFO half empty */
#define ARM_SCRA_ETXBC		0x00000002	/* buffer closed */
#define ARM_SCRA_ETXDMA		0x00000001	/* enable tx DMA */

/*
 * Serial Channel Control Register B
 */
#define ARM_SCRB(n)		ARM_REGISTER ((n) ? 0xffd00044 : 0xffd00004)

#define ARM_SCRB_RDM1		0x80000000	/* receive data match */
#define ARM_SCRB_RDM2		0x40000000
#define ARM_SCRB_RDM3		0x20000000
#define ARM_SCRB_RDM4		0x10000000
#define ARM_SCRB_RBGT		0x08000000	/* receive buffer gap timer */
#define ARM_SCRB_RCGT		0x04000000	/* receive char gap timer */

#define ARM_SCRB_MODE_MASK	0x00300000	/* SCC mode */
#define ARM_SCRB_MODE_UART	0x00000000	/* UART */
#define ARM_SCRB_MODE_HDLC	0x00100000	/* HDLC */
#define ARM_SCRB_MODE_SPI_M	0x00200000	/* SPI Master */
#define ARM_SCRB_MODE_SPI_S	0x00300000	/* SPI Slave */

#define ARM_SCRB_BITORDR_MSB	0x00080000	/* MSB bit ordering */
#define ARM_SCRB_MAM1		0x00040000	/* match address mode 1 */
#define ARM_SCRB_MAM2		0x00020000	/* match address mode 2 */

#define ARM_SCRB_TENC_MASK	0x00000e00	/* transmit data encoding */
#define ARM_SCRB_TENC_NRZ	0x00000000	/* NRZ */
#define ARM_SCRB_TENC_NRZB	0x00000200	/* NRZ inverted */
#define ARM_SCRB_TENC_NRZI_M	0x00000400	/* NRZI Mark */
#define ARM_SCRB_TENC_NRZI_S	0x00000600	/* NRZI Space */
#define ARM_SCRB_TENC_FM0	0x00000800	/* FM0 */
#define ARM_SCRB_TENC_FM1	0x00000a00	/* FM1 */
#define ARM_SCRB_TENC_MANCH	0x00000c00	/* Manchester */
#define ARM_SCRB_TENC_DMANCH	0x00000e00	/* Differential Manchester */

#define ARM_SCRB_RDEC_MASK	0x000001c0	/* receive data encoding */
#define ARM_SCRB_RDEC_NRZ	0x00000000	/* NRZ */
#define ARM_SCRB_RDEC_NRZB	0x00000040	/* NRZ inverted */
#define ARM_SCRB_RDEC_NRZI_M	0x00000080	/* NRZI Mark */
#define ARM_SCRB_RDEC_NRZI_S	0x000000c0	/* NRZI Space */
#define ARM_SCRB_RDEC_FM0	0x00000100	/* FM0 */
#define ARM_SCRB_RDEC_FM1	0x00000140	/* FM1 */
#define ARM_SCRB_RDEC_MANCH	0x00000180	/* Manchester */
#define ARM_SCRB_RDEC_DMANCH	0x000001c0	/* Differential Manchester */

#define ARM_SCRB_TPL_MASK	0x00000018	/* transmit preamble length */
#define ARM_SCRB_TPL_0		0x00000000	/* no preamble */
#define ARM_SCRB_TPL_8		0x00000008	/* 8 bits */
#define ARM_SCRB_TPL_16		0x00000010	/* 16 bits */
#define ARM_SCRB_TPL_32		0x00000018	/* 32 bits */

#define ARM_SCRB_TEND		0x00000004	/* tx frame ending */

#define ARM_SCRB_TPP_MASK	0x00000003	/* tx preamble pattern */
#define ARM_SCRB_TPP_0		0x00000000	/* all zeroes */
#define ARM_SCRB_TPP_10		0x00000001	/* repeating 10 */
#define ARM_SCRB_TPP_01		0x00000002	/* repeating 01 */
#define ARM_SCRB_TPP_1		0x00000003	/* all ones */

/*
 * Serial Channel Control Register C (HDLC)
 */
#define ARM_SCRC(n)		ARM_REGISTER ((n) ? 0xffd00064 : 0xffd00024)

#define ARM_SCRC_NFLAG_MASK	0xf0000000	/* number of flags */
#define ARM_SCRC_NFLAG_SHIFT	28

#define ARM_SCRC_CRC_MASK	0x00c00000	/* CRC mode */
#define ARM_SCRC_CRC_NOCRC	0x00000000	/* no CRC */
#define ARM_SCRC_CRC_CCITT_16	0x00800000	/* 16-bit CCITT CRC-16 */
#define ARM_SCRC_CRC_CCITT_32	0x00c00000	/* 32-bit CCITT CRC-32 */

#define ARM_SCRC_RXCRC		0x00200000	/* place rx CRC in data buf */
#define ARM_SCRC_CHKCRC		0x00100000	/* accept bad frames */
#define ARM_SCRC_TXCRC		0x00020000	/* do not send CRC */
#define ARM_SCRC_FLG_IDL	0x00010000	/* send FF between frames */

/*
 * Serial Channel Status Register A
 */
#define ARM_SSRA(n)		ARM_REGISTER ((n) ? 0xffd00048 : 0xffd00008)

#define ARM_SSRA_MATCH1		0x80000000	/* character match */
#define ARM_SSRA_MATCH2		0x40000000
#define ARM_SSRA_MATCH3		0x20000000
#define ARM_SSRA_MATCH4		0x10000000

#define ARM_SSRA_BGAP		0x08000000	/* buffer gap timer */
#define ARM_SSRA_CGAP		0x04000000	/* char gap timer */
#define ARM_SSRA_RXFDB		0x00300000	/* rx FIFO data available */

#define ARM_SSRA_DCD		0x00080000	/* DCD signal */
#define ARM_SSRA_RI		0x00040000	/* RI signal */
#define ARM_SSRA_DSR		0x00020000	/* DSR signal */
#define ARM_SSRA_CTS		0x00010000	/* CTS signal */

#define ARM_SSRA_RXBRK		0x00008000	/* UART break pending */
#define ARM_SSRA_RXFE		0x00004000	/* framing error */
#define ARM_SSRA_RXPE		0x00002000	/* parity error */
#define ARM_SSRA_RXOVER		0x00001000	/* overrun */

#define ARM_SSRA_RXRDY		0x00000800	/* rx register ready */
#define ARM_SSRA_RXHALF		0x00000400	/* rx FIFO half full */
#define ARM_SSRA_RXBC		0x00000200	/* rx buffer closed */
#define ARM_SSRA_RXFULL		0x00000100	/* rx FIFO full */

#define ARM_SSRA_RXDCDI		0x00000080	/* DCD signal change */
#define ARM_SSRA_RXRII		0x00000040	/* RI signal change */
#define ARM_SSRA_RXDSRI		0x00000020	/* DSR signal change */
#define ARM_SSRA_TXCTSI		0x00000010	/* CTS signal change */

#define ARM_SSRA_TXRDY		0x00000008	/* tx register empty */
#define ARM_SSRA_TXHALF		0x00000004	/* tx FIFO half empty */
#define ARM_SSRA_TXBC		0x00000002	/* tx buffer closed */
#define ARM_SSRA_TXEMPTY	0x00000001	/* tx FIFO empty */

/*
 * Serial Channel Status Register B (HDLC)
 */
#define ARM_SSRB(n)		ARM_REGISTER ((n) ? 0xffd00068 : 0xffd00028)

#define ARM_SSRB_FLAGS		0x80000000	/* receiving flags */
#define ARM_SSRB_IDLE		0x40000000	/* receiving FF */
#define ARM_SSRB_RXBCAM1	0x08000000	/* buf closed on addr match 1 */
#define ARM_SSRB_RXBCAM2	0x04000000	/* buf closed on addr match 2 */
#define ARM_SSRB_RXBCAM3	0x02000000	/* buf closed on addr match 3 */
#define ARM_SSRB_RXBCAM4	0x01000000	/* buf closed on addr match 4 */
#define ARM_SSRB_RXBCN		0x00800000	/* buf closed normal */
#define ARM_SSRB_RXBCC		0x00400000	/* buf closed on CRC error */
#define ARM_SSRB_RXBCO		0x00200000	/* buf closed on overrun */
#define ARM_SSRB_RXBCA		0x00100000	/* buf closed on align err */
#define ARM_SSRB_RXBCL		0x00080000	/* buf closed on length error */
#define ARM_SSRB_RXBCAB		0x00040000	/* buf closed on abort */
#define ARM_SSRB_TXBCC		0x00008000	/* buf closed on CTS loss */
#define ARM_SSRB_TXBCU		0x00004000	/* buf closed on underrun */

/*
 * Serial Channel Status Register C (HDLC)
 */
#define ARM_SSRC(n)		ARM_REGISTER ((n) ? 0xffd0006c : 0xffd0002c)

#define ARM_SSRC_IEEF		0x80000000	/* EFDF irq enable */
#define ARM_SSRC_EFDF		0x001f0000	/* err-free discarded frames */
#define ARM_SSRC_IECE		0x80008000	/* CRCDF irq enable */
#define ARM_SSRC_CRCDF		0x8000000f	/* CRC-error discarded frames */

/*
 * Serial Channel Bit-Rate Generator
 */
#define ARM_SBRG(n)		ARM_REGISTER ((n) ? 0xffd0004c : 0xffd0000c)

#define ARM_SBRG_EBIT		0x80000000	/* bit-rate generator enable */
#define ARM_SBRG_TMODE		0x40000000	/* x1 timing mode */
#define ARM_SBRG_RXSRC		0x20000000	/* ext rx clock source */
#define ARM_SBRG_TXSRC		0x10000000	/* ext tx clock source */

#define ARM_SBRG_RXEXT		0x08000000	/* drive rx ext clock */
#define ARM_SBRG_TXEXT		0x04000000	/* drive tx ext clock */

#define ARM_SBRG_CLKMUX_MASK	0x03000000	/* BRG input clock */
#define ARM_SBRG_CLKMUX_FXTAL	0x00000000	/* Fxtal */
#define ARM_SBRG_CLKMUX_FSYSCLK	0x01000000	/* Fsysclk */
#define ARM_SBRG_CLKMUX_A4_B4	0x02000000	/* PORTA4/B4 */
#define ARM_SBRG_CLKMUX_C5_C7	0x03000000	/* PORTC5/C7 */

#define ARM_SBRG_TXCINV		0x00800000	/* tx clock invert */
#define ARM_SBRG_RXCINV		0x00400000	/* rx clock invert */
#define ARM_SBRG_CLKINV		0x00200000	/* clock invert */

#define ARM_SBRG_TCDR_MASK	0x00180000	/* tx divide clock rate */
#define ARM_SBRG_TCDR_1		0x00000000	/* 1x (only NRZ, NRZI) */
#define ARM_SBRG_TCDR_8		0x00080000	/* 8x clock mode */
#define ARM_SBRG_TCDR_16	0x00100000	/* 16x clock mode */
#define ARM_SBRG_TCDR_32	0x00180000	/* 32x clock mode */

#define ARM_SBRG_RCDR		0x00030000	/* rx divide clock rate */
#define ARM_SBRG_RCDR_1		0x00000000	/* 1x (only NRZ, NRZI) */
#define ARM_SBRG_RCDR_8		0x00010000	/* 8x clock mode */
#define ARM_SBRG_RCDR_16	0x00020000	/* 16x clock mode */
#define ARM_SBRG_RCDR_32	0x00030000	/* 32x clock mode */

#define ARM_SBRG_TICS_MASK	0x00004000	/* tx internal clock source */
#define ARM_SBRG_TICS_BRG	0x00000000	/* BRG */
#define ARM_SBRG_TICS_DPLL	0x00004000	/* DPLL */

#define ARM_SBRG_RICS_MASK	0x00001000	/* tx internal clock source */
#define ARM_SBRG_RICS_BRG	0x00000000	/* BRG */
#define ARM_SBRG_RICS_DPLL	0x00001000	/* DPLL */

#define ARM_SBRG_NREG		0x000007ff	/* BRG frequency divider */

/*
 * Serial Channel FIFO
 */
#define ARM_SFIFO(n)		ARM_REGISTER ((n) ? 0xffd00050 : 0xffd00010)

/*
 * Serial Channel FIFO Data Register Last (HDLC)
 */
#define ARM_SFIFO_LAST(n)	ARM_REGISTER ((n) ? 0xffd00070 : 0xffd00030)

/*
 * Serial Channel Receive Buffer Timer
 */
#define ARM_SRBT(n)		ARM_REGISTER ((n) ? 0xffd00054 : 0xffd00014)

#define ARM_SRBT_TRUN		0x80000000	/* enable buffer timer to run */
#define ARM_SRBT_BT		0x00007fff	/* buffer timer */

/*
 * Serial Channel Receive Character Timer
 */
#define ARM_SRCT(n)		ARM_REGISTER ((n) ? 0xffd00058 : 0xffd00018)

#define ARM_SRCT_TRUN		0x80000000	/* enable char timer to run */
#define ARM_SRCT_CT		0x000003ff	/* character timer */

/*
 * Serial Channel Receive Match and Mask Register
 */
#define ARM_SRMR(n)		ARM_REGISTER ((n) ? 0xffd0005c : 0xffd0001c)
#define ARM_SRMMR(n)		ARM_REGISTER ((n) ? 0xffd00060 : 0xffd00020)

/*----------------------------------------------
 * MIC module
 */
/*
 * General Control Register
 */
#define ARM_GCR			ARM_REGISTER (0xffa00000)

#define ARM_GCR_ERX		0x80000000	/* enable receive FIFO */
#define ARM_GCR_ERXDMA		0x40000000	/* enable receive DMA */
#define ARM_GCR_ERXREG		0x08000000	/* enable rx data ready irq */
#define ARM_GCR_ERFIFOH		0x04000000	/* en. rx FIFO half full irq */
#define ARM_GCR_EBEI		0x01000000	/* enable ENI bus error irq */
#define ARM_GCR_ETX		0x00800000	/* enable transmit FIFO */
#define ARM_GCR_ETXDMA		0x00400000	/* enable transmit DMA */
#define ARM_GCR_ETXREG		0x00080000	/* enable tx data ready irq */
#define ARM_GCR_ETFIFOH		0x00040000	/* en. tx FIFO half empty irq */

#define ARM_GCR_BTE_MASK	0x0000c000	/* burst transfer enable */
#define ARM_GCR_BTE_NOBURST	0x00000000	/* no burst pre-fetching */
#define ARM_GCR_BTE_8_BYTES	0x00004000	/* 8 bytes */
#define ARM_GCR_BTE_16_BYTES	0x00008000	/* 16 bytes */

#define ARM_GCR_MICDIAG		0x00001000	/* enable firmware change */

#define ARM_GCR_MICMODE_MASK	0x00000007	/* mode of operation */
#define ARM_GCR_MICMODE_GPIO	0x00000000	/* GPIO */
#define ARM_GCR_MICMODE_1284	0x00000001	/* IEEE 1284 host mode */
#define ARM_GCR_MICMODE_RAM16	0x00000004	/* 16-bit shared RAM */
#define ARM_GCR_MICMODE_RAM8	0x00000005	/* 8-bit shared RAM */
#define ARM_GCR_MICMODE_FIFO16	0x00000006	/* 16-bit FIFO */
#define ARM_GCR_MICMODE_FIFO8	0x00000007	/* 8-bit FIFO */

/*
 * General Status Register
 */
#define ARM_GSR			ARM_REGISTER (0xffa00004)

#define ARM_GSR_RXFDB		0x30000000	/* rx FIFO data available */
#define ARM_GSR_RXREGR		0x08000000	/* rx FIFO register ready */
#define ARM_GSR_RXFIFOH		0x04000000	/* rx FIFO half full */
#define ARM_GSR_BEI		0x01000000	/* ENI bus error */
#define ARM_GSR_TXREGE		0x00080000	/* tx FIFO register empty */
#define ARM_GSR_TXFIFOH		0x00040000	/* tx FIFO half empty */
#define ARM_GSR_TXFIFOE		0x00010000	/* tx FIFO empty */

/*
 * ENI Control Register
 */
#define ARM_ECR			ARM_REGISTER (0xffa00030)

#define ARM_ECR_SYNC		0x00030000	/* /PACK output sync mode */
#define ARM_ECR_WAIT		0x0000e000	/* added wait states */
#define ARM_ECR_DMAE2		0x00001000	/* enable DMA req on PINT2 */
#define ARM_ECR_FAST		0x00000800	/* fast ENI timing */
#define ARM_ECR_DPACK		0x00000400	/* disable /PACK during DMA */
#define ARM_ECR_PSIO		0x00000200	/* PSIO mode */
#define ARM_ECR_KYOINT		0x00000100	/* Kyocera interrupt option */
#define ARM_ECR_INTP		0x00000080	/* interrupt pending */
#define ARM_ECR_WR_OC		0x00000040	/* wait/ready open collector */
#define ARM_ECR_DINT2		0x00000020	/* disable /INT2 pin */
#define ARM_ECR_I_OC		0x00000010	/* interrupt open collector */
#define ARM_ECR_DMAE		0x00000008	/* ENI DMA signals enable */
#define ARM_ECR_IRQEN		0x00000004	/* enable ENI irq */
#define ARM_ECR_EPACK		0x00000002	/* enable ENI ACK pulse */
#define ARM_ECR_PULINT		0x00000001	/* pulsed interrupt enable */

/*
 * ENI Shared RAM Address Register
 */
#define ARM_ESRAR		ARM_REGISTER (0xffa00038)

#define ARM_ESRAR_BASE_SHIFT	12
#define ARM_ESRAR_ENABLE	0x00000100	/* shared RAM enable */
#define ARM_ESRAR_RAMSIZE	0x0000000f	/* shared RAM size */
#define ARM_ESRAR_RAMSIZE_64K	0x00000000      /* 64 kbytes */
#define ARM_ESRAR_RAMSIZE_32K	0x00000008      /* 32 kbytes */
#define ARM_ESRAR_RAMSIZE_16K	0x0000000c      /* 16 kbytes */
#define ARM_ESRAR_RAMSIZE_8K	0x0000000e      /* 8 kbytes */
#define ARM_ESRAR_RAMSIZE_4K	0x0000000f      /* 4 kbytes */

/*
 * ENI Shared Register
 */
#define ARM_ESHR		ARM_REGISTER (0xffa0003C)
/*#define ARM_ESHR_OFFSET	0x10000 */
#define ARM_ESHR_OFFSET		0x40000

#define ARM_ESHR_RSTIO		0x00000080	/* reset processor */
#define ARM_ESHR_INTIOF		0x00000040	/* interrupt processor */
#define ARM_ESHR_EMMINT		0x00000020	/* spare resource bit */
#define ARM_ESHR_EHWINT		0x00000010	/* enable hardware interrupt */
#define ARM_ESHR_SINTP2		0x00000008	/* select PINT1/2 */
#define ARM_ESHR_VDAINT		0x00000004	/* interrupt to external */
#define ARM_ESHR_STSINT		0x00000002	/* interrupt to ENI */
#define ARM_ESHR_CLRINT		0x00000001	/* clear interrupt */

/*
 * FIFO Mode Data Register
 * Address of shared RAM + ARM_FMDR_OFFSET
 */
#define ARM_FMDR		ARM_REGISTER (0xffa00008)
/*#define ARM_FMDR_OFFSET	0x10004 */
#define ARM_FMDR_OFFSET		0x40004

/*
 * FIFO Mode Mask / Status Register
 */
/* #define ARM_FMSR_OFFSET	0x10008 */
#define ARM_FMSR_OFFSET		0x40008

#define ARM_FMSR_RDBUFRDY	0x00002000	/* read buffer ready */
#define ARM_FMSR_WRBUFEMP	0x00001000	/* write buffer empty */
#define ARM_FMSR_EDRDBUFRDY	0x00000800	/* enable DMA rd buf ready */
#define ARM_FMSR_EDWRBUFEMP	0x00000400	/* enable DMA wr buf empty */
#define ARM_FMSR_EIRDBUFRDY	0x00000200	/* enable irq on rd buf ready */
#define ARM_FMSR_EIWRBUFEMP	0x00000100	/* enable irq on wr buf empty */
#define ARM_FMSR_OWM_MASK	0x000000c0	/* outbound watermark */
#define ARM_FMSR_IWM_MASK	0x00000030	/* inbound watermark */

/* End of Netsilicon Net+50 register definitions.
 *----------------------------------------------*/
