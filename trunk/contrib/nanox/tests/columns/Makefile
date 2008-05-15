TOPDIR		= $(shell cd ../../..; pwd)
#TARGET		= $(TOPDIR)/targets/i386
TARGET		= $(TOPDIR)/targets/i386-nanox-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -Ixplus
CFLAGS		+= -I$(TOPDIR)/sources/posix -I$(TOPDIR)/sources/nanox/include

OUTS		= columns.out

OBJS            = xcolumns.o usleep.o uos.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

columns.out:	$(OBJS) $(TARGET)/libuos.a
		$(MAKE) -Cxplus all
		$(CC) $(LDFLAGS) $(OBJS) -Lxplus -lxplus $(LIBS) -o columns.out

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../a.img columns.out ::columns

grub:		$(OUTS)
		strip $(OUTS)
		lsz columns.out

clean:
		rm -f *.out *.[osi] *.lst *.dis *~
		$(MAKE) -Cxplus clean

include $(TOPDIR)/targets/rules.cfg

###
