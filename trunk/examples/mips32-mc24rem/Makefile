TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec test_timer.srec test_uart.srec \
		  test_ram.srec test_tcl.srec philosophers.srec test_lport.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
