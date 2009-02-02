TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#CFLAGS		+= -DKHZ=50000 # SNDS300
CFLAGS		+= -DKHZ=41250 # Cronyx
TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_t-uart.srec test_ram.srec \
		  test_hdlc.srec test_eth.srec
		  #test_tcl.srec test_cronyx.srec test_ip.srec test_snmp.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
