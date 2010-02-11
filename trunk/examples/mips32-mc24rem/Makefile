TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test-debug.srec test-task.srec test-timer.srec test-uart.srec \
		  test-ram.srec test-tcl.srec philosophers.srec
LDFLAGS		+= -T $(TARGET)/code-in-flash.ld

OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS) test-ram.dis
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

dis:		$(TESTS:%.srec=%.dis)

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

#load:		test-ram.mdb test-ram.srec
#		sudo mdb -f test-ram.mdb

load:		test-tcl.srec
		sudo mcprog test-tcl.srec

include $(OS)/sources/rules.mak

###
