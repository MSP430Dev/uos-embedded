#
# This is a generic makefile for compiling uOS modules.
# It is called with the following parameters:
# OS - absolute path of uOS sources
# TARGET - absolute path of target compilation directory
# MODULE - name of the compiled module
#
ARFLAGS = r
include $(TARGET)/target.cfg

# Find the module sources in uos/sources or in contrib directory.
CONTRIB ?= $(OS)/contrib
ifeq ($(OS)/sources/$(MODULE), $(wildcard $(OS)/sources/$(MODULE)))
    MODULEDIR = $(OS)/sources/$(MODULE)
else
    MODULEDIR = $(CONTRIB)/$(MODULE)
endif

# Get module parameters
include $(MODULEDIR)/module.cfg

clean::
		rm -rf *.[oai] *~ .deps

include $(OS)/sources/rules.mak
###
