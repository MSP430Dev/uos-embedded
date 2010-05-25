OS		 = $(shell cd ../../../..; pwd)
TARGET		 = $(OS)/examples/i386-grub-nanox

include $(TARGET)/target.cfg
CFLAGS		+= -I$(OS)/contrib -I$(OS)/contrib/posix -Dnxmain_nbreaker=nxmain
vpath %.c . ..

OUTS		 = nbreaker.out

OBJS		 = animate.o ball.o bat.o brick.o cheat.o draw.o dump.o \
		   event.o game.o init.o levels.o loader.o misc.o nbreaker.o \
		   powers.o scores.o sprite.o uos.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

nbreaker.out:	$(OBJS) $(TARGET)/libuos.a
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o nbreaker.out

gamefile.h:	mkdata default.nbk
		mkdata < default.nbk > gamefile.h

images.h:	mkimages images/*.gif
		mkimages images/*.gif > images.h

mkdata:		mkdata.c
		gcc -Wall -O $< -o mkdata

mkimages:	mkimages.c
		gcc -Wall -O $< -o mkimages

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../a.img nbreaker.out ::nbreaker

grub:		$(OUTS)
		strip $(OUTS)
		mv nbreaker.out nbreaker

clean:
		rm -f *.out *.[osi] *.lst *.dis *~
		rm -f mkdata gamefile.h mkimages images.h

include $(OS)/sources/rules.mak

###
animate.o: animate.c nbreaker.h
ball.o: ball.c nbreaker.h
bat.o: bat.c nbreaker.h
brick.o: brick.c nbreaker.h
cheat.o: cheat.c nbreaker.h
draw.o: draw.c nbreaker.h
dump.o: dump.c nbreaker.h
event.o: event.c nbreaker.h
game.o: game.c nbreaker.h
init.o: init.c nbreaker.h
levels.o: levels.c nbreaker.h
loader.o: loader.c nbreaker.h gamefile.h
misc.o: misc.c nbreaker.h
mkdata.o: mkdata.c
mkimages.o: mkimages.c
nbreaker.o: nbreaker.c nbreaker.h
powers.o: powers.c nbreaker.h
scores.o: scores.c nbreaker.h
sprite.o: sprite.c nbreaker.h images.h
uos.o: uos.c
