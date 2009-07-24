TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg


TESTS		= test_task.bin test_timer.bin test_uart.bin philosophers.bin

# Compile without kernel module in target.cfg.
TESTS		+= test_debug.bin

all:		$(TESTS)
		$(SIZE) --target=binary *.bin

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.bin *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
