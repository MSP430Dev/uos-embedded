TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-udp
#TESTS		= test-debug.hex test-lcd.hex test-task-lcd.hex
#TESTS		+= test-timer.hex test-timer-lcd.hex test-uart.hex
#TESTS		+= test-latency.hex test-tswitch.hex philosophers.hex
#TESTS		+= test-can.hex tetris.hex
TESTS           += test-mac.hex test-eth.hex test-tcp.hex test-udp.hex test-telnetd.hex
OUTS		= $(TESTS:%.hex=%.elf) udpping

# Using MILPROG utility.
PROG            = milprog

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis .deps $(TESTS) $(MODULES) udpping

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
