TOPDIR          = $(shell cd ../..; pwd)
TARGET          = $(TOPDIR)/targets/linux386
include $(TARGET)/target.cfg
CFLAGS		+= -DTEST

OBJS		= puts.o vprintf.o printf.o snprintf.o \
		  vscanf.o sscanf.o scanf.o stropen.o \
		  vprintf-getlen.o drain-input.o

all:		$(OBJS)

test:		test-scan test-print good-print
		./test-print > test-print.txt
		./good-print > good-print.txt

test-scan:	sscanf.o vscanf.o stropen.o
		$(CC) sscanf.o vscanf.o stropen.o -o $@
		./$@

test-print:	snprintf.o vprintf.o stropen.o
		$(CC) snprintf.o vprintf.o stropen.o -o $@

good-print:	good-print.o
		$(CC) good-print.o -o $@

clean:
		rm -f *.[oa] *~ a.out

###
