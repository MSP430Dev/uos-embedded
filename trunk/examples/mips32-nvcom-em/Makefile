TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-tcl
TESTS		= $(TEST).srec
TESTS		+= test-debug.srec test-task.srec test-timer.srec test-uart.srec
TESTS		+= philosophers.srec test-ram.srec test-tcl.srec
TESTS           += test-latency.srec test-tswitch.srec test-fpu.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.out

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

load:		$(TEST).srec
		mcprog $(TEST).srec

proxy:
		mcremote &

gdb:		$(TEST).out
		mipsel-elf32-gdb -x gdbinit $(TEST).out

insight:	$(TEST).out
		mipsel-elf32-insight -x gdbinit $(TEST).out &

include $(OS)/sources/rules.mak

###
