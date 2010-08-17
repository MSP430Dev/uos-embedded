TARGET		 = $(CURDIR)
OS		 = $(shell cd ../..; pwd)
include target.cfg

OUTS		 = t_kbd.elf t_mouse.elf t_timer.elf #t_tcl.elf
OUTS		+= t_task.elf
#OUTS		+= t_math.elf
OUTS		+= t_pipe.elf

ISOFLAGS	= -R -b boot/grub/stage2_eltorito -no-emul-boot \
		  -boot-load-size 4 -boot-info-table -input-charset utf-8

all:		$(OUTS) $(TESTS) uos-i386-grub.iso
		$(SIZE) $(OUTS)

uos-i386-grub.iso: $(OUTS) stage2_eltorito menu.txt
		mkdir -p iso/boot/grub
		cp stage2_eltorito iso/boot/grub
		cp menu.txt iso/boot/grub/menu.lst
		cp *.elf iso/
		genisoimage $(ISOFLAGS) -o $@ iso

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *.elf *.[oasi] *.lst *.dis *~ .deps $(MODULES) iso *.iso

include $(OS)/sources/rules.mak

###
