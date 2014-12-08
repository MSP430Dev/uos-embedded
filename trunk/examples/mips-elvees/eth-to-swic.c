#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/arp.h>
#include <timer/timer.h>
#include <elvees/eth-mcb.h>
#include <elvees/spw-mcb.h>

#define MCB_COMMON_IRQ          34  /* Прерывание от MCB */

#define SPW0_CHANNEL    0
#define SPW1_CHANNEL    3
#define SWIC_TX_SPEED   100

ARRAY (stack_tcp, 1500);
ARRAY (stack_mcb, 1500);
ARRAY (rx_stack, 1000);
ARRAY (con_stack, 1000);

ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
mem_pool_t pool;
arp_t *arp;
eth_mcb_t eth_data, *eth = &eth_data;
route_t route;
timer_t timer;
ip_t ip;
tcp_socket_t *user_socket;

#define PORT            14348
#define MSG_SIZE        1500
uint8_t tcp_msg[MSG_SIZE];
uint8_t spw_msg[MSG_SIZE];

uint8_t delimiter[] = {0x01, 0xFE, 0x02, 0xFD, 0x04, 0xFB, 0x08, 0xF7};

spw_t spw[2];
int rcv_buf[MSG_SIZE / 4] __attribute__ ((aligned (8)));

unsigned tcp_count = 0, tcp_size = 0;
unsigned spw_count = 0, spw_size = 0;
unsigned bad_len_cnt = 0;

uint8_t *find_delimiter (uint8_t *raw_msg, int size)
{
    register uint8_t tmp, *s;

    s = raw_msg;

    for (;;) {
        do {
            tmp = *s++;
            if (tmp == delimiter[0]) break;
            if (s - raw_msg >= size) return (uint8_t *) 0;
        } while (tmp != delimiter[0]);

        if (memcmp (s, delimiter + 1, sizeof(delimiter) - 1) == 0)
            return --s;
    }
}

void rx_task (void *arg)
{
    int chan = (int) arg;
    int sz;

    for (;;) {
        sz = spw_input (&spw[chan], rcv_buf, MSG_SIZE, 0);
        if (sz != 21 && sz != 1041)
            bad_len_cnt++;
            //debug_printf ("Strange packet with len: %d bytes\n", sz);
        spw_count++;
        spw_size += sz;
        /*debug_printf ("Got SPW packet #%4d, size = %d, tcp_count = %d\n",
            spw_count, sz, tcp_count);*/
    }
}

void tcp_task (void *data)
{
    tcp_socket_t *lsock;
    int sz;
    int sz_to_end;
    unsigned cycle = 0;
    uint8_t *delim;
    uint8_t *tcp_pointer;
    uint8_t *spw_pointer;

    lsock = tcp_listen (&ip, 0, PORT);
    if (! lsock) {
        printf (&debug, "Error on listen, aborted\n");
        uos_halt (0);
    }
    
    for (;;) {
        printf (&debug, "Server waiting on port %d...\n", PORT);
        printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));
        user_socket = tcp_accept (lsock);
        if (! user_socket) {
            printf (&debug, "Error on accept\n");
            uos_halt (0);
        }
        debug_printf ("Client connected\n");
        
        spw_pointer = spw_msg;

        for (;;) {
            sz = tcp_read (user_socket, tcp_msg, sizeof(tcp_msg));

            if (sz < 0) break;

            tcp_count++;
            tcp_size += sz;
            
            if (cycle % 5 == 0)
                tcp_ack_now (user_socket);
            cycle++;
            
            tcp_pointer = tcp_msg;
            sz_to_end = sz;
            
            for (;;) {
                delim = find_delimiter(tcp_pointer, sz_to_end);
                if (!delim) {
                    memcpy (spw_pointer, tcp_pointer, sz_to_end);
                    spw_pointer += sz_to_end;
                    break;
                } else {
                    sz = delim - tcp_pointer;
                    if (sz > 0) {
                        memcpy (spw_pointer, tcp_pointer, sz);
                        spw_pointer += sz;
                    }
                    
                    int advance = delim - tcp_pointer + sizeof(delimiter);
                    tcp_pointer += advance;
                    sz_to_end -= advance;
                    
                    sz = spw_pointer - spw_msg;
                    if (sz > 0) {
                        if (spw_output (&spw[0], spw_msg, sz, 0) != sz) {
                            debug_printf ("spw_output returned bad size!\n");
                        }
                        spw_pointer = spw_msg;
                    }
                    
                    if (sz_to_end <= 0)
                        break;
                }
            }
            /*
            if (spw_output (&spw[0], tcp_msg, sz, 0) != sz) {
                debug_printf ("spw_output returned bad size!\n");
            }
            */
        }

        tcp_close (user_socket);
        mem_free (user_socket);
        user_socket = 0;
    }
}

void console (void *unused)
{
    for (;;) {
        timer_delay (&timer, 2000);
        //debug_puts ("\33[H\33[2J");
        debug_printf ("Received from TCP  %d packets, overall size %d bytes\n",
            tcp_count, tcp_size);
        debug_printf ("Received from SWIC %d packets, overall size %d bytes\n",
            spw_count, spw_size);
        debug_printf ("Number of packets with bad length: %d\n", bad_len_cnt);
    }
}

void uos_init (void)
{
    /* Configure 16 Mbyte of external Flash memory at nCS3. */
    MC_CSCON3 = MC_CSCON_WS (4);        /* Wait states  */

    /* Выделяем место для динамической памяти */
    extern unsigned __bss_end[];

    /* Динамическая память в CRAM */
    extern unsigned _estack[];
    mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 1024);

    timer_init (&timer, KHZ, 50);

    /*
     * Create a group of two locks: timer and eth.
     */
    mutex_group_t *g = mutex_group_init (group, sizeof(group));
    mutex_group_add (g, &eth->netif.lock);
    mutex_group_add (g, &timer.decisec);

    arp = arp_init (arp_data, sizeof(arp_data), &ip);
    ip_init (&ip, &pool, 70, &timer, arp, g);

    /*
     * Create interface eth0
     */
    const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };

    // nCS0 и nCS1 для MCB-03
    MC_CSCON0 = MC_CSCON_AE | MC_CSCON_E | MC_CSCON_WS(1) |
        MC_CSCON_CSBA(0x00000000) | MC_CSCON_CSMASK(0xFC000000);
    MC_CSCON1 = MC_CSCON_AE | MC_CSCON_E | MC_CSCON_WS(3) |
        MC_CSCON_CSBA(0x00000000) | MC_CSCON_CSMASK(0xFC000000);

    // Включаем обработчик прерываний от MCB
    mcb_create_interrupt_task (MCB_COMMON_IRQ, 100,
        stack_mcb, sizeof (stack_mcb));

    eth_mcb_init (eth, "eth0", 80, &pool, arp, my_macaddr);

    unsigned char my_ip[] = { 11, 11, 11, 10 };
    route_add_netif (&ip, &route, my_ip, 24, &eth->netif);

    task_create (tcp_task, 0, "tcp", 20,
        stack_tcp, sizeof (stack_tcp));

    /* Инициализация SWIC */
    spw_init (&spw[0], SPW0_CHANNEL, SWIC_TX_SPEED);
    spw_init (&spw[1], SPW1_CHANNEL, SWIC_TX_SPEED);

    task_create (rx_task, (void *) 1, "rx", 11, rx_stack, sizeof (rx_stack));
    task_create (console, 0, "console", 1, con_stack, sizeof (con_stack));
}
