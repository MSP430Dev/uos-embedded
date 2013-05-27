/*
 * Register definitions for STM32F4xx.
 *
 * Copyright (C) 2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
typedef volatile unsigned int arm_reg_t;


#define ARM_SRAM_BASE       0x20000000  // Internal static memory
#define ARM_PERIPH_BASE     0x40000000  // Peripheral registers
#define ARM_SYSTEM_BASE     0xE0000000  // Core registers

#define ARM_SRAM_SIZE       (192*1024)   // Internal SRAM size

#include <runtime/cortex-m/io-cortex-m.h>

//
// Peripheral memory map
//
#define STM_TIM2_BASE           0x40000000
#define STM_TIM3_BASE           0x40000400
#define STM_TIM4_BASE           0x40000800
#define STM_TIM5_BASE           0x40000C00
#define STM_TIM6_BASE           0x40001000
#define STM_TIM7_BASE           0x40001400
#define STM_TIM12_BASE          0x40001800
#define STM_TIM13_BASE          0x40001C00
#define STM_TIM14_BASE          0x40002000
#define STM_RTC_BKP_BASE        0x40002800
#define STM_WWDG_BASE           0x40002C00
#define STM_IWDG_BASE           0x40003000
#define STM_I2S2EXT_BASE        0x40003400
#define STM_SPI2_I2S2_BASE      0x40003800
#define STM_SPI3_I2S3_BASE      0x40003C00
#define STM_I2S3EXT_BASE        0x40004000
#define STM_USART2_BASE         0x40004400
#define STM_USART3_BASE         0x40004800
#define STM_UART4_BASE          0x40004C00
#define STM_UART5_BASE          0x40005000
#define STM_I2C1_BASE           0x40005400
#define STM_I2C2_BASE           0x40005800
#define STM_I2C3_BASE           0x40005C00
#define STM_CAN1_BASE           0x40006400
#define STM_CAN2_BASE           0x40006800
#define STM_PWR_BASE            0x40007000
#define STM_DAC_BASE            0x40007400
#define STM_UART7_BASE          0x40007800
#define STM_UART8_BASE          0x40007C00
#define STM_TIM1_BASE           0x40010000
#define STM_TIM8_BASE           0x40010400
#define STM_USART1_BASE         0x40011000
#define STM_USART6_BASE         0x40011400
#define STM_ADC123_BASE         0x40012000
#define STM_SDIO_BASE           0x40012C00
#define STM_SPI1_BASE           0x40013000
#define STM_SPI4_BASE           0x40013400
#define STM_SYSCFG_BASE         0x40013800
#define STM_EXTI_BASE           0x40013C00
#define STM_TIM9_BASE           0x40014000
#define STM_TIM10_BASE          0x40014400
#define STM_TIM11_BASE          0x40014800
#define STM_SPI5_BASE           0x40015000
#define STM_SPI6_BASE           0x40015400
#define STM_GPIOA_BASE          0x40020000
#define STM_GPIOB_BASE          0x40020400
#define STM_GPIOC_BASE          0x40020800
#define STM_GPIOD_BASE          0x40020C00
#define STM_GPIOE_BASE          0x40021000
#define STM_GPIOF_BASE          0x40021400
#define STM_GPIOG_BASE          0x40021800
#define STM_GPIOH_BASE          0x40021C00
#define STM_GPIOI_BASE          0x40022000
#define STM_CRC_BASE            0x40023000
#define STM_RCC_BASE            0x40023800
#define STM_FLASH_IFACE_BASE    0x40023C00
#define STM_BKPSRAM_BASE        0x40024000
#define STM_DMA1_BASE           0x40026000
#define STM_DMA2_BASE           0x40026400
#define STM_ETH_BASE            0x40028000
#define STM_USB_OTG_HS_BASE     0x40040000
#define STM_USB_OTG_FS_BASE     0x50000000
#define STM_DCMI_BASE           0x50050000
#define STM_CRYP_BASE           0x50060000
#define STM_HASH_BASE           0x50060400
#define STM_RNG_BASE            0x50060800
#define STM_FSMC_BASE           0xA0000000


#if 0
typedef enum
{
  CAN1_IRQn                   = 0,   /* CAN1 Interrupt */
  CAN2_IRQn                   = 1,   /* CAN2 Interrupt */
  USB_IRQn                    = 2,   /* USB Host Interrupt */
  DMA_IRQn                    = 5,   /* DMA Interrupt */
  UART1_IRQn                  = 6,   /* UART1 Interrupt */
  UART2_IRQn                  = 7,   /* UART2 Interrupt */
  SSP1_IRQn                   = 8,   /* SSP1 Interrupt */
  MDR_I2C_IRQn                = 10,  /* MDR_I2C Interrupt */
  POWER_IRQn                  = 11,  /* POWER Detecor Interrupt */
  WWDG_IRQn                   = 12,  /* Window Watchdog Interrupt */
  TIMER1_IRQn                 = 14,  /* Timer1 Interrupt */
  TIMER2_IRQn                 = 15,  /* Timer2 Interrupt */
  TIMER3_IRQn                 = 16,  /* Timer3 Interrupt */
  ADC_IRQn                    = 17,  /* ADC Interrupt */
  SSP3_IRQn                   = 19,  /* SSP3 Interrupt */
  SSP2_IRQn                   = 20,  /* SSP2 Interrupt */
  BKP_IRQn                    = 27,  /* BACKUP Interrupt */
  EXT_INT1_IRQn               = 28,  /* EXT_INT1 Interrupt */
  EXT_INT2_IRQn               = 29,  /* EXT_INT2 Interrupt */
  EXT_INT3_IRQn               = 30,  /* EXT_INT3 Interrupt */
  EXT_INT4_IRQn               = 31   /* EXT_INT4 Interrupt */
} IRQn_t;
#endif


/////////////////////////////////////////
// Reset and clock control
/////////////////////////////////////////
typedef struct
{
    arm_reg_t CR;       // Clock control
    arm_reg_t PLLCFGR;  // PLL configuration
    arm_reg_t CFGR;     // Clock configuration
    arm_reg_t CIR;      // Clock interrupt
    arm_reg_t AHB1RSTR; // AHB1 peripheral reset
    arm_reg_t AHB2RSTR; // AHB2 peripheral reset
    arm_reg_t AHB3RSTR; // AHB3 peripheral reset
    arm_reg_t reserve0;
    arm_reg_t APB1RSTR; // APB1 peripheral reset
    arm_reg_t APB2RSTR; // APB2 peripheral reset
    arm_reg_t reserve1[2];
    arm_reg_t AHB1ENR;  // AHB1 peripheral clock enable
    arm_reg_t AHB2ENR;  // AHB2 peripheral clock enable
    arm_reg_t AHB3ENR;  // AHB3 peripheral clock enable
    arm_reg_t reserve2;
    arm_reg_t APB1ENR;  // APB1 peripheral clock enable
    arm_reg_t APB2ENR;  // APB2 peripheral clock enable
    arm_reg_t reserve3[2];
    arm_reg_t AHB1LPENR;// AHB1 peripheral clock enable in low power mode
    arm_reg_t AHB2LPENR;// AHB2 peripheral clock enable in low power mode
    arm_reg_t AHB3LPENR;// AHB3 peripheral clock enable in low power mode
    arm_reg_t reserve4;
    arm_reg_t APB1LPENR;// APB1 peripheral clock enable in low power mode
    arm_reg_t APB2LPENR;// APB2 peripheral clock enable in low power mode
    arm_reg_t reserve5[2];
    arm_reg_t BDCR;     // Backup domain control
    arm_reg_t CSR;      // Clock control & status
    arm_reg_t reserve6[2];
    arm_reg_t SSCGR;    // Spread spectrum clock generation
    arm_reg_t PLLI2SCFGR; // PLLI2S configuration register
} RCC_t;

#define RCC     ((RCC_t *) STM_RCC_BASE)

// CR values
#define RCC_PLLI2SRDY           (1 << 27)
#define RCC_PLLI2SON            (1 << 26)
#define RCC_PLLRDY              (1 << 25)
#define RCC_PLLON               (1 << 24)
#define RCC_CSSON               (1 << 19)
#define RCC_HSEBYP              (1 << 18)
#define RCC_HSERDY              (1 << 17)
#define RCC_HSEON               (1 << 16)
#define RCC_HSICAL(n)           ((n) << 8)
#define RCC_HSITRIM(n)          ((n) << 3)
#define RCC_HSIRDY              (1 << 1)
#define RCC_HSION               (1 << 0)

// PLLCFGR values
#define RCC_PLLQ(n)             ((n) << 24)
#define RCC_PLLSRC_HSE          (1 << 22)
#define RCC_PLLP_2              (0 << 16)
#define RCC_PLLP_4              (1 << 16)
#define RCC_PLLP_6              (2 << 16)
#define RCC_PLLP_8              (3 << 16)
#define RCC_PLLN(n)             ((n) << 6)
#define RCC_PLLM(n)             (n)

// CFGR values
#define RCC_MCO2_SYSCLK         (0 << 30)
#define RCC_MCO2_PLLI2S         (1 << 30)
#define RCC_MCO2_HSE            (2 << 30)
#define RCC_MCO2_PLL            (3 << 30)
#define RCC_MCO2PRE_NODIV       (0 << 27)
#define RCC_MCO2PRE_DIV2        (4 << 27)
#define RCC_MCO2PRE_DIV3        (5 << 27)
#define RCC_MCO2PRE_DIV4        (6 << 27)
#define RCC_MCO2PRE_DIV5        (7 << 27)
#define RCC_MCO1PRE_NODIV       (0 << 24)
#define RCC_MCO1PRE_DIV2        (4 << 24)
#define RCC_MCO1PRE_DIV3        (5 << 24)
#define RCC_MCO1PRE_DIV4        (6 << 24)
#define RCC_MCO1PRE_DIV5        (7 << 24)
#define RCC_I2SSRC_EXT          (1 << 23)
#define RCC_MCO1_HSI            (0 << 21)
#define RCC_MCO1_LSE            (1 << 21)
#define RCC_MCO1_HSE            (2 << 21)
#define RCC_MCO1_PLL            (3 << 21)
#define RCC_RTCPRE              ((n) << 16)
#define RCC_PPRE2_NODIV         (0 << 13)
#define RCC_PPRE2_DIV2          (4 << 13)
#define RCC_PPRE2_DIV4          (5 << 13)
#define RCC_PPRE2_DIV8          (6 << 13)
#define RCC_PPRE2_DIV16         (7 << 13)
#define RCC_PPRE1_NODIV         (0 << 10)
#define RCC_PPRE1_DIV2          (4 << 10)
#define RCC_PPRE1_DIV4          (5 << 10)
#define RCC_PPRE1_DIV8          (6 << 10)
#define RCC_PPRE1_DIV16         (7 << 10)
#define RCC_HPRE_NODIV          (0 << 4)
#define RCC_HPRE_DIV2           (0x8 << 4)
#define RCC_HPRE_DIV4           (0x9 << 4)
#define RCC_HPRE_DIV8           (0xA << 4)
#define RCC_HPRE_DIV16          (0xB << 4)
#define RCC_HPRE_DIV64          (0xC << 4)
#define RCC_HPRE_DIV128         (0xD << 4)
#define RCC_HPRE_DIV256         (0xE << 4)
#define RCC_HPRE_DIV512         (0xF << 4)
#define RCC_SWS_HSI             (0 << 2)
#define RCC_SWS_HSE             (1 << 2)
#define RCC_SWS_PLL             (2 << 2)
#define RCC_SWS_MASK            (3 << 2)
#define RCC_SW_HSI              (0 << 0)
#define RCC_SW_HSE              (1 << 0)
#define RCC_SW_PLL              (2 << 0)
#define RCC_SW_MASK             (3 << 0)

// CIR values
#define RCC_CSSC                (1 << 23)
#define RCC_PLLI2SSDRYC         (1 << 21)
#define RCC_PLLRDYC             (1 << 20)
#define RCC_HSERDYC             (1 << 19)
#define RCC_HSIRDYC             (1 << 18)
#define RCC_LSERDYC             (1 << 17)
#define RCC_LSIRDYC             (1 << 16)
#define RCC_PLLI2SSDRYIE        (1 << 13)
#define RCC_PLLRDYIE            (1 << 12)
#define RCC_HSERDYIE            (1 << 11)
#define RCC_HSIRDYIE            (1 << 10)
#define RCC_LSERDYIE            (1 << 9)
#define RCC_LSIRDYIE            (1 << 8)
#define RCC_CSSF                (1 << 7)
#define RCC_PLLI2SSDRYF         (1 << 5)
#define RCC_PLLRDYF             (1 << 4)
#define RCC_HSERDYF             (1 << 3)
#define RCC_HSIRDYF             (1 << 2)
#define RCC_LSERDYF             (1 << 1)
#define RCC_LSIRDYF             (1 << 0)

// AHB1RSTR values
#define RCC_OTGHSRST            (1 << 29)
#define RCC_ETHMACRST           (1 << 25)
#define RCC_DMA2RST             (1 << 22)
#define RCC_DMA1RST             (1 << 21)
#define RCC_CRCRST              (1 << 12)
#define RCC_GPIOIRST            (1 << 8)
#define RCC_GPIOHRST            (1 << 7)
#define RCC_GPIOGRST            (1 << 6)
#define RCC_GPIOFRST            (1 << 5)
#define RCC_GPIOERST            (1 << 4)
#define RCC_GPIODRST            (1 << 3)
#define RCC_GPIOCRST            (1 << 2)
#define RCC_GPIOBRST            (1 << 1)
#define RCC_GPIOARST            (1 << 0)

// AHB2RSTR values
#define RCC_OTGFSRST            (1 << 7)
#define RCC_RNGRST              (1 << 6)
#define RCC_HASHRST             (1 << 5)
#define RCC_CRYPRST             (1 << 4)
#define RCC_DCMIRST             (1 << 0)

// AHB3RSTR values
#define RCC_FSMCRST             (1 << 0)

// APB1RSTR values
#define RCC_DACRST              (1 << 29)
#define RCC_PWRRST              (1 << 28)
#define RCC_CAN2RST             (1 << 26)
#define RCC_CAN1RST             (1 << 25)
#define RCC_I2C3RST             (1 << 23)
#define RCC_I2C2RST             (1 << 22)
#define RCC_I2C1RST             (1 << 21)
#define RCC_UART5RST            (1 << 20)
#define RCC_UART4RST            (1 << 19)
#define RCC_USART3RST           (1 << 18)
#define RCC_USART2RST           (1 << 17)
#define RCC_SPI3RST             (1 << 15)
#define RCC_SPI2RST             (1 << 14)
#define RCC_WWDGRST             (1 << 11)
#define RCC_TIM14RST            (1 << 8)
#define RCC_TIM13RST            (1 << 7)
#define RCC_TIM12RST            (1 << 6)
#define RCC_TIM7RST             (1 << 5)
#define RCC_TIM6RST             (1 << 4)
#define RCC_TIM5RST             (1 << 3)
#define RCC_TIM4RST             (1 << 2)
#define RCC_TIM3RST             (1 << 1)
#define RCC_TIM2RST             (1 << 0)

// APB2RSTR values
#define RCC_TIM11RST            (1 << 18)
#define RCC_TIM10RST            (1 << 17)
#define RCC_TIM9RST             (1 << 16)
#define RCC_SYSCFGRST           (1 << 14)
#define RCC_SPI1RST             (1 << 12)
#define RCC_SDIORST             (1 << 11)
#define RCC_ADCRST              (1 << 8)
#define RCC_USART6RST           (1 << 5)
#define RCC_USART1RST           (1 << 4)
#define RCC_TIM8RST             (1 << 1)
#define RCC_TIM1RST             (1 << 0)

// AHB1ENR values
#define RCC_OTGHSULPIEN         (1 << 30)
#define RCC_OTGHSEN             (1 << 29)
#define RCC_ETHMACPTPEN         (1 << 28)
#define RCC_ETHMACRXEN          (1 << 27)
#define RCC_ETHMACTXEN          (1 << 26)
#define RCC_ETHMACEN            (1 << 25)
#define RCC_DMA2EN              (1 << 22)
#define RCC_DMA1EN              (1 << 21)
#define RCC_CCMDATARAMEN        (1 << 20)
#define RCC_BKPSRAMEN           (1 << 18)
#define RCC_CRCEN               (1 << 12)
#define RCC_GPIOIEN             (1 << 8)
#define RCC_GPIOHEN             (1 << 7)
#define RCC_GPIOGEN             (1 << 6)
#define RCC_GPIOFEN             (1 << 5)
#define RCC_GPIOEEN             (1 << 4)
#define RCC_GPIODEN             (1 << 3)
#define RCC_GPIOCEN             (1 << 2)
#define RCC_GPIOBEN             (1 << 1)
#define RCC_GPIOAEN             (1 << 0)

// AHB2ENR values
#define RCC_OTGFSEN             (1 << 7)
#define RCC_RNGEN               (1 << 6)
#define RCC_HASHEN              (1 << 5)
#define RCC_CRYPEN              (1 << 4)
#define RCC_DCMIEN              (1 << 0)

// AHB3ENR values
#define RCC_FSMCEN              (1 << 0)

// APB1ENR values
#define RCC_DACEN               (1 << 29)
#define RCC_PWREN               (1 << 28)
#define RCC_CAN2EN              (1 << 26)
#define RCC_CAN1EN              (1 << 25)
#define RCC_I2C3EN              (1 << 23)
#define RCC_I2C2EN              (1 << 22)
#define RCC_I2C1EN              (1 << 21)
#define RCC_UART5EN             (1 << 20)
#define RCC_UART4EN             (1 << 19)
#define RCC_USART3EN            (1 << 18)
#define RCC_USART2EN            (1 << 17)
#define RCC_SPI3EN              (1 << 15)
#define RCC_SPI2EN              (1 << 14)
#define RCC_WWDGEN              (1 << 11)
#define RCC_TIM14EN             (1 << 8)
#define RCC_TIM13EN             (1 << 7)
#define RCC_TIM12EN             (1 << 6)
#define RCC_TIM7EN              (1 << 5)
#define RCC_TIM6EN              (1 << 4)
#define RCC_TIM5EN              (1 << 3)
#define RCC_TIM4EN              (1 << 2)
#define RCC_TIM3EN              (1 << 1)
#define RCC_TIM2EN              (1 << 0)

// APB2ENR values
#define RCC_TIM11EN             (1 << 18)
#define RCC_TIM10EN             (1 << 17)
#define RCC_TIM9EN              (1 << 16)
#define RCC_SYSCFGEN            (1 << 14)
#define RCC_SPI1EN              (1 << 12)
#define RCC_SDIOEN              (1 << 11)
#define RCC_ADC3EN              (1 << 10)
#define RCC_ADC2EN              (1 << 9)
#define RCC_ADC1EN              (1 << 8)
#define RCC_USART6EN            (1 << 5)
#define RCC_USART1EN            (1 << 4)
#define RCC_TIM8EN              (1 << 1)
#define RCC_TIM1EN              (1 << 0)

// AHB1LPENR values
#define RCC_OTGHSULPILPEN       (1 << 30)
#define RCC_OTGHSLPEN           (1 << 29)
#define RCC_ETHMACPTPLPEN       (1 << 28)
#define RCC_ETHMACRXLPEN        (1 << 27)
#define RCC_ETHMACTXLPEN        (1 << 26)
#define RCC_ETHMACLPEN          (1 << 25)
#define RCC_DMA2LPEN            (1 << 22)
#define RCC_DMA1LPEN            (1 << 21)
#define RCC_BKPSRAMLPEN         (1 << 18)
#define RCC_SRAM2LPEN           (1 << 17)
#define RCC_SRAM1LPEN           (1 << 16)
#define RCC_FLITFLPEN           (1 << 15)
#define RCC_CRCLPEN             (1 << 12)
#define RCC_GPIOILPEN           (1 << 8)
#define RCC_GPIOHLPEN           (1 << 7)
#define RCC_GPIOGLPEN           (1 << 6)
#define RCC_GPIOFLPEN           (1 << 5)
#define RCC_GPIOELPEN           (1 << 4)
#define RCC_GPIODLPEN           (1 << 3)
#define RCC_GPIOCLPEN           (1 << 2)
#define RCC_GPIOBLPEN           (1 << 1)
#define RCC_GPIOALPEN           (1 << 0)

// AHB2LPENR values
#define RCC_OTGFSLPEN           (1 << 7)
#define RCC_RNGLPEN             (1 << 6)
#define RCC_HASHLPEN            (1 << 5)
#define RCC_CRYPLPEN            (1 << 4)
#define RCC_DCMILPEN            (1 << 0)

// AHB3LPENR values
#define RCC_FSMCLPEN            (1 << 0)

// APB1LPENR values
#define RCC_DACLPEN             (1 << 29)
#define RCC_PWRLPEN             (1 << 28)
#define RCC_CAN2LPEN            (1 << 26)
#define RCC_CAN1LPEN            (1 << 25)
#define RCC_I2C3LPEN            (1 << 23)
#define RCC_I2C2LPEN            (1 << 22)
#define RCC_I2C1LPEN            (1 << 21)
#define RCC_UART5LPEN           (1 << 20)
#define RCC_UART4LPEN           (1 << 19)
#define RCC_USART3LPEN          (1 << 18)
#define RCC_USART2LPEN          (1 << 17)
#define RCC_SPI3LPEN            (1 << 15)
#define RCC_SPI2LPEN            (1 << 14)
#define RCC_WWDGLPEN            (1 << 11)
#define RCC_TIM14LPEN           (1 << 8)
#define RCC_TIM13LPEN           (1 << 7)
#define RCC_TIM12LPEN           (1 << 6)
#define RCC_TIM7LPEN            (1 << 5)
#define RCC_TIM6LPEN            (1 << 4)
#define RCC_TIM5LPEN            (1 << 3)
#define RCC_TIM4LPEN            (1 << 2)
#define RCC_TIM3LPEN            (1 << 1)
#define RCC_TIM2LPEN            (1 << 0)

// APB2LPENR values
#define RCC_TIM11LPEN           (1 << 18)
#define RCC_TIM10LPEN           (1 << 17)
#define RCC_TIM9LPEN            (1 << 16)
#define RCC_SYSCFGLPEN          (1 << 14)
#define RCC_SPI1LPEN            (1 << 12)
#define RCC_SDIOLPEN            (1 << 11)
#define RCC_ADC3LPEN            (1 << 10)
#define RCC_ADC2LPEN            (1 << 9)
#define RCC_ADC1LPEN            (1 << 8)
#define RCC_USART6LPEN          (1 << 5)
#define RCC_USART1LPEN          (1 << 4)
#define RCC_TIM8LPEN            (1 << 1)
#define RCC_TIM1LPEN            (1 << 0)

// BDCR values
#define RCC_BDRST               (1 << 16)
#define RCC_RTCEN               (1 << 15)
#define RCC_RTCSEL_NOCLK        (0 << 8)
#define RCC_RTCSEL_LSE          (1 << 8)
#define RCC_RTCSEL_LSI          (2 << 8)
#define RCC_RTCSEL_HSE          (3 << 8)
#define RCC_LSEBYP              (1 << 2)
#define RCC_LSERDY              (1 << 1)
#define RCC_LSEON               (1 << 0)

// CSR values
#define RCC_LPWRRSTF            (1 << 31)
#define RCC_WWDGRSTF            (1 << 30)
#define RCC_IWDGRSTF            (1 << 29)
#define RCC_SFTRSTF             (1 << 28)
#define RCC_PORRSTF             (1 << 27)
#define RCC_PINRSTF             (1 << 26)
#define RCC_BORRSTF             (1 << 25)
#define RCC_RMVF                (1 << 24)
#define RCC_LSIRDY              (1 << 1)
#define RCC_LSION               (1 << 0)

// SSCGR values
#define RCC_SSCGEN              (1 << 31)
#define RCC_SPREADSEL           (1 << 30)
#define RCC_INCSTEP(n)          ((n) << 13)
#define RCC_MODPER(n)           (n)

// PLLI2SCFGR
#define RCC_PLLI2SR(n)          ((n) << 28)
#define RCC_PLLI2SN(n)          ((n) << 6)


/////////////////////////////////////////
// Flash interface
/////////////////////////////////////////
typedef struct
{
    arm_reg_t ACR;
    arm_reg_t KEYR;
    arm_reg_t OPTKEYR;
    arm_reg_t SR;
    arm_reg_t CR;
    arm_reg_t OPTCR;
} FLASH_t;

#define FLASH_IFACE     ((FLASH_t*) STM_FLASH_IFACE_BASE)

// ACR values
#define FLASH_DCRST             (1 << 12)
#define FLASH_ICRST             (1 << 11)
#define FLASH_DCEN              (1 << 10)
#define FLASH_ICEN              (1 << 9)
#define FLASH_PRFTEN            (1 << 8)
#define FLASH_LATENCY(n)        (n)

// KEYR values
#define FLASH_KEY1              0x45670123
#define FLASH_KEY2              0xCDEF89AF

// OPTKEYR values
#define FLASH_OPTKEY1           0x08192A3B
#define FLASH_OPTKEY2           0x4C5D6E7F

// SR values
#define FLASH_BSY               (1 << 16)
#define FLASH_PGSERR            (1 << 7)
#define FLASH_PGPERR            (1 << 6)
#define FLASH_PGAERR            (1 << 5)
#define FLASH_WRPERR            (1 << 4)
#define FLASH_OPERR             (1 << 1)
#define FLASH_EOP               (1 << 0)

// CR values
#define FLASH_LOCK              (1 << 31)
#define FLASH_ERRIE             (1 << 25)
#define FLASH_EOPIE             (1 << 24)
#define FLASH_STRT              (1 << 16)
#define FLASH_PSIZE_8           (0 << 8)
#define FLASH_PSIZE_16          (1 << 8)
#define FLASH_PSIZE_32          (2 << 8)
#define FLASH_PSIZE_64          (3 << 8)
#define FLASH_SNB(n)            ((n) << 3)
#define FLASH_MER               (1 << 2)
#define FLASH_SER               (1 << 1)
#define FLASH_PG                (1 << 0)

// OPTCR values
#define FLASH_NWRP(n)           (1 << (n) << 16)
#define FLASH_RDP_LEVEL0        (0xAA << 8)
#define FLASH_RDP_LEVEL1        (0x00 << 8)
#define FLASH_RDP_LEVEL2        (0xCC << 8)
#define FLASH_NRST_STDBY        (1 << 7)
#define FLASH_NRST_STOP         (1 << 6)
#define FLASH_WDG_SW            (1 << 5)
#define FLASH_BOR_LEV(n)        ((n) << 2)
#define FLASH_OPTSTRT           (1 << 1)
#define FLASH_OPTLOCK           (1 << 0)


/////////////////////////////////////////
// General purpose I/O
/////////////////////////////////////////
typedef struct
{
    arm_reg_t MODER;    // Mode
    arm_reg_t OTYPER;   // Output type
    arm_reg_t OSPEEDR;  // Output speed
    arm_reg_t PUPDR;    // Pull-up/pull-down
    arm_reg_t IDR;      // Input data
    arm_reg_t ODR;      // Output data
    arm_reg_t BSRR;     // Bit set/reset
    arm_reg_t LCKR;     // Configuration lock
    arm_reg_t AFRL;     // Alternate function low register
    arm_reg_t AFRH;     // Alternate function high register
} GPIO_t;

#define GPIOA       ((GPIO_t*) STM_GPIOA_BASE)
#define GPIOB       ((GPIO_t*) STM_GPIOB_BASE)
#define GPIOC       ((GPIO_t*) STM_GPIOC_BASE)
#define GPIOD       ((GPIO_t*) STM_GPIOD_BASE)
#define GPIOE       ((GPIO_t*) STM_GPIOE_BASE)
#define GPIOF       ((GPIO_t*) STM_GPIOF_BASE)
#define GPIOG       ((GPIO_t*) STM_GPIOG_BASE)
#define GPIOH       ((GPIO_t*) STM_GPIOH_BASE)
#define GPIOI       ((GPIO_t*) STM_GPIOI_BASE)

// MODER values
#define GPIO_IN(n)      (0 << (2 * n))  // Input mode
#define GPIO_OUT(n)     (1 << (2 * n))  // General purpose output mode
#define GPIO_ALT(n)     (2 << (2 * n))  // Alternate function mode
#define GPIO_ANA(n)     (3 << (2 * n))  // Analog mode

// OTYPER values
#define GPIO_PP(n)      (0 << (n))      // Push-pull
#define GPIO_OD(n)      (1 << (n))      // Open drain

// OSPEEDR values
#define GPIO_2MHz(n)    (0 << (2 * n))  // Low speed
#define GPIO_25MHz(n)   (1 << (2 * n))  // Medium speed
#define GPIO_50MHz(n)   (2 << (2 * n))  // Fast speed
#define GPIO_100MHz(n)  (3 << (2 * n))  // High speed

// PUPDR values
#define GPIO_NO_PULL(n)   (0 << (2 * n))  // No pull-up, no pull-down
#define GPIO_PULL_UP(n)   (1 << (2 * n))  // Pull-up
#define GPIO_PULL_DOWN(n) (2 << (2 * n))  // Pull-down

// BSRR values
#define GPIO_SET(n)     (1 << (n))      // Atomic set n-th bit of ODR
#define GPIO_RESET(n)   (1 << (n + 16)) // Atomic reset n-th bit of ODR

// LCKR values
#define GPIO_LCK(n)     (1 << (n))      // Pin configuration lock
#define GPIO_LCKK(n)    (1 << 16)       // Port configuration lock key

// Alternate functions (AFRL and AFRH values)
#define GPIO_AF_SYSTEM(n)   (0x0 << (4 * ((n) & 7)))
#define GPIO_AF_TIM1(n)     (0x1 << (4 * ((n) & 7)))
#define GPIO_AF_TIM2(n)     (0x1 << (4 * ((n) & 7)))
#define GPIO_AF_TIM3(n)     (0x2 << (4 * ((n) & 7)))
#define GPIO_AF_TIM4(n)     (0x2 << (4 * ((n) & 7)))
#define GPIO_AF_TIM5(n)     (0x2 << (4 * ((n) & 7)))
#define GPIO_AF_TIM8(n)     (0x3 << (4 * ((n) & 7)))
#define GPIO_AF_TIM9(n)     (0x3 << (4 * ((n) & 7)))
#define GPIO_AF_TIM10(n)    (0x3 << (4 * ((n) & 7)))
#define GPIO_AF_TIM11(n)    (0x3 << (4 * ((n) & 7)))
#define GPIO_AF_I2C1(n)     (0x4 << (4 * ((n) & 7)))
#define GPIO_AF_I2C2(n)     (0x4 << (4 * ((n) & 7)))
#define GPIO_AF_I2C3(n)     (0x4 << (4 * ((n) & 7)))
#define GPIO_AF_SPI1(n)     (0x5 << (4 * ((n) & 7)))
#define GPIO_AF_SPI2(n)     (0x5 << (4 * ((n) & 7)))
#define GPIO_AF_SPI3(n)     (0x6 << (4 * ((n) & 7)))
#define GPIO_AF_USART1(n)   (0x7 << (4 * ((n) & 7)))
#define GPIO_AF_USART2(n)   (0x7 << (4 * ((n) & 7)))
#define GPIO_AF_USART3(n)   (0x7 << (4 * ((n) & 7)))
#define GPIO_AF_USART4(n)   (0x8 << (4 * ((n) & 7)))
#define GPIO_AF_USART5(n)   (0x8 << (4 * ((n) & 7)))
#define GPIO_AF_USART6(n)   (0x8 << (4 * ((n) & 7)))
#define GPIO_AF_CAN1(n)     (0x9 << (4 * ((n) & 7)))
#define GPIO_AF_CAN2(n)     (0x9 << (4 * ((n) & 7)))
#define GPIO_AF_TIM12(n)    (0x9 << (4 * ((n) & 7)))
#define GPIO_AF_TIM13(n)    (0x9 << (4 * ((n) & 7)))
#define GPIO_AF_TIM14(n)    (0x9 << (4 * ((n) & 7)))
#define GPIO_AF_OTG_FS(n)   (0xA << (4 * ((n) & 7)))
#define GPIO_AF_OTG_HS(n)   (0xA << (4 * ((n) & 7)))
#define GPIO_AF_ETH(n)      (0xB << (4 * ((n) & 7)))
#define GPIO_AF_FSMC(n)     (0xC << (4 * ((n) & 7)))
#define GPIO_AF_SDIO(n)     (0xC << (4 * ((n) & 7)))
#define GPIO_AF_OTG_HS_FS(n) (0xC << (4 * ((n) & 7)))
#define GPIO_AF_DCMI(n)     (0xD << (4 * ((n) & 7)))
#define GPIO_AF_EVENTOUT(n) (0xF << (4 * ((n) & 7)))


/////////////////////////////////////////
// USART
/////////////////////////////////////////
typedef struct
{
    arm_reg_t SR;
    arm_reg_t DR;
    arm_reg_t BRR;
    arm_reg_t CR1;
    arm_reg_t CR2;
    arm_reg_t CR3;
    arm_reg_t GTPR;
} USART_t;

#define USART1   ((USART_t*) STM_USART1_BASE)
#define USART2   ((USART_t*) STM_USART2_BASE)
#define USART3   ((USART_t*) STM_USART3_BASE)
#define UART4    ((USART_t*) STM_UART4_BASE)
#define UART5    ((USART_t*) STM_UART5_BASE)
#define USART6   ((USART_t*) STM_USART6_BASE)

// SR values
#define USART_CTS           (1 << 9)
#define USART_LBD           (1 << 8)
#define USART_TXE           (1 << 7)
#define USART_TC            (1 << 6)
#define USART_RXNE          (1 << 5)
#define USART_IDLE          (1 << 4)
#define USART_ORE           (1 << 3)
#define USART_NF            (1 << 2)
#define USART_FE            (1 << 1)
#define USART_PE            (1 << 0)

// BRR values
#define USART_DIV_MANTISSA(n)   ((n) << 4)
#define USART_DIV_FRACTION(n)   (n)

// CR1 values
#define USART_OVER8         (1 << 15)
#define USART_UE            (1 << 13)
#define USART_M             (1 << 12)
#define USART_WAKE          (1 << 11)
#define USART_PCE           (1 << 10)
#define USART_PS            (1 << 9)
#define USART_PEIE          (1 << 8)
#define USART_TXEIE         (1 << 7)
#define USART_TCIE          (1 << 6)
#define USART_RXNEIE        (1 << 5)
#define USART_IDLEIE        (1 << 4)
#define USART_TE            (1 << 3)
#define USART_RE            (1 << 2)
#define USART_RWU           (1 << 1)
#define USART_SBK           (1 << 0)

// CR2 values
#define USART_LINEN         (1 << 14)
#define USART_STOP_1        (0 << 12)
#define USART_STOP_05       (1 << 12)
#define USART_STOP_2        (2 << 12)
#define USART_STOP_15       (3 << 12)
#define USART_CLKEN         (1 << 11)
#define USART_CPOL          (1 << 10)
#define USART_CPHA          (1 << 9)
#define USART_LBCL          (1 << 8)
#define USART_LBDIE         (1 << 6)
#define USART_LBDL          (1 << 5)
#define USART_ADD(n)        (n)

// CR3 values
#define USART_ONEBIT        (1 << 11)
#define USART_CTSIE         (1 << 10)
#define USART_CTSE          (1 << 9)
#define USART_RTSE          (1 << 8)
#define USART_DMAT          (1 << 7)
#define USART_DMAR          (1 << 6)
#define USART_SCEN          (1 << 5)
#define USART_NACK          (1 << 4)
#define USART_HDSEL         (1 << 3)
#define USART_IRLP          (1 << 2)
#define USART_IREN          (1 << 1)
#define USART_EIE           (1 << 0)

// GTPR values
#define USART_GT(n)         ((n) << 8)
#define USART_PSC(n)        (n)
