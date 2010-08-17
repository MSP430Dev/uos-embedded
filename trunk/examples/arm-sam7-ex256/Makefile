TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin test_task.bin test_timer.bin \
		  test_uart.bin test_t-uart.bin test_lcd.bin #\
#		  test_tcl.bin test_mmc.bin test_can.bin \
#		  test_usb.bin test_eth.bin test_ip.bin test_telnetd.bin
OUTS		= $(TESTS:%.bin=%.elf)

# Using SAM-BA utility.
PROG            = sam-ba /dev/ttyUSB? AT91SAM7X256-EK load-flash.tcl

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.bin *.lst *.dis .deps png2rgb $(MODULES)

png2rgb:	../../sources/lcd6100/png2rgb.c
		gcc -Wall -O ../../sources/lcd6100/png2rgb.c -lpng -o $@

test_lcd.o:	test_lcd.c image_frog.c
		$(CC) $(CFLAGS) -c test_lcd.c

image_frog.c:	frog.png png2rgb
		./png2rgb frog.png > image_frog.c

probe:
		$(PROG)

load:		test_lcd.bin
		$(PROG) $<

olimex:		load-flash.tcl sam7-ex256-ge12.olimex
		$(PROG) sam7-ex256-ge12.olimex

include $(OS)/sources/rules.mak
