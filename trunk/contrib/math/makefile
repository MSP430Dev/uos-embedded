# Double precision Cephes library
# Makefile for unix or GCC

CC = gcc
CFLAGS = -g -O3 -Wall -fno-builtin
AR = ar
RANLIB = ranlib
INCS = mconf.h
AS = as

OBJS = atan.o const.o sin.o exp.o tan.o sqrt.o log.o mtherr.o polevl.o floor.o isnan.o

all: libmd.a

libmd.a: $(OBJS) $(INCS)
	$(AR) rv libmd.a $(OBJS)
	$(RANLIB) libmd.a

clean:
	rm -f *.o
	rm -f libmd.a

