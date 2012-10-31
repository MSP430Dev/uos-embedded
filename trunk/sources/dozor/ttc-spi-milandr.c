#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <kernel/internal.h>
#include <stream/stream.h>

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
	int cnt = 0;

	while (! (reg->SR & ARM_SSP_SR_RNE)) {
		if (cnt++ > 10000) break;
	}
	*word = reg->DR;
}

void ttc_spi_throw (int port, int count)
{
	SSP_t *reg = (port == 0) ? ARM_SSP1 : ARM_SSP2;
	unsigned short word;
	int i;
	int cnt = 0;
	
	for (i = 0; i < count; ++i) {
		while (! (reg->SR & ARM_SSP_SR_RNE)) {
			if (cnt++ > 10000) break;
		}
		word = reg->DR;
	}
}

void ttc_spi_init (int port, unsigned nsec_per_bit)
{
	/* Выбор соответствующего интерфейса SSP и
	 * установка внешних сигналов. */
	SSP_t *reg;
	if (port == 0) {
		/* Включаем тактирование порта SSP1. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP1;
		reg = ARM_SSP1;
		/* Разрешение тактовой частоты на SSP1, источник HCLK. */
		ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
			ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
	} else {
		/* Включаем тактирование порта SSP2. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP2;
		reg = ARM_SSP2;
		/* Разрешение тактовой частоты на SSP2, источник HCLK. */
		ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
			ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
	}

	/* Инициализация всех регистров данного интерфейса SSP.
	 * Ловим прерывания от приёмника. */
	reg->CR0 = ARM_SSP_CR0_FRF_SPI | ARM_SSP_CR0_DSS (16) | ARM_SSP_CR0_SPH;
	/* Режим master. */
	unsigned divisor = (KHZ * nsec_per_bit + 1999999) / 2000000;
	reg->CR0 |= ARM_SSP_CR0_SCR (divisor - 1);
	reg->CR1 = 0;
	reg->CPSR = 2;
	reg->DMACR = 0;
	reg->IM = ARM_SSP_IM_RX | ARM_SSP_IM_RT;
}

void ttc_spi_init_pins (int port)
{
	if (port == 0) {
		milandr_init_pin (SSP1_RXD_GPIO, PORT(SSP1_RXD), PIN(SSP1_RXD), SSP1_RXD_FUNC);
		milandr_init_pin (SSP1_TXD_GPIO, PORT(SSP1_TXD), PIN(SSP1_TXD), SSP1_TXD_FUNC);
		milandr_init_pin (SSP1_FSS_GPIO, PORT(SSP1_FSS), PIN(SSP1_FSS), SSP1_FSS_FUNC);
		milandr_init_pin (SSP1_CLK_GPIO, PORT(SSP1_CLK), PIN(SSP1_CLK), SSP1_CLK_FUNC);
	} else {
		milandr_init_pin (SSP2_RXD_GPIO, PORT(SSP2_RXD), PIN(SSP2_RXD), SSP2_RXD_FUNC);
		milandr_init_pin (SSP2_TXD_GPIO, PORT(SSP2_TXD), PIN(SSP2_TXD), SSP2_TXD_FUNC);
		milandr_init_pin (SSP2_FSS_GPIO, PORT(SSP2_FSS), PIN(SSP2_FSS), SSP2_FSS_FUNC);
		milandr_init_pin (SSP2_CLK_GPIO, PORT(SSP2_CLK), PIN(SSP2_CLK), SSP2_CLK_FUNC);	
	}
}

