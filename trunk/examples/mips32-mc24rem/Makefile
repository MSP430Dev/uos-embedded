TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#TESTS		= test_ram.srec
#LDFLAGS		+= -T $(TARGET)/all-in-cram.ld

TESTS		= test_debug.srec test_task.srec test_timer.srec test_uart.srec \
		  test_ram.srec test_tcl.srec philosophers.srec test_lport.srec
LDFLAGS		+= -T $(TARGET)/code-in-flash.ld

OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS) test_ram.dis
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
