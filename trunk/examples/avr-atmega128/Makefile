TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS		+= -DKHZ=10000
TESTS		= test_uart.sre test_nvram.sre \
		  test_timer.sre test_crt.sre test_ram.sre \
		  test_slip.sre test_ip.sre test_cs8900.sre \
		  test_ip8900.sre #test_cksum.sre test_snmp.sre test_snmp8900.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.out *.hex *.sre *.[oasi] *.lst *.dis *~ .deps $(MODULES)

include $(OS)/sources/rules.mak

###
