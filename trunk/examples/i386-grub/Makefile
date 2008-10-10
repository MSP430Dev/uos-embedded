TARGET		 = $(CURDIR)
OS		 = $(shell cd ../..; pwd)
include target.cfg

OUTS		 = t_kbd.out t_mouse.out t_timer.out #t_tcl.out
OUTS		+= t_task.out
#OUTS		+= t_math.out
OUTS		+= t_pipe.out

all:		$(OUTS) $(TESTS)
		size $(OUTS)

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../nanox/a.img t_tcl.out ::tcl

grub:		$(OUTS)
		strip $(OUTS)
		mv t_tcl.out tcl
#		mv t_timer.out timer
#		mv t_mouse.out mouse
#		mv t_kbd.out kbd

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.out *.[oasi] *.lst *.dis *~ .deps $(MODULES)

include $(OS)/sources/rules.mak

###
