TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

all:		startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

olimex:		load-flash.tcl sam7-ex256-ge12.bin
		sam-ba /dev/ttyUSB? AT91SAM7X256-EK load-flash.tcl sam7-ex256-ge12.bin

include $(OS)/sources/rules.mak
