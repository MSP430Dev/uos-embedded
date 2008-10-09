OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/atmega128
include $(TARGET)/target.cfg

#E		= hex
E		= sre
CFLAGS		+= -DKHZ=10000
TESTS		= test_uart.$(E) test_nvram.$(E) \
		  test_timer.$(E) test_crt.$(E) test_cksum.$(E) test_ram.$(E) \
		  test_slip.$(E) test_ip.$(E) test_cs8900.$(E) \
		  test_ip8900.$(E) #test_snmp.$(E) test_snmp8900.$(E)
OUTS		= $(TESTS:%.$(E)=%.out)

LOAD            = test_debug.sre

all:		.deps target $(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

.deps:
		mkdir .deps

target:
		$(MAKE) -C $(TARGET)

clean:
		rm -rf *.out *.hex *.$(E) *.[osi] *.lst *.dis *~ .deps

include $(OS)/sources/rules.mak

###
