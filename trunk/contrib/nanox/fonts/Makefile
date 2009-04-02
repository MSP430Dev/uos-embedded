all:		convbdf 10x20.c lucidasans11.c cursor.c

# Get 10x20.bdf from UCS fonts.
# http://www.cl.cam.ac.uk/~mgk25/ucs-fonts.html
10x20.c:	10x20.bdf.gz convbdf
		gzip -d < 10x20.bdf.gz > 10x20.bdf
		./convbdf -c -l255 10x20.bdf
		rm -f 10x20.bdf

cursor.c:	cursor.bdf.gz convbdf
		gzip -d < cursor.bdf.gz > cursor.bdf
		./convbdf -c -l255 cursor.bdf
		rm -f cursor.bdf

# Get Lucida_Sans_Unicode.ttf from internet free font collection:
# http://www.webpagepublicity.com/free-fonts/l/Lucida%20Sans%20Unicode.ttf
# Use ttf2bdf utility from ftp://crl.nmsu.edu/CLR/multiling/General/
# Or, under Ubuntu, install freetype1-tools package.
lucidasans11.c:	Lucida_Sans_Unicode.ttf convbdf
		-ttf2bdf -p 11 Lucida_Sans_Unicode.ttf > lucidasans11.bdf
		./convbdf -c -l255 -u164 -a2 -d3 lucidasans11.bdf
		rm -f lucidasans11.bdf

lucidasans10.c:	Lucida_Sans_Unicode.ttf convbdf
		-ttf2bdf -p 10 Lucida_Sans_Unicode.ttf > lucidasans10.bdf
		./convbdf -c -l255 -u164 -a3 -d2 lucidasans10.bdf
		rm -f lucidasans10.bdf

lucidacon14.c:	Lucida_Console.ttf convbdf
		-ttf2bdf -p 14 Lucida_Console.ttf > lucidacon14.bdf
		./convbdf -c -l255 lucidacon14.bdf
		rm -f lucidacon14.bdf

convbdf:	convbdf.c
		gcc -Wall -O convbdf.c -o convbdf

clean:
		-rm -f *~ convbdf 10x20.bdf lucidasans11.bdf cursor.bdf
