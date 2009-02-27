TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

# Compile without kernel module in target.cfg.
TESTS		= test_debug.hex test_task.hex test_timer.hex test_lcd.hex \
		  test_uart.hex test_olimex.hex philosophers.hex #test_nvram.hex test_cs8900.hex

OUTS            = $(TESTS:%.srec=%.out)

# Using msp430-jtag programmer with JTAG LPT adapter.
PROG            = env LIBMSPGCC_PATH=/usr/local/lib msp430-jtag

all:		$(OUTS) $(TESTS)
		for f in *.out; do $(SIZE) *.out; break; done

$(TESTS):	startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.srec *.lst *.dis .deps $(MODULES) $(TESTS)

probe:
		$(PROG) -D

load:		test_uart.hex
		$(PROG) -e $<

# Original software from http://www.olimex.com/dev/soft/msp430/easyweb_test.tar.gz
olimex:		easyweb.hex
		$(PROG) -e $<

test_lcd.out:	test_lcd.o startup.o libuos.a lcd.o
		$(CC) $(LDFLAGS) test_lcd.o lcd.o $(LIBS) -o $@

test_nvram.out:	test_nvram.o startup.o libuos.a lcd.o
		$(CC) $(LDFLAGS) test_nvram.o lcd.o $(LIBS) -o $@

test_olimex.out: test_olimex.o startup.o libuos.a lcd.o
		$(CC) $(LDFLAGS) test_olimex.o lcd.o $(LIBS) -o $@

philosophers.out: philosophers.o startup.o libuos.a lcd.o
		$(CC) $(LDFLAGS) philosophers.o lcd.o $(LIBS) -o $@

include $(OS)/sources/rules.mak

###
