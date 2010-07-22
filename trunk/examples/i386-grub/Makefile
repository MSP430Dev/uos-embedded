TARGET		 = $(CURDIR)
OS		 = $(shell cd ../..; pwd)
include target.cfg

OUTS		 = t_kbd.out t_mouse.out t_timer.out #t_tcl.out
OUTS		+= t_task.out
#OUTS		+= t_math.out
OUTS		+= t_pipe.out

ISOFLAGS	= -R -b boot/grub/stage2_eltorito -no-emul-boot \
		  -boot-load-size 4 -boot-info-table -input-charset utf-8

all:		$(OUTS) $(TESTS) uos-i386-grub.iso
		$(SIZE) $(OUTS)

uos-i386-grub.iso: $(OUTS) stage2_eltorito menu.txt
		mkdir -p iso/boot/grub
		cp stage2_eltorito iso/boot/grub
		cp menu.txt iso/boot/grub/menu.lst
		cp *.out iso/
		genisoimage $(ISOFLAGS) -o $@ iso

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.out *.[oasi] *.lst *.dis *~ .deps $(MODULES) iso *.iso

include $(OS)/sources/rules.mak

###
