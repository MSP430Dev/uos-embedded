TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TESTS		= test_debug.bin
OUTS		= $(TESTS:%.bin=%.out)

# Using SAM-BA utility.
PROG            = sam-ba /dev/ttyUSB? AT91SAM9260-EK load-flash.tcl

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.bin *.lst *.dis .deps png2rgb $(MODULES)

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

include $(OS)/sources/rules.mak
