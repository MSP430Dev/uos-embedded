#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

ARRAY(stack_test, 1000);
ARRAY(stack_test2, 1000);

mutex_t lock;

void task_test(void* arg);
void task_test2(void* arg);
static int system_timer_irq_handler(void* arg);

unsigned long counter;

void uos_init()
{
	mutex_attach_irq(&lock, 15, (handler_t)system_timer_irq_handler, 0);
	
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_TIMER2;
	ARM_RSTCLK->TIM_CLOCK |= ARM_TIM2_CLK_EN;
	ARM_RSTCLK->TIM_CLOCK &= ~ARM_TIM2_BRG(7);
	ARM_TIMER2->TIM_CNTRL = 0;
	ARM_TIMER2->TIM_CNT = 0;
	ARM_TIMER2->TIM_PSG = KHZ - 1;
	ARM_TIMER2->TIM_ARR = 1000 - 1;
	ARM_TIMER2->TIM_IE = 2;
	ARM_TIMER2->TIM_CNTRL = 1;
	
	task_create(task_test, 0, "task_test", 2, stack_test, sizeof(stack_test));
	task_create(task_test2, 0, "task_test2", 1, stack_test2, sizeof(stack_test2));
}

void task_test(void* arg)
{
	debug_printf("test task init\n");
	
	mutex_lock(&lock);

	while (1)
	{
		mutex_wait(&lock);
		debug_printf("test irq %d\n", counter);
	}
}

void task_test2(void* arg)
{
	volatile int counter1 = 0;
	int counter2 = 0;
	arch_state_t x;
	
	while (1)
	{
		counter1++;
		if (counter1 == 10000000)
		{
			counter1 = 0;
			counter2++;
		    if (counter2 == 5) {
		        debug_printf("DISABLING INTERRUPTS\n");
			    arch_intr_disable(&x);
		    }
			debug_printf("counter reload, ISER0 = %08X\n", ARM_NVIC_ISER(0));
		}
	}
}

static int system_timer_irq_handler(void* arg)
{
	debug_printf("timer irq, TIM_CNT = %d\n", ARM_TIMER2->TIM_CNT);
	counter++;
	ARM_TIMER2->TIM_STATUS = 0;
	arch_intr_allow(15);

	return 0;
}
