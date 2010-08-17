TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CURRENT		= test-uart
TESTS		+= test-debug.srec test-task.srec test-timer.srec
TESTS		+= test-uart.srec test-ram.srec test-tcl.srec
TESTS		+= philosophers.srec test-latency.srec test-tswitch.srec
TESTS		+= test-fpu.srec
TESTS		+= $(CURRENT).srec
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(CURRENT).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS) $(CURRENT).elf: startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES)

load:		$(CURRENT).srec
		mcprog $(CURRENT).srec

gdb:		$(CURRENT).elf
		arm-elf-gdb -x gdbinit $(CURRENT).elf

include $(OS)/sources/rules.mak

###
