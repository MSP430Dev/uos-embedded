TOPDIR          = $(shell cd ../..; pwd)
#TARGET          = $(TOPDIR)/targets/linux386
#TARGET          = $(TOPDIR)/targets/atmega128
TARGET          = $(TOPDIR)/targets/thumb-dbg
#TARGET          = $(TOPDIR)/targets/i86-dos
include $(TARGET)/target.cfg

OBJS		= tclget.o tclproc.o tclvar.o tclassem.o tclcmdah.o \
		  tclcmdmz.o tclhash.o tclparse.o \
		  tclcmdil.o tclbasic.o tclexpr.o tclutil.o
#		  tclalloc.o tclenv.o tcldosaz.o tcldosut.o tcldosst.o
#		  tcldosgl.o borland.o dos.o readdir.o

#CFLAGS		+= -Wall -O -g -I.. -DTCL_GENERIC_ONLY

all:		tcl

tcl:		$(OBJS) test_tcl.o
		$(CC) $(LDFLAGS) test_tcl.o $(OBJS) $(LIBS) -o tcl

clean:
		rm -f *.[oa] *~ tcl

###
tclassem.o: tclassem.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclbasic.o: tclbasic.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclcmdah.o: tclcmdah.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclcmdil.o: tclcmdil.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclcmdmz.o: tclcmdmz.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclexpr.o: tclexpr.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclget.o: tclget.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclhash.o: tclhash.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclparse.o: tclparse.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclproc.o: tclproc.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclutil.o: tclutil.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
tclvar.o: tclvar.c ../tcl/internal.h ../tcl/tcl.h ../tcl/hash.h
test_tcl.o: test_tcl.c ../tcl/tcl.h
