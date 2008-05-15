ARFLAGS		?= r
RANLIB		?= @:

startup.o:	$(OS)/sources/runtime/$(ARCH)/$(STARTUP)
		$(CC) $(ASFLAGS) -c $< -o $@

libuos.a:	always
		@for m in $(MODULES); do\
			[ -d $$m ] || mkdir $$m;\
			$(MAKE) -f$(OS)/sources/module.mak -C$$m\
				OS=$(OS) TARGET=$(TARGET) MODULE=$$m;\
		done
		$(RANLIB) libuos.a

always:

.SUFFIXES:	.i .sre .hex .dis .com .adb .cpp .cxx .fl

.PHONY:		$(MODULES) depend

.c.o:
		@[ -d .deps ] || mkdir .deps
		$(CC) $(CFLAGS) $(DEPFLAGS) -c $<

.cpp.o:
		@[ -d .deps ] || mkdir .deps
		$(CXX) $(CFLAGS) $(DEPFLAGS) -c $<

.cxx.o:
		@[ -d .deps ] || mkdir .deps
		$(CXX) $(CFLAGS) $(DEPFLAGS) -c $<

.S.o:
		@[ -d .deps ] || mkdir .deps
		$(CC) $(ASFLAGS) $(DEPFLAGS) -c $<

.c.s:
		$(CC) $(CFLAGS) -S $<

.c.i:
		$(CC) $(CFLAGS) -E $< > $@

.c.out:
		@[ -d .deps ] || mkdir .deps
		$(CC) $(LDFLAGS) $(CFLAGS) $(DEPFLAGS) $< $(LIBS) -o $@

.o.out:
		$(CC) $(LDFLAGS) $< $(LIBS) -o $@

.c.com:
		$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

.o.com:
		$(CC) $(LDFLAGS) $< $(LIBS) -o $@

.fl.cxx .fl.h:
		fluid -c $<

.out.sre:
		$(OBJCOPY) -O srec $< $@
		@chmod -x $@

.out.hex:
		$(OBJCOPY) -O ihex $< $@
		@chmod -x $@

.out.dis:
		$(OBJDUMP) -D -S $? > $@

ifeq (.deps, $(wildcard .deps))
-include .deps/*.dep
endif
