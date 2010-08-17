TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec #test_timer.srec test_uart.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS) test_debug.dis
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis .deps $(TESTS) $(MODULES)

probe:
		$(PROG)

load:		test_debug.srec
		$(PROG) $<

include $(OS)/sources/rules.mak
