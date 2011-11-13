TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CURRENT		= test-timer
TESTS		+= test-debug.srec test-task.srec test-timer.srec
TESTS		+= test-uart.srec #test-tcl.srec test-ram.srec
TESTS		+= philosophers.srec test-latency.srec test-tswitch.srec
TESTS		+= test-fpu.srec
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(CURRENT).srec $(CURRENT).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

$(CURRENT).elf: $(CURRENT).o
		$(CC) $(LDFLAGS) -Wl,-Map=$(CURRENT).map $< $(LIBS) -o $@

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis *.map .deps $(MODULES)

load:		$(CURRENT).srec
		mcprog $(CURRENT).srec

gdb:		$(CURRENT).elf
		arm-elf-gdb -x gdbinit $(CURRENT).elf

include $(OS)/sources/rules.mak

###
