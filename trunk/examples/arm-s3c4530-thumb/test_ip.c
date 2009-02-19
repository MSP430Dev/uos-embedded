/*
 * Testing IP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/ip.h>
#include <timer/timer.h>
#include <s3c4530/eth.h>

/*
 * Установлена микросхема 1M x 16 - имеем 2 мегабайта памяти.
 */
#define RAM_START      0x02000000		/* SDRAM start address */
#define RAM_SIZE       (2*1024*1024)		/* SDRAM size (bytes) */
#define RAM_END        (RAM_START+RAM_SIZE)	/* SDRAM end address */
#define REFRESH_USEC	8			/* refresh period (usec) */
#define IO_START	0x03600000		/* address of i/o space */

mem_pool_t pool;
timer_t timer;
ip_t ip;
ARRAY (group, sizeof(lock_group_t) + 4 * sizeof(lock_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
arp_t *arp;
eth_t *eth;
route_t route;

void configure_ram (unsigned long ram_start, unsigned long ram_end,
	int refresh_usec, unsigned long io_start)
{
	arm_memory_regs_t reg;

	arm_get_memory_regs (&reg);

	/* DRAM bank 0: 16-bit bus width. */
	reg.extdbwth |= ARM_EXTDBWTH_16BIT << ARM_EXTDBWTH_DSD0_shift;

	/* DRAM bank 0 address, size and timings. */
	reg.dramcon0 = ARM_DRAMCON_BASE (ram_start) |
		ARM_DRAMCON_NEXT (ram_end) |
		ARM_DRAMCON_CAN_8B |	/* column address = 8-bit */
		ARM_DRAMCON_TCS_1C |	/* CAS strobe time = 1 cycle */
					/* CAS pre-charge time = 1 cycle */
		ARM_DRAMCON_TRC_2C |	/* RAS to CAS delay = 2 cycles */
		ARM_DRAMCON_TRP_4C;	/* RAS pre-charge time = 4 cycles */

	/* Setup DRAM refresh cycle = 8 usec. */
	reg.refextcon = ARM_REFEXTCON_BASE (io_start) |
		ARM_REFEXTCON_VSF |	/* validity of special regs */
		ARM_REFEXTCON_REN |	/* refresh enable */
		ARM_REFEXTCON_TCHR_4C | /* CAS hold time = 4 cycles */
					/* CAS setup time = 1 cycle */
		ARM_REFEXTCON_RFR (refresh_usec, KHZ);

	/* Disable write buffer and cache. */
	ARM_SYSCFG &= ~(ARM_SYSCFG_WE | ARM_SYSCFG_CE);

	/* Sync DRAM mode. */
	ARM_SYSCFG |= ARM_SYSCFG_SDM;

	/* Set memory configuration registers, all at once. */
	arm_set_memory_regs (&reg);

	/* Enable write buffer and cache. */
	ARM_SYSCFG |= ARM_SYSCFG_WE | ARM_SYSCFG_CE;
}

void uos_init (void)
{
	lock_group_t *g;

	/* Baud 9600. */
	ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
	ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	configure_ram (RAM_START, RAM_END, REFRESH_USEC, IO_START);
	mem_init (&pool, RAM_START, RAM_END);
	eth = mem_alloc (&pool, sizeof (eth_t));
	if (! eth) {
		debug_printf ("No memory for eth_t\n");
		uos_halt (1);
	}

	arp = arp_init (arp_data, sizeof(arp_data), &ip);

	timer_init (&timer, 100, KHZ, 10);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &eth->netif.lock);
	lock_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, arp, g);

	/*
	 * Create interface eth0 144.206.181.187 / 255.255.255.0
	 */
	eth_init (eth, "eth0", 80, 70, &pool, arp);
	route_add_netif (&ip, &route, (unsigned char*) "\220\316\265\273",
		24, &eth->netif);

	debug_puts ("\nTesting IP.\n");
}
