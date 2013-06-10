TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb

TEST		= tetris
TESTS		= $(TEST).hex
TESTS		+= test-led.hex lol.hex test-lcd.hex test-task.hex
TESTS		+= test-timer.hex philosophers.hex
OUTS		= $(TESTS:%.hex=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.srec *.bin *.lst *.dis .deps $(MODULES)

hex:		$(TESTS)
		for srec in $(TESTS); do \
			$(OBJCOPY) -O ihex -I srec $$srec `basename $$srec .srec`.hex; \
		done

load:		$(TEST).hex
		pic32prog -p $(TEST).hex

debug:          $(TEST).hex
		$(GDB) $(TEST).elf

include $(OS)/sources/rules.mak

###
