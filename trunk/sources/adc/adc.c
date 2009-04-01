#include "runtime/lib.h"
#include "kernel/uos.h"
#include "adc/adc.h"

#if __AVR__
#   define ADC_IRQ		20	/* ADC complete */
#endif

/*
 * Read a result from ADC.
 */
unsigned
adc_read (adc_t *v)
{
	unsigned val;

	lock_take (&v->lock);

#if __AVR__
	/* Start measuring in Single Conversion mode. */
	setb (ADSC, ADCSR);

	/* Wait until ADSC becomes zero.
	 * It could not take us too long. */
	while (testb (ADSC, ADCSR))
		lock_wait (&v->lock);

	val = ADCL;
	val |= ADCH << 8;
#endif

#if ARM_AT91SAM
	/* Start conversion. */
	*AT91C_ADC_CR = AT91C_ADC_START;

	/* Wait for end of convertion. */
	while (! (*AT91C_ADC_SR & (1 << v->channel)))
		continue;

	val = AT91C_ADC_CDR0 [v->channel];
#endif
	lock_release (&v->lock);
	return val;
}

/*
 * Select analog channel connected to ADC.
 */
void
adc_select_channel (adc_t *v, small_uint_t cnum)
{
	v->channel = cnum;
#if __AVR__
	ADMUX = v->channel;
#endif
#if ARM_AT91SAM
	/* Enable desired channel */
	*AT91C_ADC_CHER = 1 << v->channel;
#endif
}

void
adc_init (adc_t *v)
{
#if __AVR__
	/* Turn ADC on, prescaler 1/128. */
	ADCSR = 0x87;

	/* Get the interrupt. */
	lock_take_irq (&v->lock, ADC_IRQ, 0, 0);
	lock_release (&v->lock);
#endif

#if ARM_AT91SAM
	/* Enable clock for interface. */
	*AT91C_PMC_PCER = 1 << AT91C_ID_ADC;

	/* Reset */
	*AT91C_ADC_CR = 0x1;
	*AT91C_ADC_CR = 0x0;

	/* Set maximum startup time and hold time. */
	*AT91C_ADC_MR = 0x0F1F0F00;
#endif
}
