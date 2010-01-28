/*
 * Testing TCP protocol: server side.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <timer/timer.h>
#include <tap/tap.h>

#define MEM_SIZE	2400

ARRAY (task, 6000);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
char memory [MEM_SIZE];
mem_pool_t pool;
tap_t tap;
route_t route;
timer_t timer;
ip_t ip;

void main_task (void *data)
{
	tcp_socket_t *listen_socket, *sock;
	int n, serv_port = 2222;
	unsigned char ch, buf [512];

/*	printf (&debug, "Server started on port %d\n", serv_port);*/
	listen_socket = tcp_listen (&ip, 0, serv_port);
	if (! listen_socket) {
		printf (&debug, "Error on listen, aborted\n");
		abort();
	}
	/* Добавляем заголовок - длину пакета (LSB). */
	memset (buf, 0xff, sizeof (buf));
	*(short*) buf = sizeof (buf);
	for (;;) {
		/* Ждём, когда юзер подключится к нам. */
		printf (&debug, "Server: waiting on port %d\n", serv_port);
		printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));
		sock = tcp_accept (listen_socket);
		if (! sock) {
			printf (&debug, "Error on accept\n");
			break;
		}

		/* Пересылаем данные, пока юзер не отключится. */
		for (;;) {
			/* Десять пакетов в секуду. */
/*			mutex_wait (&timer.decisec);*/
/*			printf (&debug, "<%d> ", mem_available (&pool));*/

			for (n=0; n<2; ++n) {
				if (tcp_write (sock, buf, sizeof (buf)) < 0) {
					/*printf (&debug, "tcp_write failed\n");*/
					goto closed;
				}
			}

			/* Обрабатываем команды от пользователя. */
			for (;;) {
				n = tcp_read_poll (sock, &ch, 1, 1);
				if (n < 0)
					goto closed;
				if (n == 0)
					break;
				/*process_command (ch);*/
			}
		}
closed:		tcp_close (sock);
		mem_free (sock);
	}
	tcp_close (listen_socket);
	printf (&debug, "Server finished\n");
	uos_halt (0);
}

void uos_init (void)
{
	mutex_group_t *g;
	unsigned char my_ip[] = "\310\0\0\2";

	timer_init (&timer, KHZ, 10);
	mem_init (&pool, (size_t) memory, (size_t) memory + MEM_SIZE);

	/*
	 * Create a group of two locks: timer and tap.
	 */
	g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &tap.netif.lock);
	mutex_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, 0, g);

	/*
	 * Create interface tap0 200.0.0.2 / 0.0.0.0
	 */
	tap_init (&tap, "tap0", 80, &pool, 0);
	if (system ("sudo ifconfig tap0 10.0.0.2 dstaddr 200.0.0.2") == -1)
		/*ignore*/;
	route_add_netif (&ip, &route, my_ip, 0, &tap.netif);

	task_create (main_task, 0, "main", 1, task, sizeof (task));
}
