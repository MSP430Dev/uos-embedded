TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= phil-uart
TESTS		+= test-led.bin test-debug.bin test-task.bin test-timer.bin
TESTS		+= test-latency.bin test-tswitch.bin phil-uart.bin #test-uart.bin
TESTS		+= test-mil_std_bc.bin test-mil_std_rt.bin test-arinc.bin test-arinc-poll.bin
TESTS		+= test-dma.bin
OUTS		= $(TESTS:%.bin=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis *.bin .deps $(TESTS) $(MODULES) udpping


include $(OS)/sources/rules.mak
