TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
TESTS		= test_debug.hex

#TESTS		= test_task.hex test_timer.hex test_uart.hex philosophers.hex

# Using msp430-jtag programmer with JTAG LPT adapter.
PROG            = env LIBMSPGCC_PATH=/usr/local/lib msp430-jtag

all:		$(TESTS)
		for f in *.out; do $(SIZE) $$f; done

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.srec *.lst *.dis .deps $(MODULES) $(TESTS)

probe:
		$(PROG) -D

load:		test_debug.hex
		$(PROG) -e $<

# Original software from http://www.olimex.com/dev/soft/msp430/easyweb_test.tar.gz
olimex:		easyweb.hex
		$(PROG) -e $<

include $(OS)/sources/rules.mak

###
