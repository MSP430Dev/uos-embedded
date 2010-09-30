TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-can
#TESTS		= test-debug.srec test-lcd.srec test-task-lcd.srec
#TESTS		+= test-timer.srec test-timer-lcd.srec test-uart.srec
#TESTS		+= test-latency.srec test-tswitch.srec philosophers.srec
TESTS		+= test-can.srec #tetris.srec
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
