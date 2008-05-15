TOPDIR		 = $(shell cd ../../..; pwd)
#TARGET		 = $(TOPDIR)/targets/i386
TARGET		 = $(TOPDIR)/targets/i386-nanox-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -I$(TOPDIR)/sources/posix

OUTS		 = nanowm.out

OBJS		 = nanowm.o events.o wlist.o clients.o actions.o nterm.o uos.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

nanowm.out:	$(OBJS) $(TARGET)/libuos.a
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o nanowm.out

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../a.img nanowm.out ::nxdemo

grub:		$(OUTS)
		strip $(OUTS)
		mv nanowm.out nanowm

clean:
		rm -f *.out *.[osi] *.lst *.dis *~

include $(TOPDIR)/targets/rules.cfg

###
