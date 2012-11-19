CFLAGS		= -Wall -O -g
LDFLAGS		= -lpng

all:		png2rgb convbdf lucidasans11.c

# Get Lucida_Sans_Unicode.ttf from internet free font collection:
# http://www.webpagepublicity.com/free-fonts/l/Lucida%20Sans%20Unicode.ttf
# Use otf2bdf utility from ftp://crl.nmsu.edu/CLR/multiling/General/
# Under Ubuntu, install otf2bdf package.
bigdigits.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 32 -l "32 48_58" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > bigdigits.bdf
		./convbdf -c -a12 -d15 bigdigits.bdf
		rm -f bigdigits.bdf

middigits.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 20 -l "32 48_58" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > middigits.bdf
		./convbdf -c -a9 -d11 middigits.bdf
		rm -f middigits.bdf

lucidasans11.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 11 -l "20_126 164 1025 1105 1040_1103" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > lucidasans11.bdf
		./convbdf -c -u164 -a2 -d3 lucidasans11.bdf
		rm -f lucidasans11.bdf

lucidasans9.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 9 -l "20_126 164 1025 1105 1040_1103" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > lucidasans9.bdf
		./convbdf -c -u164 -a2 -d3 lucidasans9.bdf
		rm -f lucidasans9.bdf

lucidasans7.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 7 -l "20_126 164 1025 1105 1040_1103" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > lucidasans7.bdf
		./convbdf -c -u164 -a2 -d3 lucidasans7.bdf
		rm -f lucidasans7.bdf

verdana7.c:	convbdf
		-otf2bdf -p 7 -l "20_126 164 1025 1105 1040_1103" /usr/share/fonts/truetype/msttcorefonts/Verdana.ttf > verdana7.bdf
		./convbdf -c -a2 verdana7.bdf
		rm -f verdana7.bdf

5x7.c:		convbdf
		./convbdf -c -u0 -l0x44f -x0xa0-0x40f /home/vak/Qt/qt/src/3rdparty/fonts/5x7.bdf

6x9.c:		convbdf
		./convbdf -c -u0 -l0x44f -x0xa0-0x40f 6x9.bdf

cronyxcourier9.c: convbdf
		perl bdfrecode.pl -table=koi8-unicode.txt < crox1c.bdf > cronyxcourier9.bdf
		./convbdf -c -u1 -a1 cronyxcourier9.bdf
		rm -f cronyxcourier9.bdf

cronyxhelvetica9.c: convbdf
		perl bdfrecode.pl -table=koi8-unicode.txt < crox1h.bdf > cronyxhelvetica9.bdf
		./convbdf -c -u1 -a1 cronyxhelvetica9.bdf
		rm -f cronyxhelvetica9.bdf

cronyxtimes9.c:	convbdf crox1t.bdf
		perl bdfrecode.pl -table=koi8-unicode.txt < crox1t.bdf > cronyxtimes9.bdf
		./convbdf -c -u1 -a1 cronyxtimes9.bdf
		rm -f cronyxtimes9.bdf

font7x9.c:	convbdf bdfrecode.pl full-table.txt koi6x10.bdf
		perl bdfrecode.pl -table=koi8-unicode.txt < koi6x10.bdf > font6x10.bdf
		./convbdf -c -a1 -o $@ font6x10.bdf
		rm -f font6x10.bdf

font6x9.c:	convbdf bdfrecode.pl full-table.txt koi6x10.bdf
		perl bdfrecode.pl -table=koi8-unicode.txt < koi6x10.bdf > font6x10.bdf
		./convbdf -c -a1 -o $@ font6x10.bdf
		rm -f font6x10.bdf

convfont:	convfont.c phyton_6x8.c
		$(CC) $(CFLAGS) -DFONTNAME=Font_6x8 -DFONTFILE=\"phyton_6x8.c\" convfont.c -o $@

clean:
		-rm -f *~ png2rgb convbdf convfont lucidasans11.bdf
