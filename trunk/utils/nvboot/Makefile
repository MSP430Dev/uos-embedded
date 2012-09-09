TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

PROGRAM		= nvboot
APPLICATION	= app

all:		$(PROGRAM).elf $(PROGRAM).dis $(PROGRAM).bin
		$(SIZE) *.elf

$(PROGRAM).elf: startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES) $(PROGRAM).dis $(PROGRAM).bin img.bin

load:		$(PROGRAM).srec
		dd if=$(PROGRAM).bin > img.bin
		stat --format="%s" app.bin | dd >> img.bin
		dd if=$(APPLICATION).bin >> img.bin
		mcprog img.bin 0x1fc00000

include $(OS)/sources/rules.mak

###
