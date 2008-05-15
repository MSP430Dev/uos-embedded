#
# Makefile for GCC compiler
#

OBJS		= regexp.o regsub.o

CFLAGS		+= -Wall -O -g -I.. #-DDEBUG_REGEXP

all:		try

try:		try.o $(OBJS)
		$(CC) try.o $(LDFLAGS) $(OBJS) -o try

clean:
		rm -f *~ *.o try
###
regexp.o: regexp.c regexp.h regpriv.h
regsub.o: regsub.c regexp.h regpriv.h
try.o: try.c regexp.h regpriv.h
