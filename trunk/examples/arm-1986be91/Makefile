TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test-dma
TESTS		= test-debug.hex test-lcd.hex test-task-lcd.hex tetris.hex
TESTS		+= test-timer.hex test-timer-lcd.hex test-latency.hex test-tswitch.hex
TESTS		+= phil-lcd.hex test-user-timer.hex phil-uart.hex
TESTS		+= spi-master.hex spi-slave.hex test-can.hex
TESTS		+= test-mac.hex test-eth.hex test-tcp.hex test-udp.hex test-telnetd.hex
TESTS		+= test-led.hex test-iwdt.hex test-dma.hex test-irq.hex
OUTS		= $(TESTS:%.hex=%.elf) #udpping

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
