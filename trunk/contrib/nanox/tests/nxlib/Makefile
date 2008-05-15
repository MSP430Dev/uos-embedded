TOPDIR		 = $(shell cd ../../..; pwd)
#TARGET		 = $(TOPDIR)/targets/i386
TARGET		 = $(TOPDIR)/targets/i386-nanox-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -I$(TOPDIR)/sources/posix -I$(TOPDIR)/sources/nanox/include

OUTS		 = example.out

OBJS		 = example.o uos.o

all:		$(OUTS) $(TESTS)
		size $(OUTS)

example.out:	$(OBJS) $(TARGET)/libuos.a
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o example.out

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../a.img example.out ::nlibexam

grub:		$(OUTS)
		strip $(OUTS)
		mv example.out nlibexam

clean:
		rm -f *.out *.[osi] *.lst *.dis *~ nlibexam

include $(TOPDIR)/targets/rules.cfg

###
