/*
 * Testing Ethernet through MDB
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <elvees/eth.h>


/* software breakpoint */
#define STOP            asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile(".word 0x4d"); \
                        asm volatile("nop"); \
                        asm volatile("nop"); \
                        asm volatile("nop")
                        

ARRAY (stack_test, 1000);		/* Task: transmit/receive packets */
mem_pool_t pool;
eth_t eth;
timer_t timer;
int start_delay = 3000;
int packet_size = 1500;
int local_loop = 1;
int nb_of_packets = 1000;
int nb_of_received = 0;
unsigned nerr = 0;
unsigned char *data_pattern;

void main_test ()
{
	buf_t *p;
    int i = 0;
    const unsigned delay = 20;
    
    mdelay(start_delay);
    
	data_pattern = mem_alloc (&pool, packet_size);
	memset (data_pattern, 0xa5, packet_size);
    
	if (packet_size >= 12)
		memcpy (data_pattern+6, eth.netif.ethaddr, 6);

    unsigned long time0 = timer_milliseconds(&timer);
    
    while (1) {
	//for (i = 0; i < nb_of_packets; ++i) {
		/* Check received data. */
		p = netif_input (&eth.netif);
		if (p) {
			if (memcmp (p->payload, data_pattern, packet_size) != 0)
				nerr++;
            nb_of_received++;
			buf_free (p);
			continue;
		}
		/* Send packets - make transmit queue full. */
        if (i++ < nb_of_packets) {
            p = buf_alloc (&pool, packet_size, 16);
            if (p) {
                memcpy (p->payload, data_pattern, packet_size);
                netif_output (&eth.netif, p, 0, 0);
            } else debug_printf("no mem\n");
        }
        
        if ((nb_of_received == nb_of_packets) ||
            timer_passed(&timer, time0, delay * nb_of_packets * 2))
            break;

		timer_delay (&timer, delay);
	}
    
    nerr += nb_of_packets - nb_of_received;
    
    STOP;
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */
    
	/* Выделяем место для динамической памяти */
	extern unsigned __bss_end[];
#ifdef ELVEES_DATA_SDRAM
	/* Динамическая память в SDRAM */
	if (((unsigned) __bss_end & 0xF0000000) == 0x80000000)
		mem_init (&pool, (unsigned) __bss_end, 0x82000000);
	else
		mem_init (&pool, (unsigned) __bss_end, 0xa2000000);
#else
	/* Динамическая память в CRAM */
	extern unsigned _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
#endif

	timer_init (&timer, KHZ, 1);

	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (&eth, "eth0", 80, &pool, 0, my_macaddr);
    
    eth_set_promisc (&eth, 1, 1);
    
    if (local_loop)
        eth_set_loop (&eth, local_loop);

	task_create (main_test, 0, "test", 5,
		stack_test, sizeof (stack_test));
}
