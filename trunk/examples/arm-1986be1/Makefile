TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-tswitch
TESTS		+= test-led.hex test-debug.hex test-task.hex test-timer.hex
TESTS		+= test-latency.hex test-tswitch.hex phil-uart.hex #test-uart.hex
TESTS		+= test-mil_std_bc.hex test-mil_std_rt.hex test_arinc.hex
OUTS		= $(TESTS:%.hex=%.elf)

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS) $(TEST).dis $(TEST).bin
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis *.bin .deps $(TESTS) $(MODULES) udpping

probe:
		$(PROG)

load:		$(TEST).hex
		$(PROG) $<

proxy:
		openocd -f openocd.tcl

gdb:		$(TEST).elf
		arm-elf-gdb -x gdbinit $(TEST).elf

insight:	$(TEST).elf
		arm-elf-insight -x gdbinit $(TEST).elf &

udpping:	udpping.c
		gcc -Wall -O $< -lpthread -o $@

include $(OS)/sources/rules.mak
