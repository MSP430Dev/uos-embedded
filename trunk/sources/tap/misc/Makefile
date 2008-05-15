KCFLAGS		= -O -Wall -I/usr/src/linux/include -D__KERNEL__ -DMODULE -DEXPORT_SYMTAB -D_LOOSE_KERNEL_NAMES

all:		netlink_dev.o ethertap.o test-tap0

netlink_dev.o:	netlink_dev.c
		gcc $(KCFLAGS) -c $<

ethertap.o:	ethertap.c
		gcc $(KCFLAGS) -c $<

test-tap0:	test-tap0.o
		gcc $(LDFLAGS) $< -o $@

clean:
		rm -f *.o *~ test-tap0

load:
		insmod netlink_dev.o
		insmod ethertap.o
		ifconfig tap0 200.0.0.1

unload:
		ifconfig tap0 down
		rmmod ethertap
		rmmod netlink_dev
