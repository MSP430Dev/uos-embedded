TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin test_task.bin test_timer.bin test_uart.bin \
		  test_ram.bin test_tcl.bin philosophers.bin test_lport.bin
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.bin *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
