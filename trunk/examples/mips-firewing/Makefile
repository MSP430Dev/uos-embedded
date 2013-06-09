TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb

TEST		= test-debug
TESTS		= $(TEST).srec
TESTS		+= test-led.srec lol.srec
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
		pic32prog -p $(TEST).srec

debug:          $(TEST).srec
		$(GDB) $(TEST).elf

include $(OS)/sources/rules.mak

###
