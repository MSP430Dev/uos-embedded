CC		= gcc -g
RANLIB		= true
O		= o
CFLAGS		= -Wall -Wno-missing-braces -Wno-parentheses -Wno-switch -O2 -c -I.
PREFIX		= /usr/local
LIB		= libregexp9.a
VERSION		= 2.0
NAME		= libregexp9

OFILES		= regcomp.$O regerror.$O regexec.$O regsub.$O regaux.$O \
		  rregexec.$O rregsub.$O rune.$O runestrchr.$O

HFILES		= regexp9.h regcomp.h

all:		$(LIB) test

install:	$(LIB)
		mkdir -p $(PREFIX)/share/man/man3 $(PREFIX)/man/man7
		install -m 0644 regexp9.3 $(PREFIX)/share/man/man3/regexp9.3
		install -m 0644 regexp9.7 $(PREFIX)/man/man7/regexp9.7
		mkdir -p $(PREFIX)/lib
		install -m 0644 $(LIB) $(PREFIX)/lib/$(LIB)
		mkdir -p $(PREFIX)/include
		install -m 0644 regexp9.h $(PREFIX)/include/regexp9.h

test:		test.$O $(LIB)
		$(CC) -o test test.$O $(LIB) -L/usr/local/lib

test2:		test2.$O $(LIB)
		$(CC) -o test2 test2.$O $(LIB) -L/usr/local/lib

$(LIB):		$(OFILES)
		ar rvc $(LIB) $(OFILES)
		$(RANLIB) $(LIB)

.c.$O:
		$(CC) $(CFLAGS) $*.c

%.$O: %.c
		$(CC) $(CFLAGS) $*.c


$(OFILES):	$(HFILES)

clean:
		rm -f *.o *.a *~ test test2

.phony:		all clean nuke install tgz rpm ports
