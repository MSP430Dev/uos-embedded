TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test_timer
TESTS		= test_led.bin test_debug.bin test_task.bin test_timer.bin \
		  test_delay.bin #test_uart_raw.bin
OUTS		= $(TESTS:%.bin=%.elf)
DIS		= $(TESTS:%.bin=%.dis)

all:		$(OUTS) $(TESTS) $(DIS)
		$(SIZE) $(OUTS)
		cp $(TEST).bin MLO

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.bin *.lst *.dis *.map .deps png2rgb $(MODULES) MLO

include $(OS)/sources/rules.mak
