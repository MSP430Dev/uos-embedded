TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

OBJS 		= usb-hid.o

all:		firmware.elf firmware.srec firmware.dis
		$(SIZE) firmware.elf

firmware.elf:   startup.o libuos.a $(OBJS)
		$(CC) $(LDFLAGS) $(CFLAGS) $(DEPFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -rf *~ usb/*~ *.[oasi] *.elf *.lst *.dis .deps *.srec *.hex $(MODULES)

load:		firmware.srec
		$(OBJCOPY) -I srec -O ihex firmware.srec firmware.hex
		ubw32 -write firmware.hex -reset

include $(OS)/sources/rules.mak
