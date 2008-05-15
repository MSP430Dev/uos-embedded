OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/i86-dos
include $(TARGET)/target.cfg

#CFLAGS		+= -v
#OUTS		+= t_debug.com t_printf.com
OUTS		+= t_task.com t_timer.com
#test_uart.com test_nvram.com \
#		  test_snmp.com test_crt.com test_cksum.com test_ram.com \
#		  test_slip.com test_ip.com test_cs8900.com \
#		  test_ip8900.com test_snmp8900.com

all:		$(OUTS)
#		avr-size $(OUTS)

clean:
		rm -f *.out *.com *.[osi] *.lst *.dis *~

include $(OS)/sources/rules.mak

###
