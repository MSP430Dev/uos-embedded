#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#ifdef ELVEES_MCB03
#   include <elvees/spw-mcb.h>
#   define MCB_COMMON_IRQ          34  /* Прерывание от MCB */
#else
#   include <elvees/spw.h>
#endif

#define SPW0_CHANNEL	0
#define SPW1_CHANNEL	3

#define TX_SPEED	10
#define MSG_SIZE	4096	// в байтах
#define TX0_PER_CYCLE	1
#define TX1_PER_CYCLE	1

spw_t spw[2];
int channel_n[] = {SPW0_CHANNEL, SPW1_CHANNEL};
timer_t timer;

ARRAY (tx_stack,  1000);
ARRAY (rx0_stack, 1000);
ARRAY (rx1_stack, 1000);
ARRAY (con_stack, 1000);
ARRAY (stack_mcb, 1000);

task_t *rx0, *rx1, *tx;

unsigned snd_buf[2][MSG_SIZE / 4] __attribute__ ((aligned (8)));
unsigned rcv_buf[2][MSG_SIZE / 4] __attribute__ ((aligned (8)));
//unsigned *snd_buf[] = {(unsigned *) 0xb8400000,                  (unsigned *) (0xb8400000 + MSG_SIZE)};
//unsigned *rcv_buf[] = {(unsigned *) (0xb8400000 + 2 * MSG_SIZE), (unsigned *) (0xb8400000 + 3 * MSG_SIZE)};
unsigned tcounter[2] = {0, 0};
unsigned rcounter[2] = {0, 0};
unsigned tx_err[2] = {0, 0};
unsigned rx_err[2] = {0, 0};
unsigned tx_outputs[2] = {0, 0};

void fill_snd_buf (unsigned buf[], unsigned *start)
{
	int i;
	for (i = 0; i < MSG_SIZE / 4; ++i)
		buf[i] = *start + i;
	*start += i;	
}

void tx_task (void *unused)
{
	int i;
	
	for (;;) {
		for (i = 0; i < TX0_PER_CYCLE; ++i) {
			fill_snd_buf (snd_buf[0], &tcounter[0]);
			if (spw_output (&spw[0], snd_buf[0], MSG_SIZE, 0) != MSG_SIZE) {
				//debug_printf ("spw_output returned bad size!\n");
				tx_err[0]++;
			}
			tx_outputs[0]++;
		}
		for (i = 0; i < TX1_PER_CYCLE; ++i) {
			fill_snd_buf (snd_buf[1], &tcounter[1]);
			if (spw_output (&spw[1], snd_buf[1], MSG_SIZE, 0) != MSG_SIZE) {
				//debug_printf ("spw_output returned bad size!\n");
				tx_err[1]++;
			}
			tx_outputs[1]++;
		}
	}
}

void rx_task (void *arg)
{
	int chan = (int) arg;
	int j;
	int sz;
	
	for (;;) {
		sz = spw_input (&spw[chan], rcv_buf[chan], MSG_SIZE, 0);
		for (j = 0; j < (sz >> 2); ++j) {
			if (rcounter[chan] != rcv_buf[chan][j]) {
debug_printf ("channel %d: got %d, expected %d, j = %d\n", chan, rcv_buf[chan][j], rcounter[chan], j);
//dump (rcv_buf[chan], MSG_SIZE);
				rcounter[chan] = rcv_buf[chan][j];
				rx_err[chan]++;
			}
			rcounter[chan]++;
		}
	}
}

void console (void *unused)
{
	unsigned cur_time = 0, prev_time = 0;
	unsigned secs, mins, hours;
	unsigned cur_period;
	unsigned prev_tcounter[2] = {0, 0};
	unsigned prev_rcounter[2] = {0, 0};
	int i;
	
	for (;;) {
		timer_delay (&timer, 1000);
		cur_time = timer_milliseconds (&timer);
		secs = cur_time / 1000 % 60;
		mins = cur_time / 60000 % 60;
		hours = cur_time / 3600000 % 24;
		cur_period = cur_time - prev_time;
		debug_puts ("\33[H\33[2J");
		debug_printf ("======================================\n");		
		debug_printf ("= SPACEWIRE TEST. Duration: %02d:%02d:%02d =\n", hours, mins, secs);
		debug_printf ("======================================\n\n");
		for (i = 0; i < 2; ++i) {
			debug_printf ("SpaceWire %d:\n\n", channel_n[i]);
			debug_printf ("tx rate = %8u, tx errors = %2u, rx rate = %8u, rx errors = %2u\n\n",
				4 * 1000 * ((tcounter[i] - prev_tcounter[i]) / cur_period), tx_err[i],
				4 * 1000 * ((rcounter[i] - prev_rcounter[i]) / cur_period), rx_err[i]);
			debug_printf ("   Driver statistics:\n");
			debug_printf ("      rx_eop     = %9u, rx_eep   = %9u, rx_bytes = %9u\n", 
				spw[i].rx_eop, spw[i].rx_eep, spw[i].rx_bytes);
			debug_printf ("      tx_packets = %9u, tx_bytes = %9u, tx_waits = %9u\n", 
				spw[i].tx_packets, spw[i].tx_bytes, spw[i].txdma_waits);
			debug_printf ("--------------------------------------------------------------------------\n\n");
			
			prev_tcounter[i] = tcounter[i];
			prev_rcounter[i] = rcounter[i];
		}
		prev_time = timer_milliseconds (&timer);
		/*
		debug_printf ("STATUS chan 0: %08X, STATUS chan 1: %08X\n", 
			MC_SWIC_STATUS (0), MC_SWIC_STATUS (1));
		debug_printf ("QSTR2 = %08X, MASKR2 = %08X\n", MC_QSTR2, MC_MASKR2);
		debug_printf ("(0) CSR: TXDESC=%08X, TXDATA=%08X, RXDESC=%08X, RXDATA=%08X\n",
			MC_SWIC_TX_DESC_RUN(0), MC_SWIC_TX_DATA_RUN(0), MC_SWIC_RX_DESC_RUN(0), MC_SWIC_RX_DATA_RUN(0));
		debug_printf ("(1) CSR: TXDESC=%08X, TXDATA=%08X, RXDESC=%08X, RXDATA=%08X\n",
			MC_SWIC_TX_DESC_RUN(1), MC_SWIC_TX_DATA_RUN(1), MC_SWIC_RX_DESC_RUN(1), MC_SWIC_RX_DATA_RUN(1));
		*/
	}
}

void init_sdram (void)
{
#ifdef ELVEES_MC24R2
	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_T |			/* Sync memory */
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xF8000000);	/* Address mask */

	MC_SDRCON = MC_SDRCON_PS_512 |		/* Page size 512 */
		MC_SDRCON_CL_3 |		/* CAS latency 3 cycles */
		MC_SDRCON_RFR (64000000/8192, MPORT_KHZ); /* Refresh period */

	MC_SDRTMR = MC_SDRTMR_TWR(2) |		/* Write recovery delay */
		MC_SDRTMR_TRP(2) |		/* Минимальный период Precharge */
		MC_SDRTMR_TRCD(2) |		/* Между Active и Read/Write */
		MC_SDRTMR_TRAS(5) |		/* Между * Active и Precharge */
		MC_SDRTMR_TRFC(15);		/* Интервал между Refresh */

	MC_SDRCSR = 1;				/* Initialize SDRAM */
	udelay (2);
#endif
#ifdef ELVEES_MCT02
	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_WS(4) |		/* Wait states */
		MC_CSCON_CSBA (0x08000000) |	/* Base address */
		MC_CSCON_CSMASK (0xFC000000);	/* Address mask */
		
//debug_printf ("CSCON0 = %08X\n", MC_CSCON0);

	MC_CSCON1 = MC_CSCON_E | MC_CSCON_T |
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xFC000000);	/* Address mask */
	
//debug_printf ("CSCON1 = %08X\n", MC_CSCON1);

	MC_SDRCON = MC_SDRCON_PS_512 |		/* Page size 512 */
		MC_SDRCON_CL_3 |		/* CAS latency 3 cycles */
		//MC_SDRCON_RFR (64000000/8192, MPORT_KHZ); /* Refresh period */
		(0x030D << 16);
		
//debug_printf ("SDRCON = %08X\n", MC_SDRCON);

	MC_SDRTMR = MC_SDRTMR_TWR(2) |		/* Write recovery delay */
		MC_SDRTMR_TRP(2) |		/* Минимальный период Precharge */
		MC_SDRTMR_TRCD(2) |		/* Между Active и Read/Write */
		MC_SDRTMR_TRAS(5) |		/* Между * Active и Precharge */
		MC_SDRTMR_TRFC(15);		/* Интервал между Refresh */
		
//debug_printf ("SDRTMR = %08X\n", MC_SDRTMR);

	MC_SDRCSR = 1;				/* Initialize SDRAM */
	udelay (2);
#endif
}

void uos_init (void)
{
	debug_printf ("\n\nTesting SpaceWire!\n");
	
#ifdef EXTERNAL_SETUP
	init_sdram ();
#endif

#ifdef ELVEES_MCB03
    // nCS0 и nCS1 для MCB-03
    MC_CSCON0 = MC_CSCON_AE | MC_CSCON_E | MC_CSCON_WS(1) |
        MC_CSCON_CSBA(0x00000000) | MC_CSCON_CSMASK(0xFC000000);
    MC_CSCON1 = MC_CSCON_AE | MC_CSCON_E | MC_CSCON_WS(3) |
        MC_CSCON_CSBA(0x00000000) | MC_CSCON_CSMASK(0xFC000000);

    // Включаем обработчик прерываний от MCB
    mcb_create_interrupt_task (MCB_COMMON_IRQ, 100, 
        stack_mcb, sizeof (stack_mcb));
#endif

	
	spw_init (&spw[0], SPW0_CHANNEL, TX_SPEED);
	spw_init (&spw[1], SPW1_CHANNEL, TX_SPEED);
	timer_init (&timer, KHZ, 100);
	
	tx  = task_create (tx_task, 0, "tx_", 14, tx_stack, sizeof (tx_stack));
	rx0 = task_create (rx_task, (void *) 0, "rx0", 11, rx0_stack, sizeof (rx0_stack));
	rx1 = task_create (rx_task, (void *) 1, "rx1", 12, rx1_stack, sizeof (rx1_stack));
	task_create (console, 0, "console", 15, con_stack, sizeof (con_stack));
}
