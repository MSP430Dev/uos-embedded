TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS		+= -DKHZ=10000
TESTS		= test_uart.srec test_nvram.srec \
		  test_timer.srec test_ram.srec \
		  test_slip.srec test_ip.srec test_cs8900.srec \
		  test_ip8900.srec #test_cksum.srec test_snmp.srec test_snmp8900.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.out *.hex *.srec *.[oasi] *.lst *.dis *~ .deps $(MODULES)

include $(OS)/sources/rules.mak

###
