TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-debug
TESTS		= $(TEST).srec
TESTS		+= test-debug.srec test-leds.srec #test-task.srec #test-timer.srec test-uart.srec
#TESTS		+= philosophers.srec test-ram.srec #test-tcl.srec
#TESTS           += test-latency.srec test-tswitch.srec
#TESTS           += test-fpu.srec
#TESTS           += test-mac.srec test-eth.srec test-tcp.srec test-udp.srec test-telnetd.srec
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES)

hex:		$(TESTS)
		for srec in $(TESTS); do \
			$(OBJCOPY) -O ihex -I srec $$srec `basename $$srec .srec`.hex; \
		done

load:		$(TEST).hex
		ubw32 -write $(TEST).hex -reset

include $(OS)/sources/rules.mak

###
