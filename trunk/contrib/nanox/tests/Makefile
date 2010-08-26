OS		 = $(shell cd ../../..; pwd)
TARGET		 = $(OS)/examples/i386-grub-nanox

include $(TARGET)/target.cfg
vpath %.c . tuxchess nbreaker

CFLAGS		+= -I$(OS)/contrib -I$(OS)/contrib/posix

OUTS		+= nxtcl.elf
OUTS		+= nxdemo.elf
OUTS		+= polydemo.elf
OUTS		+= nxfont.elf
OUTS		+= nxinput.elf
#OUTS		+= nxclock.elf
#OUTS		+= landmine.elf
#OUTS		+= ntetris.elf
#OUTS		+= nxeyes.elf

OBJS		 = nterm.o tcl.o uos.o
OBJS		+= landmine.o ntetris.o nxclock.o nxeyes.o
OBJS		+= tuxchess.o board.o data.o eval.o search.o images.o
OBJS            += animate.o ball.o bat.o brick.o cheat.o draw.o dump.o \
		   event.o game.o init.o levels.o loader.o misc.o nbreaker.o \
		   powers.o scores.o sprite.o

all:		$(OUTS) $(TESTS)
		$(SIZE) $(OUTS)

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -ia.img nxtcl.elf ::nxtcl
#		mcopy -Do -ia.img nxdemo.elf ::nxdemo
#		mcopy -Do -ia.img nxinput.elf ::nxdemo
#		mcopy -Do -ia.img nxclock.elf ::nxclock
#		mcopy -Do -ia.img landmine.elf ::landmine
#		mcopy -Do -ia.img ntetris.elf ::ntetris

grub:		$(OUTS)
		strip $(OUTS)
		mv nxclock.elf nxclock
		mv nxdemo.elf nxdemo
#		mv nxfont.elf nxfont

clean:
		rm -f *.elf *.[osi] *.lst *.dis *~
		$(MAKE) -Cnbreaker clean
		$(MAKE) -Ctuxchess clean

nxtcl.elf:	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

landmine.elf:	landmine.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

ntetris.elf:	ntetris.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxclock.elf:	nxclock.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxeyes.elf:	nxeyes.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxfont.elf:	nxfont.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

polydemo.elf:	polydemo.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxinput.elf:	nxinput.o
		$(CC) $(LDFLAGS) $*.o $(LIBS) -o $@

nxdemo.elf:	nxdemo.o
		$(CC) $(LDFLAGS) $*.o $(LIBS) -o $@

images.o:	tuxchess/images.c tuxchess/images.h
loader.o:	nbreaker/loader.c nbreaker/gamefile.h
sprite.o:	nbreaker/sprite.c nbreaker/images.h

tuxchess/images.h:
		$(MAKE) -Ctuxchess images.h

nbreaker/images.h:
		$(MAKE) -Cnbreaker images.h

nbreaker/gamefile.h:
		$(MAKE) -Cnbreaker gamefile.h

include $(OS)/sources/rules.mak

###
landmine.o: landmine.c
ntetris.o: ntetris.c ntetris.h
nxclock.o: nxclock.c
nxdemo.o: nxdemo.c
nxeyes.o: nxeyes.c nxeyes.h
nxfont.o: nxfont.c
nxinput.o: nxinput.c
polydemo.o: polydemo.c
