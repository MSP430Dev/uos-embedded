TOPDIR		 = $(shell cd ../..; pwd)
#TARGET		 = $(TOPDIR)/targets/i386
TARGET		 = $(TOPDIR)/targets/i386-nanox-dbg
include $(TARGET)/target.cfg
CFLAGS		+= -I$(TOPDIR)/sources/posix -I$(TOPDIR)/sources/fltk

OUTS		+= mandelbrot.out
OUTS		+= forms.out
#OUTS		+= hello.out
#OUTS		+= editor.out
OUTS		+= fltcl.out

all:		$(OUTS) $(TESTS)
		size $(OUTS)

$(TARGET)/libuos.a: always
		$(MAKE) -C $(TARGET)

always:

img:		$(OUTS)
		strip $(OUTS)
#		mcopy -Do -i../nanox/a.img mandelbrot.out ::nxdemo
#		mcopy -Do -i../nanox/a.img forms.out ::nxdemo
#		mcopy -Do -i../nanox/a.img hello.out ::nxdemo
#		mcopy -Do -i../nanox/a.img editor.out ::nxdemo
		mcopy -Do -i../nanox/a.img fltcl.out ::nxdemo

grub:		$(OUTS)
		strip $(OUTS)
#		mv mandelbrot.out mandelbrot
#		mv forms.out forms
		lsz $(OUTS)

clean:
		rm -f *.out *.[osi] *.lst *.dis *~ *_ui.cxx  *_ui.h
		rm -f $(TARGET)/nanox/timestamp
		rm -f $(TARGET)/fltk/timestamp

cleanall:	clean
		$(MAKE) -C $(TARGET) clean

mandelbrot.out:	mandelbrot.o mandelbrot_ui.o uos.o $(TARGET)/libuos.a
		$(CXX) $(LDFLAGS) $*.o mandelbrot_ui.o uos.o $(LIBS) -o $@

forms.out:	forms.o uos.o $(TARGET)/libuos.a
		$(CXX) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

hello.out:	hello.o uos.o $(TARGET)/libuos.a
		$(CXX) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

editor.out:	editor.o uos.o $(TARGET)/libuos.a
		$(CXX) $(LDFLAGS) $*.o uos.o $(LIBS) -o $@

fltcl.out:	output.o tcl.o uos.o $(TARGET)/libuos.a
		$(CXX) $(LDFLAGS) output.o tcl.o uos.o $(LIBS) -o $@

include $(TOPDIR)/targets/rules.cfg

###
mandelbrot.o: mandelbrot.cxx mandelbrot_ui.h
