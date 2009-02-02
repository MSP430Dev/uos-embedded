TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_uart.srec test_timer.srec #test_nvram.srec
OUTS		= $(TESTS:%.srec=%.out)

# Using avrdude programmer with STK200 LPT adapter.
PROG		= avrdude -cstk200 -patmega128 -Enoreset
FUSES		= -Ulfuse:w:0xFE:m -Uhfuse:w:0x1F:m -Uefuse:w:0xFF:m

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.out *.hex *.srec *.[oasi] *.lst *.dis *~ .deps $(MODULES)

probe:
		$(PROG) -v

load:		test_debug.srec
		$(PROG) -e -U$< $(FUSES)

test:		mt-128.olimex
		$(PROG) -e -U$< $(FUSES)

# Using avrdude programmer with JTAGICE USB adapter.
probe2:
		avrdude -cjtag1 -P /dev/ttyUSB? -patmega128 -v

load2:		test_debug.srec
		avrdude -cjtag1 -P /dev/ttyUSB? -patmega128 -e -U$< $(FUSES)

include $(OS)/sources/rules.mak

###
