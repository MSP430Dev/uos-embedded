#include "runtime/lib.h"
#include "kernel/uos.h"
#include "adc/adc.h"

#if __AVR__
#   define ADC_IRQ		20	/* ADC complete */
#endif

/*
 * Read a result from ADC.
 */
unsigned short
adc_read (adc_t *v)
{
	unsigned short val;

	lock_take (&v->lock);

	/* Start measuring in Single Conversion mode. */
	setb_const (ADSC, ADCSR);

	/* Wait until ADSC becomes zero.
	 * It could not take us too long. */
	while (testb (ADSC, ADCSR))
		lock_wait (&v->lock);

	val = inb (ADCL);
	val |= inb (ADCH) << 8;

	lock_release (&v->lock);
	return val;
}

/*
 * Select analog channel connected to ADC.
 */
void
adc_select_channel (adc_t *v, unsigned char cnum)
{
	outb (cnum, ADMUX);
}

void
adc_init (adc_t *v)
{
	/* Turn ADC on, prescaler 1/128. */
	outb (0x87, ADCSR);

	/* Get the interrupt. */
	lock_take_irq (&v->lock, ADC_IRQ, 0, 0);
	lock_release (&v->lock);
}
