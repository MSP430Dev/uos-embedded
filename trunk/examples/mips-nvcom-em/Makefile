TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-latency
TESTS		+= test-debug.bin test-task.bin test-timer.bin test-uart.bin
TESTS		+= philosophers.bin test-ram.bin
TESTS           += test-latency.bin test-tswitch.bin test-fpu.bin
#TESTS           +=  test-snmp.bin test-telnetd.bin test-slip.bin
TESTS           += test-mac.bin test-eth.bin test-tcp.bin test-udp.bin
TESTS		+= test-tcp-serv.bin test-tcp-client.bin
TESTS		+= test-udp-serv.bin test-udp-client.bin
TESTS		+= test-dcache.bin test-tcl.bin
TESTS		+= dhrystone.bin linpack.bin whetstone.bin #livermore.bin
#TESTS		+= coremark.bin
OUTS		= $(TESTS:%.bin=%.elf)
SREC		= $(TESTS:%.bin=%.srec)
#DIS		= $(TESTS:%.bin=%.dis)

all:		$(OUTS) $(TESTS) $(SREC) $(DIS)
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.bin *.srec *.lst *.dis *.map .deps $(MODULES)

load:		$(TEST).srec
		mcprog $(TEST).srec

proxy:
		mcremote &

gdb:		$(TEST).elf
		mipsel-elf32-gdb -x gdbinit $(TEST).elf

insight:	$(TEST).elf
		mipsel-elf32-insight -x gdbinit $(TEST).elf &

include $(OS)/sources/rules.mak

###
