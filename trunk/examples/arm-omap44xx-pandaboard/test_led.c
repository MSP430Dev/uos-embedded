/*
 * Testing leds
 */
#include <runtime/lib.h>

void _irq_handler_ ()
{
}

void my_delay(unsigned int count)
{
	int i;
	for (i = 0; i < count; i++)
		asm volatile ("nop");
}

void main (void)
{
	// Включение синхров для GPIO1
	CM_WKUP_GPIO1_CLKCTRL = MODULEMODE(1);
	while (GET_IDLEST(CM_WKUP_GPIO1_CLKCTRL));

	
	// Настройка мультиплексоров ножек, связанных с LED
	CONTROL_WKUP_PAD0_FREF_CLK3_OUT_PAD1_FREF_CLK4_REQ = MUXMODE2(3);
	CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K = MUXMODE1(3);
	
	// Настройка направления ножек (OUT)
	GPIO_OE(1) &= ~((1 << 7) | (1 << 8));
	
	// Мигание светодиодами
	for (;;) {
		GPIO_DATAOUT(1) &= ~((1 << 7) | (1 << 8));
		my_delay(10000000);
		GPIO_DATAOUT(1) |= (1 << 7) | (1 << 8);
		my_delay(10000000);
	}
}
