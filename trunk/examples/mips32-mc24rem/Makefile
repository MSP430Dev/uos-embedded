TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#TESTS		= test-ram.srec
#LDFLAGS		+= -T $(TARGET)/all-in-cram.ld

TESTS		= test-debug.srec test-task.srec test-timer.srec test-uart.srec \
		  test-ram.srec test-tcl.srec philosophers.srec test-lport.srec
LDFLAGS		+= -T $(TARGET)/code-in-flash.ld

OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS) test-ram.dis
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
