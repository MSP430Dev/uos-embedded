OS		= $(shell cd ../..; pwd)
#TARGET		= $(OS)/targets/linux386
TARGET		= $(OS)/targets/thumb-dbg
include $(TARGET)/target.cfg

PROG		= phil.sre
CFLAGS		+= -DKHZ=50000

all:		$(PROG)

$(PROG):	phil.o
#		$(CC) $(LDFLAGS) $< $(LIBS) -o $@

load:		$(PROG)
		zip -q - $(PROG) | ssh -x 144.206.181.73 /home/vak/bin/:armload $(PROG)

clean:
		rm -f *.[osi] *.lst *.dis *.out $(PROG) *~

include $(OS)/sources/rules.mak

###
