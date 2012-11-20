TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb

CFLAGS          += -g

TEST		= test-lcd2
TESTS		= $(TEST).srec
TESTS		+= test-leds.srec test-lcd.srec test-timer.srec test-task.srec
TESTS		+= test-lcd2.srec philosophers.srec tetris.srec
OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.bin *.lst *.dis .deps $(MODULES)

hex:		$(TESTS)
		for srec in $(TESTS); do \
			$(OBJCOPY) -O ihex -I srec $$srec `basename $$srec .srec`.hex; \
		done

load:		$(TEST).srec
		pic32prog -S $(TEST).srec

debug:          $(TEST).srec
		$(GDB) $(TEST).elf

include $(OS)/sources/rules.mak

###
