/*
 * Testing UART.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "elvees/mct-02.h"

ARRAY (task[UART_MAX], 0x400);
uart_t uartx[UART_MAX];
volatile unsigned int uart[UART_MAX]={ UART0,UART1,UART2,UART3 };
#define DELAY_TASK	500000

void hello (void *data)
{
	int i,n;

	n=(int)data;
	for (i=0;;i++) {
		printf (&uartx[n], "\n[%d] Hello, world! >> UART%d", i, uart[n]);
		if ((i%10)==0) getchar (&uartx[n]);
		//udelay(DELAY_TASK);
	}
}

void uos_init (void)
{
	int i;
	unsigned char task_name[64];

	for (i=0;i<UART_MAX;i++) {
		uart_init (&uartx[i], uart[i], 90+i, KHZ, 115200);
		/* Стираем экран. */
		printf (&uartx[i], "\33[H\33[2J");
		printf (&uartx[i], "\nStart testing UARTs. >> UART%d\n", uart[i]);
		snprintf(task_name, sizeof(task_name), "hello%d", i);
		task_create (hello,(void*)i, (const char*)task_name, 2+i, task[i], sizeof (task[i]));
	};
}
