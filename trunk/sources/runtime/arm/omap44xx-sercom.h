///////////////////////////////////////////////////////////////
//            OMAP 4430 UART/IrDA/CIR Registers
///////////////////////////////////////////////////////////////
#define OMAP_UART_R(i,off)      OMAP_R((((i)==1) ? 0x4806A000 : \
                                        ((i)==2) ? 0x4806C000 : \
                                        ((i)==3) ? 0x48020000 : \
                                                   0x4806E000) + (off))
                                                   
#define UART_THR(i)             OMAP_UART_R(i, 0x00)
#define UART_RHR(i)             OMAP_UART_R(i, 0x00)
#define UART_DLL(i)             OMAP_UART_R(i, 0x00)
#define UART_IER(i)             OMAP_UART_R(i, 0x04)
#define UART_IER_IRDA(i)        OMAP_UART_R(i, 0x04)
#define UART_IER_CIR(i)         OMAP_UART_R(i, 0x04)
#define UART_DLH(i)             OMAP_UART_R(i, 0x04)
#define UART_IIR(i)             OMAP_UART_R(i, 0x08)
#define UART_IIR_IRDA(i)        OMAP_UART_R(i, 0x08)
#define UART_IIR_CIR(i)         OMAP_UART_R(i, 0x08)
#define UART_FCR(i)             OMAP_UART_R(i, 0x08)
#define UART_EFR(i)             OMAP_UART_R(i, 0x08)
#define UART_LCR(i)             OMAP_UART_R(i, 0x0C)
#define UART_XON1_ADDR1(i)      OMAP_UART_R(i, 0x10)
#define UART_MCR(i)             OMAP_UART_R(i, 0x10)
#define UART_LSR(i)             OMAP_UART_R(i, 0x14)
#define UART_LSR_IRDA(i)        OMAP_UART_R(i, 0x14)
#define UART_LSR_CIR(i)         OMAP_UART_R(i, 0x14)
#define UART_XON2_ADDR2(i)      OMAP_UART_R(i, 0x14)
#define UART_TCR(i)             OMAP_UART_R(i, 0x18)
#define UART_XOFF1(i)           OMAP_UART_R(i, 0x18)
#define UART_MSR(i)             OMAP_UART_R(i, 0x18)
#define UART_SPR(i)             OMAP_UART_R(i, 0x1C)
#define UART_TLR(i)             OMAP_UART_R(i, 0x1C)
#define UART_XOFF2(i)           OMAP_UART_R(i, 0x1C)
#define UART_MDR1(i)            OMAP_UART_R(i, 0x20)
#define UART_MDR2(i)            OMAP_UART_R(i, 0x24)
#define UART_SFLSR(i)           OMAP_UART_R(i, 0x28)
#define UART_TXFLL(i)           OMAP_UART_R(i, 0x28)
#define UART_RESUME(i)          OMAP_UART_R(i, 0x2C)
#define UART_TXFLH(i)           OMAP_UART_R(i, 0x2C)
#define UART_SFREGL(i)          OMAP_UART_R(i, 0x30)
#define UART_RXFLL(i)           OMAP_UART_R(i, 0x30)
#define UART_SFREGH(i)          OMAP_UART_R(i, 0x34)
#define UART_RXFLH(i)           OMAP_UART_R(i, 0x34)
#define UART_BLR(i)             OMAP_UART_R(i, 0x38)
#define UART_UASR(i)            OMAP_UART_R(i, 0x38)
#define UART_ACREG(i)           OMAP_UART_R(i, 0x3C)
#define UART_SCR(i)             OMAP_UART_R(i, 0x40)
#define UART_SSR(i)             OMAP_UART_R(i, 0x44)
#define UART_EBLR(i)            OMAP_UART_R(i, 0x48)
#define UART_MVR(i)             OMAP_UART_R(i, 0x50)
#define UART_SYSC(i)            OMAP_UART_R(i, 0x54)
#define UART_SYSS(i)            OMAP_UART_R(i, 0x58)
#define UART_WER(i)             OMAP_UART_R(i, 0x5C)
#define UART_CFPS(i)            OMAP_UART_R(i, 0x60)
#define UART_RXFIFO_LVL(i)      OMAP_UART_R(i, 0x64)
#define UART_TXFIFO_LVL(i)      OMAP_UART_R(i, 0x68)
#define UART_IER2(i)            OMAP_UART_R(i, 0x6C)
#define UART_ISR2(i)            OMAP_UART_R(i, 0x70)
#define UART_FREQ_SEL(i)        OMAP_UART_R(i, 0x74)
#define UART_MDR3(i)            OMAP_UART_R(i, 0x80)
#define UART_TX_DMA_THRESHOLD(i) OMAP_UART_R(i, 0x84)

///////////////////////////////////////////////////////////////
//            OMAP 4430 UART/IrDA/CIR Bits
///////////////////////////////////////////////////////////////

// UART_IER
#define UART_IER_RHR_IT         (1 << 0)
#define UART_IER_THR_IT         (1 << 1)
#define UART_IER_LINE_STS_IT    (1 << 2)
#define UART_IER_MODEM_STS_IT   (1 << 3)
#define UART_IER_SLEEP_MODE     (1 << 4)
#define UART_IER_XOFF_IT        (1 << 5)
#define UART_IER_RTS_IT         (1 << 6)
#define UART_IER_CTS_IT         (1 << 7)

// UART_IER_IRDA
#define UART_IER_IRDA_RHR_IT            (1 << 0)
#define UART_IER_IRDA_THR_IT            (1 << 1)
#define UART_IER_IRDA_LAST_RX_BYTE_IT   (1 << 2)
#define UART_IER_IRDA_RX_OVERRUN_IT     (1 << 3)
#define UART_IER_IRDA_STS_FIFO_TRIG_IT  (1 << 4)
#define UART_IER_IRDA_TX_STATUS_IT      (1 << 5)
#define UART_IER_IRDA_LINE_STS_IT       (1 << 6)
#define UART_IER_IRDA_EOF_IT            (1 << 7)

// UART_IER_CIR
#define UART_IER_CIR_RHR_IT             (1 << 0)
#define UART_IER_CIR_THR_IT             (1 << 1)
#define UART_IER_CIR_RX_STOP_IT         (1 << 2)
#define UART_IER_CIR_RX_OVERRUN_IT      (1 << 3)
#define UART_IER_CIR_TX_STATUS_IT       (1 << 5)

// UART_IIR
#define UART_IIR_IT_PENDING             (1 << 0)
#define UART_IIR_IT_TYPE(x)             (((x) & 0x1F) << 1)
#define UART_IIR_FCR_MIRROR(x)          (((x) & 0x3) << 6)

// UART_IIR_IRDA
#define UART_IIR_IRDA_RHR_IT                (1 << 0)
#define UART_IIR_IRDA_THR_IT                (1 << 1)
#define UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT  (1 << 2)
#define UART_IIR_IRDA_RX_OE_IT              (1 << 3)
#define UART_IIR_IRDA_STS_FIFO_IT           (1 << 4)
#define UART_IIR_IRDA_TX_STATUS_IT          (1 << 5)
#define UART_IIR_IRDA_LINE_STS_IT           (1 << 6)
#define UART_IIR_IRDA_EOF_IT                (1 << 7)

// UART_IIR_CIR
#define UART_IIR_CIR_RHR_IT             (1 << 0)
#define UART_IIR_CIR_THR_IT             (1 << 1)
#define UART_IIR_CIR_RX_STOP_IT         (1 << 2)
#define UART_IIR_CIR_RX_OE_IT           (1 << 3)
#define UART_IIR_CIR_TX_STATUS_IT       (1 << 5)

// UART_FCR
#define UART_FCR_FIFO_EN                (1 << 0)
#define UART_FCR_RX_FIFO_CLEAR          (1 << 1)
#define UART_FCR_TX_FIFO_CLEAR          (1 << 2)
#define UART_FCR_DMA_MODE               (1 << 3)
#define UART_FCR_TX_FIFO_TRIG(x)        (((x) & 0x3) << 4)
#define UART_FCR_RX_FIFO_TRIG(x)        (((x) & 0x3) << 6)

// UART_EFR
#define UART_EFR_SW_FLOW_CONTROL(x)     ((x) & 0xF)
#define UART_EFR_ENHANCED_EN            (1 << 4)
#define UART_EFR_SPECIAL_CHAR_DETECT    (1 << 5)
#define UART_EFR_AUTO_RTS_EN            (1 << 6)
#define UART_EFR_AUTO_CTS_EN            (1 << 7)

// UART_LCR
#define UART_LCR_CHAR_LENGTH(x)         ((x) & 0x3)
#define UART_LCR_NB_STOP                (1 << 2)
#define UART_LCR_PARITY_EN              (1 << 3)
#define UART_LCR_PARITY_TYPE1           (1 << 4)
#define UART_LCR_PARITY_TYPE2           (1 << 5)
#define UART_LCR_BREAK_EN               (1 << 6)
#define UART_LCR_DIV_EN                 (1 << 7)

// Char lengths (UART_LCR CHAR_LENGTH field values)
#define UART_LCR_CHAR_LENGTH_5_BIT      0x0
#define UART_LCR_CHAR_LENGTH_6_BIT      0x1
#define UART_LCR_CHAR_LENGTH_7_BIT      0x2
#define UART_LCR_CHAR_LENGTH_8_BIT      0x3

// Configuration modes switch values
#define UART_CONF_MODE_A                0x0080
#define UART_CONF_MODE_B                0x00BF

// UART_MCR
#define UART_MCR_DTR                    (1 << 0)
#define UART_MCR_RTS                    (1 << 1)
#define UART_MCR_RI_STS_CH              (1 << 2)
#define UART_MCR_CD_STS_CH              (1 << 3)
#define UART_MCR_LOOPBACK_EN            (1 << 4)
#define UART_MCR_XON_EN                 (1 << 5)
#define UART_MCR_TCR_TLR                (1 << 6)

// UART_LSR
#define UART_LSR_RX_FIFO_E              (1 << 0)
#define UART_LSR_RX_OE                  (1 << 1)
#define UART_LSR_RX_PE                  (1 << 2)
#define UART_LSR_RX_FE                  (1 << 3)
#define UART_LSR_RX_BI                  (1 << 4)
#define UART_LSR_TX_FIFO_E              (1 << 5)
#define UART_LSR_TX_SR_E                (1 << 6)
#define UART_LSR_RX_FIFO_STS            (1 << 7)

// UART_LSR_IRDA
#define UART_LSR_IRDA_RX_FIFO_E         (1 << 0)
#define UART_LSR_IRDA_STS_FIFO_E        (1 << 1)
#define UART_LSR_IRDA_CRC               (1 << 2)
#define UART_LSR_IRDA_ABORT             (1 << 3)
#define UART_LSR_IRDA_FRAME_TOO_LONG    (1 << 4)
#define UART_LSR_IRDA_RX_LAST_BYTE      (1 << 5)
#define UART_LSR_IRDA_STS_FIFO_FULL     (1 << 6)
#define UART_LSR_IRDA_THR_EMPTY         (1 << 7)

// UART_LSR_CIR
#define UART_LSR_CIR_RX_FIFO_E          (1 << 0)
#define UART_LSR_CIR_RX_STOP            (1 << 5)
#define UART_LSR_CIR_THR_EMPTY          (1 << 7)

// UART_TCR
#define UART_TCR_RX_FIFO_TRIG_HALT(x)   ((x) & 0xF)
#define UART_TCR_RX_FIFO_TRIG_START(x)  (((x) & 0xF) << 4)

// UART_MSR
#define UART_MSR_CTS_STS                (1 << 0)
#define UART_MSR_DSR_STS                (1 << 1)
#define UART_MSR_RI_STS                 (1 << 2)
#define UART_MSR_DCD_STS                (1 << 3)
#define UART_MSR_NCTS_STS               (1 << 4)
#define UART_MSR_NDSR_STS               (1 << 5)
#define UART_MSR_NRI_STS                (1 << 6)
#define UART_MSR_NCD_STS                (1 << 7)

// UART_TLR
#define UART_TLR_TX_FIFO_TRIG_DMA(x)    ((x) & 0xF)
#define UART_TLR_RX_FIFO_TRIG_DMA(x)    (((x) & 0xF) << 4)

// UART_MDR1
#define UART_MDR1_MODE_SELECT(x)        ((x) & 0x7)
#define UART_MDR1_IR_SLEEP              (1 << 3)
#define UART_MDR1_SET_TXIR              (1 << 4)
#define UART_MDR1_SCT                   (1 << 5)
#define UART_MDR1_SIP_MODE              (1 << 6)
#define UART_MDR1_FRAME_END_MODE        (1 << 7)

// UART modes (for UART_MDR1 MODE_SELECT field):
#define UART_MODE_16X                   0x0
#define UART_MODE_SIR                   0x1
#define UART_MODE_16X_AUTO_BAUD         0x2
#define UART_MODE_13X                   0x3
#define UART_MODE_MIR                   0x4
#define UART_MODE_FIR                   0x5
#define UART_MODE_CIR                   0x6
#define UART_MODE_DISABLE               0x7

// UART_MDR2
#define UART_MDR2_IRTX_UNDERRUN         (1 << 0)
#define UART_MDR2_STS_FIFO_TRIG(x)      (((x) & 0x3) << 1)
#define UART_MDR2_UART_PULSE            (1 << 3)
#define UART_MDR2_CIR_PULSE_MODE(x)     (((x) & 0x3) << 4)
#define UART_MDR2_IRRXINVERT            (1 << 6)
#define UART_MDR2_SET_TXIR_ALT          (1 << 7)

// UART_SFLSR
#define UART_SFLSR_CRC_ERROR            (1 << 1)
#define UART_SFLSR_ABORT_DETECT         (1 << 2)
#define UART_SFLSR_FRAME_TOO_LONG_ERROR (1 << 3)
#define UART_SFLSR_OE_ERROR             (1 << 4)

// UART_BLR
#define UART_BLR_XBOF_TYPE              (1 << 6)
#define UART_BLR_STS_FIFO_RESET         (1 << 7)

// UART_UASR
#define UART_UASR_SPEED(x)              ((x) & 0x1F)
#define UART_UASR_BIT_BY_CHAR           (1 << 5)
#define UART_UASR_PARITY_TYPE(x)        (((x) & 0x3) << 6)

// UART_ACREG
#define UART_ACREG_EOT_EN               (1 << 0)
#define UART_ACREG_ABORT_EN             (1 << 1)
#define UART_ACREG_SCTX_EN              (1 << 2)
#define UART_ACREG_SEND_SIP             (1 << 3)
#define UART_ACREG_DIS_TX_UNDERRUN      (1 << 4)
#define UART_ACREG_DIS_IR_RX            (1 << 5)
#define UART_ACREG_SD_MOD               (1 << 6)
#define UART_ACREG_PULSE_TYPE           (1 << 7)

// UART_SCR
#define UART_SCR_DMA_MODE_CTL               (1 << 0)
#define UART_SCR_DMA_MODE_2(x)              (((x) & 0x3) << 1)
#define UART_SCR_TX_EMPTY_CTL_IT            (1 << 3)
#define UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE  (1 << 4)
#define UART_SCR_DSR_IT                     (1 << 5)
#define UART_SCR_TX_TRIG_GRANU1             (1 << 6)
#define UART_SCR_RX_TRIG_GRANU1             (1 << 7)

// UART_SSR
#define UART_SSR_TX_FIFO_FULL           (1 << 0)
#define UART_SSR_RX_CTS_DSR_WAKE_UP_STS (1 << 1)
#define UART_SSR_DMA_COUNTER_RST        (1 << 2)

// UART_SYSC
#define UART_SYSC_AUTOIDLE              (1 << 0)
#define UART_SYSC_SOFTRESET             (1 << 1)
#define UART_SYSC_ENAWAKEUP             (1 << 2)
#define UART_SYSC_IDLEMODE(x)           (((x) & 0x3) << 3)

// UART_SYSS
#define UART_SYSS_RESETDONE             (1 << 0)

// UART_WER
#define UART_WER_EVENT_0_CTS_ACTIVITY                   (1 << 0)
#define UART_WER_EVENT_1_DSR_ACTIVITY                   (1 << 1)
#define UART_WER_EVENT_2_RI_ACTIVITY                    (1 << 2)
#define UART_WER_EVENT_3_DCD_CD_ACTIVITY                (1 << 3)
#define UART_WER_EVENT_4_RX_ACTIVITY                    (1 << 4)
#define UART_WER_EVENT_5_RHR_INTERRUPT                  (1 << 5)
#define UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT (1 << 6)
#define UART_WER_TX_WAKEUP_EN                           (1 << 7)

// UART_IER2
#define UART_IER2_EN_RXFIFO_EMPTY       (1 << 0)
#define UART_IER2_EN_TXFIFO_EMPTY       (1 << 1)

// UART_ISR2
#define UART_ISR2_RXFIFO_EMPTY_STS      (1 << 0)
#define UART_ISR2_TXFIFO_EMPTY_STS      (1 << 1)

// UART_MDR3
#define UART_MDR3_DISABLE_CIR_RX_DEMOD  (1 << 0)
#define UART_MDR3_NONDEFAULT_FREQ       (1 << 1)
#define UART_MDR3_SET_DMA_TX_THRESHOLD  (1 << 2)

