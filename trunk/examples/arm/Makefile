OS		= $(shell cd ../..; pwd)
#TARGET		= $(OS)/targets/arm
TARGET		= $(OS)/targets/thumb
#TARGET		= $(OS)/targets/thumb-dbg
#TARGET		= $(OS)/targets/arm-dbg
include $(TARGET)/target.cfg

#E		= hex
E		= sre
#CFLAGS		+= -DKHZ=50000 # SNDS300
CFLAGS		+= -DKHZ=41250 # Cronyx
TESTS		= test_debug.sre test_task.sre test_timer.sre \
		  test_uart.sre test_t-uart.sre test_ram.sre \
		  test_tcl.sre test_hdlc.sre test_eth.sre \
		  test_cronyx.sre #test_ip.sre test_snmp.sre
OUTS		= $(TESTS:%.sre=%.out)

all:		.deps target $(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

.deps:
		mkdir .deps

target:
		$(MAKE) -C $(TARGET)

clean:
		rm -rf *.out *.hex *.sre *.[osi] *.lst *.dis *~ .deps

include $(OS)/sources/rules.mak

###
