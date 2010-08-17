TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_t-uart.srec test_ram.srec \
		  test_hdlc.srec test_eth.srec test_ip.srec \
		  test_tcl.srec test_telnetd.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using Cronyx armprog utility.
PROG            = ./armprog -f /dev/ttyS0

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES)

probe:		flashid
		./flashid

load:		test_telnetd.srec
		$(PROG) $<

include $(OS)/sources/rules.mak

###
