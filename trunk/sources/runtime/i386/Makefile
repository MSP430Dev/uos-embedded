all:		call86.h

call86.h:	call86.S
		$(CC) -I../.. -nostdlib -Wl,-Ttext,0x500 -o call86.o call86.S
		objcopy -O binary call86.o call86.bin
		hexdump -v -e '16/1 "%d," "\n"' call86.bin | \
			sed 's/,,,*/,/' > call86.h
		rm -f call86.o call86.bin

clean:
		rm -f *~ *.o call86.o call86.bin
###
