TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

OBJS 		= main.o hello.o usb_descriptors.o

all:		firmware.elf firmware.hex firmware.dis
		$(SIZE) firmware.elf

firmware.elf:   startup.o libuos.a $(OBJS)
		$(CC) $(LDFLAGS) $(CFLAGS) $(DEPFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -rf *~ usb/*~ *.[oasi] *.elf *.lst *.dis .deps *.hex $(MODULES)

load:		firmware.hex
		ubw32 -write $< -reset

include $(OS)/sources/rules.mak
