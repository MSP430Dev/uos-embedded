CFLAGS		= -Wall -O -g
LDFLAGS		= -lpng

all:		png2rgb convbdf lucidasans11.c

# Get Lucida_Sans_Unicode.ttf from internet free font collection:
# http://www.webpagepublicity.com/free-fonts/l/Lucida%20Sans%20Unicode.ttf
# Use otf2bdf utility from ftp://crl.nmsu.edu/CLR/multiling/General/
# Or, under Ubuntu, install freetype1-tools package.
lucidasans11.c:	convbdf ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf
		-otf2bdf -p 11 -l "20_126 164 1025 1105 1040_1103" ../../contrib/nanox/fonts/Lucida_Sans_Unicode.ttf > lucidasans11.bdf
		./convbdf -c -u164 -a2 -d3 lucidasans11.bdf
		rm -f lucidasans11.bdf

clean:
		-rm -f *~ png2rgb convbdf lucidasans11.bdf
