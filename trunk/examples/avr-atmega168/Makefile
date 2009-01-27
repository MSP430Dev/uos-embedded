TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS		+= -DKHZ=16000
TESTS		= test_debug.sre test_uart.sre test_timer.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.[oasi] *~ $(MODULES) *.out *.hex *.sre *.lst *.dis .deps

include $(OS)/sources/rules.mak
