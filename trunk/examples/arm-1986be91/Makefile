TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-task
TESTS		= test-debug.srec test-task.srec test-lcd.srec #test-timer.srec test-uart.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis .deps $(TESTS) $(MODULES)

probe:
		$(PROG)

load:		$(TEST).srec
		$(PROG) $<

proxy:
		openocd -f openocd.tcl

gdb:		$(TEST).elf
		arm-elf-gdb -x gdbinit $(TEST).elf

insight:	$(TEST).elf
		arm-elf-insight -x gdbinit $(TEST).elf &

include $(OS)/sources/rules.mak
