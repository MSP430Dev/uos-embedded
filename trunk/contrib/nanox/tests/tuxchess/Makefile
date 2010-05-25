OS		 = $(shell cd ../../../..; pwd)
TARGET		 = $(OS)/examples/i386-grub-nanox

include $(TARGET)/target.cfg
CFLAGS		+= -I$(OS)/contrib -Dnxmain_tuxchess=nxmain
vpath %.c . ..

OUTS		 = tuxchess.out

OBJS		 = tuxchess.o board.o data.o eval.o search.o images.o uos.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

tuxchess.out:	$(OBJS) $(TARGET)/libuos.a
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o tuxchess.out

images.h:	mkimages
		mkimages > images.h

mkimages:	mkimages.c
		gcc -Wall -O $< -o mkimages

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../a.img tuxchess.out ::tuxchess

grub:		$(OUTS)
		strip $(OUTS)
		mv tuxchess.out tuxchess

clean:
		rm -f *.out *.[osi] *.lst *.dis *~ mkimages images.h

include $(OS)/sources/rules.mak

###
images.o: images.c images.h
