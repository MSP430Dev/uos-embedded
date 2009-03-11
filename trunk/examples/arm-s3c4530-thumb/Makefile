TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_t-uart.srec test_ram.srec \
		  test_hdlc.srec test_eth.srec test_ip.srec test_snmp.srec
#		  test_tcl.srec test_cronyx.srec
OUTS		= $(TESTS:%.srec=%.out)

# Using Cronyx armprog utility.
PROG            = ./armprog -f /dev/ttyS0

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

probe:		flashid
		./flashid

load:		test_hdlc.srec
		$(PROG) $<

include $(OS)/sources/rules.mak

###
