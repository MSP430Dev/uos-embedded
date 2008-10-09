OS		 = $(shell cd ../..; pwd)
TARGET		 = $(OS)/targets/i386
include $(TARGET)/target.cfg

OUTS		 = t_kbd.out t_mouse.out t_timer.out t_tcl.out
#OUTS		+= t_task.out
#OUTS		+= t_math.out
OUTS		+= t_pipe.out

all:		.deps startup.o libuos.a $(OUTS) $(TESTS)
		size $(OUTS)

.deps:
		mkdir .deps

img:		$(OUTS)
		strip $(OUTS)
		mcopy -Do -i../nanox/a.img t_tcl.out ::tcl

grub:		$(OUTS)
		strip $(OUTS)
		mv t_tcl.out tcl
#		mv t_timer.out timer
#		mv t_mouse.out mouse
#		mv t_kbd.out kbd

clean:
		rm -rf *.out *.[osi] *.lst *.dis *~ .deps

include $(OS)/sources/rules.mak

###
