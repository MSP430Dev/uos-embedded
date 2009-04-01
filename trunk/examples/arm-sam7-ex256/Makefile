TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin test_task.bin test_timer.bin \
		  test_uart.bin test_t-uart.bin test_lcd.bin #\
#		  test_mmc.bin test_usb.bin test_can.bin test_tcl.bin \
#		  test_eth.bin test_ip.bin test_telnetd.bin
OUTS		= $(TESTS:%.bin=%.out)

# Using SAM-BA utility.
PROG            = sam-ba /dev/ttyUSB? AT91SAM7X256-EK load-flash.tcl

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.bin *.lst *.dis .deps $(MODULES)

probe:
		$(PROG)

load:		test_lcd.bin
		$(PROG) $<

olimex:		load-flash.tcl sam7-ex256-ge12.olimex
		$(PROG) sam7-ex256-ge12.olimex

include $(OS)/sources/rules.mak
