/*
 * Проверка работы сторожевого таймера IDWT
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <milandr/iwdt.h>

ARRAY (task, 1000);

void hello (void *arg)
{
    unsigned counter = 0;
    
	for (;;) {
	    debug_printf ("hello\n");
	    mdelay(200);
	    if (++counter > 9) {
	        //ack_iwdt ();
	    }
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting IWDT\r\n\n");
	
	task_create (hello, "Hello!", "hello", 1, task, sizeof (task));
	
	init_iwdt (2000);
}
