OS		 = $(shell cd ../../..; pwd)
TARGET		 = $(OS)/examples/i386-grub-nanox

include $(TARGET)/target.cfg
vpath %.c . tuxchess nbreaker

CFLAGS		+= -I$(OS)/contrib -I$(OS)/contrib/posix

OUTS		+= nxtcl.out
OUTS		+= nxdemo.out
OUTS		+= polydemo.out
OUTS		+= nxfont.out
OUTS		+= nxinput.out
#OUTS		+= nxclock.out
#OUTS		+= landmine.out
#OUTS		+= ntetris.out
#OUTS		+= nxeyes.out

OBJS		 = nterm.o tcl.o uos.o
OBJS		+= landmine.o ntetris.o nxclock.o nxeyes.o
OBJS		+= tuxchess.o board.o data.o eval.o search.o images.o
OBJS            += animate.o ball.o bat.o brick.o cheat.o draw.o dump.o \
		   event.o game.o init.o levels.o loader.o misc.o nbreaker.o \
		   powers.o scores.o sprite.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -ia.img nxtcl.out ::nxtcl
#		mcopy -Do -ia.img nxdemo.out ::nxdemo
#		mcopy -Do -ia.img nxinput.out ::nxdemo
#		mcopy -Do -ia.img nxclock.out ::nxclock
#		mcopy -Do -ia.img landmine.out ::landmine
#		mcopy -Do -ia.img ntetris.out ::ntetris

grub:		$(OUTS)
		strip $(OUTS)
		mv nxclock.out nxclock
		mv nxdemo.out nxdemo
#		mv nxfont.out nxfont

clean:
		rm -f *.out *.[osi] *.lst *.dis *~
		$(MAKE) -Cnbreaker clean
		$(MAKE) -Ctuxchess clean

nxtcl.out:	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

landmine.out:	landmine.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

ntetris.out:	ntetris.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxclock.out:	nxclock.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxeyes.out:	nxeyes.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxfont.out:	nxfont.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

polydemo.out:	polydemo.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

nxinput.out:	nxinput.o
		$(CC) $(LDFLAGS) $*.o $(LIBS) -o $@

nxdemo.out:	nxdemo.o
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
