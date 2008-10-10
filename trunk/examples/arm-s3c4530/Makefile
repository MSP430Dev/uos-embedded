TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)
include target.cfg

all:		startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.sre *.lst *.dis .deps $(MODULES)

include $(OS)/sources/rules.mak
