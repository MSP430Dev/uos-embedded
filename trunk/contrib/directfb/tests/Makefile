TOPDIR		 = $(shell cd ../..; pwd)
#TARGET		 = $(TOPDIR)/targets/i386
TARGET		 = $(TOPDIR)/targets/i386-directfb-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -I$(TOPDIR)/sources/posix
CFLAGS		+= -I$(TOPDIR)/sources/directfb
CFLAGS		+= -I$(TOPDIR)/sources/directfb/include
CFLAGS		+= -I$(TOPDIR)/sources/directfb/lib
CFLAGS		+= -DDATADIR=\"\"

OUTS		 = df_andi.out

all:		$(OUTS)
		size $(OUTS)

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -ia.img df_andi.out ::dfandi

grub:		$(OUTS)
		strip $(OUTS)
		mv df_andi.out dfandi

clean:
		rm -f *.out *.[osi] *.lst *.dis *~

df_andi.out:	df_andi.o uos.o
		$(CC) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

include $(TOPDIR)/targets/rules.cfg

###
