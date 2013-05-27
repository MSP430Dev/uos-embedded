TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS          += -g #-DSIMULATOR

TEST		= test-timer
TESTS		= $(TEST).srec
TESTS		+= test-leds.srec
TESTS		+= test-debug.srec 
TESTS		+= test-task.srec
TESTS		+= test-timer.srec
TESTS		+= test-tswitch.srec
TESTS		+= test-latency.srec
TESTS		+= philosophers.srec

OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.bin *.lst *.dis *.map .deps $(MODULES)

hex:		$(TESTS)
		for srec in $(TESTS); do \
			$(OBJCOPY) -O ihex -I srec $$srec `basename $$srec .srec`.hex; \
		done

include $(OS)/sources/rules.mak

###
