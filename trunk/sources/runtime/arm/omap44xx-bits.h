// Fields

// 
// !!! PLEASE NOTE
// that not all flags are applicable to all registers! Refer to
// datasheet.
//
// Registers with only one field starting from bit 0 are
// often left undescribed.
//
// In most cases names of fields are given the same as in the 
// datasheet. But in some cases they are
// 1) generalized for several registers in order to shorten 
//    the description (and this file also).
// 2) mangled in order to solve name conflicts because a few fields
//    are named identically in the datasheet.
//

// PRM_IRQSTATUS_MPU_A9
// PRM_IRQENABLE_MPU_A9
// PRM_IRQSTATUS_MPU_M3
// PRM_IRQENABLE_MPU_M3
// PRM_IRQSTATUS_DSP
// PRM_IRQENABLE_DSP
#define DPLL_CORE_RECAL                 (1 << 0)
#define DPLL_MPU_RECAL                  (1 << 1)
#define DPLL_IVA_RECAL                  (1 << 2)
#define DPLL_PER_RECAL                  (1 << 3)
#define DPLL_ABE_RECAL                  (1 << 4)
#define TRANSITION                      (1 << 8)
#define IO                              (1 << 9)
#define FORCEWKUP_ST                    (1 << 10)
#define VC_SAERR                        (1 << 11)
#define VC_RAERR                        (1 << 12)
#define VC_TOERR                        (1 << 13)
#define VC_BYPASSACK                    (1 << 14)
#define VP_CORE_OPPCHANGEDONE           (1 << 16)
#define VP_CORE_MINVDD                  (1 << 17)
#define VP_CORE_MAXVDD                  (1 << 18)
#define VP_CORE_NOSMPSACK               (1 << 19)
#define VP_CORE_EQVALUE                 (1 << 20)
#define VP_CORE_TRANXDONE               (1 << 21)
#define VC_CORE_VPACK                   (1 << 22)
#define VP_IVA_OPPCHANGEDONE            (1 << 24)
#define VP_IVA_MINVDD                   (1 << 25)
#define VP_IVA_MAXVDD                   (1 << 26)
#define VP_IVA_NOSMPSACK                (1 << 27)
#define VP_IVA_EQVALUE                  (1 << 28)
#define VP_IVA_TRANXDONE                (1 << 29)
#define VC_IVA_VPACK                    (1 << 30)
#define ABB_IVA_DONE                    (1 << 31)

// PRM_IRQSTATUS_MPU_A9_2
// PRM_IRQENABLE_MPU_A9_2
#define VP_MPU_OPPCHANGEDONE            (1 << 0)
#define VP_MPU_MINVDD                   (1 << 1)
#define VP_MPU_MAXVDD                   (1 << 2)
#define VP_MPU_NOSMPSACK                (1 << 3)
#define VP_MPU_EQVALUE                  (1 << 4)
#define VP_MPU_TRANXDONE                (1 << 5)
#define VC_MPU_VPACK                    (1 << 6)
#define ABB_MPU_DONE                    (1 << 7)

// *_CLKCTRL
#define MODULEMODE(x)                   ((x) & 0x3)
#define SAR_MODE                        (1 << 4)
#define OPTFCLKEN_DBCLK                 (1 << 8)
#define OPTFCLKEN_BGAP_32K              (1 << 8)
#define OPTFCLKEN_FCLK0                 (1 << 8)
#define OPTFCLKEN_CLK32K                (1 << 8)
#define OPTFCLKEN_DLL_CLK               (1 << 8)
#define OPTFCLKEN_CTRLCLK               (1 << 8)
#define OPTFCLKEN_DSSCLK                (1 << 8)
#define OPTFCLKEN_UTMI_P1_CLK           (1 << 8)
#define OPTFCLKEN_XCLK                  (1 << 8)
#define OPTFCLKEN_USB_CH0_CLK           (1 << 8)
#define OPTFCLKEN_PHY_48M               (1 << 8)
#define OPTFCLKEN_DBCLK                 (1 << 8)
#define OPTFCLKEN_PER24MC_FCLK          (1 << 8)
#define OPTFCLKEN_FCLK1                 (1 << 9)
#define OPTFCLKEN_48MHZ_CLK             (1 << 9)
#define OPTFCLKEN_UTMI_P2_CLK           (1 << 9)
#define OPTFCLKEN_USB_CH1_CLK           (1 << 9)
#define OPTFCLKEN_PERABE24M_FCLK        (1 << 9)
#define OPTFCLKEN_FCLK2                 (1 << 10)
#define OPTFCLKEN_SYS_CLK               (1 << 10)
#define OPTFCLKEN_UTMI_P3_CLK           (1 << 10)
#define OPTFCLKEN_ABE_SLIMBUS_CLK       (1 << 10)
#define OPTFCLKEN_L4PER_SLIMBUS_CLK     (1 << 11)
#define OPTFCLKEN_TV_FCLK               (1 << 11)
#define OPTFCLKEN_HSIC60M_P1_CLK        (1 << 11)
#define OPTFCLKEN_HSIC60M_P2_CLK        (1 << 12)
#define OPTFCLKEN_HSIC480M_P1_CLK       (1 << 13)
#define OPTFCLKEN_HSIC480M_P2_CLK       (1 << 14)
#define OPTFCLKEN_FUNC48MCLK            (1 << 15)
#define IDLEST(x)                       (((x) & 0x3) << 16)
#define GET_IDLEST(reg)                 (((reg) >> 16) & 0x3)
#define STBYST                          (1 << 18)
#define PMD_STM_MUX_CTRL(x)             (((x) & 0x3) << 20)
#define PMD_TRACE_MUX_CTRL(x)           (((x) & 0x3) << 22)
#define CLKSEL_PMD_TRACE_CLK(x)         (((x) & 0x7) << 24)
#define CLKSEL_SOURCE(x)                (((x) & 0x3) << 24)
#define CLKSEL_FCLK(x)                  (((x) & 0x3) << 24)
#define CLKSEL_HSI(x)                   (((x) & 0x3) << 24)
#define CLKSEL_AESS_FCLK                (1 << 24)
#define CLKSEL_GPTIMER                  (1 << 24)
#define CLKSEL_HSMMC                    (1 << 24)
#define CLKSEL_SGX_FCLK                 (1 << 24)
#define CLKSEL_UTMI_P1                  (1 << 24)
#define CLKSEL_60M                      (1 << 24)
#define CLKSEL_SOURCE_MCBSP             (1 << 24)
#define CLKSEL_UTMI_P2                  (1 << 25)
#define CLKSEL_INTERNAL_SOURCE_MCBSP    (1 << 25)
#define CLKSEL_INTERNAL_SOURCE(x)       (((x) & 0x3) << 26)
#define CLKSEL_PMD_STM_CLK(x)           (((x) & 0x7) << 27)

// *_DEBUG_CFG
#define SEL0(x)                         ((x) & 0x7F)
#define SEL1(x)                         (((x) & 0x7F) << 8)
#define SEL2(x)                         (((x) & 0x7F) << 16)
#define SEL3(x)                         (((x) & 0x7F) << 24)

// CM_ABE_DSS_SYS_CLKSEL
// CM_L4_WKUP_CLKSEL
// CM_ABE_PLL_REF_CLKSEL
// CM_CLKSEL_MPU_M3_ISS_ROOT
// CM_CLKSEL_USB_60MHZ
#define CLKSEL                          (1 << 0)

// CM_SYS_CLKSEL
#define SYS_CLKSEL(x)                   ((x) & 0x7)

// CM_SCALE_FCLK
#define SCALE_FCLK                      (1 << 0)

// CM_*_DVFS_PERF*
#define PERF_REQ(x)                     ((x) & 0xFF)

// CM_*_DVFS_CURRENT
#define PERF_CURRENT(x)                 ((x) & 0xFF)

// *_PWRSTCTRL
#define POWERSTATE(x)                   ((x) & 0x3)
#define LOGICRETSTATE                   (1 << 2)
#define LOWPOWERSTATECHANGE             (1 << 4)
#define MPU_L1_RETSTATE                 (1 << 8)
#define DSP_L1_RETSTATE                 (1 << 8)
#define AESSMEM_RETSTATE                (1 << 8)
#define CORE_OTHER_BANK_RETSTATE        (1 << 8)
#define HWA_MEM_RETSTATE                (1 << 8)
#define DSS_MEM_RETSTATE                (1 << 8)
#define L3INIT_BANK1_RETSTATE           (1 << 8)
#define RETAINED_BANK_RETSTATE          (1 << 8)
#define MPU_L2_RETSTATE                 (1 << 9)
#define DSP_L2_RETSTATE                 (1 << 9)
#define CORE_OCMRAM_RETSTATE            (1 << 9)
#define SL2_MEM_RETSTATE                (1 << 9)
#define NONRETAINED_BANK_RETSTATE       (1 << 9)
#define MPU_RAM_RETSTATE                (1 << 10)
#define DSP_EDMA_RETSTATE               (1 << 10)
#define PERIPHMEM_RETSTATE              (1 << 10)
#define MPU_M3_L2RAM_RETSTATE           (1 << 10)
#define TCM1_MEM_RETSTATE               (1 << 10)
#define MPU_M3_UNICACHE_RETSTATE        (1 << 11)
#define TCM2_MEM_RETSTATE               (1 << 11)
#define INTRCONN_NRET_BANK_RETSTATE     (1 << 12)
#define MPU_L1_ONSTATE(x)               (((x) & 0x3) << 16)
#define DSP_L1_ONSTATE(x)               (((x) & 0x3) << 16)
#define AESSMEM_ONSTATE(x)              (((x) & 0x3) << 16)
#define CORE_OTHER_BANK_ONSTATE(x)      (((x) & 0x3) << 16)
#define HWA_MEM_ONSTATE(x)              (((x) & 0x3) << 16)
#define CAM_MEM_ONSTATE(x)              (((x) & 0x3) << 16)
#define DSS_MEM_ONSTATE(x)              (((x) & 0x3) << 16)
#define SGX_MEM_ONSTATE(x)              (((x) & 0x3) << 16)
#define L3INIT_BANK1_ONSTATE(x)         (((x) & 0x3) << 16)
#define RETAINED_BANK_ONSTATE(x)        (((x) & 0x3) << 16)
#define EMU_BANK_ONSTATE(x)             (((x) & 0x3) << 16)
#define MPU_L2_ONSTATE(x)               (((x) & 0x3) << 18)
#define DSP_L2_ONSTATE(x)               (((x) & 0x3) << 18)
#define CORE_OCMRAM_ONSTATE(x)          (((x) & 0x3) << 18)
#define SL2_MEM_ONSTATE(x)              (((x) & 0x3) << 18)
#define NONRETAINED_BANK_ONSTATE(x)     (((x) & 0x3) << 18)
#define MPU_RAM_ONSTATE(x)              (((x) & 0x3) << 20)
#define DSP_EDMA_ONSTATE(x)             (((x) & 0x3) << 20)
#define PERIPHMEM_ONSTATE(x)            (((x) & 0x3) << 20)
#define TCM1_MEM_ONSTATE(x)             (((x) & 0x3) << 20)
#define MPU_M3_L2RAM_ONSTATE(x)         (((x) & 0x3) << 22)
#define TCM2_MEM_ONSTATE(x)             (((x) & 0x3) << 22)
#define INTRCONN_NRET_BANK_ONSTATE      (((x) & 0x3) << 24)

// *_PWRSTST
#define POWERSTATEST(x)                 ((x) & 0x3)
#define LOGICRETSTATEST                 (1 << 2)
#define MPU_L1_STATEST(x)               (((x) & 0x3) << 4)
#define DSP_L1_STATEST(x)               (((x) & 0x3) << 4)
#define AESSMEM_STATEST(x)              (((x) & 0x3) << 4)
#define CORE_OTHER_BANK_STATEST(x)      (((x) & 0x3) << 4)
#define HWA_MEM_STATEST(x)              (((x) & 0x3) << 4)
#define CAM_MEM_STATEST(x)              (((x) & 0x3) << 4)
#define DSS_MEM_STATEST(x)              (((x) & 0x3) << 4)
#define SGX_MEM_STATEST(x)              (((x) & 0x3) << 4)
#define L3INIT_BANK1_STATEST(x)         (((x) & 0x3) << 4)
#define RETAINED_BANK_STATEST(x)        (((x) & 0x3) << 4)
#define EMU_BANK_STATEST(x)             (((x) & 0x3) << 4)
#define MPU_L2_STATEST(x)               (((x) & 0x3) << 6)
#define DSP_L2_STATEST(x)               (((x) & 0x3) << 6)
#define CORE_OCMRAM_STATEST(x)          (((x) & 0x3) << 6)
#define SL2_MEM_STATEST(x)              (((x) & 0x3) << 6)
#define NONRETAINED_BANK_STATEST(x)     (((x) & 0x3) << 6)
#define MPU_RAM_STATEST(x)              (((x) & 0x3) << 8)
#define DSP_RAM_STATEST(x)              (((x) & 0x3) << 8)
#define PERIPHMEM_STATEST(x)            (((x) & 0x3) << 8)
#define MPU_M3_L2RAM_STATEST(x)         (((x) & 0x3) << 8)
#define TCM1_MEM_STATEST(x)             (((x) & 0x3) << 8)
#define MPU_M3_UNICACHE_STATEST(x)      (((x) & 0x3) << 10)
#define TCM2_MEM_STATEST(x)             (((x) & 0x3) << 10)
#define INTRCONN_NRET_BANK_STATEST(x)   (((x) & 0x3) << 12)
#define INTRANSITION                    (1 << 20)
#define LASTPOWERSTATEENTERED(x)        (((x) & 0x3) << 24)

// RM_MPU_RSTST
#define EMULATION_RST                   (1 << 0)

// RM_DSP_RSTCTRL
// RM_MPU_M3_RSTCTRL
// RM_IVAHD_RSTCTRL
#define RST1                            (1 << 0)
#define RST2                            (1 << 1)
#define RST3                            (1 << 2)

// PRM_RSTCTRL
#define RST_GLOBAL_WARM_software        (1 << 0)
#define RST_GLOBAL_COLD_software        (1 << 1)

// RM_DSP_RSTST
// RM_MPU_M3_RSTST
// RM_IVAHD_RSTST
#define RST1ST                          (1 << 0)
#define RST2ST                          (1 << 1)
#define RST3ST                          (1 << 2)
#define DSPSS_EMU_RSTST                 (1 << 2)
#define DSP_DSP_EMU_REQ_RSTST           (1 << 3)
#define EMULATION_RST1ST                (1 << 3)
#define EMULATION_SEQ1_RST1ST           (1 << 3)
#define EMULATION_RST2ST                (1 << 4)
#define EMULATION_SEQ2_RST2ST           (1 << 4)
#define ICECRUSHER_RST1ST               (1 << 5)
#define ICECRUSHER_SEQ1_RST1ST          (1 << 5)
#define ICECRUSHER_RST2ST               (1 << 6)
#define ICECRUSHER_SEQ2_RST2ST          (1 << 6)

// PRM_RSTST
#define GLOBAL_COLD_RST                 (1 << 0)
#define GLOBAL_WARM_SW_RST              (1 << 1)
#define MPU_WDT_RST                     (1 << 3)
#define EXTERNAL_WARM_RST               (1 << 5)
#define VDD_MPU_VOLT_MGR_RST            (1 << 6)
#define VDD_IVA_VOLT_MGR_RST            (1 << 7)
#define VDD_CORE_VOLT_MGR_RST           (1 << 8)
#define ICEPICK_RST                     (1 << 9)
#define C2C_RST                         (1 << 10)

// *_CLKSTCTRL
// *_CLKSTCTRL_RESTORE
#define CLKTRCTRL(x)                    ((x) & 0x3)
#define CLKACTIVITY_SYS_CLK             (1 << 8)
#define CLKACTIVITY_EMU_SYS_CLK         (1 << 8)
#define CLKACTIVITY_MPU_DPLL_CLK        (1 << 8)
#define CLKACTIVITY_DSP_ROOT_CLK        (1 << 8)
#define CLKACTIVITY_DPLL_ABE_X2_CLK     (1 << 8)
#define CLKACTIVITY_L4_AO_ICLK          (1 << 8)
#define CLKACTIVITY_L3_1_ICLK           (1 << 8)
#define CLKACTIVITY_L3_2_ICLK           (1 << 8)
#define CLKACTIVITY_MPU_M3_CLK          (1 << 8)
#define CLKACTIVITY_DMA_L3_ICLK         (1 << 8)
#define CLKACTIVITY_L3_EMIF_ICLK        (1 << 8)
#define CLKACTIVITY_L3_C2C_ICLK         (1 << 8)
#define CLKACTIVITY_CFG_L4_ICLK         (1 << 8)
#define CLKACTIVITY_L3_INSTR_ICLK       (1 << 8)
#define CLKACTIVITY_IVAHD_CLK           (1 << 8)
#define CLKACTIVITY_ISS_CLK             (1 << 8)
#define CLKACTIVITY_DSS_L3_ICLK         (1 << 8)
#define CLKACTIVITY_SGX_L3_ICLK         (1 << 8)
#define CLKACTIVITY_INIT_L3_ICLK        (1 << 8)
#define CLKACTIVITY_L4_PER_ICLK         (1 << 8)
#define CLKACTIVITY_L3_SECURE_GICLK     (1 << 8)
#define CLKACTIVITY_ABE_LP_CLK          (1 << 9)
#define CLKACTIVITY_CORE_DPLL_EMU_CLK   (1 << 9)
#define CLKACTIVITY_ABE_ICLK2           (1 << 9)
#define CLKACTIVITY_SR_MPU_SYSCLK       (1 << 9)
#define CLKACTIVITY_DLL_CLK             (1 << 9)
#define CLKACTIVITY_L4_C2C_ICLK         (1 << 9)
#define CLKACTIVITY_CAM_PHY_CTRL_CLK    (1 << 9)
#define CLKACTIVITY_DSS_FCLK            (1 << 9)
#define CLKACTIVITY_SGX_FCLK            (1 << 9)
#define CLKACTIVITY_INIT_L4_ICLK        (1 << 9)
#define CLKACTIVITY_GPT10_FCLK          (1 << 9)
#define CLKACTIVITY_L4_SECURE_GICLK     (1 << 9)
#define CLKACTIVITY_24M_FCLK            (1 << 10)
#define CLKACTIVITY_SR_IVA_SYSCLK       (1 << 10)
#define CLKACTIVITY_PHY_ROOT_CLK        (1 << 10)
#define CLKACTIVITY_L3X2_C2C_ICLK       (1 << 10)
#define CLKACTIVITY_FDIF_FCLK           (1 << 10)
#define CLKACTIVITY_DSS_ALWON_SYS_CLK   (1 << 10)
#define CLKACTIVITY_GPT11_FCLK          (1 << 10)
#define CLKACTIVITY_WKUP_32K_FCLK       (1 << 11)
#define CLKACTIVITY_ABE_SYSCLK          (1 << 11)
#define CLKACTIVITY_SR_CORE_SYSCLK      (1 << 11)
#define CLKACTIVITY_HDMI_PHY_48M_FCLK   (1 << 11)
#define CLKACTIVITY_GPT2_FCLK           (1 << 11)
#define CLKACTIVITY_ASYNC_DLL_CLK       (1 << 11)
#define CLKACTIVITY_L4_WKUP_ICLK        (1 << 12)
#define CLKACTIVITY_ABE_ALWON_32K_CLK   (1 << 12)
#define CLKACTIVITY_CORE_ALWON_32K_GFCLK (1 << 12)
#define CLKACTIVITY_INIT_48M_FCLK       (1 << 12)
#define CLKACTIVITY_GPT3_FCLK           (1 << 12)
#define CLKACTIVITY_ASYNC_PHY1_CLK      (1 << 12)
#define CLKACTIVITY_ABE_24M_FCLK        (1 << 13)
#define CLKACTIVITY_INIT_48MC_FCLK      (1 << 13)
#define CLKACTIVITY_GPT4_FCLK           (1 << 13)
#define CLKACTIVITY_ASYNC_PHY2_CLK      (1 << 13)
#define CLKACTIVITY_USB_DPLL_CLK        (1 << 14)
#define CLKACTIVITY_GPT9_FCLK           (1 << 14)
#define CLKACTIVITY_USB_DPLL_HS_CLK     (1 << 15)
#define CLKACTIVITY_12M_FCLK            (1 << 15)
#define CLKACTIVITY_INIT_HSI_FCLK       (1 << 16)
#define CLKACTIVITY_PER_24MC_FCLK       (1 << 16)
#define CLKACTIVITY_INIT_HSMMC1_FCLK    (1 << 17)
#define CLKACTIVITY_PER_32K_FCLK        (1 << 17)
#define CLKACTIVITY_INIT_HSMMC2_FCLK    (1 << 18)
#define CLKACTIVITY_PER_48M_FCLK        (1 << 18)
#define CLKACTIVITY_PER_96M_FCLK        (1 << 19)
#define CLKACTIVITY_HSIC_P1_480M_FCLK   (1 << 20)
#define CLKACTIVITY_HSIC_P2_480M_FCLK   (1 << 21)
#define CLKACTIVITY_TLL_CH0_FCLK        (1 << 22)
#define CLKACTIVITY_PER_MCBSP4_FCLK     (1 << 22)
#define CLKACTIVITY_TLL_CH1_FCLK        (1 << 23)
#define CLKACTIVITY_UTMI_ROOT_FCLK      (1 << 25)
#define CLKACTIVITY_PER_ABE_24M_FCLK    (1 << 25)
#define CLKACTIVITY_HSIC_P1_FCLK        (1 << 26)
#define CLKACTIVITY_HSIC_P2_FCLK        (1 << 27)
#define CLKACTIVITY_INIT_60M_P1_FCLK    (1 << 28)
#define CLKACTIVITY_INIT_60M_P2_FCLK    (1 << 29)

// PRM_CLKREQCTRL
#define CLKREQ_COND(x)                  ((x) & 0x7)

// PRM_VOLTCTRL
#define AUTO_CTRL_VDD_CORE_L(x)         ((x) & 0x3)
#define AUTO_CTRL_VDD_MPU_L(x)          (((x) & 0x3) << 2)
#define AUTO_CTRL_VDD_IVA_L(x)          (((x) & 0x3) << 4)
#define VDD_MPU_PRESENCE                (1 << 8)
#define VDD_IVA_PRESENCE                (1 << 9)
#define VDD_CORE_I2C_DISABLE            (1 << 12)
#define VDD_MPU_I2C_DISABLE             (1 << 13)
#define VDD_IVA_I2C_DISABLE             (1 << 14)

// PRM_PWRREQCTRL
#define PWRREQ_COND(x)                  ((x) & 0x3)

// PRM_IO_PMCTRL
#define ISOCLK_OVERRIDE                 (1 << 0)
#define ISOCLK_STATUS                   (1 << 1)
#define ISOOVR_EXTEND                   (1 << 4)
#define IOON_STATUS                     (1 << 5)
#define WUCLK_CTRL                      (1 << 8)
#define WUCLK_STATUS                    (1 << 9)
#define GLOBAL_WUEN                     (1 << 16)

// *_CONTEXT
#define LOSTCONTEXT_DFF                 (1 << 0)
#define LOSTCONTEXT_RFF                 (1 << 1)
#define LOSTMEM_MPU_L1                  (1 << 8)
#define LOSTMEM_DSP_L1                  (1 << 8)
#define LOSTMEM_AESSMEM                 (1 << 8)
#define LOSTMEM_PERIPHMEM               (1 << 8)
#define LOSTMEM_CORE_OCMRAM             (1 << 8)
#define LOSTMEM_MPU_M3_UNICACHE         (1 << 8)
#define LOSTMEM_CORE_OTHER_BANK         (1 << 8)
#define LOSTMEM_OCP_WP1_CORE_NRET_BANK  (1 << 8)
#define LOSTMEM_TCM1_MEM                (1 << 8)
#define LOSTMEM_SL2_MEM                 (1 << 8)
#define LOSTMEM_CAM_MEM                 (1 << 8)
#define LOSTMEM_DSS_MEM                 (1 << 8)
#define LOSTMEM_SGX_MEM                 (1 << 8)
#define LOSTMEM_L3INIT_BANK1            (1 << 8)
#define LOSTMEM_NONRETAINED_BANK        (1 << 8)
#define LOSTMEM_RETAINED_BANK           (1 << 8)
#define LOSTMEM_WKUP_BANK               (1 << 8)
#define LOSTMEM_EMU_BANK                (1 << 8)
#define LOSTMEM_MPU_L2                  (1 << 9)
#define LOSTMEM_DSP_L2                  (1 << 9)
#define LOSTMEM_MPU_M3_L2RAM            (1 << 9)
#define LOSTMEM_DMM_CORE_NRET_BANK      (1 << 9)
#define LOSTMEM_TCM2_MEM                (1 << 9)
#define LOSTMEM_MPU_RAM                 (1 << 10)
#define LOSTMEM_DSP_EDMA                (1 << 10s)
#define LOSTMEM_HWA_MEM                 (1 << 10)

// PM_ABE_PDM_WKDEP
#define WKUPDEP_PDM_IRQ_MPU         (1 << 0)
#define WKUPDEP_PDM_IRQ_DSP         (1 << 2)
#define WKUPDEP_PDM_DMA_DSP         (1 << 6)
#define WKUPDEP_PDM_DMA_SDMA        (1 << 7)

// PM_ABE_DMIC_WKDEP
#define WKUPDEP_DMIC_IRQ_MPU        (1 << 0)
#define WKUPDEP_DMIC_IRQ_DSP        (1 << 2)
#define WKUPDEP_DMIC_DMA_DSP        (1 << 6)
#define WKUPDEP_DMIC_DMA_SDMA       (1 << 7)

// PM_ABE_MCASP_WKDEP   
#define WKUPDEP_MCASP1_IRQ_MPU      (1 << 0)
#define WKUPDEP_MCASP1_IRQ_DSP      (1 << 2)
#define WKUPDEP_MCASP1_DMA_DSP      (1 << 6)
#define WKUPDEP_MCASP1_DMA_SDMA     (1 << 7)

// PM_ABE_MCBSP1_WKDEP
#define WKUPDEP_MCBSP1_MPU          (1 << 0)
#define WKUPDEP_MCBSP1_DSP          (1 << 2)
#define WKUPDEP_MCBSP1_SDMA         (1 << 3)

// PM_ABE_MCBSP2_WKDEP
#define WKUPDEP_MCBSP2_MPU          (1 << 0)
#define WKUPDEP_MCBSP2_DSP          (1 << 2)
#define WKUPDEP_MCBSP2_SDMA         (1 << 3)

// PM_ABE_MCBSP3_WKDEP
#define WKUPDEP_MCBSP3_MPU          (1 << 0)
#define WKUPDEP_MCBSP3_DSP          (1 << 2)
#define WKUPDEP_MCBSP3_SDMA         (1 << 3)

// PM_ABE_SLIMBUS_WKDEP
#define WKUPDEP_SLIMBUS1_IRQ_MPU    (1 << 0)
#define WKUPDEP_SLIMBUS1_IRQ_DSP    (1 << 2)
#define WKUPDEP_SLIMBUS1_DMA_DSP    (1 << 6)
#define WKUPDEP_SLIMBUS1_DMA_SDMA   (1 << 7)

// PM_ABE_GPTIMER5_WKDEP
#define WKUPDEP_TIMER5_MPU          (1 << 0)
#define WKUPDEP_TIMER5_DSP          (1 << 2)

// PM_ABE_GPTIMER6_WKDEP
#define WKUPDEP_TIMER6_MPU          (1 << 0)
#define WKUPDEP_TIMER6_DSP          (1 << 2)

// PM_ABE_GPTIMER7_WKDEP
#define WKUPDEP_TIMER7_MPU          (1 << 0)
#define WKUPDEP_TIMER7_DSP          (1 << 2)

// PM_ABE_GPTIMER8_WKDEP
#define WKUPDEP_TIMER8_MPU          (1 << 0)
#define WKUPDEP_TIMER8_DSP          (1 << 2)

// PM_WKUP_WDTIMER2_WKDEP
#define WKUPDEP_WDT2_MPU            (1 << 0)
#define WKUPDEP_WDT2_MPU_M3         (1 << 1)

// PM_ABE_WDTIMER3_WKDEP
#define WKUPDEP_WDT3_MPU            (1 << 0)

// PM_ALWON_SR_MPU_WKDEP
#define WKUPDEP_SR_IVA_MPU          (1 << 0)
#define WKUPDEP_SR_IVA_MPU_M3       (1 << 1)

// PM_ALWON_SR_MPU_WKDEP
#define WKUPDEP_SR_CORE_MPU         (1 << 0)
#define WKUPDEP_SR_CORE_MPU_M3      (1 << 1)

// PM_DSS_DSS_WKDEP
#define WKUPDEP_DISPC_MPU           (1 << 0)
#define WKUPDEP_DISPC_MPU_M3        (1 << 1)
#define WKUPDEP_DISPC_DSP           (1 << 2)
#define WKUPDEP_DISPC_SDMA          (1 << 3)
#define WKUPDEP_DSI1_MPU            (1 << 4)
#define WKUPDEP_DSI1_MPU_M3         (1 << 5)
#define WKUPDEP_DSI1_DSP            (1 << 6)
#define WKUPDEP_DSI1_SDMA           (1 << 7)
#define WKUPDEP_DSI2_MPU            (1 << 8)
#define WKUPDEP_DSI2_MPU_M3         (1 << 9)
#define WKUPDEP_DSI2_DSP            (1 << 10)
#define WKUPDEP_DSI2_SDMA           (1 << 11)
#define WKUPDEP_HDMIIRQ_MPU         (1 << 12)
#define WKUPDEP_HDMIIRQ_MPU_M3      (1 << 13)
#define WKUPDEP_HDMIIRQ_DSP         (1 << 14)
#define WKUPDEP_HDMIDMA_SDMA        (1 << 19)

// PM_L3INIT_MMC1_WKDEP
#define WKUPDEP_MMC1_MPU            (1 << 0)
#define WKUPDEP_MMC1_MPU_M3         (1 << 1)
#define WKUPDEP_MMC1_DSP            (1 << 2)
#define WKUPDEP_MMC1_SDMA           (1 << 3)

// PM_L3INIT_MMC2_WKDEP
#define WKUPDEP_MMC2_MPU            (1 << 0)
#define WKUPDEP_MMC2_MPU_M3         (1 << 1)
#define WKUPDEP_MMC2_DSP            (1 << 2)
#define WKUPDEP_MMC2_SDMA           (1 << 3)

// PM_L3INIT_HSI_WKDEP
#define WKUPDEP_HSI_MCU_MPU         (1 << 0)
#define WKUPDEP_HSI_MCU_MPU_M3      (1 << 1)
#define WKUPDEP_HSI_DSP_DSP         (1 << 6)
#define WKUPDEP_WGM_HSI_WAKE_MPU    (1 << 8)

// PM_L3INIT_HSUSBHOST_WKDEP
#define WKUPDEP_HSUSBHOST_MPU       (1 << 0)
#define WKUPDEP_HSUSBHOST_MPU_M3    (1 << 1)

// PM_L3INIT_HSUSBOTG_WKDEP
#define WKUPDEP_HSUSBOTG_MPU        (1 << 0)
#define WKUPDEP_HSUSBOTG_MPU_M3     (1 << 1)

// PM_L3INIT_HSUSBTLL_WKDEP
#define WKUPDEP_HSUSBTLL_MPU        (1 << 0)
#define WKUPDEP_HSUSBTLL_MPU_M3     (1 << 1)

// PM_L3INIT_FSUSB_WKDEP
#define WKUPDEP_FSUSB_MPU           (1 << 0)
#define WKUPDEP_FSUSB_MPU_M3        (1 << 1)

// PM_L4PER_GPTIMER1_WKDEP
#define WKUPDEP_TIMER1_MPU        (1 << 0)

// PM_L4PER_GPTIMER2_WKDEP
#define WKUPDEP_DMTIMER2_MPU        (1 << 0)

// PM_L4PER_GPTIMER3_WKDEP
#define WKUPDEP_DMTIMER3_MPU        (1 << 0)
#define WKUPDEP_DMTIMER3_MPU_M3     (1 << 1)

// PM_L4PER_GPTIMER4_WKDEP
#define WKUPDEP_DMTIMER4_MPU        (1 << 0)
#define WKUPDEP_DMTIMER4_MPU_M3     (1 << 1)

// PM_L4PER_GPTIMER9_WKDEP
#define WKUPDEP_DMTIMER9_MPU        (1 << 0)
#define WKUPDEP_DMTIMER9_MPU_M3     (1 << 1)

// PM_L4PER_GPTIMER10_WKDEP
#define WKUPDEP_DMTIMER10_MPU       (1 << 0)

// PM_L4PER_GPTIMER11_WKDEP
#define WKUPDEP_DMTIMER11_MPU       (1 << 0)
#define WKUPDEP_DMTIMER11_MPU_M3    (1 << 1)

// PM_WKUP_GPIO1_WKDEP
#define WKUPDEP_GPIO1_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO1_IRQ1_MPU_M3   (1 << 1)
#define WKUPDEP_GPIO1_IRQ2_DSP      (1 << 6)

// PM_L4PER_GPIO2_WKDEP
#define WKUPDEP_GPIO2_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO2_IRQ1_MPU_M3   (1 << 1)
#define WKUPDEP_GPIO2_IRQ2_DSP      (1 << 6)

// PM_L4PER_GPIO3_WKDEP
#define WKUPDEP_GPIO3_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO3_IRQ2_DSP      (1 << 6)

// PM_L4PER_GPIO4_WKDEP
#define WKUPDEP_GPIO4_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO4_IRQ2_DSP      (1 << 6)

// PM_L4PER_GPIO5_WKDEP
#define WKUPDEP_GPIO5_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO5_IRQ2_DSP      (1 << 6)

// PM_L4PER_GPIO6_WKDEP
#define WKUPDEP_GPIO6_IRQ1_MPU      (1 << 0)
#define WKUPDEP_GPIO6_IRQ2_DSP      (1 << 6)

// PM_L4PER_I2C1_WKDEP
#define WKUPDEP_I2C1_IRQ_MPU        (1 << 0)
#define WKUPDEP_I2C1_IRQ_MPU_M3     (1 << 1)
#define WKUPDEP_I2C1_DMA_SDMA       (1 << 7)

// PM_L4PER_I2C2_WKDEP
#define WKUPDEP_I2C2_IRQ_MPU        (1 << 0)
#define WKUPDEP_I2C2_IRQ_MPU_M3     (1 << 1)
#define WKUPDEP_I2C2_DMA_SDMA       (1 << 7)

// PM_L4PER_I2C3_WKDEP
#define WKUPDEP_I2C3_IRQ_MPU        (1 << 0)
#define WKUPDEP_I2C3_IRQ_MPU_M3     (1 << 1)
#define WKUPDEP_I2C3_DMA_SDMA       (1 << 7)

// PM_L4PER_I2C4_WKDEP
#define WKUPDEP_I2C4_IRQ_MPU        (1 << 0)
#define WKUPDEP_I2C4_IRQ_MPU_M3     (1 << 1)
#define WKUPDEP_I2C4_DMA_SDMA       (1 << 7)

// PM_L4PER_MCBSP4_WKDEP
#define WKUPDEP_MCBSP4_MPU          (1 << 0)
#define WKUPDEP_MCBSP4_DSP          (1 << 2)
#define WKUPDEP_MCBSP4_SDMA         (1 << 3)

// PM_L4PER_MCSPI1_WKDEP
#define WKUPDEP_MCSPI1_MPU          (1 << 0)
#define WKUPDEP_MCSPI1_MPU_M3       (1 << 1)
#define WKUPDEP_MCSPI1_DSP          (1 << 2)
#define WKUPDEP_MCSPI1_SDMA         (1 << 3)

// PM_L4PER_MCSPI2_WKDEP
#define WKUPDEP_MCSPI2_MPU          (1 << 0)
#define WKUPDEP_MCSPI2_MPU_M3       (1 << 1)
#define WKUPDEP_MCSPI2_SDMA         (1 << 3)

// PM_L4PER_MCSPI3_WKDEP
#define WKUPDEP_MCSPI3_MPU          (1 << 0)
#define WKUPDEP_MCSPI3_SDMA         (1 << 3)

// PM_L4PER_MCSPI4_WKDEP
#define WKUPDEP_MCSPI4_MPU          (1 << 0)
#define WKUPDEP_MCSPI4_SDMA         (1 << 3)

// PM_L4PER_MMCSD3_WKDEP
#define WKUPDEP_MMCSD3_MPU          (1 << 0)
#define WKUPDEP_MMCSD3_MPU_M3       (1 << 1)
#define WKUPDEP_MMCSD3_SDMA         (1 << 3)

// PM_L4PER_MMCSD4_WKDEP
#define WKUPDEP_MMCSD4_MPU          (1 << 0)
#define WKUPDEP_MMCSD4_MPU_M3       (1 << 1)
#define WKUPDEP_MMCSD4_SDMA         (1 << 3)

// PM_L4PER_MMCSD5_WKDEP
#define WKUPDEP_MMCSD5_MPU          (1 << 0)
#define WKUPDEP_MMCSD5_MPU_M3       (1 << 1)
#define WKUPDEP_MMCSD5_SDMA         (1 << 3)

// PM_L4PER_SLIMBUS2_WKDEP
#define WKUPDEP_SLIMBUS2_IRQ_MPU    (1 << 0)
#define WKUPDEP_SLIMBUS2_IRQ_DSP    (1 << 2)
#define WKUPDEP_SLIMBUS2_DMA_DSP    (1 << 6)
#define WKUPDEP_SLIMBUS2_DMA_SDMA   (1 << 7)

// PM_L4PER_UART1_WKDEP
#define WKUPDEP_UART1_MPU           (1 << 0)
#define WKUPDEP_UART1_SDMA          (1 << 3)

// PM_L4PER_UART2_WKDEP
#define WKUPDEP_UART2_MPU           (1 << 0)
#define WKUPDEP_UART2_SDMA          (1 << 3)

// PM_L4PER_UART3_WKDEP
#define WKUPDEP_UART3_MPU           (1 << 0)
#define WKUPDEP_UART3_MPU_M3        (1 << 1)
#define WKUPDEP_UART3_DSP           (1 << 2)
#define WKUPDEP_UART3_SDMA          (1 << 3)

// PM_L4PER_UART4_WKDEP
#define WKUPDEP_UART4_MPU           (1 << 0)
#define WKUPDEP_UART4_SDMA          (1 << 3)

// PM_WKUP_KEYBOARD_WKDEP
#define WKUPDEP_KEYBOARD_MPU        (1 << 0)

// CM_*_STATICDEP
#define MPU_M3_STATDEP              (1 << 0)
#define DSP_STATDEP                 (1 << 1)
#define IVAHD_STATDEP               (1 << 2)
#define ABE_STATDEP                 (1 << 3)
#define MEMIF_STATDEP               (1 << 4)
#define L3_1_STATDEP                (1 << 5)
#define L3_2_STATDEP                (1 << 6)
#define L3INIT_STATDEP              (1 << 7)
#define DSS_STATDEP                 (1 << 8)
#define ISS_STATDEP                 (1 << 9)
#define SGX_STATDEP                 (1 << 10)
#define SDMA_STATDEP                (1 << 11)
#define L4CFG_STATDEP               (1 << 12)
#define L4PER_STATDEP               (1 << 13)
#define L4SEC_STATDEP               (1 << 14)
#define L4WKUP_STATDEP              (1 << 15)
#define ALWONCORE_STATDEP           (1 << 16)
#define C2C_STATDEP                 (1 << 18)

// CM_*_DYNAMICDEP
#define MPU_M3_DYNDEP               (1 << 0)
#define DSP_DYNDEP                  (1 << 1)
#define IVAHD_DYNDEP                (1 << 2)
#define ABE_DYNDEP                  (1 << 3)
#define MEMIF_DYNDEP                (1 << 4)
#define L3_1_DYNDEP                 (1 << 5)
#define L3_2_DYNDEP                 (1 << 6)
#define L3_INIT_DYNDEP              (1 << 7)
#define DSS_DYNDEP                  (1 << 8)
#define CAM_DYNDEP                  (1 << 9)
#define SGX_DYNDEP                  (1 << 10)
#define L4CFG_DYNDEP                (1 << 12)
#define L4PER_DYNDEP                (1 << 13)
#define L4SEC_DYNDEP                (1 << 14)
#define L4WKUP_DYNDEP               (1 << 15)
#define ALWONCORE_DYNDEP            (1 << 16)
#define C2C_DYNDEP                  (1 << 18)
#define WINDOWSIZE(x)               (((x) & 0x3) << 24)

// PRM_RSTTIME
#define RSTTIME1(x)                 ((x) & 0x1FF)
#define RSTTIME2(x)                 (((x) & 0x3F) << 10)

// PRM_PSCON_COUNT
#define PCHARGE_TIME(x)             ((x) & 0xFF)
#define PONOUT_2_PGOODIN_TIME(x)    (((x) & 0xFF) << 8)

// PRM_IO_COUNT
#define ISO_2_ON_TIME(x)            ((x) & 0xFF)

// PRM_VOLTSETUP_WARMRESET
#define STABLE_COUNT(x)             ((x) & 0x3F)
#define STABLE_PRESCAL(x)           (((x) & 0x3) << 8)

// PRM_VOLTSETUP_*_OFF
// PRM_VOLTSETUP_*_RET_SLEEP
#define RAMP_UP_COUNT               ((x) & 0x3F)
#define RAMP_UP_PRESCAL(x)          (((x) & 0x3) << 8)
#define RAMP_DOWN_COUNT(x)          (((x) & 0x3F) << 16)
#define RAMP_DOWN_PRESCAL(x)        (((x) & 0x3) << 24)

// PRM_VP_*_CONFIG
#define VPENABLE                    (1 << 0)
#define FORCEUPDATE                 (1 << 1)
#define INITVDD                     (1 << 2)
#define TIMEOUTEN                   (1 << 3)
#define INITVOLTAGE(x)              (((x) & 0xFF) << 8)
#define ERRORGAIN(x)                (((x) & 0xFF) << 16)
#define ERROROFFSET(x)              (((x) & 0xFF) << 24)

// PRM_VP_*_STATUS
#define VPINIDLE                    (1 << 0)

// PRM_VP_*_VLIMITTO
#define TIMEOUT(x)                  ((x) & 0xFFFF)
#define VDDMIN(x)                   (((x) & 0xFF) << 16)
#define VDDMAX(x)                   (((x) & 0xFF) << 24)

// PRM_VP_*_VOLTAGE
#define VPVOLTAGE(x)                ((x) & 0xFF)
#define FORCEUPDATEWAIT(x)          (((x) & 0xFFFFFF) << 8)

// PRM_VP_*_VSTEPMAX
#define VSTEPMAX(x)                 ((x) & 0xFF)
#define SMPsoftwareAITTIMEMAX(x)    (((x) & 0xFFFF) << 8)

// PRM_VP_*_VSTEPMIN
#define VSTEPMIN(x)                 ((x) & 0xFF)
#define SMPsoftwareAITTIMEMIN(x)    (((x) & 0xFFFF) << 8)

// PRM_VC_SMPS_SA
#define SA_VDD_CORE_L(x)            ((x) & 0x7F)
#define SA_VDD_IVA_L(x)             (((x) & 0x7F) << 8)
#define SA_VDD_MPU_L(x)             (((x) & 0x7F) << 16)

// PRM_VC_VAL_SMPS_RA_VOL
#define VOLRA_VDD_CORE_L(x)         ((x) & 0x7F)
#define VOLRA_VDD_IVA_L(x)          (((x) & 0x7F) << 8)
#define VOLRA_VDD_MPU_L(x)          (((x) & 0x7F) << 16)

// PRM_VC_VAL_SMPS_RA_CMD
#define CMDRA_VDD_CORE_L(x)         ((x) & 0x7F)
#define CMDRA_VDD_IVA_L(x)          (((x) & 0x7F) << 8)
#define CMDRA_VDD_MPU_L(x)          (((x) & 0x7F) << 16)

// PRM_VC_VAL_CMD_VDD_*_L
#define OFF(x)                      ((x) & 0xFF)
#define RET(x)                      (((x) & 0xFF) << 8)
#define ONLP(x)                     (((x) & 0xFF) << 16)
#define ON(x)                       (((x) & 0xFF) << 24)

// PRM_VC_VAL_BYPASS
#define SLAVEADDR(x)                ((x) & 0x7F)
#define REGADDR(x)                  (((x) & 0xFF) << 8)
#define DATA(x)                     (((x) & 0xFF) << 16)
#define VALID                       (1 << 24)

// PRM_VC_CFG_CHANNEL
#define SEL_SA_VDD_CORE_L           (1 << 0)
#define RAV_VDD_CORE_L              (1 << 1)
#define RAC_VDD_CORE_L              (1 << 2)
#define RACEN_VDD_CORE_L            (1 << 3)
#define CMD_VDD_CORE_L              (1 << 4)
#define SEL_SA_VDD_IVA_L            (1 << 8)
#define RAV_VDD_IVA_L               (1 << 9)
#define RAC_VDD_IVA_L               (1 << 10)
#define RACEN_VDD_IVA_L             (1 << 11)
#define CMD_VDD_IVA_L               (1 << 12)
#define SEL_SA_VDD_MPU_L            (1 << 16)
#define CMD_VDD_MPU_L               (1 << 17)
#define RAV_VDD_MPU_L               (1 << 18)
#define RAC_VDD_MPU_L               (1 << 19)
#define RACEN_VDD_MPU_L             (1 << 20)

// PRM_VC_CFG_I2C_MODE
#define HSMCODE(x)                  ((x) & 0x7)
#define HSMODEEN                    (1 << 3)
#define SRMODEEN                    (1 << 4)
#define DFILTEREN                   (1 << 6)

// PRM_VC_CFG_I2C_CLK
#define SCLH(x)                     ((x) & 0xFF)
#define SCLL(x)                     (((x) & 0xFF) << 8)
#define HSSCLH(x)                   (((x) & 0xFF) << 16)
#define HSSCLL(x)                   (((x) & 0xFF) << 24)

// PRM_SRAM_COUNT
#define PCHARGECNT_VALUE(x)         ((x) & 0x3F)
#define VSETUPCNT_VALUE(x)          (((x) & 0xFF) << 8)
#define SLPCNT_VALUE(x)             (((x) & 0xFF) << 16)
#define STARTUP_COUNT(x)            (((x) & 0xFF) << 24)

// PRM_SRAM_*_SETUP
#define DISABLE_RTA_EXPORT          (1 << 0)
#define ABBOFF_ACT_EXPORT           (1 << 1)
#define ABBOFF_SLEEP_EXPORT         (1 << 2)
#define ENFUNC1_EXPORT              (1 << 3)
#define ENFUNC2_EXPORT              (1 << 4)
#define ENFUNC3_EXPORT              (1 << 5)
#define ENFUNC4                     (1 << 6)
#define ENFUNC5                     (1 << 7)
#define AIPOFF                      (1 << 8)

// PRM_LDO_SRAM_*_CTRL
#define RETMODE_ENABLE              (1 << 0)
#define SRAMLDO_STATUS              (1 << 8)
#define SRAM_IN_TRANSITION          (1 << 9)

// PRM_LDO_ABB_*_SETUP
#define SR2EN                       (1 << 0)
#define ACTIVE_FBB_SEL              (1 << 2)
#define SR2_WTCNT_VALUE(x)          (((x) & 0xFF) << 8)

// PRM_LDO_ABB_*_CTRL
#define OPP_SEL(x)                  ((x) & 0x3)
#define OPP_CHANGE                  (1 << 2)
#define SR2_STATUS(x)               (((x) & 0x3) << 3)
#define SR2_IN_TRANSITION           (1 << 6)

// PRM_LDO_BANDGAP_SETUP
#define BANDGAP_STARTUP_COUNT(x)    ((x) & 0xFF)

// PRM_DEVICE_OFF_CTRL
#define DEVICE_OFF_ENABLE           (1 << 0)

// PRM_VC_ERRST
#define SMPS_SA_ERR_CORE            (1 << 0)
#define SMPS_RA_ERR_CORE            (1 << 1)
#define SMPS_TIMEOUT_ERR_CORE       (1 << 2)
#define VFSM_SA_ERR_CORE            (1 << 3)
#define VFSM_RA_ERR_CORE            (1 << 4)
#define VFSM_TIMEOUT_ERR_CORE       (1 << 5)
#define SMPS_SA_ERR_IVA             (1 << 8)
#define SMPS_RA_ERR_IVA             (1 << 9)
#define SMPS_TIMEOUT_ERR_IVA        (1 << 10)
#define VFSM_SA_ERR_IVA             (1 << 11)
#define VFSM_RA_ERR_IVA             (1 << 12)
#define VFSM_TIMEOUT_ERR_IVA        (1 << 13)
#define SMPS_SA_ERR_MPU             (1 << 16)
#define SMPS_RA_ERR_MPU             (1 << 17)
#define SMPS_TIMEOUT_ERR_MPU        (1 << 18)
#define VFSM_SA_ERR_MPU             (1 << 19)
#define VFSM_RA_ERR_MPU             (1 << 20)
#define VFSM_TIMEOUT_ERR_MPU        (1 << 21)
#define BYPS_SA_ERR                 (1 << 24)
#define BYPS_RA_ERR                 (1 << 25)
#define BYPS_TIMEOUT_ERR            (1 << 26)

// *_SYS_CONFIG
#define SOFTRESET                   (1 << 0)
#define IDLEMODE(x)                 (((x) & 0x3) << 2)

// *_STATUS
#define FIFOEMPTY                   (1 << 8)

// *_CONFIGURATION
#define EVT_CAPT_EN                 (1 << 7)
#define CLAIM_1                     (1 << 28)
#define CLAIM_2                     (1 << 29)
#define CLAIM_3(x)                  (((x) & 0x3) << 30)

// *_CLASS_FILTERING
#define SNAP_CAPT_EN_00             (1 << 0)
#define SNAP_CAPT_EN_01             (1 << 1)
#define SNAP_CAPT_EN_02             (1 << 2)
#define SNAP_CAPT_EN_03             (1 << 3)

// *_TRIGGERING
#define TRIG_START_EN               (1 << 0)
#define TRIG_STOP_EN                (1 << 1)

// *_SAMPLING
#define SAMP_WIND_SIZE(x)           ((x) & 0xFF)
#define FCLK_DIV_FACOR(x)           (((x) & 0xF) << 16)

// CM_CLKSEL_CORE
// CM_CLKSEL_CORE_RESTORE
#define CLKSEL_CORE                 (1 << 0)
#define CLKSEL_L3                   (1 << 4)
#define CLKSEL_L4                   (1 << 8)

// CM_CLKSEL_ABE
#define CLKSEL_OPP(x)               ((x) & 0x3)
#define PAD_CLKS_GATE               (1 << 8)
#define SLIMBUS_CLK_GATE            (1 << 10)

// CM_DLL_CTRL
#define DLL_OVERRIDE             (1 << 0)

// CM_CLKMODE_DPLL_*
// CM_CLKMODE_DPLL_*_RESTORE
#define DPLL_EN(x)                  ((x) & 0x7)
#define DPLL_DRIFTGUARD_EN          (1 << 8)
#define DPLL_LPMODE_EN              (1 << 10)
#define DPLL_REGM4XEN               (1 << 11)
#define DPLL_SSC_EN                 (1 << 12)
#define DPLL_SSC_ACK                (1 << 13)
#define DPLL_SSC_DOWNSPREAD         (1 << 14)

// CM_IDLEST_DPLL_*
#define ST_DPLL_CLK                 (1 << 0)
#define ST_MN_BYPASS                (1 << 8)

// CM_AUTOIDLE_DPLL_*
// CM_AUTOIDLE_DPLL_*_RESTORE
#define AUTO_DPLL_MODE(x)           ((x) & 0x7)
#define DPLL_DCOCLKLDO_PWDN         (1 << 4)

// CM_CLKSEL_DPLL_*
// CM_CLKSEL_DPLL_*_RESTORE
#define DPLL_DIV(x)                 ((x) & 0x7F)
#define DPLL_MULT(x)                (((x) & 0x7FF) << 8)
#define DPLL_CLKOUTHIF_CLKSEL       (1 << 20)
#define DPLL_BYP_CLKSEL             (1 << 23)

// CM_DIV_M*_DPLL_*
// CM_DIV_M*_DPLL_*_RESTORE
#define DPLL_CLKx_DIV(x)            ((x) & 0x1F)
#define DPLL_CLKx_DIVCHACK          (1 << 5)
#define DPLL_CLKx_GATE_CTRL         (1 << 8)
#define ST_DPLL_CLKx                (1 << 9)
#define HSDIVIDER_CLKx_PWDN         (1 << 12)

// CM_SSC_DELTAMSTEP_DPLL_*
// CM_SSC_DELTAMSTEP_DPLL_*_RESTORE
#define DELTAMSTEP(x)               ((x) & 0xFFFFF)

// CM_SSC_MODFREQDIV_DPLL_*
// CM_SSC_MODFREQDIV_DPLL_*_RESTORE
#define MODFREQDIV_MANTISSA(x)      ((x) & 0x7F)
#define MODFREQDIV_EXPONENT(x)      (((x) & 0x7) << 8)

// CM_EMU_OVERRIDE_DPLL_CORE
#define CORE_DPLL_EMU_DIV(x)        ((x) & 0x7F)
#define CORE_DPLL_EMU_MULT(x)       (((x) & 0x7FF) << 8)
#define OVERRIDE_ENABLE             (1 << 19)

// CM_BYPCLK_DPLL_*
#define BYPCLK_DPLL_CLKSEL(x)       ((x) & 0x3)

// CM_SHADOW_FREQ_CONFIG1
// CM_SHADOW_FREQ_CONFIG1_RESTORE
#define FREQ_UPDATE                 (1 << 0)
#define DLL_OVERRIDE_SHADOW         (1 << 2)
#define DLL_RESET                   (1 << 3)
#define DPLL_CORE_DPLL_EN(x)        (((x) & 0x7) << 8)
#define DPLL_CORE_M2_DIV(x)         (((x) & 0xF) << 11)

// CM_SHADOW_FREQ_CONFIG2
// CM_SHADOW_FREQ_CONFIG2_RESTORE
#define GPMC_FREQ_UPDATE            (1 << 0)
#define CLKSEL_CORE_SHADOW          (1 << 1)
#define CLKSEL_L3_SHADOW            (1 << 2)
#define DPLL_CORE_M5_DIV(x)         (((x) & 0x1F) << 3)

// CM_DYN_DEP_PRESCAL
// CM_DYN_DEP_PRESCAL_RESTORE
#define PRESCAL(x)                  ((x) & 0x3F)

// CM_RESTORE_ST
#define PHASE1_COMPLETED            (1 << 0)
#define PHASE2A_COMPLETED           (1 << 1)
#define PHASE2B_COMPLETED           (1 << 2)


///////////////////////////////////////////////////////////////
//                OMAP 4430 SCRM Bit Fields
///////////////////////////////////////////////////////////////

// CLKSETUPTIME
#define SETUPTIME(x)                ((x) & 0xFFF)
#define DOWNTIME(x)                 (((x) & 0x7F) << 16)

// PMICSETUPTIME
#define SLEEPTIME(x)                ((x) & 0x7F)
#define WAKEUPTIME(x)               (((x) & 0x7F) << 16)

// ALTCLKSRC
#define MODE(x)                     ((x) & 0x3)
#define ENABLE_INT                  (1 << 2)
#define ENABLE_EXT                  (1 << 3)

// C2CCLKM
#define CLK_32KHZ                   (1 << 0)
#define SYSCLK                      (1 << 1)

// EXTCLKREQ
// PWRREQ
// AUXCLKREQ*
// C2CCLKREQ
#define POLARITY                    (1 << 0)
#define ACCURACY                    (1 << 1)
#define MAPPING(x)                  (((x) & 0x7) << 2)

// AUXCLK*
// #define POLARITY                    (1 << 0)
#define SRCSELECT(x)                (((x) & 0x3) << 1)
#define ENABLE_AUXCLK               (1 << 8)
#define DISABLECLK                  (1 << 9)
#define CLKDIV(x)                   (((x) & 0xF) << 16)

// RSTTIME_REG
#define RSTTIME(x)                  ((x) & 0xF)

// C2CRSTCTRL
#define COLDRST                     (1 << 0)
#define WARMRST                     (1 << 1)

// EXTPWRONRSTCTRL
#define ENABLE_EXTPWRON             (1 << 0)
#define PWRONRST                    (1 << 1)

// EXTWARMRSTST_REG
#define EXTWARMRSTST                (1 << 0)

// APEWARMRSTST_REG
#define APEWARMRSTST                (1 << 1)

// C2CWARMRSTST_REG
#define C2CWARMRSTST                (1 << 3)


///////////////////////////////////////////////////////////////
//                 OMAP 4430 SR Bit Fields
///////////////////////////////////////////////////////////////

// SRCONFIG
#define SENPENABLE                  (1 << 0)
#define SENNENABLE                  (1 << 1)
#define MINMAXAVGENABLE             (1 << 8)
#define ERRORGENERATORENABLE        (1 << 9)
#define SENENABLE                   (1 << 10)
#define SRENABLE                    (1 << 11)
#define SRCLKLENGTH(x)              (((x) & 0x3FF) << 12)
#define ACCUMDATA(x)                (((x) & 0x3FF) << 22)

// SRSTATUS
#define MINMAXAVGACCUMVALID         (1 << 0)
#define ERRORGENERATORVALID         (1 << 1)
#define MINMAXAVGVALID              (1 << 2)
#define AVGERRVALID                 (1 << 3)

// SENVAL
#define SENNVAL(x)                  ((x) & 0xFFFF)
#define SENPVAL(x)                  (((x) & 0xFFFF) << 16)

// SENMIN
#define SENNMIN(x)                  ((x) & 0xFFFF)
#define SENPMIN(x)                  (((x) & 0xFFFF) << 16)

// SENMAX
#define SENNMAX(x)                  ((x) & 0xFFFF)
#define SENPMAX(x)                  (((x) & 0xFFFF) << 16)

// SENAVG
#define SENNAVG(x)                  ((x) & 0xFFFF)
#define SENPAVG(x)                  (((x) & 0xFFFF) << 16)

// AVGWEIGHT
#define SENNAVGWEIGHT(x)            ((x) & 0x3)
#define SENPAVGWEIGHT(x)            (((x) & 0x3) << 2)

// NVALUERECIPROCAL
#define SENNRN(x)                   ((x) & 0xFF)
#define SENPRN(x)                   (((x) & 0xFF) << 8)
#define SENNGAIN(x)                 (((x) & 0xF) << 16)
#define SENPGAIN(x)                 (((x) & 0xF) << 20)

// IRQSTATUS_RAW
#define MCUDISABLEACKINTSTATRAW     (1 << 0)
#define MCUBOUNDSINTSTATRAW         (1 << 1)
#define MCUVALIDINTSTATRAW          (1 << 2)
#define MCUACCUMINTSTATRAW          (1 << 3)

// IRQSTATUS
#define MCUDISABLEACKINTSTATENA     (1 << 0)
#define MCUBOUNDSINTSTATENA         (1 << 1)
#define MCUVALIDINTSTATENA          (1 << 2)
#define MCUACCUMINTSTATENA          (1 << 3)

// IRQENABLE_SET
#define MCUDISABLEACTINTENASET      (1 << 0)
#define MCUBOUNDSINTENASET          (1 << 1)
#define MCUVALIDINTENASET           (1 << 2)
#define MCUACCUMINTENASET           (1 << 3)

// IRQENABLE_CLR
#define MCUDISABLEACTINTENACLR      (1 << 0)
#define MCUBOUNDSINTENACLR          (1 << 1)
#define MCUVALIDINTENACLR           (1 << 2)
#define MCUACCUMINTENACLR           (1 << 3)

// SENERROR
#define SENERROR(x)                 ((x) & 0xFF)
#define AVGERROR(x)                 (((x) & 0xFF) << 8)

// ERRCONFIG
#define ERRMINLIMIT(x)              ((x) & 0xFF)
#define ERRMAXLIMIT(x)              (((x) & 0xFF) << 8)
#define ERRWEIGHT(x)                (((x) & 0x7) << 16)
#define VPBOUNDSINTENABLE           (1 << 22)
#define VPBOUNDSINTSTATENA          (1 << 23)
#define IDLEMODE_ERRCONFIG(x)       (((x) & 0x3) << 24)
#define WAKEUPENABLE                (1 << 26)
 

///////////////////////////////////////////////////////////////
//              OMAP 4430 Control Bit Fields
///////////////////////////////////////////////////////////////

//
// FUSE and identification registers are skipped!
// 

// CONTROL_*_SYSCONFIG
#define IP_SYSCONFIG_IDLEMODE(x)    (((x) & 0x3) << 2)

// CONTROL_SEC_ERR_STATUS_DEBUG
#define L3RAM_DBGFW_ERROR           (1 << 1)
#define GPMC_DBGFW_ERROR            (1 << 2)
#define EMIF_DBGFW_ERROR            (1 << 3)
#define IVAHD_DBGFW_ERROR           (1 << 4)
#define DUAL_CORTEX_M3_DBGFW_ERROR  (1 << 5)
#define SL2_DBGFW_ERROR             (1 << 6)
#define C2C_DBGFW_ERROR             (1 << 12)
#define SGX_DBGFW_ERROR             (1 << 13)
#define DSS_DBGFW_ERROR             (1 << 14)
#define ISS_DBGFW_ERROR             (1 << 15)
#define L4_PERIPH_DBGFW_ERROR       (1 << 16)
#define L4_CONFIG_DBGFW_ERROR       (1 << 17)
#define DEBUGSS_DBGFW_ERROR         (1 << 18)
#define L4_AUDIOBE_DBGFW_ERROR      (1 << 19)
#define C2C_INIT_DBGFW_ERROR        (1 << 20)

// CONTROL_DEV_CONF
#define USBPHY_PD                   (1 << 0)

// CONTROL_DSP_BOOTADDR
#define DSP_BOOT_LOAD_ADDR(x)       (((x) & 0x3FFFFF) << 10)

// CONTROL_LDOVBB_IVA_VOLTAGE_CTRL
#define LDOVBBIVA_FBB_VSET_OUT      ((x) & 0x1F)
#define LDOVBBIVA_FBB_VSET_IN       (((x) & 0x1F) << 5)
#define LDOVBBIVA_FBB_MUX_CTRL      (1 << 10)

// CONTROL_LDOVBB_MPU_VOLTAGE_CTRL
#define LDOVBBMPU_FBB_VSET_OUT      ((x) & 0x1F)
#define LDOVBBMPU_FBB_VSET_IN       (((x) & 0x1F) << 5)
#define LDOVBBMPU_FBB_MUX_CTRL      (1 << 10)

// CONTROL_LDOSRAM_IVA_VOLTAGE_CTRL
#define LDOSRAMIVA_ACTMODE_VSET_OUT(x)      ((x) & 0x1F)
#define LDOSRAMIVA_ACTMODE_VSET_IN(x)       (((x) & 0x1F) << 5)
#define LDOSRAMIVA_ACTMODE_MUX_CTRL         (1 << 10)
#define LDOSRAMIVA_RETMODE_VSET_OUT(x)      (((x) & 0x1F) << 16)
#define LDOSRAMIVA_RETMODE_VSET_IN(x)       (((x) & 0x1F) << 21)
#define LDOSRAMIVA_RETMODE_MUX_CTRL         (1 << 26)

// CONTROL_LDOSRAM_MPU_VOLTAGE_CTRL
#define LDOSRAMMPU_ACTMODE_VSET_OUT(x)      ((x) & 0x1F)
#define LDOSRAMMPU_ACTMODE_VSET_IN(x)       (((x) & 0x1F) << 5)
#define LDOSRAMMPU_ACTMODE_MUX_CTRL         (1 << 10)
#define LDOSRAMMPU_RETMODE_VSET_OUT(x)      (((x) & 0x1F) << 16)
#define LDOSRAMMPU_RETMODE_VSET_IN(x)       (((x) & 0x1F) << 21)
#define LDOSRAMMPU_RETMODE_MUX_CTRL         (1 << 26)

// CONTROL_LDOSRAM_CORE_VOLTAGE_CTRL
#define LDOSRAMCORE_ACTMODE_VSET_OUT(x)      ((x) & 0x1F)
#define LDOSRAMCORE_ACTMODE_VSET_IN(x)       (((x) & 0x1F) << 5)
#define LDOSRAMCORE_ACTMODE_MUX_CTRL         (1 << 10)
#define LDOSRAMCORE_RETMODE_VSET_OUT(x)      (((x) & 0x1F) << 16)
#define LDOSRAMCORE_RETMODE_VSET_IN(x)       (((x) & 0x1F) << 21)
#define LDOSRAMCORE_RETMODE_MUX_CTRL         (1 << 26)

// CONTROL_TEMP_SENSOR
#define BGAP_TEMP_SENSOR_DTEMP(x)           ((x) & 0xFF)
#define BGAP_TEMP_SENSOR_EOCZ               (1 << 8)
#define BGAP_TEMP_SENSOR_SOC                (1 << 9)
#define BGAP_TEMP_SENSOR_CONTCONV           (1 << 10)
#define BGAP_TSHUT                          (1 << 11)
#define BGAP_TEMPSOFF                       (1 << 12)

// CONTROL_DPLL_NWELL_TRIM_0
#define DPLL_MPU_NWELL_TRIM(x)              ((x) & 0x1F)
#define DPLL_MPU_NWELL_TRIM_MUX_CTRL        (1 << 5)
#define DPLL_IVA_NWELL_TRIM(x)              (((x) & 0x1F) << 6)
#define DPLL_IVA_NWELL_TRIM_MUX_CTRL        (1 << 11)
#define DPLL_CORE_NWELL_TRIM(x)             (((x) & 0x1F) << 12)
#define DPLL_CORE_NWELL_TRIM_MUX_CTRL       (1 << 17)
#define DPLL_PER_NWELL_TRIM(x)              (((x) & 0x1F) << 18)
#define DPLL_PER_NWELL_TRIM_MUX_CTRL        (1 << 23)
#define DPLL_ABE_NWELL_TRIM(x)              (((x) & 0x1F) << 24)
#define DPLL_ABE_NWELL_TRIM_MUX_CTRL        (1 << 29)

// CONTROL_DPLL_NWELL_TRIM_1
#define DPLL_DSI1_NWELL_TRIM(x)             ((x) & 0x1F)
#define DPLL_DSI1_NWELL_TRIM_MUX_CTRL       (1 << 5)
#define DPLL_DSI2_NWELL_TRIM(x)             (((x) & 0x1F) << 6)
#define DPLL_DSI2_NWELL_TRIM_MUX_CTRL       (1 << 11)
#define DPLL_USB_NWELL_TRIM(x)              (((x) & 0x1F) << 18)
#define DPLL_USB_NWELL_TRIM_MUX_CTRL        (1 << 23)

// CONTROL_USBOTGHS_CONTROL
#define AVALID                              (1 << 0)
#define BVALID                              (1 << 1)
#define VBUSVALID                           (1 << 2)
#define SESSEND                             (1 << 3)
#define IDDIG                               (1 << 4)
#define IDPULLUP                            (1 << 5)
#define DRVVBUS                             (1 << 6)
#define CHRGVBUS                            (1 << 7)
#define DISCHRGVBUS                         (1 << 8)

// CONTROL_DSS_CONTROL
#define DSS_MUX6_SELECT                     (1 << 0)

// CONTROL_CORTEX_M3_MMUADDRTRANSLTR
#define CORTEX_M3_MMUADDRTRANSLTR(x)        ((x) & 0xFFFFF)

// CONTROL_CORTEX_M3_MMUADDRLOGICTR
#define CORTEX_M3_MMUADDRLOGICTR(x)         ((x) & 0xFFFFF)

// CONTROL_HWOBS_CONTROL
#define HWOBS_MACRO_ENABLE                  (1 << 0)
#define HWOBS_ALL_ONE_MODE                  (1 << 1)
#define HWOBS_ALL_ZERO_MODE                 (1 << 2)
#define HWOBS_CLKDIV_SEL(x)                 (((x) & 0x1F) << 3)

// CONTROL_GEN_CORE_OCPREG_SPARE
// CONTROL_OCPREG_SPARE
#define OCPREG_SPARE(n)                     (1 << (n))

// CONTROL_WKUP_PROT_EMIF1_SDRAM_CONFIG_REG
// CONTROL_WKUP_PROT_EMIF2_SDRAM_CONFIG_REG
#define EMIF_SDRAM_PAGESIZE(x)             ((x) & 0x7)
#define EMIF_SDRAM_EBANK                   (1 << 3)
#define EMIF_SDRAM_IBANK(x)                (((x) & 0x7) << 4)
#define EMIF_SDRAM_ROWSIZE(x)              (((x) & 0x7) << 7)
#define EMIF_SDRAM_CL(x)                   (((x) & 0xF) << 10)
#define EMIF_SDRAM_NARROW_MODE(x)          (((x) & 0x3) << 14)
#define EMIF_SDRAM_DDR_DISABLE_DLL         (1 << 20)
#define EMIF_SDRAM_DDR2_DDQS               (1 << 23)
#define EMIF_SDRAM_IBANK_POS(x)            (((x) & 0x3) << 27)
#define EMIF_SDRAM_TYPE(x)                 (((x) & 0x7) << 29)

// CONTROL_WKUP_PROT_EMIF1_SDRAM_CONFIG2_REG
// CONTROL_WKUP_PROT_EMIF2_SDRAM_CONFIG2_REG
#define EMIF_SDRAM_RDBSIZE(x)              ((x) & 0x7)
#define EMIF_SDRAM_RDBNUM(x)               (((x) & 0x3) << 4)
#define EMIF_SDRAM_CS1NVMEN                (1 << 30)

// Bit map for pad configuration registers. Fields with suffix 1
// are dedicated to first pad in the name of a register, with suffix 2 -
// to the second pad. 
// For example, register CONTROL_CORE_PAD0_GPIO63_PAD1_GPIO64,
// as it can be seen from the name of register, controls pads PAD0_GPIO63 
// and PAD1_GPIO64. Fields MUXMODE1, PULLUDENABLE1, ..., WAKEUPEVENT1
// configure PAD0_GPIO63 and MUXMODE2, PULLUDENABLE2, ..., WAKEUPEVENT2
// configure PAD1_GPIO64.
#define MUXMODE1(x)                             ((x) & 0x7)
#define PULLUDENABLE1                           (1 << 3)
#define PULLTYPESELECT1                         (1 << 4)
#define INPUTENABLE1                            (1 << 8)
#define OFFMODEENABLE1                          (1 << 9)
#define OFFMODEOUTENABLE1                       (1 << 10)
#define OFFMODEOUTVALUE1                        (1 << 11)
#define OFFMODEPULLUDENABLE1                    (1 << 12)
#define OFFMODEPULLTYPESELECT1                  (1 << 13)
#define WAKEUPENABLE1                           (1 << 14)
#define WAKEUPEVENT1                            (1 << 15)
#define MUXMODE2(x)                             (((x) & 0x7) << 16)
#define PULLUDENABLE2                           (1 << 19)
#define PULLTYPESELECT2                         (1 << 20)
#define INPUTENABLE2                            (1 << 24)
#define OFFMODEENABLE2                          (1 << 25)
#define OFFMODEOUTENABLE2                       (1 << 26)
#define OFFMODEOUTVALUE2                        (1 << 27)
#define OFFMODEPULLUDENABLE2                    (1 << 28)
#define OFFMODEPULLTYPESELECT2                  (1 << 29)
#define WAKEUPENABLE2                           (1 << 30)
#define WAKEUPEVENT2                            (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_0
#define GPMC_AD0_DUPLICATEWAKEUPEVENT           (1 << 0)
#define GPMC_AD1_DUPLICATEWAKEUPEVENT           (1 << 1)
#define GPMC_AD2_DUPLICATEWAKEUPEVENT           (1 << 2)
#define GPMC_AD3_DUPLICATEWAKEUPEVENT           (1 << 3)
#define GPMC_AD4_DUPLICATEWAKEUPEVENT           (1 << 4)
#define GPMC_AD5_DUPLICATEWAKEUPEVENT           (1 << 5)
#define GPMC_AD6_DUPLICATEWAKEUPEVENT           (1 << 6)
#define GPMC_AD7_DUPLICATEWAKEUPEVENT           (1 << 7)
#define GPMC_AD8_DUPLICATEWAKEUPEVENT           (1 << 8)
#define GPMC_AD9_DUPLICATEWAKEUPEVENT           (1 << 9)
#define GPMC_AD10_DUPLICATEWAKEUPEVENT          (1 << 10)
#define GPMC_AD11_DUPLICATEWAKEUPEVENT          (1 << 11)
#define GPMC_AD12_DUPLICATEWAKEUPEVENT          (1 << 12)
#define GPMC_AD13_DUPLICATEWAKEUPEVENT          (1 << 13)
#define GPMC_AD14_DUPLICATEWAKEUPEVENT          (1 << 14)
#define GPMC_AD15_DUPLICATEWAKEUPEVENT          (1 << 15)
#define GPMC_AD16_DUPLICATEWAKEUPEVENT          (1 << 16)
#define GPMC_AD17_DUPLICATEWAKEUPEVENT          (1 << 17)
#define GPMC_AD18_DUPLICATEWAKEUPEVENT          (1 << 18)
#define GPMC_AD19_DUPLICATEWAKEUPEVENT          (1 << 19)
#define GPMC_AD20_DUPLICATEWAKEUPEVENT          (1 << 20)
#define GPMC_AD21_DUPLICATEWAKEUPEVENT          (1 << 21)
#define GPMC_AD22_DUPLICATEWAKEUPEVENT          (1 << 22)
#define GPMC_AD23_DUPLICATEWAKEUPEVENT          (1 << 23)
#define GPMC_AD24_DUPLICATEWAKEUPEVENT          (1 << 24)
#define GPMC_AD25_DUPLICATEWAKEUPEVENT          (1 << 25)
#define GPMC_NCS0_DUPLICATEWAKEUPEVENT          (1 << 26)
#define GPMC_NCS1_DUPLICATEWAKEUPEVENT          (1 << 27)
#define GPMC_NCS2_DUPLICATEWAKEUPEVENT          (1 << 28)
#define GPMC_NCS3_DUPLICATEWAKEUPEVENT          (1 << 29)
#define GPMC_NWP_DUPLICATEWAKEUPEVENT           (1 << 30)
#define GPMC_CLK_DUPLICATEWAKEUPEVENT           (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_1
#define GPMC_NADV_ALE_DUPLICATEWAKEUPEVENT      (1 << 0)
#define GPMC_NOE_DUPLICATEWAKEUPEVENT           (1 << 1)
#define GPMC_NWE_DUPLICATEWAKEUPEVENT           (1 << 2)
#define GPMC_NBE0_CLE_DUPLICATEWAKEUPEVENT      (1 << 3)
#define GPMC_NBE1_DUPLICATEWAKEUPEVENT          (1 << 4)
#define GPMC_WAIT0_DUPLICATEWAKEUPEVENT         (1 << 5)
#define GPMC_WAIT1_DUPLICATEWAKEUPEVENT         (1 << 6)
#define GPMC_WAIT2_DUPLICATEWAKEUPEVENT         (1 << 7)
#define GPMC_NCS4_DUPLICATEWAKEUPEVENT          (1 << 8)
#define GPMC_NCS5_DUPLICATEWAKEUPEVENT          (1 << 9)
#define GPMC_NCS6_DUPLICATEWAKEUPEVENT          (1 << 10)
#define GPMC_NCS7_DUPLICATEWAKEUPEVENT          (1 << 11)
#define GPIO63_DUPLICATEWAKEUPEVENT             (1 << 12)
#define GPIO64_DUPLICATEWAKEUPEVENT             (1 << 13)
#define GPIO65_DUPLICATEWAKEUPEVENT             (1 << 14)
#define GPIO66_DUPLICATEWAKEUPEVENT             (1 << 15)
#define CSI21_DX0_DUPLICATEWAKEUPEVENT          (1 << 16)
#define CSI21_DY0_DUPLICATEWAKEUPEVENT          (1 << 17)
#define CSI21_DX1_DUPLICATEWAKEUPEVENT          (1 << 18)
#define CSI21_DY1_DUPLICATEWAKEUPEVENT          (1 << 19)
#define CSI21_DX2_DUPLICATEWAKEUPEVENT          (1 << 20)
#define CSI21_DY2_DUPLICATEWAKEUPEVENT          (1 << 21)
#define CSI21_DX3_DUPLICATEWAKEUPEVENT          (1 << 22)
#define CSI21_DY3_DUPLICATEWAKEUPEVENT          (1 << 23)
#define CSI21_DX4_DUPLICATEWAKEUPEVENT          (1 << 24)
#define CSI21_DY4_DUPLICATEWAKEUPEVENT          (1 << 25)
#define CSI22_DX0_DUPLICATEWAKEUPEVENT          (1 << 26)
#define CSI22_DY0_DUPLICATEWAKEUPEVENT          (1 << 27)
#define CSI22_DX1_DUPLICATEWAKEUPEVENT          (1 << 28)
#define CSI22_DY1_DUPLICATEWAKEUPEVENT          (1 << 29)
#define CAM_SHUTTER_DUPLICATEWAKEUPEVENT        (1 << 30)
#define CAM_STROBE_DUPLICATEWAKEUPEVENT         (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_2
#define CAM_GLOBALRESET_DUPLICATEWAKEUPEVENT    (1 << 0)
#define USBB1_ULPITLL_CLK_DUPLICATEWAKEUPEVENT  (1 << 1)
#define USBB1_ULPITLL_STP_DUPLICATEWAKEUPEVENT  (1 << 2)
#define USBB1_ULPITLL_DIR_DUPLICATEWAKEUPEVENT  (1 << 3)
#define USBB1_ULPITLL_NXT_DUPLICATEWAKEUPEVENT  (1 << 4)
#define USBB1_ULPITLL_DAT0_DUPLICATEWAKEUPEVENT (1 << 5)
#define USBB1_ULPITLL_DAT1_DUPLICATEWAKEUPEVENT (1 << 6)
#define USBB1_ULPITLL_DAT2_DUPLICATEWAKEUPEVENT (1 << 7)
#define USBB1_ULPITLL_DAT3_DUPLICATEWAKEUPEVENT (1 << 8)
#define USBB1_ULPITLL_DAT4_DUPLICATEWAKEUPEVENT (1 << 9)
#define USBB1_ULPITLL_DAT5_DUPLICATEWAKEUPEVENT (1 << 10)
#define USBB1_ULPITLL_DAT6_DUPLICATEWAKEUPEVENT (1 << 11)
#define USBB1_ULPITLL_DAT7_DUPLICATEWAKEUPEVENT (1 << 12)
#define USBB1_HSIC_DATA_DUPLICATEWAKEUPEVENT    (1 << 13)
#define USBB1_HSIC_STROBE_DUPLICATEWAKEUPEVENT  (1 << 14)
#define USBC1_ICUSB_DP_DUPLICATEWAKEUPEVENT     (1 << 15)
#define USBC1_ICUSB_DM_DUPLICATEWAKEUPEVENT     (1 << 16)
#define SDMMC1_CLK_DUPLICATEWAKEUPEVENT         (1 << 17)
#define SDMMC1_CMD_DUPLICATEWAKEUPEVENT         (1 << 18)
#define SDMMC1_DAT0_DUPLICATEWAKEUPEVENT        (1 << 19)
#define SDMMC1_DAT1_DUPLICATEWAKEUPEVENT        (1 << 20)
#define SDMMC1_DAT2_DUPLICATEWAKEUPEVENT        (1 << 21)
#define SDMMC1_DAT3_DUPLICATEWAKEUPEVENT        (1 << 22)
#define SDMMC1_DAT4_DUPLICATEWAKEUPEVENT        (1 << 23)
#define SDMMC1_DAT5_DUPLICATEWAKEUPEVENT        (1 << 24)
#define SDMMC1_DAT6_DUPLICATEWAKEUPEVENT        (1 << 25)
#define SDMMC1_DAT7_DUPLICATEWAKEUPEVENT        (1 << 26)
#define ABE_MCBSP2_CLKX_DUPLICATEWAKEUPEVENT    (1 << 27)
#define ABE_MCBSP2_DR_DUPLICATEWAKEUPEVENT      (1 << 28)
#define ABE_MCBSP2_DX_DUPLICATEWAKEUPEVENT      (1 << 29)
#define ABE_MCBSP2_FSX_DUPLICATEWAKEUPEVENT     (1 << 30)
#define ABE_MCBSP1_CLKX_DUPLICATEWAKEUPEVENT    (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_3
#define ABE_MCBSP1_DR_DUPLICATEWAKEUPEVENT      (1 << 0)
#define ABE_MCBSP1_DX_DUPLICATEWAKEUPEVENT      (1 << 1)
#define ABE_MCBSP1_FSX_DUPLICATEWAKEUPEVENT     (1 << 2)
#define ABE_PDM_UL_DATA_DUPLICATEWAKEUPEVENT    (1 << 3)
#define ABE_PDM_DL_DATA_DUPLICATEWAKEUPEVENT    (1 << 4)
#define ABE_PDM_FRAME_DUPLICATEWAKEUPEVENT      (1 << 5)
#define ABE_PDM_LB_CLK_DUPLICATEWAKEUPEVENT     (1 << 6)
#define ABE_CLKS_DUPLICATEWAKEUPEVENT           (1 << 7)
#define ABE_DMIC_CLK1_DUPLICATEWAKEUPEVENT      (1 << 8)
#define ABE_DMIC_DIN1_DUPLICATEWAKEUPEVENT      (1 << 9)
#define ABE_DMIC_DIN2_DUPLICATEWAKEUPEVENT      (1 << 10)
#define ABE_DMIC_DIN3_DUPLICATEWAKEUPEVENT      (1 << 11)
#define UART2_CTS_DUPLICATEWAKEUPEVENT          (1 << 12)
#define UART2_RTS_DUPLICATEWAKEUPEVENT          (1 << 13)
#define UART2_RX_DUPLICATEWAKEUPEVENT           (1 << 14)
#define UART2_TX_DUPLICATEWAKEUPEVENT           (1 << 15)
#define HDQ_SIO_DUPLICATEWAKEUPEVENT            (1 << 16)
#define I2C1_SCL_DUPLICATEWAKEUPEVENT           (1 << 17)
#define I2C1_SDA_DUPLICATEWAKEUPEVENT           (1 << 18)
#define I2C2_SCL_DUPLICATEWAKEUPEVENT           (1 << 19)
#define I2C2_SDA_DUPLICATEWAKEUPEVENT           (1 << 20)
#define I2C3_SCL_DUPLICATEWAKEUPEVENT           (1 << 21)
#define I2C3_SDA_DUPLICATEWAKEUPEVENT           (1 << 22)
#define I2C4_SCL_DUPLICATEWAKEUPEVENT           (1 << 23)
#define I2C4_SDA_DUPLICATEWAKEUPEVENT           (1 << 24)
#define MCSPI1_CLK_DUPLICATEWAKEUPEVENT         (1 << 25)
#define MCSPI1_SOMI_DUPLICATEWAKEUPEVENT        (1 << 26)
#define MCSPI1_SIMO_DUPLICATEWAKEUPEVENT        (1 << 27)
#define MCSPI1_CS0_DUPLICATEWAKEUPEVENT         (1 << 28)
#define MCSPI1_CS1_DUPLICATEWAKEUPEVENT         (1 << 29)
#define MCSPI1_CS2_DUPLICATEWAKEUPEVENT         (1 << 30)
#define MCSPI1_CS3_DUPLICATEWAKEUPEVENT         (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_4
#define UART3_CTS_RCTX_DUPLICATEWAKEUPEVENT     (1 << 0)
#define UART3_RTS_SD_DUPLICATEWAKEUPEVENT       (1 << 1)
#define UART3_RX_IRRX_DUPLICATEWAKEUPEVENT      (1 << 2)
#define UART3_TX_IRTX_DUPLICATEWAKEUPEVENT      (1 << 3)
#define SDMMC5_CLK_DUPLICATEWAKEUPEVENT         (1 << 4)
#define SDMMC5_CMD_DUPLICATEWAKEUPEVENT         (1 << 5)
#define SDMMC5_DAT0_DUPLICATEWAKEUPEVENT        (1 << 6)
#define SDMMC5_DAT1_DUPLICATEWAKEUPEVENT        (1 << 7)
#define SDMMC5_DAT2_DUPLICATEWAKEUPEVENT        (1 << 8)
#define SDMMC5_DAT3_DUPLICATEWAKEUPEVENT        (1 << 9)
#define MCSPI4_CLK_DUPLICATEWAKEUPEVENT         (1 << 10)
#define MCSPI4_SIMO_DUPLICATEWAKEUPEVENT        (1 << 11)
#define MCSPI4_SOMI_DUPLICATEWAKEUPEVENT        (1 << 12)
#define MCSPI4_CS0_DUPLICATEWAKEUPEVENT         (1 << 13)
#define UART4_RX_DUPLICATEWAKEUPEVENT           (1 << 14)
#define UART4_TX_DUPLICATEWAKEUPEVENT           (1 << 15)
#define USBB2_ULPITLL_CLK_DUPLICATEWAKEUPEVENT  (1 << 16)
#define USBB2_ULPITLL_STP_DUPLICATEWAKEUPEVENT  (1 << 17)
#define USBB2_ULPITLL_DIR_DUPLICATEWAKEUPEVENT  (1 << 18)
#define USBB2_ULPITLL_NXT_DUPLICATEWAKEUPEVENT  (1 << 19)
#define USBB2_ULPITLL_DAT0_DUPLICATEWAKEUPEVENT (1 << 20)
#define USBB2_ULPITLL_DAT1_DUPLICATEWAKEUPEVENT (1 << 21)
#define USBB2_ULPITLL_DAT2_DUPLICATEWAKEUPEVENT (1 << 22)
#define USBB2_ULPITLL_DAT3_DUPLICATEWAKEUPEVENT (1 << 23)
#define USBB2_ULPITLL_DAT4_DUPLICATEWAKEUPEVENT (1 << 24)
#define USBB2_ULPITLL_DAT5_DUPLICATEWAKEUPEVENT (1 << 25)
#define USBB2_ULPITLL_DAT6_DUPLICATEWAKEUPEVENT (1 << 26)
#define USBB2_ULPITLL_DAT7_DUPLICATEWAKEUPEVENT (1 << 27)
#define USBB2_HSIC_DATA_DUPLICATEWAKEUPEVENT    (1 << 28)
#define USBB2_HSIC_STROBE_DUPLICATEWAKEUPEVENT  (1 << 29)
#define KPD_COL3_DUPLICATEWAKEUPEVENT           (1 << 30)
#define KPD_COL4_DUPLICATEWAKEUPEVENT           (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_5
#define KPD_COL5_DUPLICATEWAKEUPEVENT           (1 << 0)
#define KPD_COL0_DUPLICATEWAKEUPEVENT           (1 << 1)
#define KPD_COL1_DUPLICATEWAKEUPEVENT           (1 << 2)
#define KPD_COL2_DUPLICATEWAKEUPEVENT           (1 << 3)
#define KPD_ROW3_DUPLICATEWAKEUPEVENT           (1 << 4)
#define KPD_ROW4_DUPLICATEWAKEUPEVENT           (1 << 5)
#define KPD_ROW5_DUPLICATEWAKEUPEVENT           (1 << 6)
#define KPD_ROW0_DUPLICATEWAKEUPEVENT           (1 << 7)
#define KPD_ROW1_DUPLICATEWAKEUPEVENT           (1 << 8)
#define KPD_ROW2_DUPLICATEWAKEUPEVENT           (1 << 9)
#define USBA0_OTG_DP_DUPLICATEWAKEUPEVENT       (1 << 10)
#define FREF_CLK1_OUT_DUPLICATEWAKEUPEVENT      (1 << 11)
#define FREF_CLK2_OUT_DUPLICATEWAKEUPEVENT      (1 << 12)
#define SYS_NIRQ1_DUPLICATEWAKEUPEVENT          (1 << 13)
#define SYS_NIRQ2_DUPLICATEWAKEUPEVENT          (1 << 14)
#define SYS_BOOT0_DUPLICATEWAKEUPEVENT          (1 << 15)
#define SYS_BOOT1_DUPLICATEWAKEUPEVENT          (1 << 16)
#define SYS_BOOT2_DUPLICATEWAKEUPEVENT          (1 << 17)
#define SYS_BOOT3_DUPLICATEWAKEUPEVENT          (1 << 18)
#define SYS_BOOT4_DUPLICATEWAKEUPEVENT          (1 << 19)
#define SYS_BOOT5_DUPLICATEWAKEUPEVENT          (1 << 20)
#define DPM_EMU0_DUPLICATEWAKEUPEVENT           (1 << 21)
#define DPM_EMU1_DUPLICATEWAKEUPEVENT           (1 << 22)
#define DPM_EMU2_DUPLICATEWAKEUPEVENT           (1 << 23)
#define DPM_EMU3_DUPLICATEWAKEUPEVENT           (1 << 24)
#define DPM_EMU4_DUPLICATEWAKEUPEVENT           (1 << 25)
#define DPM_EMU5_DUPLICATEWAKEUPEVENT           (1 << 26)
#define DPM_EMU6_DUPLICATEWAKEUPEVENT           (1 << 27)
#define DPM_EMU7_DUPLICATEWAKEUPEVENT           (1 << 28)
#define DPM_EMU8_DUPLICATEWAKEUPEVENT           (1 << 29)
#define DPM_EMU9_DUPLICATEWAKEUPEVENT           (1 << 30)
#define DPM_EMU10_DUPLICATEWAKEUPEVENT          (1 << 31)

// CONTROL_PADCONF_WAKEUPEVENT_6
#define DPM_EMU11_DUPLICATEWAKEUPEVENT          (1 << 0)
#define DPM_EMU12_DUPLICATEWAKEUPEVENT          (1 << 1)
#define DPM_EMU13_DUPLICATEWAKEUPEVENT          (1 << 2)
#define DPM_EMU14_DUPLICATEWAKEUPEVENT          (1 << 3)
#define DPM_EMU15_DUPLICATEWAKEUPEVENT          (1 << 4)
#define DPM_EMU16_DUPLICATEWAKEUPEVENT          (1 << 5)
#define DPM_EMU17_DUPLICATEWAKEUPEVENT          (1 << 6)
#define DPM_EMU18_DUPLICATEWAKEUPEVENT          (1 << 7)
#define DPM_EMU19_DUPLICATEWAKEUPEVENT          (1 << 8)

// CONTROL_PADCONF_GLOBAL
#define FORCE_OFFMODE_EN                        (1 << 31)

// CONTROL_CORE_PADCONF_MODE
#define VDDS_DV_BANK2_SHARED0                   (1 << 19)
#define VDDS_DV_GPMC1                           (1 << 20)
#define VDDS_DV_BANK7                           (1 << 21)
#define VDDS_DV_SDMMC2                          (1 << 22)
#define VDDS_DV_GPMC0                           (1 << 23)
#define VDDS_DV_CAM                             (1 << 24)
#define VDDS_DV_C2C                             (1 << 25)
#define VDDS_DV_BANK6                           (1 << 26)
#define VDDS_DV_BANK5                           (1 << 27)
#define VDDS_DV_BANK4                           (1 << 28)
#define VDDS_DV_BANK3                           (1 << 29)
#define VDDS_DV_BANK1                           (1 << 30)
#define VDDS_DV_BANK0                           (1 << 31)

// CONTROL_SMART1IO_PADCONF_0
#define UART3_DR1_SC(x)                     (((x) & 0x3) << 4)
#define UART3_DR0_SC(x)                     (((x) & 0x3) << 6)
#define UART1_DR0_SC(x)                     (((x) & 0x3) << 8)
#define MCSPI1_DR0_SC(x)                    (((x) & 0x3) << 10)
#define GPIO_63_64_DR0_SC(x)                (((x) & 0x3) << 12)
#define GPMC_DR3_SC(x)                      (((x) & 0x3) << 16)
#define GPIO_DR9_SC(x)                      (((x) & 0x3) << 20)
#define GPIO_DR8_SC(x)                      (((x) & 0x3) << 22)
#define CAM_DR0_SC(x)                       (((x) & 0x3) << 28)
#define ABE_DR0_SC(x)                       (((x) & 0x3) << 30)

// CONTROL_SMART1IO_PADCONF_1
#define UART3_DR1_LB(x)                     (((x) & 0x3) << 4)
#define UART3_DR0_LB(x)                     (((x) & 0x3) << 6)
#define UART1_DR0_LB(x)                     (((x) & 0x3) << 8)
#define MCSPI1_DR0_LB(x)                    (((x) & 0x3) << 10)
#define GPIO_63_64_DR0_LB(x)                (((x) & 0x3) << 12)
#define GPMC_DR3_LB(x)                      (((x) & 0x3) << 16)
#define GPIO_DR9_LB(x)                      (((x) & 0x3) << 20)
#define GPIO_DR8_LB(x)                      (((x) & 0x3) << 22)
#define CAM_DR0_LB(x)                       (((x) & 0x3) << 28)
#define ABE_DR0_LB(x)                       (((x) & 0x3) << 30)

// CONTROL_SMART3IO_PADCONF_0
#define SPI2_DR0_MB(x)                      (((x) & 0x3) << 0)
#define SDMMC3_DR0_MB(x)                    (((x) & 0x3) << 8)
#define MCSPI4_DR1_MB(x)                    (((x) & 0x3) << 10)
#define MCSPI4_DR0_MB(x)                    (((x) & 0x3) << 12)
#define MCBSP2_DR0_MB(x)                    (((x) & 0x3) << 14)
#define HSI_DR3_MB(x)                       (((x) & 0x3) << 16)
#define HSI_DR2_MB(x)                       (((x) & 0x3) << 18)
#define HSI_DR1_MB(x)                       (((x) & 0x3) << 20)
#define GPIO_DR6_MB(x)                      (((x) & 0x3) << 22)
#define GPIO_DR5_MB(x)                      (((x) & 0x3) << 24)
#define GPIO_DR4_MB(x)                      (((x) & 0x3) << 26)
#define GPIO_DR3_MB(x)                      (((x) & 0x3) << 28)
#define DMIC_DR0_MB(x)                      (((x) & 0x3) << 30)

// CONTROL_SMART3IO_PADCONF_1
#define USBB1_DR2_MB(x)                     (((x) & 0x3) << 0)
#define FREF_DR3_MB(x)                      (((x) & 0x3) << 2)
#define FREF_DR2_MB(x)                      (((x) & 0x3) << 4)
#define PDM_DR0_MB(x)                       (((x) & 0x3) << 6)
#define GPMC_DR1_MB(x)                      (((x) & 0x3) << 12)
#define HSI_DR0_MB(x)                       (((x) & 0x3) << 20)
#define UART4_DR0_MB(x)                     (((x) & 0x3) << 22)
#define UART2_DR1_MB(x)                     (((x) & 0x3) << 24)
#define UART2_DR0_MB(x)                     (((x) & 0x3) << 26)
#define SPI2_DR2_MB(x)                      (((x) & 0x3) << 28)
#define SPI2_DR1_MB(x)                      (((x) & 0x3) << 30)

// CONTROL_SMART3IO_PADCONF_2
#define USBB1_DR2_LB                        (1 << 4)
#define SDMMC3_DR0_LB                       (1 << 5)
#define FREF_DR2_LB                         (1 << 6)
#define PDM_DR0_LB                          (1 << 7)
#define GPMC_DR1_LB                         (1 << 8)
#define FREF_DR3_LB                         (1 << 9)
#define HSI_DR0_LB                          (1 << 10)
#define UART4_DR0_LB                        (1 << 11)
#define UART2_DR1_LB                        (1 << 12)
#define UART2_DR0_LB                        (1 << 13)
#define SPI2_DR2_LB                         (1 << 14)
#define SPI2_DR1_LB                         (1 << 15)
#define SPI2_DR0_LB                         (1 << 16)
#define MCSPI4_DR1_LB                       (1 << 21)
#define MCSPI4_DR0_LB                       (1 << 22)
#define MCBSP2_DR0_LB                       (1 << 23)
#define HSI_DR3_LB                          (1 << 24)
#define HSI_DR2_LB                          (1 << 25)
#define HSI_DR1_LB                          (1 << 26)
#define GPIO_DR6_LB                         (1 << 27)
#define GPIO_DR5_LB                         (1 << 28)
#define GPIO_DR4_LB                         (1 << 29)
#define GPIO_DR3_LB                         (1 << 30)
#define DMIC_DR0_LB                         (1 << 31)

// CONTROL_USBB_HSIC
#define USBB2_HSIC_STROBE_OFFMODE_WD(x)     (((x) & 0x3) << 2)
#define USBB2_HSIC_STROBE_OFFMODE_WD_ENABLE (1 << 4)
#define USBB2_HSIC_DATA_OFFMODE_WD(x)       (((x) & 0x3) << 5)
#define USBB2_HSIC_DATA_OFFMODE_WD_ENABLE   (1 << 7)
#define USBB1_HSIC_STROBE_OFFMODE_WD(x)     (((x) & 0x3) << 8)
#define USBB1_HSIC_STROBE_OFFMODE_WD_ENABLE (1 << 10)
#define USBB1_HSIC_DATA_OFFMODE_WD(x)       (((x) & 0x3) << 11)
#define USBB1_HSIC_DATA_OFFMODE_WD_ENABLE   (1 << 13)
#define USBB2_HSIC_STROBE_WD(x)             (((x) & 0x3) << 14)
#define USBB2_HSIC_DATA_WD(x)               (((x) & 0x3) << 16)
#define USBB1_HSIC_STROBE_WD(x)             (((x) & 0x3) << 18)
#define USBB1_HSIC_DATA_WD(x)               (((x) & 0x3) << 20)
#define USBB1_DR1_I(x)                      (((x) & 0x7) << 22)
#define USBB1_DR1_SR(x)                     (((x) & 0x3) << 25)
#define USBB2_DR1_I(x)                      (((x) & 0x7) << 27)
#define USBB2_DR1_SR(x)                     (((x) & 0x3) << 30)

// CONTROL_SMART3IO_PADCONF_3
#define SLIMBUS2_DR0_LB                     (1 << 14)
#define SLIMBUS1_DR1_LB                     (1 << 15)
#define SLIMBUS2_DR3_LB                     (1 << 16)
#define SLIMBUS2_DR2_LB                     (1 << 17)
#define SLIMBUS2_DR1_LB                     (1 << 18)
#define SLIMBUS1_DR0_LB                     (1 << 19)
#define SLIMBUS2_DR3_MB(x)                  (((x) & 0x3) << 20)
#define SLIMBUS2_DR2_MB(x)                  (((x) & 0x3) << 22)
#define SLIMBUS2_DR1_MB(x)                  (((x) & 0x3) << 24)
#define SLIMBUS2_DR0_MB(x)                  (((x) & 0x3) << 26)
#define SLIMBUS1_DR1_MB(x)                  (((x) & 0x3) << 28)
#define SLIMBUS1_DR0_MB(x)                  (((x) & 0x3) << 30)

// CONTROL_SMART2IO_PADCONF_2
#define USBB1_DR0_DS                        (1 << 11)
#define USBB2_DR0_DS                        (1 << 12)
#define USBA_DR2_DS                         (1 << 13)
#define USBA0_DR1_DS                        (1 << 14)
#define USBA0_DR0_DS                        (1 << 15)
#define UART3_DR5_DS                        (1 << 16)
#define UART3_DR4_DS                        (1 << 17)
#define UART3_DR3_DS                        (1 << 18)
#define UART3_DR2_DS                        (1 << 19)
#define SPI3_DR1_DS                         (1 << 20)
#define SPI3_DR0_DS                         (1 << 21)
#define SDMMC4_DR1_DS                       (1 << 22)
#define SDMMC4_DR0_DS                       (1 << 23)
#define SDMMC3_DR0_DS                       (1 << 24)
#define HSI2_DR2_DS                         (1 << 25)
#define HSI2_DR1_DS                         (1 << 26)
#define HSI2_DR0_DS                         (1 << 27)
#define GPIO_DR10_DS                        (1 << 28)
#define DPM_DR3_DS                          (1 << 29)
#define DPM_DR2_DS                          (1 << 30)
#define DPM_DR1_DS                          (1 << 31)

// CONTROL_SMART1IO_PADCONF_2
#define HDQ_DR0_SC(x)                       (((x) & 0x3) << 22)
#define KPD_DR3_SC(x)                       (((x) & 0x3) << 24)
#define KPD_DR2_SC(x)                       (((x) & 0x3) << 26)
#define KPD_DR1_SC(x)                       (((x) & 0x3) << 28)
#define KPD_DR0_SC(x)                       (((x) & 0x3) << 30)

// CONTROL_SMART1IO_PADCONF_3
#define HDQ_DR0_LB(x)                       (((x) & 0x3) << 22)
#define KPD_DR3_LB(x)                       (((x) & 0x3) << 24)
#define KPD_DR2_LB(x)                       (((x) & 0x3) << 26)
#define KPD_DR1_LB(x)                       (((x) & 0x3) << 28)
#define KPD_DR0_LB(x)                       (((x) & 0x3) << 30)

// CONTROL_C2CIO_PADCONF_0
#define C2C_INT_VREF_AUTO_EN                (1 << 7)
#define C2C_INT_VREF_EN                     (1 << 8)
#define C2C_VREF_CCAP(x)                    (((x) & 0x3) << 9)
#define CMOSEN_C2C_1_FRM_CTRL               (1 << 11)
#define CMOSEN_C2C_0_FRM_CTRL               (1 << 12)
#define SDMMC2_DR0_LB0                      (1 << 13)
#define KPD_DR5_LB0                         (1 << 14)
#define KPD_DR4_LB0                         (1 << 15)
#define GPMC_DR9_LB0                        (1 << 16)
#define GPMC_DR8_LB0                        (1 << 17)
#define GPMC_DR7_LB0                        (1 << 18)
#define GPMC_DR6_LB0                        (1 << 19)
#define GPMC_DR5_LB0                        (1 << 20)
#define GPMC_DR4_LB0                        (1 << 21)
#define GPMC_DR2_LB0                        (1 << 22)
#define GPMC_DR10_LB0                       (1 << 23)
#define GPMC_DR11_LB0                       (1 << 24)
#define GPMC_DR0_LB0                        (1 << 25)
#define GPIO_DR2_LB0                        (1 << 26)
#define GPIO_DR1_LB0                        (1 << 27)
#define GPIO_DR0_LB0                        (1 << 28)
#define C2C_DR2_LB0                         (1 << 29)
#define C2C_DR1_LB0                         (1 << 30)
#define C2C_DR0_LB0                         (1 << 31)

// CONTROL_PBIASLITE
#define USBC1_ICUSB_PWRDNZ                  (1 << 20)
#define MMC1_PBIASLITE_VMODE                (1 << 21)
#define MMC1_PBIASLITE_PWRDNZ               (1 << 22)
#define MMC1_PBIASLITE_VMODE_ERROR          (1 << 23)
#define MMC1_PBIASLITE_SUPPLY_HI_OUT        (1 << 24)
#define MMC1_PBIASLITE_HIZ_MODE             (1 << 25)
#define MMC1_PWRDNZ                         (1 << 26)
#define PBIASLITE1_VMODE                    (1 << 27)
#define PBIASLITE1_PWRDNZ                   (1 << 28)
#define PBIASLITE1_VMODE_ERROR              (1 << 29)
#define PBIASLITE1_SUPPLY_HI_OUT            (1 << 30)
#define PBIASLITE1_HIZ_MODE                 (1 << 31)

// CONTROL_I2C_0
#define I2C1_SCL_PULLUPRESX                 (1 << 0)
#define I2C1_SCL_LOAD_BITS(x)               (((x) & 0x3) << 1)
#define I2C1_SCL_GLFENB                     (1 << 3)
#define I2C2_SCL_PULLUPRESX                 (1 << 4)
#define I2C2_SCL_LOAD_BITS(x)               (((x) & 0x3) << 5)
#define I2C2_SCL_GLFENB                     (1 << 7)
#define I2C3_SCL_PULLUPRESX                 (1 << 8)
#define I2C3_SCL_LOAD_BITS(x)               (((x) & 0x3) << 9)
#define I2C3_SCL_GLFENB                     (1 << 11)
#define I2C4_SCL_PULLUPRESX                 (1 << 12)
#define I2C4_SCL_LOAD_BITS(x)               (((x) & 0x3) << 13)
#define I2C4_SCL_GLFENB                     (1 << 15)
#define I2C1_SDA_PULLUPRESX                 (1 << 16)
#define I2C1_SDA_LOAD_BITS(x)               (((x) & 0x3) << 17)
#define I2C1_SDA_GLFENB                     (1 << 18)
#define I2C2_SDA_PULLUPRESX                 (1 << 19)
#define I2C2_SDA_LOAD_BITS(x)               (((x) & 0x3) << 21)
#define I2C2_SDA_GLFENB                     (1 << 23)
#define I2C3_SDA_PULLUPRESX                 (1 << 24)
#define I2C3_SDA_LOAD_BITS(x)               (((x) & 0x3) << 25)
#define I2C3_SDA_GLFENB                     (1 << 27)
#define I2C4_SDA_PULLUPRESX                 (1 << 28)
#define I2C4_SDA_LOAD_BITS(x)               (((x) & 0x3) << 29)
#define I2C4_SDA_GLFENB                     (1 << 31)

// CONTROL_CAMERA_RX
#define CAMERARX_CSI21_CAMMODE(x)           (((x) & 0x3) << 16)
#define CAMERARX_CSI21_CTRLCLKEN            (1 << 18)
#define CAMERARX_CSI22_CAMMODE(x)           (((x) & 0x3) << 19)
#define CAMERARX_CSI22_CTRLCLKEN            (1 << 21)
#define CAMERARX_CSI21_LANEENABLE0          (1 << 24)
#define CAMERARX_CSI21_LANEENABLE1          (1 << 25)
#define CAMERARX_CSI21_LANEENABLE2          (1 << 26)
#define CAMERARX_CSI21_LANEENABLE3          (1 << 27)
#define CAMERARX_CSI21_LANEENABLE4          (1 << 28)
#define CAMERARX_CSI22_LANEENABLE0          (1 << 29)
#define CAMERARX_CSI22_LANEENABLE1          (1 << 30)

// CONTROL_AVDAC
#define AVDAC_INPUTINV                      (1 << 29)
#define AVDAC_TVOUTBYPASS                   (1 << 30)
#define AVDAC_ACEN                          (1 << 31)

// CONTROL_MMC2
#define MMC2_FEEDBACK_CLK_SEL               (1 << 31)

// CONTROL_DSIPHY
#define DSI2_PIPD(x)                        (((x) & 0x1F) << 14)
#define DSI1_PIPD(x)                        (((x) & 0x1F) << 19)
#define DSI1_LANEENABLE(x)                  (((x) & 0x1F) << 24)
#define DSI2_LANEENABLE(x)                  (((x) & 0x7) << 29)

// CONTROL_MCBSPLP
#define ALBCTRLRX_CLKX                      (1 << 30)
#define ALBCTRLRX_FSX                       (1 << 31)

// CONTROL_USB2PHYCORE
#define USB2PHY_RESETDONETCLK               (1 << 5)
#define USBDPLL_FREQLOCK                    (1 << 6)
#define USB2PHY_DATAPOLARITYN               (1 << 7)
#define USB2PHY_TXBITSTUFFENABLE            (1 << 8)
#define USB2PHY_UTMIRESETDONE               (1 << 9)
#define USB2PHY_MCPCMODEEN                  (1 << 11)
#define USB2PHY_MCPCPUEN                    (1 << 12)
#define USB2PHY_CHGDETECTED                 (1 << 13)
#define USB2PHY_CHGDETDONE                  (1 << 14)
#define USB2PHY_RESTARTCHGDET               (1 << 15)
#define USB2PHY_SRCONDM                     (1 << 16)
#define USB2PHY_SINKONDP                    (1 << 17)
#define USB2PHY_DATADET                     (1 << 18)
#define USB2PHY_CHG_DET_DP_COMP             (1 << 19)
#define USB2PHY_CHG_DET_DM_COMP             (1 << 20)
#define USB2PHY_CHG_DET_STATUS(x)           (((x) & 0x7) << 21)
#define USB2PHY_CHG_ISINK_EN                (1 << 24)
#define USB2PHY_CHG_VSRC_EN                 (1 << 25)
#define USB2PHY_RDP_PU_CHGDET_EN            (1 << 26)
#define USB2PHY_RDM_PD_CHGDET_EN            (1 << 27)
#define USB2PHY_CHG_DET_EXT_CTL             (1 << 28)
#define USB2PHY_GPIOMODE                    (1 << 29)
#define USB2PHY_DISCHGDET                   (1 << 30)
#define USB2PHY_AUTORESUME_EN               (1 << 31)

// CONTROL_I2C_1
#define GPIO65_NMODE                        (1 << 20)
#define GPIO66_NMODE                        (1 << 22)

// CONTROL_MMC1
#define USBC1_ICUSB_DM_PDDIS                (1 << 21)
#define USBC1_ICUSB_DP_PDDIS                (1 << 22)
#define USB_FD_CDEN                         (1 << 23)
#define USBC1_DR0_SPEEDCTRL                 (1 << 24)
#define SDMMC1_DR2_SPEEDCTRL                (1 << 25)
#define SDMMC1_DR1_SPEEDCTRL                (1 << 26)
#define SDMMC1_DR0_SPEEDCTRL                (1 << 27)
#define SDMMC1_PUSTRENGTH_GRP3              (1 << 28)
#define SDMMC1_PUSTRENGTH_GRP2              (1 << 29)
#define SDMMC1_PUSTRENGTH_GRP1              (1 << 30)
#define SDMMC1_PUSTRENGTH_GRP0              (1 << 31)

// CONTROL_HSI
#define HSI2_CALMUX_SEL                     (1 << 28)
#define HSI2_CALLOOP_SEL                    (1 << 29)
#define HSI1_CALMUX_SEL                     (1 << 30)
#define HSI1_CALLOOP_SEL                    (1 << 31)

// CONTROL_USB
#define CARKIT_USBA0_ULPIPHY_DAT1_AUTO_EN   (1 << 30)
#define CARKIT_USBA0_ULPIPHY_DAT0_AUTO_EN   (1 << 31)

// CONTROL_LPDDR2IO1_0
// CONTROL_LPDDR2IO2_0
#define LPDDR2IO_GR1_WD(x)                  (((x) & 0x3) << 1)
#define LPDDR2IO_GR1_I(x)                   (((x) & 0x7) << 3)
#define LPDDR2IO_GR1_SR(x)                  (((x) & 0x3) << 6)
#define LPDDR2IO_GR2_WD(x)                  (((x) & 0x3) << 9)
#define LPDDR2IO_GR2_I(x)                   (((x) & 0x7) << 11)
#define LPDDR2IO_GR2_SR(x)                  (((x) & 0x3) << 14)
#define LPDDR2IO_GR3_WD(x)                  (((x) & 0x3) << 17)
#define LPDDR2IO_GR3_I(x)                   (((x) & 0x7) << 19)
#define LPDDR2IO_GR3_SR(x)                  (((x) & 0x3) << 22)
#define LPDDR2IO_GR4_WD(x)                  (((x) & 0x3) << 25)
#define LPDDR2IO_GR4_I(x)                   (((x) & 0x7) << 27)
#define LPDDR2IO_GR4_SR(x)                  (((x) & 0x3) << 30)

// CONTROL_LPDDR2IO1_1
// CONTROL_LPDDR2IO2_1
#define LPDDR2IO_GR5_WD(x)                  (((x) & 0x3) << 1)
#define LPDDR2IO_GR5_I(x)                   (((x) & 0x7) << 3)
#define LPDDR2IO_GR5_SR(x)                  (((x) & 0x3) << 6)
#define LPDDR2IO_GR6_WD(x)                  (((x) & 0x3) << 9)
#define LPDDR2IO_GR6_I(x)                   (((x) & 0x7) << 11)
#define LPDDR2IO_GR6_SR(x)                  (((x) & 0x3) << 14)
#define LPDDR2IO_GR7_WD(x)                  (((x) & 0x3) << 17)
#define LPDDR2IO_GR7_I(x)                   (((x) & 0x7) << 19)
#define LPDDR2IO_GR7_SR(x)                  (((x) & 0x3) << 22)
#define LPDDR2IO_GR8_WD(x)                  (((x) & 0x3) << 25)
#define LPDDR2IO_GR8_I(x)                   (((x) & 0x7) << 27)
#define LPDDR2IO_GR8_SR(x)                  (((x) & 0x3) << 30)

// CONTROL_LPDDR2IO1_2
// CONTROL_LPDDR2IO2_2
#define LPDDR2IO_GR9_WD(x)                  (((x) & 0x3) << 9)
#define LPDDR2IO_GR9_I(x)                   (((x) & 0x7) << 11)
#define LPDDR2IO_GR9_SR(x)                  (((x) & 0x3) << 14)
#define LPDDR2IO_GR10_WD(x)                 (((x) & 0x3) << 17)
#define LPDDR2IO_GR10_I(x)                  (((x) & 0x7) << 19)
#define LPDDR2IO_GR10_SR(x)                 (((x) & 0x3) << 22)
#define LPDDR2IO_GR11_WD(x)                 (((x) & 0x3) << 25)
#define LPDDR2IO_GR11_I(x)                  (((x) & 0x7) << 27)
#define LPDDR2IO_GR11_SR(x)                 (((x) & 0x3) << 30)

// CONTROL_LPDDR2IO1_3
// CONTROL_LPDDR2IO2_3
#define LPDDR2_INT_VREF_AUTO_EN_DQ          (1 << 0)
#define LPDDR2_INT_VREF_AUTO_EN_CA          (1 << 1)
#define LPDDR2_INT_VREF_EN_DQ               (1 << 2)
#define LPDDR2_INT_VREF_EN_CA               (1 << 3)
#define LPDDR2_VREF_DQ_INT3_TAP1            (1 << 4)
#define LPDDR2_VREF_DQ_INT2_TAP1            (1 << 5)
#define LPDDR2_VREF_DQ_INT3_TAP0            (1 << 6)
#define LPDDR2_VREF_DQ_INT2_TAP0            (1 << 7)
#define LPDDR2_VREF_DQ_INT3_CCAP1           (1 << 8)
#define LPDDR2_VREF_DQ_INT2_CCAP1           (1 << 9)
#define LPDDR2_VREF_DQ_INT3_CCAP0           (1 << 10)
#define LPDDR2_VREF_DQ_INT2_CCAP0           (1 << 11)
#define LPDDR2_VREF_DQ_TAP1                 (1 << 12)
#define LPDDR2_VREF_DQ_TAP0                 (1 << 13)
#define LPDDR2_VREF_DQ_CCAP1                (1 << 14)
#define LPDDR2_VREF_DQ_CCAP0                (1 << 15)
#define LPDDR2_VREF_DQ_INT1_TAP1            (1 << 16)
#define LPDDR2_VREF_DQ_INT1_TAP0            (1 << 17)
#define LPDDR2_VREF_DQ_INT1_CCAP1           (1 << 18)
#define LPDDR2_VREF_DQ_INT1_CCAP0           (1 << 19)
#define LPDDR2_VREF_DQ_INT0_TAP1            (1 << 20)
#define LPDDR2_VREF_DQ_INT0_TAP0            (1 << 21)
#define LPDDR2_VREF_DQ_INT0_CCAP1           (1 << 22)
#define LPDDR2_VREF_DQ_INT0_CCAP0           (1 << 23)
#define LPDDR2_VREF_CA_TAP1                 (1 << 24)
#define LPDDR2_VREF_CA_TAP0                 (1 << 25)
#define LPDDR2_VREF_CA_INT_TAP1             (1 << 26)
#define LPDDR2_VREF_CA_INT_TAP0             (1 << 27)
#define LPDDR2_VREF_CA_INT_CCAP1            (1 << 28)
#define LPDDR2_VREF_CA_INT_CCAP0            (1 << 29)
#define LPDDR2_VREF_CA_CCAP1                (1 << 30)
#define LPDDR2_VREF_CA_CCAP0                (1 << 31)

// CONTROL_CORE_CONTROL_SPARE_R_C0
#define CORE_CONTROL_SPARE_R_C7             (1 << 24)
#define CORE_CONTROL_SPARE_R_C6             (1 << 25)
#define CORE_CONTROL_SPARE_R_C5             (1 << 26)
#define CORE_CONTROL_SPARE_R_C4             (1 << 27)
#define CORE_CONTROL_SPARE_R_C3             (1 << 28)
#define CORE_CONTROL_SPARE_R_C2             (1 << 29)
#define CORE_CONTROL_SPARE_R_C1             (1 << 30)
#define CORE_CONTROL_SPARE_R_C0             (1 << 31)

// CONTROL_EFUSE_1
#define AVDAC_TRIM_BYTE0(x)                 ((x) & 0xFF)
#define AVDAC_TRIM_BYTE1(x)                 (((x) & 0xFF) << 8)
#define AVDAC_TRIM_BYTE2(x)                 (((x) & 0xFF) << 16)
#define AVDAC_TRIM_BYTE3(x)                 (((x) & 0x7F) << 24)

// CONTROL_EFUSE_2
#define LPDDR2_PTV_P5                       (1 << 14)
#define LPDDR2_PTV_P4                       (1 << 15)
#define LPDDR2_PTV_P3                       (1 << 16)
#define LPDDR2_PTV_P2                       (1 << 17)
#define LPDDR2_PTV_P1                       (1 << 18)
#define LPDDR2_PTV_N5                       (1 << 19)
#define LPDDR2_PTV_N4                       (1 << 20)
#define LPDDR2_PTV_N3                       (1 << 21)
#define LPDDR2_PTV_N2                       (1 << 22)
#define LPDDR2_PTV_N1                       (1 << 23)
#define EFUSE_SMART2TEST_N3                 (1 << 24)
#define EFUSE_SMART2TEST_N2                 (1 << 25)
#define EFUSE_SMART2TEST_N1                 (1 << 26)
#define EFUSE_SMART2TEST_N0                 (1 << 27)
#define EFUSE_SMART2TEST_P3                 (1 << 28)
#define EFUSE_SMART2TEST_P2                 (1 << 29)
#define EFUSE_SMART2TEST_P1                 (1 << 30)
#define EFUSE_SMART2TEST_P0                 (1 << 31)

// CONTROL_EFUSE_3
#define STD_FUSE_SPARE_4(x)                 ((x) & 0xFF)
#define STD_FUSE_SPARE_3(x)                 (((x) & 0xFF) << 8)
#define STD_FUSE_SPARE_2(x)                 (((x) & 0xFF) << 16)
#define STD_FUSE_SPARE_1(x)                 (((x) & 0xFF) << 24)

// CONTROL_EFUSE_4
#define STD_FUSE_SPARE_8(x)                 ((x) & 0xFF)
#define STD_FUSE_SPARE_7(x)                 (((x) & 0xFF) << 8)
#define STD_FUSE_SPARE_6(x)                 (((x) & 0xFF) << 16)
#define STD_FUSE_SPARE_5(x)                 (((x) & 0xFF) << 24)

// CONTROL_WKUP_PADCONF_WAKEUPEVENT_0
#define GPIO_WK0_DUPLICATEWAKEUPEVENT               (1 << 0)
#define GPIO_WK1_DUPLICATEWAKEUPEVENT               (1 << 1)
#define GPIO_WK2_DUPLICATEWAKEUPEVENT               (1 << 2)
#define GPIO_WK3_DUPLICATEWAKEUPEVENT               (1 << 3)
#define GPIO_WK4_DUPLICATEWAKEUPEVENT               (1 << 4)
#define SR_SCL_DUPLICATEWAKEUPEVENT                 (1 << 5)
#define SR_SDA_DUPLICATEWAKEUPEVENT                 (1 << 6)
#define FREF_CLK_IOREQ_DUPLICATEWAKEUPEVENT         (1 << 7)
#define FREF_CLK0_OUT_DUPLICATEWAKEUPEVENT          (1 << 8)
#define FREF_CLK3_REQ_DUPLICATEWAKEUPEVENT          (1 << 9)
#define FREF_CLK3_OUT_DUPLICATEWAKEUPEVENT          (1 << 10)
#define FREF_CLK4_REQ_DUPLICATEWAKEUPEVENT          (1 << 11)
#define FREF_CLK4_OUT_DUPLICATEWAKEUPEVENT          (1 << 12)
#define SYS_32K_DUPLICATEWAKEUPEVENT                (1 << 13)
#define SYS_NRESWARM_DUPLICATEWAKEUPEVENT           (1 << 14)
#define SYS_PWR_REQ_DUPLICATEWAKEUPEVENT            (1 << 15)
#define SYS_PWRON_RESET_OUT_DUPLICATEWAKEUPEVENT    (1 << 16)
#define SYS_BOOT6_DUPLICATEWAKEUPEVENT              (1 << 17)
#define SYS_BOOT7_DUPLICATEWAKEUPEVENT              (1 << 18)
#define JTAG_NTRST_DUPLICATEWAKEUPEVENT             (1 << 19)
#define JTAG_TCK_DUPLICATEWAKEUPEVENT               (1 << 20)
#define JTAG_RTCK_DUPLICATEWAKEUPEVENT              (1 << 21)
#define JTAG_TMS_TMSC_DUPLICATEWAKEUPEVENT          (1 << 22)
#define JTAG_TDI_DUPLICATEWAKEUPEVENT               (1 << 23)
#define JTAG_TDO_DUPLICATEWAKEUPEVENT               (1 << 24)

// CONTROL_SMART1NOPMIO_PADCONF_0
#define GPIOWK4_DR0_SC(x)                   (((x) & 0x3) << 12)
#define DPM_DR0_SC(x)                       (((x) & 0x3) << 14)
#define GPIO_DR7_SC(x)                      (((x) & 0x3) << 18)
#define FREF_DR0_SC(x)                      (((x) & 0x3) << 30)

// CONTROL_SMART1NOPMIO_PADCONF_1
#define GPIOWK4_DR0_LB(x)                   (((x) & 0x3) << 12)
#define DPM_DR0_LB(x)                       (((x) & 0x3) << 14)
#define GPIO_DR7_LB(x)                      (((x) & 0x3) << 18)
#define FREF_DR0_LB(x)                      (((x) & 0x3) << 30)

// CONTROL_WKUP_PADCONF_MODE
#define VDDS_DV_BANK2_SHARED1               (1 << 30)
#define VDDS_DV_FREF                        (1 << 31)

// CONTROL_XTAL_OSCILLATOR
#define OSCILLATOR_OS_OUT                   (1 << 30)
#define OSCILLATOR_BOOST                    (1 << 31)

// CONTROL_SMART3NOPMIO_PADCONF_0
#define FREF_DR4_MB(x)                      (((x) & 0x3) << 22)
#define FREF_DR7_MB(x)                      (((x) & 0x3) << 24)
#define FREF_DR6_MB(x)                      (((x) & 0x3) << 26)
#define FREF_DR5_MB(x)                      (((x) & 0x3) << 28)
#define FREF_DR1_MB(x)                      (((x) & 0x3) << 30)

// CONTROL_SMART3NOPMIO_PADCONF_1
#define FREF_DR4_LB(x)                      (((x) & 0x3) << 22)
#define FREF_DR7_LB(x)                      (((x) & 0x3) << 24)
#define FREF_DR6_LB(x)                      (((x) & 0x3) << 26)
#define FREF_DR5_LB(x)                      (((x) & 0x3) << 28)
#define FREF_DR1_LB(x)                      (((x) & 0x3) << 30)

// CONTROL_GPIOWK
#define GPIOWK_IO_PWRDNZ                    (1 << 28)
#define PAD_GPIO_WK2_LOW                    (1 << 29)
#define PAD_GPIO_WK1_LOW                    (1 << 31)

// CONTROL_I2C_2
#define SR_SCL_PULLUPRESX                   (1 << 24)
#define SR_SCL_LOAD_BITS(x)                 (((x) & 0x3) << 25)
#define SR_SCL_GLFENB                       (1 << 27)
#define SR_SDA_PULLUPRESX                   (1 << 28)
#define SR_SDA_LOAD_BITS(x)                 (((x) & 0x3) << 29)
#define SR_SDA_GLFENB                       (1 << 30)

// CONTROL_JTAG
#define JTAG_TDO_EN                         (1 << 27)
#define JTAG_TDI_EN                         (1 << 28)
#define JTAG_RTCK_EN                        (1 << 29)
#define JTAG_TCK_EN                         (1 << 30)
#define JTAG_NTRST_EN                       (1 << 31)

// CONTROL_SYS
#define SYS_NRESWARM_PIPU                   (1 << 31)

// CONTROL_WKUP_CONTROL_SPARE_R_C0
#define WKUP_CONTROL_SPARE_R_C7             (1 << 24)
#define WKUP_CONTROL_SPARE_R_C6             (1 << 25)
#define WKUP_CONTROL_SPARE_R_C5             (1 << 26)
#define WKUP_CONTROL_SPARE_R_C4             (1 << 27)
#define WKUP_CONTROL_SPARE_R_C3             (1 << 28)
#define WKUP_CONTROL_SPARE_R_C2             (1 << 29)
#define WKUP_CONTROL_SPARE_R_C1             (1 << 30)
#define WKUP_CONTROL_SPARE_R_C0             (1 << 31)

