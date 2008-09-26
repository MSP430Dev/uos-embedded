OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/mc24
include $(TARGET)/target.cfg

TESTS		= test_debug.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		.deps target $(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

.deps:
		mkdir .deps

target:
		$(MAKE) -C $(TARGET)

clean:
		rm -rf *.out *.hex *.sre *.[osi] *.lst *.dis *~ .deps

include $(OS)/sources/rules.mak

###
