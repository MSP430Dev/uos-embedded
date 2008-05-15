TOPDIR		= $(shell cd ../../../..; pwd)
#TARGET		= $(TOPDIR)/targets/i386
TARGET		= $(TOPDIR)/targets/i386-nanox-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -I$(TOPDIR)/sources/posix -I$(TOPDIR)/sources/nanox/include

OUTS		= libxplus.a

OBJS		= screen.o window.o event.o sensor.o pen.o cursor.o color.o\
		  ttable.o slist.o cutils.o bitmap.o font.o pixmap.o\
		  ntable.o htable.o #textbuf.o image.o

all:		$(OUTS)

libxplus.a:	$(OBJS)
		rm -f $@
		$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
		rm -f *.out *.[osia] *.lst *.dis *~

include $(TOPDIR)/targets/rules.cfg

###
screen.o: window.h font.h event.h extern.h slist.h qlist.h ttable.h ntable.h htable.h
window.o: window.h event.h sensor.h cursor.h extern.h slist.h qlist.h
event.o: event.h
sensor.o: sensor.h event.h
cursor.o: window.h pen.h cursor.h font.h extern.h
color.o: window.h
ttable.o: ttable.h
slist.o: slist.h
bitmap.o: window.h pen.h extern.h
textbuf.o: textbuf.h
font.o: window.h font.h extern.h
pixmap.o: window.h pen.h extern.h
pen.o: window.h pen.h font.h image.h extern.h
ntable.o: ntable.h extern.h
htable.o: htable.h
