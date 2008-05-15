OS		= $(shell cd ../..; pwd)
TARGET		= $(OS)/targets/linux386
include $(TARGET)/target.cfg

PROG		= hello
CFLAGS		+= -DKHZ=50000

all:		$(PROG)

$(PROG):	$(PROG).o
		$(CC) $(LDFLAGS) $< $(LIBS) -o $@

clean:
		rm -f *.[osi] *.lst *.dis $(PROG) *~

include $(OS)/sources/rules.mak

###
