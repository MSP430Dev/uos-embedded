TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test_tcl
TESTS		= $(TEST).srec
TESTS		+= test_debug.srec test_task.srec test_timer.srec test_uart.srec
TESTS		+= philosophers.srec test_ram.srec test_tcl.srec
OUTS		= $(TESTS:%.srec=%.out)

# No caches
#LDFLAGS        += -T $(TARGET)/code-in-flash.ld

# Enable instruction cache
LDFLAGS         += -T $(TARGET)/code-in-kseg0.ld

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
