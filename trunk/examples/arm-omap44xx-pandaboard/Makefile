TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

TEST		= test_led
TESTS		= test_led.bin #test_debug.bin test_task.bin test_timer.bin \
#		  test_uart.bin test_t-uart.bin test_lcd.bin #\
#		  test_tcl.bin test_mmc.bin test_can.bin \
#		  test_usb.bin test_eth.bin test_ip.bin test_telnetd.bin
OUTS		= $(TESTS:%.bin=%.elf)
DIS		= $(TESTS:%.bin=%.dis)

# Using SAM-BA utility.
PROG            = sam-ba /dev/ttyUSB? AT91SAM7X256-EK load-flash.tcl

all:		$(OUTS) $(TESTS) $(DIS)
		$(SIZE) $(OUTS)
		cp $(TEST).bin MLO

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.bin *.lst *.dis *.map .deps png2rgb $(MODULES) MLO

probe:
		$(PROG)

load:		test_lcd.bin
		$(PROG) $<

include $(OS)/sources/rules.mak
