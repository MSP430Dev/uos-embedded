TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_task.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak
