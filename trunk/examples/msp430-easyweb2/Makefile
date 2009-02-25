TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
TESTS		= test_debug.hex test_task.hex test_timer.hex
OUTS            = $(TESTS:%.srec=%.out)

#test_uart.hex test_lcd.hex test_olimex.hex test_nvram.hex philosophers.hex test_cs8900.hex

# Using msp430-jtag programmer with JTAG LPT adapter.
PROG            = env LIBMSPGCC_PATH=/usr/local/lib msp430-jtag

all:		$(OUTS) $(TESTS)
		for f in *.out; do $(SIZE) *.out; break; done

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.srec *.lst *.dis .deps $(MODULES) $(TESTS)

probe:
		$(PROG) -D

load:		test_timer.hex
		$(PROG) -e $<

# Original software from http://www.olimex.com/dev/soft/msp430/easyweb_test.tar.gz
olimex:		easyweb.hex
		$(PROG) -e $<

include $(OS)/sources/rules.mak

###
