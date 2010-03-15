TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#CURRENT		= test-tswitch
#CURRENT		= test-latency
#CURRENT		= test-fpu
CURRENT		= test-timer
#TESTS		+= test-debug.srec test-task.srec test-timer.srec
#TESTS		+= test-uart.srec test-ram.srec test-tcl.srec
#TESTS		+= philosophers.srec test-latency.srec
TESTS		+= $(CURRENT).srec
OUTS		= $(TESTS:%.srec=%.out)
#LDFLAGS		+= -T $(TARGET)/code-in-flash.ld

# Enable instruction cache
LDFLAGS		+= -T $(TARGET)/code-in-kseg0.ld

all:		$(OUTS) $(TESTS) $(CURRENT).dis
		$(SIZE) *.out

$(OUTS) $(TESTS) $(CURRENT).out: startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

load:		$(CURRENT).srec
		mcprog $(CURRENT).srec

include $(OS)/sources/rules.mak

###
