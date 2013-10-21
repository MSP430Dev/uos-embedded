/*
 * Чтение/запись памяти и регистров MBA осуществляется с помощью обычного обращения
 * по адресу, чтение/запись регистров SWIC, PMSC и адресного окна PCI должна
 * осуществляться только с помощью функций mcb_read_reg/mcb_write_reg.
 * Для разъяснений см. Руководство пользователя на микросхему 1892ХД1Я, п. 5.3.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#ifdef ELVEES_MCB01
#   include <elvees/mcb-01.h>
#elif defined ELVEES_MCB03
#   include <elvees/mcb-03.h>
#endif

#define TIMEOUT	100000

unsigned
mcb_read_reg (unsigned addr)
{
	unsigned count;
	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
//	if (! count)
//		debug_printf ("mcb_read_reg: timeout before writing %08X to BDR\n",
//			addr);

	MCB_MBA_BDR = addr;

	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count) {
//		debug_printf ("mcb_read_reg: timeout after writing %08X to BDR\n",
//			addr);
		return ~0;
	}
	return MCB_MBA_BDR;
}

void
mcb_write_reg (unsigned addr, unsigned value)
{
	volatile unsigned *modif_addr = (unsigned*) (addr | MCB_BASE);
	unsigned count;
	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count)
		debug_printf ("mcb_write_reg: timeout before writing %08X to %08X\n",
			value, modif_addr);

	*modif_addr = value;

	for (count=TIMEOUT; count>0; count--)
		if (! MCB_MBA_BUSY)
			break;
	if (! count)
		debug_printf ("mcb_write_reg: timeout after writing %08X to %08X\n",
			value, modif_addr);
}


static mutex_t interrupt_mutex;
static list_t  interrupt_handlers;
static int     interrupt_task_created = 0;

static void interrupt_task (void *arg)
{
    int irq = (int) arg;
    
    mutex_lock_irq (&interrupt_mutex, irq, 0, 0);
    
    for (;;) {
        mutex_wait (&interrupt_mutex);
        
        if (! list_is_empty (&interrupt_handlers)) {
            mcb_intr_handler_t *ih;
            list_iterate (ih, &interrupt_handlers) {
                if ((MCB_MBA_QSTR & ih->mask0) ||
                    (MCB_MBA_QSTR1 & ih->mask1)) {
//debug_printf ("MCB_MBA_QSTR = %08X, ih->mask0 = %08X, MCB_MBA_QSTR1 = %08X, ih->mask1 = %08X, handler @ %p\n",
//    MCB_MBA_QSTR, ih->mask0, MCB_MBA_QSTR1, ih->mask1, ih->handler);
                        if (ih->handler_lock)
                            mutex_lock (ih->handler_lock);
                        if (ih->handler)
                            ih->handler(ih->handler_arg);
                        if (ih->handler_lock) {
                            mutex_signal (ih->handler_lock, 0);
                            mutex_unlock (ih->handler_lock);
                        }
                }
            }
        }
    }
}

void mcb_create_interrupt_task (int irq, int prio, void *stack, int stacksz)
{
    if (! interrupt_task_created) {
        list_init (&interrupt_handlers);
        task_create (interrupt_task, (void *)irq, "mcb_interrupt", 
            prio, stack, stacksz);
        interrupt_task_created = 1;
    }
}

void mcb_register_interrupt_handler (mcb_intr_handler_t *ih)
{
    list_init (&ih->item);
    mutex_lock (&interrupt_mutex);
    list_append (&interrupt_handlers, &ih->item);
    MCB_MBA_MASK |= ih->mask0;
    MCB_MBA_MASK1 |= ih->mask1;
    mutex_unlock (&interrupt_mutex);
}
