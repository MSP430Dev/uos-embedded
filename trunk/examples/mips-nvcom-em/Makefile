TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-slip
TESTS		= $(TEST).srec
#TESTS		+= test-debug.srec test-task.srec test-timer.srec test-uart.srec
#TESTS		+= philosophers.srec test-ram.srec #test-tcl.srec
#TESTS           += test-latency.srec test-tswitch.srec
#TESTS           += test-fpu.srec
TESTS           += test-slip.srec test-snmp.srec
#TESTS           += test-mac.srec test-eth.srec test-tcp.srec test-udp.srec test-telnetd.srec
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES)

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
