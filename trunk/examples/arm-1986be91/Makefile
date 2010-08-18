TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test-debug.srec test-task.srec test-lcd.srec #test-timer.srec test-uart.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS) test-debug.dis
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis .deps $(TESTS) $(MODULES)

probe:
		$(PROG)

load:		test-debug.srec
		$(PROG) $<

include $(OS)/sources/rules.mak
