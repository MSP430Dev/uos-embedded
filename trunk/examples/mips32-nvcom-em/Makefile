TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test_debug
TESTS		= $(TEST).srec
TESTS		+= test_debug.srec test_task.srec test_timer.srec test_uart.srec
#TESTS		+= test_ram.srec test_tcl.srec philosophers.srec test_lport.srec
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
