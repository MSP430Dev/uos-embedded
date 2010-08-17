TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
TESTS		= test_debug.hex test_task.hex test_timer.hex test_lcd.hex \
		  test_uart.hex test_olimex.hex philosophers.hex test_eth.hex
# TODO: test_nvram.hex

OUTS            = $(TESTS:%.srec=%.elf)

# Using msp430-jtag programmer with JTAG LPT adapter.
#PROG            = env LIBMSPGCC_PATH=/usr/local/lib msp430-jtag

# Using msp430-jtag programmer with JTAG USB adapter, Bi-Wire protocol.
PROG            = env LIBMSPGCC_PATH=/usr/local/lib msp430-jtag --spy-bi-wire --lpt=/dev/ttyUSB0

all:		$(OUTS) $(TESTS)
		for f in *.elf; do $(SIZE) *.elf; break; done

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.srec *.lst *.dis .deps $(MODULES) $(TESTS)

probe:
		$(PROG) -D

load:		philosophers.hex
		$(PROG) -e $<

# Original software from http://www.olimex.com/dev/soft/msp430/easyweb_test.tar.gz
olimex:		easyweb.hex
		$(PROG) -e $<

include $(OS)/sources/rules.mak

###
