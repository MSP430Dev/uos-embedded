TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS		+= -DKHZ=16000
TESTS		= test_debug.srec test_uart.srec test_timer.srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.[oasi] *~ $(MODULES) *.out *.hex *.srec *.lst *.dis .deps

probe:
		avrdude -cstk500v1 -patmega168 -b19200 -P /dev/ttyUSB?

probe3:
		avrdude -cjtag1 -patmega168 -b115200 -P /dev/ttyUSB?

load:
		avrdude -cstk500v1 -patmega168 -b19200 -P /dev/ttyUSB? -D -Utest_debug.srec

# To restore a bootblock, you should use STK200 LPT adapter.
# Connect it to ICSP 6-pin header of Arduino Diecimila board.
# efuse=00: BOOTSZ[0:1]=0 (max size of boot sector), BOOTRST=0 (reset jumps to boot)
# hfuse=DD: SPIEN=0 (always), BODLEVEL[2:0]=5 (brown-out detector level 2.7 V)
# lfuse=FF: CKSEL[3:0]=F (frequency 8-16 MHz), SUT[1:0]=3 (slowly rising power)
boot:		ATmegaBOOT_168_diecimila.hex
		avrdude -cstk200 -patmega168 -e -u -Ulock:w:0x3f:m -Uefuse:w:0x00:m -Uhfuse:w:0xDD:m -Ulfuse:w:0xFF:m
		avrdude -cstk200 -patmega168 -Uflash:w:$< -Ulock:w:0x0f:m

probe2:
		avrdude -cstk200 -patmega168 -v

load2:
		avrdude -cstk200 -patmega168 -Utest_debug.srec -u -Ulock:w:0x3f:m -Uefuse:w:0x01:m -Uhfuse:w:0xDD:m -Ulfuse:w:0xFF:m

include $(OS)/sources/rules.mak
