///////////////////////////////////////////////////////////////
//                 Cortex-A Generic Registers
///////////////////////////////////////////////////////////////


///////////////////
// Private Timer //
///////////////////
#define ARM_PRT_LOAD            OMAP_R(ARM_CORTEX_REG_BASE+0x0600)
#define ARM_PRT_COUNTER         OMAP_R(ARM_CORTEX_REG_BASE+0x0604)
#define ARM_PRT_CONTROL         OMAP_R(ARM_CORTEX_REG_BASE+0x0608)
#define ARM_PRT_INT_STATUS      OMAP_R(ARM_CORTEX_REG_BASE+0x060C)

// ARM_PRT_CONTROL
#define ARM_PRT_TIMER_EN        (1 << 0)
#define ARM_PRT_AUTO_RELOAD     (1 << 1)
#define ARM_PRT_IRQ_EN          (1 << 2)
#define ARM_PRT_PRESCALER(x)    ((x) << 8)

// ARM_PRT_INT_STATUS
#define ARM_PRT_EVENT           (1 << 0)


////////////////////
// Watchdog Timer //
////////////////////
#define ARM_WDT_LOAD            OMAP_R(ARM_CORTEX_REG_BASE+0x0620)
#define ARM_WDT_COUNTER         OMAP_R(ARM_CORTEX_REG_BASE+0x0624)
#define ARM_WDT_CONTROL         OMAP_R(ARM_CORTEX_REG_BASE+0x0628)
#define ARM_WDT_INT_STATUS      OMAP_R(ARM_CORTEX_REG_BASE+0x062C)
#define ARM_WDT_RST_STATUS      OMAP_R(ARM_CORTEX_REG_BASE+0x0630)
#define ARM_WDT_DISABLE         OMAP_R(ARM_CORTEX_REG_BASE+0x0634)

// ARM_WDT_CONTROL
#define ARM_WDT_EN              (1 << 0)
#define ARM_WDT_AUTO_RELOAD     (1 << 1)
#define ARM_WDT_IT_EN           (1 << 2)
#define ARM_WDT_TIM_MODE        (0 << 3)
#define ARM_WDT_WDT_MODE        (1 << 3)
#define ARM_WDT_PRESCALER(x)    ((x) << 8)

// ARM_WDT_INT_STATUS
#define ARM_WDT_EVENT           (1 << 0)

// ARM_WDT_RST_STATUS
#define ARM_WDT_RESET           (1 << 0)


/////////////////////
// GIC Distributor //
/////////////////////
#define ARM_ICDDCR              OMAP_R(ARM_CORTEX_REG_BASE+0x1000)
#define ARM_ICDICTR             OMAP_R(ARM_CORTEX_REG_BASE+0x1004)
#define ARM_ICDIIDR             OMAP_R(ARM_CORTEX_REG_BASE+0x1008)
#define ARM_ICDISR(n)           OMAP_R(ARM_CORTEX_REG_BASE+0x1080+(n)*4)
#define ARM_ICDISER(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1100+(n)*4)
#define ARM_ICDICER(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1180+(n)*4)
#define ARM_ICDISPR(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1200+(n)*4)
#define ARM_ICDICPR(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1280+(n)*4)
#define ARM_ICDABR(n)           OMAP_R(ARM_CORTEX_REG_BASE+0x1300+(n)*4)
#define ARM_ICDIPR(n)           OMAP_R(ARM_CORTEX_REG_BASE+0x1400+(n)*4)
#define ARM_ICDIPTR(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1800+(n)*4)
#define ARM_ICDICFR(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1C00+(n)*4)
#define ARM_ICPPISR             OMAP_R(ARM_CORTEX_REG_BASE+0x1D00)
#define ARM_ICSPISR(n)          OMAP_R(ARM_CORTEX_REG_BASE+0x1D04+(n)*4)
#define ARM_ICDSGIR             OMAP_R(ARM_CORTEX_REG_BASE+0x1F00)
#define ARM_ICPIDR(n)           OMAP_R(ARM_CORTEX_REG_BASE+0x1FD0+(n)*4)
#define ARM_ICCIDR(n)           OMAP_R(ARM_CORTEX_REG_BASE+0x1FF0+(n)*4)

// ARM_ICDICTR
#define ARM_ICDICTR_IT_LINES(x) ((x) & 0x1F)
#define ARM_ICDICTR_CPU_NB(x)   (((x) & 0x7) << 5)
#define ARM_ICDICTR_SECUR_EXT   (1 << 10)
#define ARM_ICDICTR_LSPI(x)     (((x) & 0x1F) << 11)

// ARM_ICDIIDR
#define ARM_ICDIIDR_IMPLEMENTER(x)      ((x) & 0xFFF)
#define ARM_ICDIIDR_REVISION(x)         (((x) & 0xFFF) << 12)
#define ARM_ICDIIDR_IMPL_VERSION(x)     (((x) & 0xFF) << 24)

// ARM_ICPPISR
#define ARM_ICPPISR_PPI(n)              (1 << (11 + (n)))


///////////////////////
// GIC CPU Interface //
///////////////////////
#define ARM_ICCICR              OMAP_R(ARM_CORTEX_REG_BASE+0x0100)
#define ARM_ICCPMR              OMAP_R(ARM_CORTEX_REG_BASE+0x0104)
#define ARM_ICCBPR              OMAP_R(ARM_CORTEX_REG_BASE+0x0108)
#define ARM_ICCIAR              OMAP_R(ARM_CORTEX_REG_BASE+0x010C)
#define ARM_ICCEOIR             OMAP_R(ARM_CORTEX_REG_BASE+0x0110)
#define ARM_ICCRPR              OMAP_R(ARM_CORTEX_REG_BASE+0x0114)
#define ARM_ICCHPIR             OMAP_R(ARM_CORTEX_REG_BASE+0x0118)
#define ARM_ICCABPR             OMAP_R(ARM_CORTEX_REG_BASE+0x011C)
#define ARM_ICCIDR_IMPL         OMAP_R(ARM_CORTEX_REG_BASE+0x01FC)

// ARM_ICCIDR
#define ARM_ICCIDR_IMPLEMENTER(x)       ((x) & 0xFFF)
#define ARM_ICCIDR_REVISION(x)          (((x) & 0xF) << 12)
#define ARM_ICCIDR_ARCHITECTURE(x)      (((x) & 0xF) << 16)
#define ARM_ICCIDR_PART_NUMBER(x)       (((x) & 0xFFF) << 20)

