TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
#TESTS		= test_debug.srec

TESTS		= test_task.srec test_timer.srec test_uart.srec philosophers.srec
TESTS		+= test_ram.srec
#TESTS		+= test_tcp_server.srec test_tcp_client.srec

all:		$(TESTS)
		$(SIZE) --target=srec *.srec

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
