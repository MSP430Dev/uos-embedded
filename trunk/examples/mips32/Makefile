TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
#TESTS		= test_debug.sre

TESTS		= test_task.sre test_timer.sre test_uart.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.sre *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
