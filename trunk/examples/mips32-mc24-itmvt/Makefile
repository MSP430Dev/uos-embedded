TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
#TESTS		= test_debug.srec

TESTS		= test_task.srec test_timer.srec test_uart.srec philosophers.srec
TESTS		+= test_ram.srec
#TESTS		+= test_tcp_server.srec test_tcp_client.srec

PROGRAM		= sample_control
OBJS		= sample_control.o Sample_Control_Law.o

all:		$(PROGRAM) $(TESTS)
		$(SIZE) --target=srec *.srec

$(TESTS):	startup.o libuos.a

$(PROGRAM):	$(OBJS) startup.o libuos.a
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM).elf
		$(OBJCOPY) -O srec $(PROGRAM).elf $(PROGRAM).srec
		@chmod -x $(PROGRAM).srec

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak

###
