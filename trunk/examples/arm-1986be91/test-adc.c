//
// Тест проверяет работу ADC.
// На отладочной плате читается напряжение на подстроечном резисторе TRIM.
//

#include <runtime/lib.h>

int main (void)
{
    unsigned x;
    
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_ADC;
	ARM_RSTCLK->ADC_MCO_CLOCK |= ARM_ADC_CLK_EN;

	ARM_ADC->ADC1_CHSEL = 0;
	ARM_ADC->ADC1_CFG = ARM_CFG_REG_ADON | ARM_CFG_REG_DIVCLK(3) | ARM_TS_EN | 
		ARM_TS_BUF_EN | ARM_DELAY_GO(8);
	ARM_ADC->ADC1_RESULT;
	ARM_ADC->ADC1_STATUS = 0;

	ARM_ADC->ADC2_CHSEL = 0;
	ARM_ADC->ADC2_CFG = 0;
	ARM_ADC->ADC2_RESULT;
	ARM_ADC->ADC2_STATUS = 0;

	for (;;) {
    	debug_printf ("\33[2J\33[H");
	    ARM_ADC->ADC1_CFG = (ARM_ADC->ADC1_CFG & ~ARM_CFG_REG_CHS(0x1F)) | ARM_CFG_REG_CHS(7);
	    ARM_ADC->ADC1_CFG |= ARM_CFG_REG_GO;
	    while ((ARM_ADC->ADC1_STATUS & ARM_FLG_REG_EOCIF) == 0);
   	    x = ARM_ADC->ADC1_RESULT & 0xFFF;      
	    debug_printf ("x = %d\n", x);

	    mdelay(100);
	}
}

