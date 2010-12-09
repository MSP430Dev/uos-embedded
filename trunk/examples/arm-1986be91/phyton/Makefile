TARGET		= $(CURDIR)
OS		= $(shell cd ../../..; pwd)
include target.cfg

OBJS 		= font_12x16.o font_6x8.o font_7x10_bold.o font_7x10_thin.o \
		  gl.o joystick.o lcd.o leds.o main.o menu.o menu_gl.o \
		  menu_leds.o menu_text.o text.o

all:		demo.elf demo.hex demo.dis
		$(SIZE) demo.elf

demo.elf:       startup.o libuos.a $(OBJS)
		$(CC) $(LDFLAGS) $(CFLAGS) $(DEPFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -rf *~ *.[oasi] *.elf *.lst *.dis .deps *.hex $(MODULES)

probe:
		milprog

load:		demo.hex
		milprog $<

include $(OS)/sources/rules.mak
