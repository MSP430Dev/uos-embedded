#!/bin/sh
#
# Create distribution archive for MIPS32 platform.
#
rm -rf uos-mips32
ln -s trunk uos-mips32
tar cf uos-mips32.tar uos-mips32/. \
	--exclude uos-mips32/./contrib \
	--exclude uos-mips32/./examples/arm-cronyx-bridge \
	--exclude uos-mips32/./examples/arm-s3c4530 \
	--exclude uos-mips32/./examples/arm-sam7-ex256 \
	--exclude uos-mips32/./examples/arm-sam9-s3e \
	--exclude uos-mips32/./examples/avr-arduino-diecimila \
	--exclude uos-mips32/./examples/avr-atmega103 \
	--exclude uos-mips32/./examples/avr-atmega128 \
	--exclude uos-mips32/./examples/avr-atmega161 \
	--exclude uos-mips32/./examples/avr-atmega256 \
	--exclude uos-mips32/./examples/avr-mt-128 \
	--exclude uos-mips32/./examples/i386-grub \
	--exclude uos-mips32/./examples/i386-grub-directfb \
	--exclude uos-mips32/./examples/i386-grub-nanox \
	--exclude uos-mips32/./examples/linux386 \
	--exclude uos-mips32/./examples/msp430-easyweb2 \
	--exclude uos-mips32/./sources/adc \
	--exclude uos-mips32/./sources/cs8900 \
	--exclude uos-mips32/./sources/enc28j60 \
	--exclude uos-mips32/./sources/i8042 \
	--exclude uos-mips32/./sources/iic \
	--exclude uos-mips32/./sources/input \
	--exclude uos-mips32/./sources/regexp9 \
	--exclude uos-mips32/./sources/s3c4530 \
	--exclude uos-mips32/./sources/smc91c111 \
	--exclude uos-mips32/./sources/snmp \
	--exclude uos-mips32/./sources/tap \
	--exclude uos-mips32/./sources/vesa \
	--exclude '*/.svn'
rm uos-mips32
tar xf uos-mips32.tar
(cd uos-mips32/examples/mips32-mc24rem && make clean && (make > compile.log 2>&1))
rm -f uos-mips32.zip uos-mips32.tar
zip -r uos-mips32 uos-mips32
