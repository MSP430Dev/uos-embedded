TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin test_task.bin test_timer.bin test_uart.bin \
		  test_fpga.bin
OUTS		= $(TESTS:%.bin=%.out)

# Using SAM-BA utility.
PROG            = sam-ba /dev/ttyUSB? AT91SAM9260-SK load-flash.tcl

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.bin *.lst *.dis .deps $(MODULES)

probe:
		$(PROG)

load:		test_fpga.bin
		$(PROG) $<

include $(OS)/sources/rules.mak
