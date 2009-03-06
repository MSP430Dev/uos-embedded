TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#CFLAGS		+= -DKHZ=50000 # SNDS300
CFLAGS		+= -DKHZ=41250 # Cronyx
TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_t-uart.srec test_ram.srec \
		  test_hdlc.srec test_eth.srec \
		  test_ip.srec test_snmp.srec
#		  test_tcl.srec test_cronyx.srec
OUTS		= $(TESTS:%.srec=%.out)

# Using Cronyx armprog utility.
PROG            = ./armprog -f /dev/ttyS0 -d1000 -D -D

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

load:		test_debug.srec
		while :; do $(PROG) $<; done

include $(OS)/sources/rules.mak

###
