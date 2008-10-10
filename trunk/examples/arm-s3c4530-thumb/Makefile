TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

#CFLAGS		+= -DKHZ=50000 # SNDS300
CFLAGS		+= -DKHZ=41250 # Cronyx
TESTS		= test_debug.sre test_task.sre test_timer.sre \
		  test_uart.sre test_t-uart.sre test_ram.sre \
		  test_hdlc.sre test_eth.sre
		  #test_tcl.sre test_cronyx.sre test_ip.sre test_snmp.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.sre *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
