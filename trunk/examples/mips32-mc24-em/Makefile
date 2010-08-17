TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin test_task.bin test_timer.bin test_uart.bin \
		  test_ram.bin test_tcl.bin philosophers.bin test_lport.bin
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS)
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.bin *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
