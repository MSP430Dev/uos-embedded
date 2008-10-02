OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/mc24
include $(TARGET)/target.cfg

TESTS		= test_debug.sre test_task.sre test_timer.sre test_uart.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		.deps startup.o libuos.a $(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

.deps:
		mkdir .deps

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.sre *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
