TARGET		= $(CURDIR)
OS		= $(shell cd ../..; pwd)

include target.cfg

all:		startup.o libuos.a libdirectfb.a

clean:
		rm -rf *.[oasi] *~ $(MODULES)

include $(OS)/sources/rules.mak
