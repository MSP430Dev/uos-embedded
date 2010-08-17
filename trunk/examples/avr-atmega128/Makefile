TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_nvram.srec test_ram.srec \
		  test_slip.srec test_ip.srec test_cs8900.srec \
		  test_ip8900.srec #test_cksum.srec test_snmp.srec test_snmp8900.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using avrdude programmer with STK200 LPT adapter.
#PROG           = avrdude -cstk200 -Enoreset -patmega128

# Using avrdude programmer with JTAGICE USB adapter.
PROG            = avrdude -cjtag1 -P /dev/ttyUSB? -patmega128

# Using avrdude programmer with AVRISP USB adapter.
#PROG           = avrdude -cstk500v2 -P /dev/ttyUSB? -patmega128

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.elf *.hex *.srec *.[oasi] *.lst *.dis *~ .deps $(MODULES)

probe:
		$(PROG) -v

load:		test_debug.srec
		$(PROG) -e -U$< #$(FUSES)

include $(OS)/sources/rules.mak

###
