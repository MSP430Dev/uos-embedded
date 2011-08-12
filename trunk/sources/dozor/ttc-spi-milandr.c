#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <kernel/internal.h>

void ttc_spi_out (int port, unsigned short *data, int count)
{
	SSP_t *reg = (port == 0) ? ARM_SSP1 : ARM_SSP2;
	int i;

	reg->CR1 &= ~ARM_SSP_CR1_SSE;
	for (i = 0; i < count; ++i)
		reg->DR = *(data + i);	
	reg->CR1 |= ARM_SSP_CR1_SSE;
}

void ttc_spi_in (int port, unsigned short *word)
{
	SSP_t *reg = (port == 0) ? ARM_SSP1 : ARM_SSP2;
	while (! (reg->SR & ARM_SSP_SR_RNE));
	*word = reg->DR;
}

void ttc_spi_throw (int port, int count)
{
	SSP_t *reg = (port == 0) ? ARM_SSP1 : ARM_SSP2;
	unsigned short word;
	int i;

	for (i = 0; i < count; ++i) {
		while (! (reg->SR & ARM_SSP_SR_RNE));
		word = reg->DR;
	}
}

static void spi_init_pin (GPIO_t *gpio, unsigned port, unsigned pin, unsigned func)
{
//debug_printf ("SPI init: port = %d, pin = %d, func = %d\n", port, pin, func);
	/* Подача синхроимпульсов */
	ARM_RSTCLK->PER_CLOCK |= (ARM_PER_CLOCK_GPIOA << port);
	/* Установка функции */
	gpio->FUNC = (gpio->FUNC & ~ARM_FUNC_MASK(pin)) | ARM_FUNC(pin, func);
	/* Цифровой вывод */
	gpio->ANALOG |= (1 << pin);
	/* Быстрый фронт */
	gpio->PWR = (gpio->PWR & ~ARM_PWR_MASK(pin)) | ARM_PWR_FAST(pin);
}

/*
 * Инициализация внешних сигналов SSP1.
 */
static void spi_setup_ssp1 ()
{
/* Сигнал SSP1_RXD */
#ifndef SSP1_RXD
#define SSP1_RXD	PD11
#endif

#if (PORT(SSP1_RXD)==PORT_B)
#	define SSP1_RXD_GPIO ARM_GPIOB
#	define SSP1_RXD_FUNC FUNC_ALT
#elif (PORT(SSP1_RXD)==PORT_D)
#	define SSP1_RXD_GPIO ARM_GPIOD
#	define SSP1_RXD_FUNC FUNC_REDEF
#elif (PORT(SSP1_RXD)==PORT_E)
#	define SSP1_RXD_GPIO ARM_GPIOE
#	define SSP1_RXD_FUNC FUNC_ALT
#elif (PORT(SSP1_RXD)==PORT_F)
#	define SSP1_RXD_GPIO ARM_GPIOF
#	define SSP1_RXD_FUNC FUNC_ALT
#else
#	error "SSP1_RXD pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP1_TXD */
#ifndef SSP1_TXD
#define SSP1_TXD	PD12
#endif

#if (PORT(SSP1_TXD)==PORT_B)
#	define SSP1_TXD_GPIO ARM_GPIOB
#	define SSP1_TXD_FUNC FUNC_ALT
#elif (PORT(SSP1_TXD)==PORT_D)
#	define SSP1_TXD_GPIO ARM_GPIOD
#	define SSP1_TXD_FUNC FUNC_REDEF
#elif (PORT(SSP1_TXD)==PORT_F)
#	define SSP1_TXD_GPIO ARM_GPIOF
#	define SSP1_TXD_FUNC FUNC_ALT
#else
#	error "SSP1_TXD pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP1_FSS */
#ifndef SSP1_FSS
#define SSP1_FSS	PD9
#endif

#if (PORT(SSP1_FSS)==PORT_B)
#	define SSP1_FSS_GPIO ARM_GPIOB
#	define SSP1_FSS_FUNC FUNC_ALT
#elif (PORT(SSP1_FSS)==PORT_D)
#	define SSP1_FSS_GPIO ARM_GPIOD
#	define SSP1_FSS_FUNC FUNC_REDEF
#elif (PORT(SSP1_FSS)==PORT_E)
#	define SSP1_FSS_GPIO ARM_GPIOE
#	define SSP1_FSS_FUNC FUNC_ALT
#elif (PORT(SSP1_FSS)==PORT_F)
#	define SSP1_FSS_GPIO ARM_GPIOF
#	define SSP1_FSS_FUNC FUNC_ALT
#else
#	error "SSP1_FSS pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP1_CLK */
#ifndef SSP1_CLK
#define SSP1_CLK	PD10
#endif

#if (PORT(SSP1_CLK)==PORT_B)
#	define SSP1_CLK_GPIO ARM_GPIOB
#	define SSP1_CLK_FUNC FUNC_ALT
#elif (PORT(SSP1_CLK)==PORT_D)
#	define SSP1_CLK_GPIO ARM_GPIOD
#	define SSP1_CLK_FUNC FUNC_REDEF
#elif (PORT(SSP1_CLK)==PORT_F)
#	define SSP1_CLK_GPIO ARM_GPIOF
#	define SSP1_CLK_FUNC FUNC_ALT
#else
#	error "SSP1_CLK pin is not assigned in CFLAGS of target.cfg"
#endif
	/* Включаем тактирование порта SSP1. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP1;

	milandr_init_pin (SSP1_RXD_GPIO, PORT(SSP1_RXD), PIN(SSP1_RXD), SSP1_RXD_FUNC);
	milandr_init_pin (SSP1_TXD_GPIO, PORT(SSP1_TXD), PIN(SSP1_TXD), SSP1_TXD_FUNC);
	milandr_init_pin (SSP1_FSS_GPIO, PORT(SSP1_FSS), PIN(SSP1_FSS), SSP1_FSS_FUNC);
	milandr_init_pin (SSP1_CLK_GPIO, PORT(SSP1_CLK), PIN(SSP1_CLK), SSP1_CLK_FUNC);

	/* Разрешение тактовой частоты на SSP1, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
		ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
}

/*
 * Инициализация внешних сигналов SSP2.
 */
static void spi_setup_ssp2 ()
{
/* Сигнал SSP2_RXD */
#ifndef SSP2_RXD
#define SSP2_RXD	PD2
#endif

#if (PORT(SSP2_RXD)==PORT_B)
#	define SSP2_RXD_GPIO ARM_GPIOB
#	define SSP2_RXD_FUNC FUNC_REDEF
#elif (PORT(SSP2_RXD)==PORT_C)
#	define SSP2_RXD_GPIO ARM_GPIOC
#	if (PIN(SSP2_RXD)==2)
#		define SSP2_RXD_FUNC FUNC_REDEF
#	elif (PIN(SSP2_RXD)==15)
#		define SSP2_RXD_FUNC FUNC_ALT
#	endif
#elif (PORT(SSP2_RXD)==PORT_D)
#	define SSP2_RXD_GPIO ARM_GPIOD
#	define SSP2_RXD_FUNC FUNC_ALT
#elif (PORT(SSP2_RXD)==PORT_F)
#	define SSP2_RXD_GPIO ARM_GPIOF
#	define SSP2_RXD_FUNC FUNC_REDEF
#else
#	error "SSP2_RXD pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP2_TXD */
#ifndef SSP2_TXD
#define SSP2_TXD	PD6
#endif

#if (PORT(SSP2_TXD)==PORT_B)
#	define SSP2_TXD_GPIO ARM_GPIOB
#	define SSP2_TXD_FUNC FUNC_REDEF
#elif (PORT(SSP2_TXD)==PORT_C)
#	define SSP2_TXD_GPIO ARM_GPIOC
#	define SSP2_TXD_FUNC FUNC_REDEF
#elif (PORT(SSP2_TXD)==PORT_D)
#	define SSP2_TXD_GPIO ARM_GPIOD
#	define SSP2_TXD_FUNC FUNC_ALT
#elif (PORT(SSP2_TXD)==PORT_F)
#	define SSP2_TXD_GPIO ARM_GPIOF
#	define SSP2_TXD_FUNC FUNC_REDEF
#else
#	error "SSP2_TXD pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP2_FSS */
#ifndef SSP2_FSS
#define SSP2_FSS	PD3
#endif

#if (PORT(SSP2_FSS)==PORT_B)
#	define SSP2_FSS_GPIO ARM_GPIOB
#	define SSP2_FSS_FUNC FUNC_REDEF
#elif (PORT(SSP2_FSS)==PORT_C)
#	define SSP2_FSS_GPIO ARM_GPIOC
#	if (PIN(SSP2_FSS)==0)
#		define SSP2_FSS_FUNC FUNC_REDEF
#	elif (PIN(SSP2_FSS)==14)
#		define SSP2_FSS_FUNC FUNC_ALT
#	endif
#elif (PORT(SSP2_FSS)==PORT_D)
#	define SSP2_FSS_GPIO ARM_GPIOD
#	define SSP2_FSS_FUNC FUNC_ALT
#elif (PORT(SSP2_FSS)==PORT_F)
#	define SSP2_FSS_GPIO ARM_GPIOF
#	define SSP2_FSS_FUNC FUNC_REDEF
#else
#	error "SSP2_FSS pin is not assigned in CFLAGS of target.cfg"
#endif

/* Сигнал SSP2_CLK */
#ifndef SSP2_CLK
#define SSP2_CLK	PD5
#endif

#if (PORT(SSP2_CLK)==PORT_B)
#	define SSP2_CLK_GPIO ARM_GPIOB
#	define SSP2_CLK_FUNC FUNC_REDEF
#elif (PORT(SSP2_CLK)==PORT_C)
#	define SSP2_CLK_GPIO ARM_GPIOC
#	define SSP2_CLK_FUNC FUNC_REDEF
#elif (PORT(SSP2_CLK)==PORT_D)
#	define SSP2_CLK_GPIO ARM_GPIOD
#	define SSP2_CLK_FUNC FUNC_ALT
#elif (PORT(SSP2_CLK)==PORT_F)
#	define SSP2_CLK_GPIO ARM_GPIOF
#	define SSP2_CLK_FUNC FUNC_REDEF
#else
#	error "SSP2_CLK pin is not assigned in CFLAGS of target.cfg"
#endif
	/* Включаем тактирование порта SSP2. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP2;

	milandr_init_pin (SSP2_RXD_GPIO, PORT(SSP2_RXD), PIN(SSP2_RXD), SSP2_RXD_FUNC);
	milandr_init_pin (SSP2_TXD_GPIO, PORT(SSP2_TXD), PIN(SSP2_TXD), SSP2_TXD_FUNC);
	milandr_init_pin (SSP2_FSS_GPIO, PORT(SSP2_FSS), PIN(SSP2_FSS), SSP2_FSS_FUNC);
	milandr_init_pin (SSP2_CLK_GPIO, PORT(SSP2_CLK), PIN(SSP2_CLK), SSP2_CLK_FUNC);


	/* Разрешение тактовой частоты на SSP2, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
		ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
}

void ttc_spi_init (int port, unsigned nsec_per_bit)
{
	/* Выбор соответствующего интерфейса SSP и
	 * установка внешних сигналов. */
	SSP_t *reg;
	if (port == 0) {
		spi_setup_ssp1 ();
		reg = ARM_SSP1;
	} else {
		spi_setup_ssp2 ();
		reg = ARM_SSP2;
	}

	/* Инициализация всех регистров данного интерфейса SSP.
	 * Ловим прерывания от приёмника. */
	reg->CR0 = ARM_SSP_CR0_FRF_SPI | ARM_SSP_CR0_DSS (16) | ARM_SSP_CR0_SPH;
	/* Режим master. */
	unsigned divisor = (KHZ * nsec_per_bit + 1000000) / 2000000;
	reg->CR0 |= ARM_SSP_CR0_SCR (divisor);
	reg->CR1 = 0;
	reg->CPSR = 2;
	reg->DMACR = 0;
	reg->IM = ARM_SSP_IM_RX | ARM_SSP_IM_RT;
}

