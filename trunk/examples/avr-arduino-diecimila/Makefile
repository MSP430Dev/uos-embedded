TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

CFLAGS		+= -DKHZ=16000
TESTS		= test_debug.srec test_task.srec test_timer.srec \
		  test_uart.srec test_nvram.srec
OUTS		= $(TESTS:%.srec=%.elf)

# Using avrdude programmer with STK200 LPT adapter.
#PROG		= avrdude -cstk200 -Enoreset -patmega168

# Using avrdude programmer with JTAGICE USB adapter.
#PROG		= avrdude -cjtag1 -P /dev/ttyUSB? -patmega168

# Using avrdude programmer with AVRISP USB adapter.
#PROG		= avrdude -cstk500v2 -P /dev/ttyUSB? -patmega168
PROG		= avrdude -cstk500v2 -P /dev/tty.usbserial* -patmega168

FUSES		= -Ulfuse:w:0xFF:m -Uhfuse:w:0xDD:m -Uefuse:w:0x01:m

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.[oasi] *~ $(MODULES) *.elf *.srec *.lst *.dis .deps

probe:
		$(PROG) -v

load:		test_nvram.srec
		$(PROG) -e -U$< $(FUSES)

# Using avrdude programmer with Arduino boot block.
probe2:
		avrdude -cstk500v1 -patmega168 -b19200 -P /dev/ttyUSB?

load2:		test_debug.srec
		avrdude -cstk500v1 -patmega168 -b19200 -P /dev/ttyUSB? -D -U$<

# To restore a bootblock, you should use STK200 LPT adapter.
# Connect it to ICSP 6-pin header of Arduino Diecimila board.
# efuse=00: BOOTSZ[0:1]=0 (max size of boot sector), BOOTRST=0 (reset jumps to boot)
# hfuse=DD: SPIEN=0 (always), BODLEVEL[2:0]=5 (brown-out detector level 2.7 V)
# lfuse=FF: CKSEL[3:0]=F (frequency 8-16 MHz), SUT[1:0]=3 (slowly rising power)
boot:		ATmegaBOOT_168_diecimila.hex
		$(PROG) -e -U$< -Ulfuse:w:0xFF:m -Uhfuse:w:0xDD:m -Uefuse:w:0x00:m -Ulock:w:0x0f:m

include $(OS)/sources/rules.mak
