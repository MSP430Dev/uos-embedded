OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/linux386
include $(TARGET)/target.cfg

CFLAGS		+= -DKHZ=10000
TESTS_ALL	= test_debug test_task test_timer test_uart test_mem \
		  test_group test_tap test_arp test_ip test_udp test_snmp \
		  test_pipe test_tcp_client test_tcp_server test_telnet
TESTS	 	= test_telnet
PROGS		= #tcp-client tcp-server

all:		.deps target $(TESTS) $(PROGS)

.deps:
		mkdir .deps

target:
		$(MAKE) -C $(TARGET)

test_debug:	test_debug.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_task:	test_task.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_timer:	test_timer.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_uart:	test_uart.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_mem:	test_mem.o mem.o
		$(CC) $(LDFLAGS) $(CFLAGS) test_mem.o mem.o $(LIBS) -o $@

test_pipe:	test_pipe.o
		$(CC) $(LDFLAGS) $(CFLAGS) test_pipe.o $(LIBS) -o $@

test_tap:	test_tap.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_arp:	test_arp.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_group:	test_group.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_ip:	test_ip.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_snmp:	test_snmp.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_udp:	test_udp.o
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_tcp_client: test_tcp_client.o ../../targets/linux386/libuos.a
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_tcp_server: test_tcp_server.o ../../targets/linux386/libuos.a
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

test_telnet:	test_telnet.o ../../targets/linux386/libuos.a
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

tcp-client:	tcp-client.c
		cc -O -Wall $< -o $@

tcp-server:	tcp-server.c
		cc -O -Wall $< -o $@

clean:
		rm -rf *.[osi] *.lst *.dis *~ .deps $(TESTS_ALL) $(PROGS)

mem.o:		../../sources/mem/mem.c
		$(CC) $(CFLAGS) -DMEM_DEBUG -c $< -o $@

include $(OS)/sources/rules.mak
###
