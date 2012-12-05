///////////////////////////////////////////////////////////////
//                 OMAP 4430 PRCM Registers
///////////////////////////////////////////////////////////////

//
// INTRCONN_SOCKET_PRM Registers
//
#define REVISION_PRM                    OMAP_R(0x4A306000)
#define PRM_IRQSTATUS_MPU_A9            OMAP_R(0x4A306010)
#define PRM_IRQSTATUS_MPU_A9_2          OMAP_R(0x4A306014)
#define PRM_IRQENABLE_MPU_A9            OMAP_R(0x4A306018)
#define PRM_IRQENABLE_MPU_A9_2          OMAP_R(0x4A30601C)
#define PRM_IRQSTATUS_MPU_M3            OMAP_R(0x4A306020)
#define PRM_IRQENABLE_MPU_M3            OMAP_R(0x4A306028)
#define PRM_IRQSTATUS_DSP               OMAP_R(0x4A306030)
#define PRM_IRQENABLE_DSP               OMAP_R(0x4A306038)
#define CM_PRM_PROFILING_CLKCTRL        OMAP_R(0x4A306040)
#define PRM_DEBUG_CFG                   OMAP_R(0x4A3060F0)

//
// CKGEN_PRM Registers
//
#define CM_ABE_DSS_SYS_CLKSEL           OMAP_R(0x4A306100)
#define CM_L4_WKUP_CLKSEL               OMAP_R(0x4A306108)
#define CM_ABE_PLL_REF_CLKSEL           OMAP_R(0x4A30610C)
#define CM_SYS_CLKSEL                   OMAP_R(0x4A306110)

//
// MPU_PRM Registers
//
#define PM_MPU_PWRSTCTRL                OMAP_R(0x4A306300)
#define PM_MPU_PWRSTST                  OMAP_R(0x4A306304)
#define RM_MPU_RSTST                    OMAP_R(0x4A306314)
#define RM_MPU_MPU_CONTEXT              OMAP_R(0x4A306324)

//
// DSP_PRM Registers
//
#define PM_DSP_PWRSTCTRL                OMAP_R(0x4A306400)
#define PM_DSP_PWRSTST                  OMAP_R(0x4A306404)
#define RM_DSP_RSTCTRL                  OMAP_R(0x4A306410)
#define RM_DSP_RSTST                    OMAP_R(0x4A306414)
#define RM_DSP_DSP_CONTEXT              OMAP_R(0x4A306424)

//
// ABE_PRM Registers
//
#define PM_ABE_PWRSTCTRL                OMAP_R(0x4A306500)
#define PM_ABE_PWRSTST                  OMAP_R(0x4A306504)
#define RM_ABE_AESS_CONTEXT             OMAP_R(0x4A30652C)
#define PM_ABE_PDM_WKDEP                OMAP_R(0x4A306530)
#define RM_ABE_PDM_CONTEXT              OMAP_R(0x4A306534)
#define PM_ABE_DMIC_WKDEP               OMAP_R(0x4A306538)
#define RM_ABE_DMIC_CONTEXT             OMAP_R(0x4A30653C)
#define PM_ABE_MCASP_WKDEP              OMAP_R(0x4A306540)
#define RM_ABE_MCASP_CONTEXT            OMAP_R(0x4A306544)
#define PM_ABE_MCBSP1_WKDEP             OMAP_R(0x4A306548)
#define RM_ABE_MCBSP1_CONTEXT           OMAP_R(0x4A30654C)
#define PM_ABE_MCBSP2_WKDEP             OMAP_R(0x4A306550)
#define RM_ABE_MCBSP2_CONTEXT           OMAP_R(0x4A306554)
#define PM_ABE_MCBSP3_WKDEP             OMAP_R(0x4A306558)
#define RM_ABE_MCBSP3_CONTEXT           OMAP_R(0x4A30655C)
#define PM_ABE_SLIMBUS_WKDEP            OMAP_R(0x4A306560)
#define RM_ABE_SLIMBUS_CONTEXT          OMAP_R(0x4A306564)
#define PM_ABE_GPTIMER5_WKDEP           OMAP_R(0x4A306568)
#define RM_ABE_GPTIMER5_CONTEXT         OMAP_R(0x4A30656C)
#define PM_ABE_GPTIMER6_WKDEP           OMAP_R(0x4A306570)
#define RM_ABE_GPTIMER6_CONTEXT         OMAP_R(0x4A306574)
#define PM_ABE_GPTIMER7_WKDEP           OMAP_R(0x4A306578)
#define RM_ABE_GPTIMER7_CONTEXT         OMAP_R(0x4A30657C)
#define PM_ABE_GPTIMER8_WKDEP           OMAP_R(0x4A306580)
#define RM_ABE_GPTIMER8_CONTEXT         OMAP_R(0x4A306584)
#define PM_ABE_WDTIMER3_WKDEP           OMAP_R(0x4A306588)
#define RM_ABE_WDTIMER3_CONTEXT         OMAP_R(0x4A30658C)

//
// ALWAYS_ON_PRM Registers
//
#define PM_ALWON_SR_MPU_WKDEP           OMAP_R(0x4A306628)
#define RM_ALWON_SR_MPU_CONTEXT         OMAP_R(0x4A30662C)
#define PM_ALWON_SR_IVA_WKDEP           OMAP_R(0x4A306630)
#define RM_ALWON_SR_IVA_CONTEXT         OMAP_R(0x4A306634)
#define PM_ALWON_SR_CORE_WKDEP          OMAP_R(0x4A306638)
#define RM_ALWON_SR_CORE_CONTEXT        OMAP_R(0x4A30663C)

//
// CORE_PRM Registers
//
#define PM_CORE_PWRSTCTRL               OMAP_R(0x4A306700)
#define PM_CORE_PWRSTST                 OMAP_R(0x4A306704)
#define RM_L3_1_L3_1_CONTEXT            OMAP_R(0x4A306724)
#define RM_L3_2_L3_2_CONTEXT            OMAP_R(0x4A306824)
#define RM_L3_2_GPMC_CONTEXT            OMAP_R(0x4A30682C)
#define RM_L3_2_OCMC_RAM_CONTEXT        OMAP_R(0x4A306834)
#define RM_MPU_M3_RSTCTRL               OMAP_R(0x4A306910)
#define RM_MPU_M3_RSTST                 OMAP_R(0x4A306914)
#define RM_MPU_M3_MPU_M3_CONTEXT        OMAP_R(0x4A306924)
#define RM_SDMA_SDMA_CONTEXT            OMAP_R(0x4A306A24)
#define RM_MEMIF_DMM_CONTEXT            OMAP_R(0x4A306B24)
#define RM_MEMIF_EMIF_FW_CONTEXT        OMAP_R(0x4A306B2C)
#define RM_MEMIF_EMIF_1_CONTEXT         OMAP_R(0x4A306B34)
#define RM_MEMIF_EMIF_2_CONTEXT         OMAP_R(0x4A306B3C)
#define RM_MEMIF_DLL_CONTEXT            OMAP_R(0x4A306B44)
#define RM_C2C_C2C_CONTEXT              OMAP_R(0x4A306C24)
#define RM_C2C_C2C_FW_CONTEXT           OMAP_R(0x4A306C34)
#define RM_L4CFG_L4_CFG_CONTEXT         OMAP_R(0x4A306D24)
#define RM_L4CFG_SPINLOCK_CONTEXT       OMAP_R(0x4A306D2C)
#define RM_L4CFG_MAILBOX_CONTEXT        OMAP_R(0x4A306D34)
#define RM_L4CFG_SAR_ROM_CONTEXT        OMAP_R(0x4A306D3C)
#define RM_L3INSTR_L3_3_CONTEXT         OMAP_R(0x4A306E24)
#define RM_L3INSTR_L3_INSTR_CONTEXT     OMAP_R(0x4A306E2C)
#define RM_L3INSTR_OCP_WP1_CONTEXT      OMAP_R(0x4A306E44)

//
// IVAHD_PRM Registers
//
#define PM_IVAHD_PWRSTCTRL              OMAP_R(0x4A306F00)
#define PM_IVAHD_PWRSTST                OMAP_R(0x4A306F04)
#define RM_IVAHD_RSTCTRL                OMAP_R(0x4A306F10)
#define RM_IVAHD_RSTST                  OMAP_R(0x4A306F14)
#define RM_IVAHD_IVAHD_CONTEXT          OMAP_R(0x4A306F24)
#define RM_IVAHD_SL2_CONTEXT            OMAP_R(0x4A306F2C)

//
// CAM_PRM Registers
//
#define PM_CAM_PWRSTCTRL                OMAP_R(0x4A307000)
#define PM_CAM_PWRSTST                  OMAP_R(0x4A307004)
#define RM_CAM_ISS_CONTEXT              OMAP_R(0x4A307024)
#define RM_CAM_FDIF_CONTEXT             OMAP_R(0x4A30702C)

//
// DSS_PRM Registers
//
#define PM_DSS_PWRSTCTRL                OMAP_R(0x4A307100)
#define PM_DSS_PWRSTST                  OMAP_R(0x4A307104)
#define PM_DSS_DSS_WKDEP RW             OMAP_R(0x4A307120)
#define RM_DSS_DSS_CONTEXT              OMAP_R(0x4A307124)

//
// SGX_PRM Registers
//
#define PM_SGX_PWRSTCTRL                OMAP_R(0x4A307200)
#define PM_SGX_PWRSTST                  OMAP_R(0x4A307204)
#define RM_SGX_SGX_CONTEXT              OMAP_R(0x4A307224)

//
// L3INIT_PRM Registers
//
#define PM_L3INIT_PWRSTCTRL             OMAP_R(0x4A307300)
#define PM_L3INIT_PWRSTST               OMAP_R(0x4A307304)
#define PM_L3INIT_MMC1_WKDEP            OMAP_R(0x4A307328)
#define RM_L3INIT_MMC1_CONTEXT          OMAP_R(0x4A30732C)
#define PM_L3INIT_MMC2_WKDEP            OMAP_R(0x4A307330)
#define RM_L3INIT_MMC2_CONTEXT          OMAP_R(0x4A307334)
#define PM_L3INIT_HSI_WKDEP             OMAP_R(0x4A307338)
#define RM_L3INIT_HSI_CONTEXT           OMAP_R(0x4A30733C)
#define PM_L3INIT_HSUSBHOST_WKDEP       OMAP_R(0x4A307358)
#define RM_L3INIT_HSUSBHOST_CONTEXT     OMAP_R(0x4A30735C)
#define PM_L3INIT_HSUSBOTG_WKDEP        OMAP_R(0x4A307360)
#define RM_L3INIT_HSUSBOTG_CONTEXT      OMAP_R(0x4A307364)
#define PM_L3INIT_HSUSBTLL_WKDEP        OMAP_R(0x4A307368)
#define RM_L3INIT_HSUSBTLL_CONTEXT      OMAP_R(0x4A30736C)
#define PM_L3INIT_FSUSB_WKDEP           OMAP_R(0x4A3073D0)
#define RM_L3INIT_FSUSB_CONTEXT         OMAP_R(0x4A3073D4)
#define RM_L3INIT_USBPHY_CONTEXT        OMAP_R(0x4A3073E4)

//
// L4PER_PRM Registers
//
#define PM_L4PER_PWRSTCTRL              OMAP_R(0x4A307400)
#define PM_L4PER_PWRSTST                OMAP_R(0x4A307404)
#define PM_L4PER_GPTIMER10_WKDEP        OMAP_R(0x4A307428)
#define RM_L4PER_GPTIMER10_CONTEXT      OMAP_R(0x4A30742C)
#define PM_L4PER_GPTIMER11_WKDEP        OMAP_R(0x4A307430)
#define RM_L4PER_GPTIMER11_CONTEXT      OMAP_R(0x4A307434)
#define PM_L4PER_GPTIMER2_WKDEP         OMAP_R(0x4A307438)
#define RM_L4PER_GPTIMER2_CONTEXT       OMAP_R(0x4A30743C)
#define PM_L4PER_GPTIMER3_WKDEP         OMAP_R(0x4A307440)
#define RM_L4PER_GPTIMER3_CONTEXT       OMAP_R(0x4A307444)
#define PM_L4PER_GPTIMER4_WKDEP         OMAP_R(0x4A307448)
#define RM_L4PER_GPTIMER4_CONTEXT       OMAP_R(0x4A30744C)
#define PM_L4PER_GPTIMER9_WKDEP         OMAP_R(0x4A307450)
#define RM_L4PER_GPTIMER9_CONTEXT       OMAP_R(0x4A307454)
#define RM_L4PER_ELM_CONTEXT            OMAP_R(0x4A30745C)
#define PM_L4PER_GPIO2_WKDEP            OMAP_R(0x4A307460)
#define RM_L4PER_GPIO2_CONTEXT          OMAP_R(0x4A307464)
#define PM_L4PER_GPIO3_WKDEP            OMAP_R(0x4A307468)
#define RM_L4PER_GPIO3_CONTEXT          OMAP_R(0x4A30746C)
#define PM_L4PER_GPIO4_WKDEP            OMAP_R(0x4A307470)
#define RM_L4PER_GPIO4_CONTEXT          OMAP_R(0x4A307474)
#define PM_L4PER_GPIO5_WKDEP            OMAP_R(0x4A307478)
#define RM_L4PER_GPIO5_CONTEXT          OMAP_R(0x4A30747C)
#define PM_L4PER_GPIO6_WKDEP            OMAP_R(0x4A307480)
#define RM_L4PER_GPIO6_CONTEXT          OMAP_R(0x4A307484)
#define RM_L4PER_HDQ1W_CONTEXT          OMAP_R(0x4A30748C)
#define PM_L4PER_I2C1_WKDEP             OMAP_R(0x4A3074A0)
#define RM_L4PER_I2C1_CONTEXT           OMAP_R(0x4A3074A4)
#define PM_L4PER_I2C2_WKDEP             OMAP_R(0x4A3074A8)
#define RM_L4PER_I2C2_CONTEXT           OMAP_R(0x4A3074AC)
#define PM_L4PER_I2C3_WKDEP             OMAP_R(0x4A3074B0)
#define RM_L4PER_I2C3_CONTEXT           OMAP_R(0x4A3074B4)
#define PM_L4PER_I2C4_WKDEP             OMAP_R(0x4A3074B8)
#define RM_L4PER_I2C4_CONTEXT           OMAP_R(0x4A3074BC)
#define RM_L4PER_L4_PER_CONTEXT         OMAP_R(0x4A3074C0)
#define PM_L4PER_MCBSP4_WKDEP           OMAP_R(0x4A3074E0)
#define RM_L4PER_MCBSP4_CONTEXT         OMAP_R(0x4A3074E4)
#define PM_L4PER_MCSPI1_WKDEP           OMAP_R(0x4A3074F0)
#define RM_L4PER_MCSPI1_CONTEXT         OMAP_R(0x4A3074F4)
#define PM_L4PER_MCSPI2_WKDEP           OMAP_R(0x4A3074F8)
#define RM_L4PER_MCSPI2_CONTEXT         OMAP_R(0x4A3074FC)
#define PM_L4PER_MCSPI3_WKDEP           OMAP_R(0x4A307500)
#define RM_L4PER_MCSPI3_CONTEXT         OMAP_R(0x4A307504)
#define PM_L4PER_MCSPI4_WKDEP           OMAP_R(0x4A307508)
#define RM_L4PER_MCSPI4_CONTEXT         OMAP_R(0x4A30750C)
#define PM_L4PER_MMCSD3_WKDEP           OMAP_R(0x4A307520)
#define RM_L4PER_MMCSD3_CONTEXT         OMAP_R(0x4A307524)
#define PM_L4PER_MMCSD4_WKDEP           OMAP_R(0x4A307528)
#define RM_L4PER_MMCSD4_CONTEXT         OMAP_R(0x4A30752C)
#define PM_L4PER_SLIMBUS2_WKDEP         OMAP_R(0x4A307538)
#define RM_L4PER_SLIMBUS2_CONTEXT       OMAP_R(0x4A30753C)
#define PM_L4PER_UART1_WKDEP            OMAP_R(0x4A307540)
#define RM_L4PER_UART1_CONTEXT          OMAP_R(0x4A307544)
#define PM_L4PER_UART2_WKDEP            OMAP_R(0x4A307548)
#define RM_L4PER_UART2_CONTEXT          OMAP_R(0x4A30754C)
#define PM_L4PER_UART3_WKDEP            OMAP_R(0x4A307550)
#define RM_L4PER_UART3_CONTEXT          OMAP_R(0x4A307554)
#define PM_L4PER_UART4_WKDEP            OMAP_R(0x4A307558)
#define RM_L4PER_UART4_CONTEXT          OMAP_R(0x4A30755C)
#define PM_L4PER_MMCSD5_WKDEP           OMAP_R(0x4A307560)
#define RM_L4PER_MMCSD5_CONTEXT         OMAP_R(0x4A307564)
#define RM_L4SEC_AES1_CONTEXT           OMAP_R(0x4A3075A4)
#define RM_L4SEC_AES2_CONTEXT           OMAP_R(0x4A3075AC)
#define RM_L4SEC_DES3DES_CONTEXT        OMAP_R(0x4A3075B4)
#define RM_L4SEC_PKA_CONTEXT            OMAP_R(0x4A3075BC)
#define RM_L4SEC_RNG_CONTEXT            OMAP_R(0x4A3075C4)
#define RM_L4SEC_SHA2MD5_CONTEXT        OMAP_R(0x4A3075CC)
#define RM_L4SEC_CRYPTODMA_CONTEXT      OMAP_R(0x4A3075DC)

//
// WKUP_PRM Registers
//
#define RM_WKUP_L4WKUP_CONTEXT          OMAP_R(0x4A307724)
#define PM_WKUP_WDTIMER2_WKDEP          OMAP_R(0x4A307730)
#define RM_WKUP_WDTIMER2_CONTEXT        OMAP_R(0x4A307734)
#define PM_WKUP_GPIO1_WKDEP             OMAP_R(0x4A307738)
#define RM_WKUP_GPIO1_CONTEXT           OMAP_R(0x4A30773C)
#define PM_WKUP_GPTIMER1_WKDEP          OMAP_R(0x4A307740)
#define RM_WKUP_GPTIMER1_CONTEXT        OMAP_R(0x4A307744)
#define RM_WKUP_32KTIMER_CONTEXT        OMAP_R(0x4A307754)
#define RM_WKUP_SARRAM_CONTEXT          OMAP_R(0x4A307764)
#define PM_WKUP_KEYBOARD_WKDEP          OMAP_R(0x4A307778)
#define RM_WKUP_KEYBOARD_CONTEXT        OMAP_R(0x4A30777C)

//
// WKUP_CM Registers
//
#define CM_WKUP_CLKSTCTRL               OMAP_R(0x4A307800)
#define CM_WKUP_L4WKUP_CLKCTRL          OMAP_R(0x4A307820)
#define CM_WKUP_WDTIMER2_CLKCTRL        OMAP_R(0x4A307830)
#define CM_WKUP_GPIO1_CLKCTRL           OMAP_R(0x4A307838)
#define CM_WKUP_GPTIMER1_CLKCTRL        OMAP_R(0x4A307840)
#define CM_WKUP_32KTIMER_CLKCTRL        OMAP_R(0x4A307850)
#define CM_WKUP_SARRAM_CLKCTRL          OMAP_R(0x4A307860)
#define CM_WKUP_KEYBOARD_CLKCTRL        OMAP_R(0x4A307878)
#define CM_WKUP_BANDGAP_CLKCTRL         OMAP_R(0x4A307888)

//
// EMU_PRM Registers
//
#define PM_EMU_PWRSTCTRL                OMAP_R(0x4A307900)
#define PM_EMU_PWRSTS                   OMAP_R(0x4A307904)
#define RM_EMU_DEBUGSS_CONTEXT          OMAP_R(0x4A307924)

//
// EMU_CM Registers
//
#define CM_EMU_CLKSTCTRL                OMAP_R(0x4A307A00)
#define CM_EMU_DYNAMICDEP               OMAP_R(0x4A307A08)
#define CM_EMU_DEBUGSS_CLKCTRL          OMAP_R(0x4A307A20)

//
// DEVICE_PRM Registers
//
#define PRM_RSTCTRL                     OMAP_R(0x4A307B00)
#define PRM_RSTST                       OMAP_R(0x4A307B04)
#define PRM_RSTTIME                     OMAP_R(0x4A307B08)
#define PRM_CLKREQCTRL                  OMAP_R(0x4A307B0C)
#define PRM_VOLTCTRL                    OMAP_R(0x4A307B10)
#define PRM_PWRREQCTRL                  OMAP_R(0x4A307B14)
#define PRM_PSCON_COUNT                 OMAP_R(0x4A307B18)
#define PRM_IO_COUNT                    OMAP_R(0x4A307B1C)
#define PRM_IO_PMCTRL                   OMAP_R(0x4A307B20)
#define PRM_VOLTSETUP_WARMRESET         OMAP_R(0x4A307B24)
#define PRM_VOLTSETUP_CORE_OFF          OMAP_R(0x4A307B28)
#define PRM_VOLTSETUP_MPU_OFF           OMAP_R(0x4A307B2C)
#define PRM_VOLTSETUP_IVA_OFF           OMAP_R(0x4A307B30)
#define PRM_VOLTSETUP_CORE_RET_SLEEP    OMAP_R(0x4A307B34)
#define PRM_VOLTSETUP_MPU_RET_SLEEP     OMAP_R(0x4A307B38)
#define PRM_VOLTSETUP_IVA_RET_SLEEP     OMAP_R(0x4A307B3C)
#define PRM_VP_CORE_CONFIG              OMAP_R(0x4A307B40)
#define PRM_VP_CORE_STATUS              OMAP_R(0x4A307B44)
#define PRM_VP_CORE_VLIMITTO            OMAP_R(0x4A307B48)
#define PRM_VP_CORE_VOLTAGE             OMAP_R(0x4A307B4C)
#define PRM_VP_CORE_VSTEPMAX            OMAP_R(0x4A307B50)
#define PRM_VP_CORE_VSTEPMIN            OMAP_R(0x4A307B54)
#define PRM_VP_MPU_CONFIG               OMAP_R(0x4A307B58)
#define PRM_VP_MPU_STATUS               OMAP_R(0x4A307B5C)
#define PRM_VP_MPU_VLIMITTO             OMAP_R(0x4A307B60)
#define PRM_VP_MPU_VOLTAGE              OMAP_R(0x4A307B64)
#define PRM_VP_MPU_VSTEPMAX             OMAP_R(0x4A307B68)
#define PRM_VP_MPU_VSTEPMIN             OMAP_R(0x4A307B6C)
#define PRM_VP_IVA_CONFIG               OMAP_R(0x4A307B70)
#define PRM_VP_IVA_STATUS               OMAP_R(0x4A307B74)
#define PRM_VP_IVA_VLIMITTO             OMAP_R(0x4A307B78)
#define PRM_VP_IVA_VOLTAGE              OMAP_R(0x4A307B7C)
#define PRM_VP_IVA_VSTEPMAX             OMAP_R(0x4A307B80)
#define PRM_VP_IVA_VSTEPMIN             OMAP_R(0x4A307B84)
#define PRM_VC_SMPS_SA                  OMAP_R(0x4A307B88)
#define PRM_VC_VAL_SMPS_RA_VOL          OMAP_R(0x4A307B8C)
#define PRM_VC_VAL_SMPS_RA_CMD          OMAP_R(0x4A307B90)
#define PRM_VC_VAL_CMD_VDD_CORE_L       OMAP_R(0x4A307B94)
#define PRM_VC_VAL_CMD_VDD_MPU_L        OMAP_R(0x4A307B98)
#define PRM_VC_VAL_CMD_VDD_IVA_L        OMAP_R(0x4A307B9C)
#define PRM_VC_VAL_BYPASS               OMAP_R(0x4A307BA0)
#define PRM_VC_CFG_CHANNEL              OMAP_R(0x4A307BA4)
#define PRM_VC_CFG_I2C_MODE             OMAP_R(0x4A307BA8)
#define PRM_VC_CFG_I2C_CLK              OMAP_R(0x4A307BAC)
#define PRM_SRAM_COUNT                  OMAP_R(0x4A307BB0)
#define PRM_SRAM_WKUP_SETUP             OMAP_R(0x4A307BB4)
#define PRM_LDO_SRAM_CORE_SETUP         OMAP_R(0x4A307BB8)
#define PRM_LDO_SRAM_CORE_CTRL          OMAP_R(0x4A307BBC)
#define PRM_LDO_SRAM_MPU_SETUP          OMAP_R(0x4A307BC0)
#define PRM_LDO_SRAM_MPU_CTRL           OMAP_R(0x4A307BC4)
#define PRM_LDO_SRAM_IVA_SETUP          OMAP_R(0x4A307BC8)
#define PRM_LDO_SRAM_IVA_CTRL           OMAP_R(0x4A307BCC)
#define PRM_LDO_ABB_MPU_SETUP           OMAP_R(0x4A307BD0)
#define PRM_LDO_ABB_MPU_CTRL            OMAP_R(0x4A307BD4)
#define PRM_LDO_ABB_IVA_SETUP           OMAP_R(0x4A307BD8)
#define PRM_LDO_ABB_IVA_CTRL            OMAP_R(0x4A307BDC)
#define PRM_LDO_BANDGAP_SETUP           OMAP_R(0x4A307BE0)
#define PRM_DEVICE_OFF_CTRL             OMAP_R(0x4A307BE4)
#define PRM_PHASE1_CNDP                 OMAP_R(0x4A307BE8)
#define PRM_PHASE2A_CNDP                OMAP_R(0x4A307BEC)
#define PRM_PHASE2B_CNDP                OMAP_R(0x4A307BF0)
#define PRM_VC_ERRST                    OMAP_R(0x4A307BF8)

//
// INSTR_PRM Registers
//
#define PMI_IDENTICATION                OMAP_R(0x4A307F00)
#define PMI_STATUS                      OMAP_R(0x4A307F10)
#define PMI_SYS_CONFIG                  OMAP_R(0x4A307F14)
#define PMI_CONFIGURATION               OMAP_R(0x4A307F24)
#define PMI_CLASS_FILTERING             OMAP_R(0x4A307F28)
#define PMI_TRIGGERING                  OMAP_R(0x4A307F2C)
#define PMI_SAMPLING                    OMAP_R(0x4A307F30)

//
// INTRCONN_SOCKET_CM1 Registers
//
#define REVISION_CM1                    OMAP_R(0x4A004000)
#define CM_CM1_PROFILING_CLKCTRL        OMAP_R(0x4A004040)
#define CM1_DEBUG_CFG                   OMAP_R(0x4A0040F0)

//
// CKGEN_CM1 Registers
//
#define CM_CLKSEL_CORE                  OMAP_R(0x4A004100)
#define CM_CLKSEL_ABE                   OMAP_R(0x4A004108)
#define CM_DLL_CTRL                     OMAP_R(0x4A004110)
#define CM_CLKMODE_DPLL_CORE            OMAP_R(0x4A004120)
#define CM_IDLEST_DPLL_CORE             OMAP_R(0x4A004124)
#define CM_AUTOIDLE_DPLL_CORE           OMAP_R(0x4A004128)
#define CM_CLKSEL_DPLL_CORE             OMAP_R(0x4A00412C)
#define CM_DIV_M2_DPLL_CORE             OMAP_R(0x4A004130)
#define CM_DIV_M3_DPLL_CORE             OMAP_R(0x4A004134)
#define CM_DIV_M4_DPLL_CORE             OMAP_R(0x4A004138)
#define CM_DIV_M5_DPLL_CORE             OMAP_R(0x4A00413C)
#define CM_DIV_M6_DPLL_CORE             OMAP_R(0x4A004140)
#define CM_DIV_M7_DPLL_CORE             OMAP_R(0x4A004144)
#define CM_SSC_DELTAMSTEP_DPLL_CORE     OMAP_R(0x4A004148)
#define CM_SSC_MODFREQDIV_DPLL_CORE     OMAP_R(0x4A00414C)
#define CM_EMU_OVERRIDE_DPLL_CORE       OMAP_R(0x4A004150)
#define CM_CLKMODE_DPLL_MPU             OMAP_R(0x4A004160)
#define CM_IDLEST_DPLL_MPU              OMAP_R(0x4A004164)
#define CM_AUTOIDLE_DPLL_MPU            OMAP_R(0x4A004168)
#define CM_CLKSEL_DPLL_MPU              OMAP_R(0x4A00416C)
#define CM_DIV_M2_DPLL_MPU              OMAP_R(0x4A004170)
#define CM_SSC_DELTAMSTEP_DPLL_MPU      OMAP_R(0x4A004188)
#define CM_SSC_MODFREQDIV_DPLL_MPU      OMAP_R(0x4A00418C)
#define CM_BYPCLK_DPLL_MPU              OMAP_R(0x4A00419C)
#define CM_CLKMODE_DPLL_IVA             OMAP_R(0x4A0041A0)
#define CM_IDLEST_DPLL_IVA              OMAP_R(0x4A0041A4)
#define CM_AUTOIDLE_DPLL_IVA            OMAP_R(0x4A0041A8)
#define CM_CLKSEL_DPLL_IVA              OMAP_R(0x4A0041AC)
#define CM_DIV_M4_DPLL_IVA              OMAP_R(0x4A0041B8)
#define CM_DIV_M5_DPLL_IVA              OMAP_R(0x4A0041BC)
#define CM_SSC_DELTAMSTEP_DPLL_IVA      OMAP_R(0x4A0041C8)
#define CM_SSC_MODFREQDIV_DPLL_IVA      OMAP_R(0x4A0041CC)
#define CM_BYPCLK_DPLL_IVA              OMAP_R(0x4A0041DC)
#define CM_CLKMODE_DPLL_ABE             OMAP_R(0x4A0041E0)
#define CM_IDLEST_DPLL_ABE              OMAP_R(0x4A0041E4)
#define CM_AUTOIDLE_DPLL_ABE            OMAP_R(0x4A0041E8)
#define CM_CLKSEL_DPLL_ABE              OMAP_R(0x4A0041EC)
#define CM_DIV_M2_DPLL_ABE              OMAP_R(0x4A0041F0)
#define CM_DIV_M3_DPLL_ABE              OMAP_R(0x4A0041F4)
#define CM_SSC_DELTAMSTEP_DPLL_ABE      OMAP_R(0x4A004208)
#define CM_SSC_MODFREQDIV_DPLL_ABE      OMAP_R(0x4A00420C)
#define CM_SHADOW_FREQ_CONFIG1          OMAP_R(0x4A004260)
#define CM_SHADOW_FREQ_CONFIG2          OMAP_R(0x4A004264)
#define CM_DYN_DEP_PRESCAL              OMAP_R(0x4A004270)
#define CM_RESTORE_ST                   OMAP_R(0x4A004280)

//
// MPU_CM1 Registers
//
#define CM_MPU_CLKSTCTRL                OMAP_R(0x4A004300)
#define CM_MPU_STATICDEP                OMAP_R(0x4A004304)
#define CM_MPU_DYNAMICDEP               OMAP_R(0x4A004308)
#define CM_MPU_MPU_CLKCTRL              OMAP_R(0x4A004320)

//
// DSP_CM1 Registers
//
#define CM_DSP_CLKSTCTRL                OMAP_R(0x4A004400)
#define CM_DSP_STATICDEP                OMAP_R(0x4A004404)
#define CM_DSP_DYNAMICDEP               OMAP_R(0x4A004408)
#define CM_DSP_DSP_CLKCTRL              OMAP_R(0x4A004420)

//
// ABE_CM1 Registers
//
#define CM1_ABE_CLKSTCTRL               OMAP_R(0x4A004500)
#define CM1_ABE_L4ABE_CLKCTRL           OMAP_R(0x4A004520)
#define CM1_ABE_AESS_CLKCTRL            OMAP_R(0x4A004528)
#define CM1_ABE_PDM_CLKCTRL             OMAP_R(0x4A004530)
#define CM1_ABE_DMIC_CLKCTRL            OMAP_R(0x4A004538)
#define CM1_ABE_MCASP_CLKCTRL           OMAP_R(0x4A004540)
#define CM1_ABE_MCBSP1_CLKCTRL          OMAP_R(0x4A004548)
#define CM1_ABE_MCBSP2_CLKCTRL          OMAP_R(0x4A004550)
#define CM1_ABE_MCBSP3_CLKCTRL          OMAP_R(0x4A004558)
#define CM1_ABE_SLIMBUS_CLKCTRL         OMAP_R(0x4A004560)
#define CM1_ABE_GPTIMER5_CLKCTRL        OMAP_R(0x4A004568)
#define CM1_ABE_GPTIMER6_CLKCTRL        OMAP_R(0x4A004570)
#define CM1_ABE_GPTIMER7_CLKCTRL        OMAP_R(0x4A004578)
#define CM1_ABE_GPTIMER8_CLKCTRL        OMAP_R(0x4A004580)
#define CM1_ABE_WDTIMER3_CLKCTRL        OMAP_R(0x4A004588)

//
// RESTORE_CM1 Registers
//
#define CM_CLKSEL_CORE_RESTORE                  OMAP_R(0x4A004E00)
#define CM_DIV_M2_DPLL_CORE_RESTORE             OMAP_R(0x4A004E04)
#define CM_DIV_M3_DPLL_CORE_RESTORE             OMAP_R(0x4A004E08)
#define CM_DIV_M4_DPLL_CORE_RESTORE             OMAP_R(0x4A004E0C)
#define CM_DIV_M5_DPLL_CORE_RESTORE             OMAP_R(0x4A004E10)
#define CM_DIV_M6_DPLL_CORE_RESTORE             OMAP_R(0x4A004E14)
#define CM_DIV_M7_DPLL_CORE_RESTORE             OMAP_R(0x4A004E18)
#define CM_CLKSEL_DPLL_CORE_RESTORE             OMAP_R(0x4A004E1C)
#define CM_SSC_DELTAMSTEP_DPLL_CORE_RESTORE     OMAP_R(0x4A004E20)
#define CM_SSC_MODFREQDIV_DPLL_CORE_RESTORE     OMAP_R(0x4A004E24)
#define CM_CLKMODE_DPLL_CORE_RESTORE            OMAP_R(0x4A004E28)
#define CM_SHADOW_FREQ_CONFIG2_RESTORE          OMAP_R(0x4A004E2C)
#define CM_SHADOW_FREQ_CONFIG1_RESTORE          OMAP_R(0x4A004E30)
#define CM_AUTOIDLE_DPLL_CORE_RESTORE           OMAP_R(0x4A004E34)
#define CM_MPU_CLKSTCTRL_RESTORE                OMAP_R(0x4A004E38)
#define CM_CM1_PROFILING_CLKCTRL_RESTORE        OMAP_R(0x4A004E3C)
#define CM_DYN_DEP_PRESCAL_RESTORE              OMAP_R(0x4A004E40)

//
// INSTR_CM1 Registers
//
#define CMI1_IDENTIFICATION             OMAP_R(0x4A004F00)
#define CMI1_SYS_CONFIG                 OMAP_R(0x4A004F10)
#define CMI1_STATUS                     OMAP_R(0x4A004F14)
#define CMI1_CONFIGURATION              OMAP_R(0x4A004F24)
#define CMI1_CLASS_FILTERING            OMAP_R(0x4A004F28)
#define CMI1_TRIGGERING                 OMAP_R(0x4A004F2C)
#define CMI1_SAMPLING                   OMAP_R(0x4A004F30)

//
// INTRCONN_SOCKET_CM2
//
#define REVISION_CM2                    OMAP_R(0x4A008000)
#define CM_CM2_PROFILING_CLKCTRL        OMAP_R(0x4A008040)
#define CM2_DEBUG_CFG                   OMAP_R(0x4A0080F0)

//
// CKGEN_CM2 Registers
//
#define CM_CLKSEL_MPU_M3_ISS_ROOT       OMAP_R(0x4A008100)
#define CM_CLKSEL_USB_60MHZ             OMAP_R(0x4A008104)
#define CM_SCALE_FCLK                   OMAP_R(0x4A008108)
#define CM_CORE_DVFS_PERF1              OMAP_R(0x4A008110)
#define CM_CORE_DVFS_PERF2              OMAP_R(0x4A008114)
#define CM_CORE_DVFS_PERF3              OMAP_R(0x4A008118)
#define CM_CORE_DVFS_PERF4              OMAP_R(0x4A00811C)
#define CM_CORE_DVFS_CURRENT            OMAP_R(0x4A008124)
#define CM_IVA_DVFS_PERF_DSP            OMAP_R(0x4A008128)
#define CM_IVA_DVFS_PERF_IVAHD          OMAP_R(0x4A00812C)
#define CM_IVA_DVFS_PERF_ABE            OMAP_R(0x4A008130)
#define CM_IVA_DVFS_CURRENT             OMAP_R(0x4A008138)
#define CM_CLKMODE_DPLL_PER             OMAP_R(0x4A008140)
#define CM_IDLEST_DPLL_PER              OMAP_R(0x4A008144)
#define CM_AUTOIDLE_DPLL_PER            OMAP_R(0x4A008148)
#define CM_CLKSEL_DPLL_PER              OMAP_R(0x4A00814C)
#define CM_DIV_M2_DPLL_PER              OMAP_R(0x4A008150)
#define CM_DIV_M3_DPLL_PER              OMAP_R(0x4A008154)
#define CM_DIV_M4_DPLL_PER              OMAP_R(0x4A008158)
#define CM_DIV_M5_DPLL_PER              OMAP_R(0x4A00815C)
#define CM_DIV_M6_DPLL_PER              OMAP_R(0x4A008160)
#define CM_DIV_M7_DPLL_PER              OMAP_R(0x4A008164)
#define CM_SSC_DELTAMSTEP_DPLL_PER      OMAP_R(0x4A008168)
#define CM_SSC_MODFREQDIV_DPLL_PER      OMAP_R(0x4A00816C)
#define CM_CLKMODE_DPLL_USB             OMAP_R(0x4A008180)
#define CM_IDLEST_DPLL_USB              OMAP_R(0x4A008184)
#define CM_AUTOIDLE_DPLL_USB            OMAP_R(0x4A008188)
#define CM_CLKSEL_DPLL_USB              OMAP_R(0x4A00818C)
#define CM_DIV_M2_DPLL_USB              OMAP_R(0x4A008190)
#define CM_SSC_DELTAMSTEP_DPLL_USB      OMAP_R(0x4A0081A8)
#define CM_SSC_MODFREQDIV_DPLL_USB      OMAP_R(0x4A0081AC)
#define CM_CLKDCOLDO_DPLL_USB           OMAP_R(0x4A0081B4)

//
// ALWAYS_ON_CM2 Registers
//
#define CM_ALWON_CLKSTCTRL              OMAP_R(0x4A008600)
#define CM_ALWON_SR_MPU_CLKCTRL         OMAP_R(0x4A008628)
#define CM_ALWON_SR_IVA_CLKCTRL         OMAP_R(0x4A008630)
#define CM_ALWON_SR_CORE_CLKCTRL        OMAP_R(0x4A008638)
#define CM_ALWON_USBPHY_CLKCTRL         OMAP_R(0x4A008640)

//
// CORE_CM2 Registers
//
#define CM_L3_1_CLKSTCTRL               OMAP_R(0x4A008700)
#define CM_L3_1_DYNAMICDEP              OMAP_R(0x4A008708)
#define CM_L3_1_L3_1_CLKCTRL            OMAP_R(0x4A008720)
#define CM_L3_2_CLKSTCTRL               OMAP_R(0x4A008800)
#define CM_L3_2_DYNAMICDEP              OMAP_R(0x4A008808)
#define CM_L3_2_L3_2_CLKCTRL            OMAP_R(0x4A008820)
#define CM_L3_2_GPMC_CLKCTRL            OMAP_R(0x4A008828)
#define CM_L3_2_OCMC_RAM_CLKCTRL        OMAP_R(0x4A008830)
#define CM_MPU_M3_CLKSTCTRL             OMAP_R(0x4A008900)
#define CM_MPU_M3_STATICDEP             OMAP_R(0x4A008904)
#define CM_MPU_M3_DYNAMICDEP            OMAP_R(0x4A008908)
#define CM_MPU_M3_MPU_M3_CLKCTRL        OMAP_R(0x4A008920)
#define CM_SDMA_CLKSTCTRL               OMAP_R(0x4A008A00)
#define CM_SDMA_STATICDEP               OMAP_R(0x4A008A04)
#define CM_SDMA_DYNAMICDEP              OMAP_R(0x4A008A08)
#define CM_SDMA_SDMA_CLKCTRL            OMAP_R(0x4A008A20)
#define CM_MEMIF_CLKSTCTRL              OMAP_R(0x4A008B00)
#define CM_MEMIF_DMM_CLKCTRL            OMAP_R(0x4A008B20)
#define CM_MEMIF_EMIF_FW_CLKCTRL        OMAP_R(0x4A008B28)
#define CM_MEMIF_EMIF_1_CLKCTRL         OMAP_R(0x4A008B30)
#define CM_MEMIF_EMIF_2_CLKCTRL         OMAP_R(0x4A008B38)
#define CM_MEMIF_DLL_CLKCTRL            OMAP_R(0x4A008B40)
#define CM_C2C_CLKSTCTRL                OMAP_R(0x4A008C00)
#define CM_C2C_STATICDEP                OMAP_R(0x4A008C04)
#define CM_C2C_DYNAMICDEP               OMAP_R(0x4A008C08)
#define CM_C2C_C2C_CLKCTRL              OMAP_R(0x4A008C20)
#define CM_C2C_C2C_FW_CLKCTRL           OMAP_R(0x4A008C30)
#define CM_L4CFG_CLKSTCTRL              OMAP_R(0x4A008D00)
#define CM_L4CFG_DYNAMICDEP             OMAP_R(0x4A008D08)
#define CM_L4CFG_L4_CFG_CLKCTRL         OMAP_R(0x4A008D20)
#define CM_L4CFG_SPINLOCK_CLKCTRL       OMAP_R(0x4A008D28)
#define CM_L4CFG_MAILBOX_CLKCTRL        OMAP_R(0x4A008D30)
#define CM_L4CFG_SAR_ROM_CLKCTRL        OMAP_R(0x4A008D38)
#define CM_L3INSTR_CLKSTCTRL            OMAP_R(0x4A008E00)
#define CM_L3INSTR_L3_3_CLKCTRL         OMAP_R(0x4A008E20)
#define CM_L3INSTR_L3_INSTR_CLKCTRL     OMAP_R(0x4A008E28)
#define CM_L3INSTR_OCP_WP1_CLKCTRL      OMAP_R(0x4A008E40)

//
// IVAHD_CM2 Registers
//
#define CM_IVAHD_CLKSTCTRL              OMAP_R(0x4A008F00)
#define CM_IVAHD_STATICDEP              OMAP_R(0x4A008F04)
#define CM_IVAHD_DYNAMICDEP             OMAP_R(0x4A008F08)
#define CM_IVAHD_IVAHD_CLKCTRL          OMAP_R(0x4A008F20)
#define CM_IVAHD_SL2_CLKCTRL            OMAP_R(0x4A008F28)

//
// CAM_CM2 Registers
//
#define CM_CAM_CLKSTCTRL                OMAP_R(0x4A009000)
#define CM_CAM_STATICDEP                OMAP_R(0x4A009004)
#define CM_CAM_DYNAMICDEP               OMAP_R(0x4A009008)
#define CM_CAM_ISS_CLKCTRL              OMAP_R(0x4A009020)
#define CM_CAM_FDIF_CLKCTRL             OMAP_R(0x4A009028)

//
// DSS_CM2 Registers
//
#define CM_DSS_CLKSTCTRL                OMAP_R(0x4A009100)
#define CM_DSS_STATICDEP                OMAP_R(0x4A009104)
#define CM_DSS_DYNAMICDEP               OMAP_R(0x4A009108)
#define CM_DSS_DSS_CLKCTRL              OMAP_R(0x4A009120)

//
// SGX_CM2 Registers
//
#define CM_SGX_CLKSTCTRL                OMAP_R(0x4A009200)
#define CM_SGX_STATICDEP                OMAP_R(0x4A009204)
#define CM_SGX_DYNAMICDEP               OMAP_R(0x4A009208)
#define CM_SGX_SGX_CLKCTRL              OMAP_R(0x4A009220)

//
// L3INIT_CM2 Registers
//
#define CM_L3INIT_CLKSTCTRL             OMAP_R(0x4A009300)
#define CM_L3INIT_STATICDEP             OMAP_R(0x4A009304)
#define CM_L3INIT_DYNAMICDEP            OMAP_R(0x4A009308)
#define CM_L3INIT_HSMMC1_CLKCTRL        OMAP_R(0x4A009328)
#define CM_L3INIT_HSMMC2_CLKCTRL        OMAP_R(0x4A009330)
#define CM_L3INIT_HSI_CLKCTRL           OMAP_R(0x4A009338)
#define CM_L3INIT_HSUSBHOST_CLKCTRL     OMAP_R(0x4A009358)
#define CM_L3INIT_HSUSBOTG_CLKCTRL      OMAP_R(0x4A009360)
#define CM_L3INIT_HSUSBTLL_CLKCTRL      OMAP_R(0x4A009368)
#define CM_L3INIT_FSUSB_CLKCTRL         OMAP_R(0x4A0093D0)
#define CM_L3INIT_USBPHY_CLKCTRL        OMAP_R(0x4A0093E0)

//
// L4PER_CM2 Registers
//
#define CM_L4PER_CLKSTCTRL              OMAP_R(0x4A009400)
#define CM_L4PER_DYNAMICDEP             OMAP_R(0x4A009408)
#define CM_L4PER_GPTIMER10_CLKCTRL      OMAP_R(0x4A009428)
#define CM_L4PER_GPTIMER11_CLKCTRL      OMAP_R(0x4A009430)
#define CM_L4PER_GPTIMER2_CLKCTRL       OMAP_R(0x4A009438)
#define CM_L4PER_GPTIMER3_CLKCTRL       OMAP_R(0x4A009440)
#define CM_L4PER_GPTIMER4_CLKCTRL       OMAP_R(0x4A009448)
#define CM_L4PER_GPTIMER9_CLKCTRL       OMAP_R(0x4A009450)
#define CM_L4PER_ELM_CLKCTRL            OMAP_R(0x4A009458)
#define CM_L4PER_GPIO2_CLKCTRL          OMAP_R(0x4A009460)
#define CM_L4PER_GPIO3_CLKCTRL          OMAP_R(0x4A009468)
#define CM_L4PER_GPIO4_CLKCTRL          OMAP_R(0x4A009470)
#define CM_L4PER_GPIO5_CLKCTRL          OMAP_R(0x4A009478)
#define CM_L4PER_GPIO6_CLKCTRL          OMAP_R(0x4A009480)
#define CM_L4PER_HDQ1W_CLKCTRL          OMAP_R(0x4A009488)
#define CM_L4PER_I2C1_CLKCTRL           OMAP_R(0x4A0094A0)
#define CM_L4PER_I2C2_CLKCTRL           OMAP_R(0x4A0094A8)
#define CM_L4PER_I2C3_CLKCTRL           OMAP_R(0x4A0094B0)
#define CM_L4PER_I2C4_CLKCTRL           OMAP_R(0x4A0094B8)
#define CM_L4PER_L4PER_CLKCTRL          OMAP_R(0x4A0094C0)
#define CM_L4PER_MCBSP4_CLKCTRL         OMAP_R(0x4A0094E0)
#define CM_L4PER_MCSPI1_CLKCTRL         OMAP_R(0x4A0094F0)
#define CM_L4PER_MCSPI2_CLKCTRL         OMAP_R(0x4A0094F8)
#define CM_L4PER_MCSPI3_CLKCTRL         OMAP_R(0x4A009500)
#define CM_L4PER_MCSPI4_CLKCTRL         OMAP_R(0x4A009508)
#define CM_L4PER_MMCSD3_CLKCTRL         OMAP_R(0x4A009520)
#define CM_L4PER_MMCSD4_CLKCTRL         OMAP_R(0x4A009528)
#define CM_L4PER_SLIMBUS2_CLKCTRL       OMAP_R(0x4A009538)
#define CM_L4PER_UART1_CLKCTRL          OMAP_R(0x4A009540)
#define CM_L4PER_UART2_CLKCTRL          OMAP_R(0x4A009548)
#define CM_L4PER_UART3_CLKCTRL          OMAP_R(0x4A009550)
#define CM_L4PER_UART4_CLKCTRL          OMAP_R(0x4A009558)
#define CM_L4PER_MMCSD5_CLKCTRL         OMAP_R(0x4A009560)
#define CM_L4SEC_CLKSTCTRL              OMAP_R(0x4A009580)
#define CM_L4SEC_STATICDEP              OMAP_R(0x4A009584)
#define CM_L4SEC_DYNAMICDEP             OMAP_R(0x4A009588)
#define CM_L4SEC_AES1_CLKCTRL           OMAP_R(0x4A0095A0)
#define CM_L4SEC_AES2_CLKCTRL           OMAP_R(0x4A0095A8)
#define CM_L4SEC_DES3DES_CLKCTRL        OMAP_R(0x4A0095B0)
#define CM_L4SEC_PKA_CLKCTRL            OMAP_R(0x4A0095B8)
#define CM_L4SEC_RNG_CLKCTRL            OMAP_R(0x4A0095C0)
#define CM_L4SEC_SHA2MD5_CLKCTRL        OMAP_R(0x4A0095C8)
#define CM_L4SEC_CRYPTODMA_CLKCTRL      OMAP_R(0x4A0095D8)

//
// RESTORE_CM2 Registers
//
#define CM_L3_1_CLKSTCTRL_RESTORE               OMAP_R(0x4A009E00)
#define CM_L3_2_CLKSTCTRL_RESTORE               OMAP_R(0x4A009E04)
#define CM_L4CFG_CLKSTCTRL_RESTORE              OMAP_R(0x4A009E08)
#define CM_MEMIF_CLKSTCTRL_RESTORE              OMAP_R(0x4A009E0C)
#define CM_L4PER_CLKSTCTRL_RESTORE              OMAP_R(0x4A009E10)
#define CM_L3INIT_CLKSTCTRL_RESTORE             OMAP_R(0x4A009E14)
#define CM_L3INSTR_L3_3_CLKCTRL_RESTORE         OMAP_R(0x4A009E18)
#define CM_L3INSTR_L3_INSTR_CLKCTRL_RESTORE     OMAP_R(0x4A009E1C)
#define CM_L3INSTR_OCP_WP1_CLKCTRL_RESTORE      OMAP_R(0x4A009E20)
#define CM_CM2_PROFILING_CLKCTRL_RESTORE        OMAP_R(0x4A009E24)
#define CM_C2C_STATICDEP_RESTORE                OMAP_R(0x4A009E28)
#define CM_L3_1_DYNAMICDEP_RESTORE              OMAP_R(0x4A009E2C)
#define CM_L3_2_DYNAMICDEP_RESTORE              OMAP_R(0x4A009E30)
#define CM_C2C_DYNAMICDEP_RESTORE               OMAP_R(0x4A009E34)
#define CM_L4CFG_DYNAMICDEP_RESTORE             OMAP_R(0x4A009E38)
#define CM_L4PER_DYNAMICDEP_RESTORE             OMAP_R(0x4A009E3C)
#define CM_L4PER_GPIO2_CLKCTRL_RESTORE          OMAP_R(0x4A009E40)
#define CM_L4PER_GPIO3_CLKCTRL_RESTORE          OMAP_R(0x4A009E44)
#define CM_L4PER_GPIO4_CLKCTRL_RESTORE          OMAP_R(0x4A009E48)
#define CM_L4PER_GPIO5_CLKCTRL_RESTORE          OMAP_R(0x4A009E4C)
#define CM_L4PER_GPIO6_CLKCTRL_RESTORE          OMAP_R(0x4A009E50)
#define CM_L3INIT_HSUSBHOST_CLKCTRL_RESTORE     OMAP_R(0x4A009E54)
#define CM_L3INIT_HSUSBTLL_CLKCTRL_RESTORE      OMAP_R(0x4A009E58)
#define CM_SDMA_STATICDEP_RESTORE               OMAP_R(0x4A009E5C)

//
// INSTR_CM2 Registers
//
#define CMI2_IDENTIFICATION             OMAP_R(0x4A009F00)
#define CMI2_SYS_CONFIG                 OMAP_R(0x4A009F10)
#define CMI2_STATUS                     OMAP_R(0x4A009F14)
#define CMI2_CONFIGURATION              OMAP_R(0x4A009F24)
#define CMI2_CLASS_FILTERING            OMAP_R(0x4A009F28)
#define CMI2_TRIGGERING                 OMAP_R(0x4A009F2C)
#define CMI2_SAMPLING                   OMAP_R(0x4A009F30)

///////////////////////////////////////////////////////////////
//                 OMAP 4430 SCRM Registers
///////////////////////////////////////////////////////////////
#define REVISION_SCRM                   OMAP_R(0x4A00A000)
#define CLKSETUPTIME                    OMAP_R(0x4A00A100)
#define PMICSETUPTIME                   OMAP_R(0x4A00A104)
#define ALTCLKSRC                       OMAP_R(0x4A00A110)
#define C2CCLKM                         OMAP_R(0x4A00A11C)
#define EXTCLKREQ                       OMAP_R(0x4A00A200)
#define ACCCLKREQ                       OMAP_R(0x4A00A204)
#define PWRREQ                          OMAP_R(0x4A00A208)
#define AUXCLKREQ0                      OMAP_R(0x4A00A210)
#define AUXCLKREQ1                      OMAP_R(0x4A00A214)
#define AUXCLKREQ2                      OMAP_R(0x4A00A218)
#define AUXCLKREQ3                      OMAP_R(0x4A00A21C)
#define AUXCLKREQ4                      OMAP_R(0x4A00A220)
#define AUXCLKREQ5                      OMAP_R(0x4A00A224)
#define C2CCLKREQ                       OMAP_R(0x4A00A234)
#define AUXCLK0                         OMAP_R(0x4A00A310)
#define AUXCLK1                         OMAP_R(0x4A00A314)
#define AUXCLK2                         OMAP_R(0x4A00A318)
#define AUXCLK3                         OMAP_R(0x4A00A31C)
#define AUXCLK4                         OMAP_R(0x4A00A320)
#define AUXCLK5                         OMAP_R(0x4A00A324)
#define RSTTIME_REG                     OMAP_R(0x4A00A400)
#define C2CRSTCTRL                      OMAP_R(0x4A00A41C)
#define EXTPWRONRSTCTRL                 OMAP_R(0x4A00A420)
#define EXTWARMRSTST_REG                OMAP_R(0x4A00A510)
#define APEWARMRSTST_REG                OMAP_R(0x4A00A514)
#define C2CWARMRSTST_REG                OMAP_R(0x4A00A51C)

///////////////////////////////////////////////////////////////
//                 OMAP 4430 SR Registers
///////////////////////////////////////////////////////////////

//
// SR_MPU
//
#define SR_MPU_SRCONFIG                 OMAP_R(0x4A0D9000)
#define SR_MPU_SRSTATUS                 OMAP_R(0x4A0D9004)
#define SR_MPU_SENVAL                   OMAP_R(0x4A0D9008)
#define SR_MPU_SENMIN                   OMAP_R(0x4A0D900C)
#define SR_MPU_SENMAX                   OMAP_R(0x4A0D9010)
#define SR_MPU_SENAVG                   OMAP_R(0x4A0D9014)
#define SR_MPU_AVGWEIGHT                OMAP_R(0x4A0D9018)
#define SR_MPU_NVALUERECIPROCAL         OMAP_R(0x4A0D901C)
#define SR_MPU_IRQSTATUS_RAW            OMAP_R(0x4A0D9024)
#define SR_MPU_IRQSTATUS                OMAP_R(0x4A0D9028)
#define SR_MPU_IRQENABLE_SET            OMAP_R(0x4A0D902C)
#define SR_MPU_IRQENABLE_CLR            OMAP_R(0x4A0D9030)
#define SR_MPU_SENERROR                 OMAP_R(0x4A0D9034)
#define SR_MPU_ERRCONFIG                OMAP_R(0x4A0D9038)

//
// SR_IVA
//
#define SR_IVA_SRCONFIG                 OMAP_R(0x4A0DB000)
#define SR_IVA_SRSTATUS                 OMAP_R(0x4A0DB004)
#define SR_IVA_SENVAL                   OMAP_R(0x4A0DB008)
#define SR_IVA_SENMIN                   OMAP_R(0x4A0DB00C)
#define SR_IVA_SENMAX                   OMAP_R(0x4A0DB010)
#define SR_IVA_SENAVG                   OMAP_R(0x4A0DB014)
#define SR_IVA_AVGWEIGHT                OMAP_R(0x4A0DB018)
#define SR_IVA_NVALUERECIPROCAL         OMAP_R(0x4A0DB01C)
#define SR_IVA_IRQSTATUS_RAW            OMAP_R(0x4A0DB024)
#define SR_IVA_IRQSTATUS                OMAP_R(0x4A0DB028)
#define SR_IVA_IRQENABLE_SET            OMAP_R(0x4A0DB02C)
#define SR_IVA_IRQENABLE_CLR            OMAP_R(0x4A0DB030)
#define SR_IVA_SENERROR                 OMAP_R(0x4A0DB034)
#define SR_IVA_ERRCONFIG                OMAP_R(0x4A0DB038)

//
// SR_CORE
//
#define SR_CORE_SRCONFIG                OMAP_R(0x4A0DD000)
#define SR_CORE_SRSTATUS                OMAP_R(0x4A0DD004)
#define SR_CORE_SENVAL                  OMAP_R(0x4A0DD008)
#define SR_CORE_SENMIN                  OMAP_R(0x4A0DD00C)
#define SR_CORE_SENMAX                  OMAP_R(0x4A0DD010)
#define SR_CORE_SENAVG                  OMAP_R(0x4A0DD014)
#define SR_CORE_AVGWEIGHT               OMAP_R(0x4A0DD018)
#define SR_CORE_NVALUERECIPROCAL        OMAP_R(0x4A0DD01C)
#define SR_CORE_IRQSTATUS_RAW           OMAP_R(0x4A0DD024)
#define SR_CORE_IRQSTATUS               OMAP_R(0x4A0DD028)
#define SR_CORE_IRQENABLE_SET           OMAP_R(0x4A0DD02C)
#define SR_CORE_IRQENABLE_CLR           OMAP_R(0x4A0DD030)
#define SR_CORE_SENERROR                OMAP_R(0x4A0DD034)
#define SR_CORE_ERRCONFIG               OMAP_R(0x4A0DD038)

///////////////////////////////////////////////////////////////////
// Bit Fields
///////////////////////////////////////////////////////////////////
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
 


