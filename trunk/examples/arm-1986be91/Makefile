TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec
OUTS		= $(TESTS:%.srec=%.out)

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.srec *.lst *.dis .deps $(MODULES)

probe:
		$(PROG)

load:		test_debug.bin
		$(PROG) $<

include $(OS)/sources/rules.mak
