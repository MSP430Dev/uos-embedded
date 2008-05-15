CFLAGS		= -Wall -O -DHAVE_CONFIG_H -DUOS -DI386 -nostdinc -fno-builtin
CFLAGS	       += -I. -Iinclude -Ilib -Isrc -I/home/vak/Project/uos/sources
CFLAGS	       += -DMODULEDIR=\"\"
CFLAGS	       += -DBUILDTIME=\"$(shell date -u '+%Y-%m-%d\ %H:%M')\"

OBJS		= ./lib/direct/cpu_accel.o \
		  ./lib/direct/conf.o \
		  ./lib/direct/debug.o \
		  ./lib/direct/direct.o \
		  ./lib/direct/hash.o \
		  ./lib/direct/interface.o \
		  ./lib/direct/list.o \
		  ./lib/direct/mem.o \
		  ./lib/direct/memcpy.o \
		  ./lib/direct/messages.o \
		  ./lib/direct/modules.o \
		  ./lib/direct/system.o \
		  ./lib/direct/trace.o \
		  ./lib/direct/tree.o \
		  ./lib/direct/thread.o \
		  ./lib/direct/utf8.o \
		  ./lib/direct/util.o \
		  ./lib/direct/clock.o \
		  ./lib/fusion/arena.o \
		  ./lib/fusion/call.o \
		  ./lib/fusion/conf.o \
		  ./lib/fusion/fusion.o \
		  ./lib/fusion/lock.o \
		  ./lib/fusion/fusion_shmalloc.o \
		  ./lib/fusion/object.o \
		  ./lib/fusion/property.o \
		  ./lib/fusion/reactor.o \
		  ./lib/fusion/ref.o \
		  ./lib/fusion/vector.o \
		  ./src/core/surfacemanager.o \
		  ./src/core/layer_region.o \
		  ./src/core/colorhash.o \
		  ./src/core/core.o \
		  ./src/core/core_parts.o \
		  ./src/core/fonts.o \
		  ./src/core/gfxcard.o \
		  ./src/core/input.o \
		  ./src/core/layers.o \
		  ./src/core/wm.o \
		  ./src/core/layer_context.o \
		  ./src/core/layer_control.o \
		  ./src/core/palette.o \
		  ./src/core/screen.o \
		  ./src/core/screens.o \
		  ./src/core/state.o \
		  ./src/core/surfaces.o \
		  ./src/core/system.o \
		  ./src/core/windows.o \
		  ./src/core/windowstack.o \
		  ./src/core/clipboard.o \
		  ./src/display/idirectfbpalette.o \
		  ./src/display/idirectfbsurface.o \
		  ./src/display/idirectfbscreen.o \
		  ./src/display/idirectfbsurface_layer.o \
		  ./src/display/idirectfbsurface_window.o \
		  ./src/display/idirectfbdisplaylayer.o \
		  ./src/gfx/generic/generic.o \
		  ./src/gfx/clip.o \
		  ./src/gfx/util.o \
		  ./src/gfx/convert.o \
		  ./src/input/idirectfbinputbuffer.o \
		  ./src/input/idirectfbinputdevice.o \
		  ./src/media/idirectfbdatabuffer.o \
		  ./src/media/idirectfbfont.o \
		  ./src/media/idirectfbdatabuffer_memory.o \
		  ./src/media/idirectfbdatabuffer_streamed.o \
		  ./src/media/idirectfbimageprovider.o \
		  ./src/misc/conf.o \
		  ./src/misc/util.o \
		  ./src/misc/gfx_util.o \
		  ./src/windows/idirectfbwindow.o \
		  ./src/directfb.o \
		  ./src/idirectfb.o \
		  ./wm/default/default.o \
		  ./interfaces/IDirectFBFont/idirectfbfont_default.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/getbits.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/getblk.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/gethdr.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/getpic.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/getvlc.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/idct.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/idctref.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/motion.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/recon.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/systems.o \
		  ./interfaces/IDirectFBImageProvider/mpeg2/store.o \
		  ./interfaces/IDirectFBImageProvider/idirectfbimageprovider_mpeg2.o \
		  ./interfaces/IDirectFBImageProvider/idirectfbimageprovider_gif.o
# Files not implemented yet
#		  ./src/media/idirectfbdatabuffer_file.o

all:		$(OBJS)

clean:
		rm -f $(OBJS) *~
depend:
		@cp Makefile Makefile~~
		(sed '/^###/,$$d' Makefile;\
		echo '### DO NOT DELETE THIS LINE';\
		$(CC) -MM $(CFLAGS) `find . -name \*.c`;\
		echo '# DEPENDENCIES MUST END AT END OF FILE';\
		echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY';\
		echo '# see make depend above') > Makefile~ &&\
		mv Makefile~ Makefile

### DO NOT DELETE THIS LINE
cpu_accel.o: lib/direct/cpu_accel.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h signal.h setjmp.h \
  lib/direct/cpu_accel.h include/dfb_types.h sys/types.h \
  lib/direct/debug.h lib/direct/build.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
conf.o: lib/direct/conf.c config.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h
debug.o: lib/direct/debug.c config.h lib/direct/build.h stdarg.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h string.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h signal.h lib/direct/clock.h \
  sys/time.h lib/direct/debug.h stdio.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/direct/list.h \
  lib/direct/thread.h lib/direct/trace.h
direct.o: lib/direct/direct.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/debug.h \
  lib/direct/build.h stdio.h errno.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h lib/direct/direct.h lib/direct/signals.h
hash.o: lib/direct/hash.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h lib/direct/debug.h \
  lib/direct/build.h stdio.h errno.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/hash.h \
  lib/direct/mem.h string.h
interface.o: lib/direct/interface.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h dirent.h dlfcn.h \
  stdio.h stdlib.h /home/vak/Project/uos/sources/mem/mem.h errno.h \
  string.h lib/direct/debug.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h /home/vak/Project/uos/sources/timer/timer.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/interface.h \
  lib/direct/mem.h lib/direct/list.h lib/direct/memcpy.h \
  lib/direct/trace.h
list.o: lib/direct/list.c config.h stddef.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h lib/direct/list.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h /home/vak/Project/uos/sources/timer/timer.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  stdio.h errno.h lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
mem.o: lib/direct/mem.c config.h lib/direct/build.h
memcpy.o: lib/direct/memcpy.c config.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h time.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h string.h include/dfb_types.h \
  sys/types.h lib/direct/conf.h lib/direct/types.h include/directfb.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/cpu_accel.h \
  lib/direct/debug.h lib/direct/build.h stdio.h errno.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/mem.h \
  lib/direct/memcpy.h
messages.o: lib/direct/messages.c config.h stdarg.h string.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h lib/direct/build.h \
  lib/direct/messages.h errno.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/trace.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
modules.o: lib/direct/modules.c config.h sys/types.h dirent.h string.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/debug.h lib/direct/build.h errno.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/mem.h \
  stdlib.h /home/vak/Project/uos/sources/mem/mem.h lib/direct/modules.h \
  lib/direct/list.h
signals.o: lib/direct/signals.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h signal.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/timer/timer.h lib/direct/clock.h \
  sys/time.h lib/direct/conf.h lib/direct/types.h include/dfb_types.h \
  sys/types.h include/directfb.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/debug.h lib/direct/build.h errno.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/list.h \
  lib/direct/mem.h string.h lib/direct/signals.h lib/direct/trace.h
system.o: lib/direct/system.c config.h errno.h unistd.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h lib/direct/build.h \
  lib/direct/system.h sys/types.h asm/page.h
trace.o: lib/direct/trace.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h sys/types.h \
  sys/stat.h sys/mman.h unistd.h \
  /home/vak/Project/uos/sources/timer/timer.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h stdio.h fcntl.h string.h \
  lib/direct/build.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h lib/direct/debug.h errno.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/memcpy.h \
  lib/direct/thread.h lib/direct/trace.h
tree.o: lib/direct/tree.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h lib/direct/mem.h \
  lib/direct/build.h string.h lib/direct/tree.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h
thread.o: lib/direct/thread.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h signal.h sched.h \
  sys/time.h /home/vak/Project/uos/sources/timer/timer.h sys/resource.h \
  lib/direct/debug.h lib/direct/build.h stdio.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/list.h \
  lib/direct/mem.h stdlib.h /home/vak/Project/uos/sources/mem/mem.h \
  string.h lib/direct/signals.h lib/direct/thread.h
utf8.o: lib/direct/utf8.c config.h lib/direct/utf8.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h
util.o: lib/direct/util.c config.h string.h fcntl.h errno.h \
  lib/direct/messages.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
clock.o: lib/direct/clock.c config.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h time.h lib/direct/clock.h \
  lib/direct/debug.h lib/direct/build.h stdio.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
dtest.o: lib/direct/dtest.c lib/direct/cpu_accel.h include/dfb_types.h \
  sys/types.h lib/direct/debug.h lib/direct/build.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/direct.h \
  lib/direct/mem.h stdlib.h /home/vak/Project/uos/sources/mem/mem.h \
  string.h lib/direct/memcpy.h lib/direct/trace.h
arena.o: lib/fusion/arena.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h sys/types.h \
  pthread.h lib/direct/debug.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/list.h \
  lib/direct/mem.h string.h lib/fusion/build.h lib/fusion/types.h \
  lib/fusion/lock.h lib/fusion/ref.h lib/fusion/call.h lib/fusion/arena.h \
  lib/fusion/shmalloc.h stddef.h lib/fusion/fusion_internal.h sys/param.h \
  lib/fusion/fusion.h
call.o: lib/fusion/call.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h lib/fusion/build.h \
  lib/direct/debug.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/types.h lib/fusion/call.h lib/fusion/fusion_internal.h \
  sys/param.h string.h lib/direct/list.h lib/fusion/fusion.h \
  lib/fusion/lock.h
conf.o: lib/fusion/conf.c config.h stddef.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h lib/fusion/conf.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h
fusion.o: lib/fusion/fusion.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h signal.h errno.h time.h \
  fcntl.h sys/types.h sys/time.h sys/stat.h sys/ioctl.h \
  lib/direct/clock.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h lib/direct/types.h include/dfb_types.h \
  include/directfb.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/direct/direct.h lib/direct/mem.h string.h lib/direct/signals.h \
  lib/direct/thread.h lib/direct/trace.h lib/fusion/build.h \
  lib/fusion/types.h lib/fusion/fusion_internal.h sys/param.h \
  lib/direct/list.h lib/fusion/fusion.h lib/fusion/lock.h \
  lib/fusion/shmalloc.h stddef.h
lock.o: lib/fusion/lock.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h signal.h \
  lib/direct/debug.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/build.h lib/fusion/types.h lib/fusion/lock.h \
  lib/fusion/fusion_internal.h sys/param.h string.h lib/direct/list.h \
  lib/fusion/fusion.h
fusion_shmalloc.o: lib/fusion/fusion_shmalloc.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h lib/direct/build.h \
  lib/direct/debug.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/direct/memcpy.h \
  string.h lib/fusion/build.h lib/fusion/shmalloc.h stddef.h \
  lib/fusion/types.h
object.o: lib/fusion/object.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/debug.h \
  lib/direct/build.h stdio.h errno.h lib/direct/conf.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h lib/direct/thread.h lib/fusion/build.h \
  lib/fusion/object.h lib/fusion/types.h lib/fusion/lock.h \
  lib/direct/list.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/reactor.h lib/fusion/shmalloc.h stddef.h \
  /home/vak/Project/uos/sources/mem/mem.h lib/fusion/fusion_internal.h \
  sys/param.h string.h lib/fusion/fusion.h
property.o: lib/fusion/property.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h signal.h \
  lib/fusion/build.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/types.h lib/fusion/property.h lib/fusion/fusion_internal.h \
  sys/param.h string.h lib/direct/list.h lib/fusion/fusion.h \
  lib/fusion/lock.h
reactor.o: lib/fusion/reactor.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h signal.h pthread.h \
  lib/fusion/build.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h \
  lib/direct/list.h lib/direct/mem.h string.h lib/direct/thread.h \
  lib/direct/trace.h lib/fusion/types.h lib/fusion/lock.h \
  lib/fusion/shmalloc.h stddef.h lib/fusion/reactor.h \
  lib/fusion/fusion_internal.h sys/param.h lib/fusion/fusion.h
ref.o: lib/fusion/ref.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h lib/fusion/build.h \
  lib/direct/debug.h lib/direct/build.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h include/directfb_keyboard.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/types.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/fusion_internal.h sys/param.h string.h lib/direct/list.h \
  lib/fusion/fusion.h lib/fusion/lock.h signal.h
vector.o: lib/fusion/vector.c config.h lib/direct/debug.h \
  lib/direct/build.h stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  sys/time.h /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/memcpy.h \
  stdlib.h /home/vak/Project/uos/sources/mem/mem.h string.h \
  lib/fusion/object.h lib/fusion/types.h lib/fusion/lock.h \
  lib/direct/list.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/reactor.h lib/fusion/shmalloc.h stddef.h lib/fusion/build.h \
  lib/fusion/vector.h limits.h
surfacemanager.o: src/core/surfacemanager.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h pthread.h \
  lib/direct/list.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h \
  lib/direct/debug.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h lib/fusion/shmalloc.h stddef.h lib/fusion/build.h \
  lib/fusion/types.h src/core/core.h lib/fusion/lock.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/reactor.h src/core/coretypes.h src/core/coredefs.h \
  src/core/gfxcard.h lib/direct/modules.h src/core/surfaces.h \
  lib/direct/serial.h src/core/surfacemanager.h src/core/system.h \
  lib/direct/mem.h lib/direct/memcpy.h
layer_region.o: src/core/layer_region.c include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/coredefs.h src/core/coretypes.h lib/direct/debug.h \
  lib/direct/build.h stdio.h errno.h lib/direct/conf.h lib/direct/types.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/fusion/build.h lib/fusion/types.h src/core/core.h lib/fusion/lock.h \
  lib/fusion/object.h lib/direct/list.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/layer_context.h src/core/layer_control.h \
  src/core/layer_region.h src/core/layers.h src/core/layers_internal.h \
  lib/fusion/property.h lib/fusion/vector.h limits.h src/core/state.h \
  lib/direct/serial.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/surfaces.h src/gfx/util.h
colorhash.o: src/core/colorhash.c config.h lib/fusion/arena.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/direct/build.h lib/fusion/build.h src/core/core.h lib/fusion/lock.h \
  pthread.h /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h stdio.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/core_parts.h src/core/palette.h \
  src/core/colorhash.h src/misc/util.h src/gfx/convert.h
core.o: src/core/core.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h dirent.h dlfcn.h errno.h \
  pthread.h lib/fusion/fusion.h lib/fusion/types.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h lib/fusion/arena.h lib/direct/list.h \
  lib/direct/debug.h lib/direct/build.h stdio.h lib/direct/conf.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h lib/fusion/shmalloc.h stddef.h lib/fusion/build.h \
  src/core/coredefs.h src/core/coretypes.h src/core/core.h \
  lib/fusion/lock.h lib/fusion/object.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/core_parts.h \
  src/core/layer_context.h src/core/layer_region.h src/core/layers.h \
  src/core/palette.h src/core/surfaces.h lib/direct/serial.h \
  src/core/gfxcard.h lib/direct/modules.h src/core/system.h \
  src/core/windows.h lib/direct/direct.h lib/direct/interface.h \
  lib/direct/mem.h string.h lib/direct/memcpy.h lib/direct/signals.h \
  lib/direct/thread.h src/misc/conf.h signal.h src/misc/util.h
core_parts.o: src/core/core_parts.c config.h lib/fusion/arena.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/direct/build.h lib/fusion/build.h src/core/coredefs.h \
  src/core/coretypes.h src/core/core.h lib/fusion/lock.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h stdio.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/core_parts.h \
  lib/direct/mem.h stdlib.h string.h
fonts.o: src/core/fonts.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h string.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  stdio.h lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/fonts.h src/core/state.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/surfaces.h \
  lib/direct/mem.h lib/direct/tree.h src/misc/conf.h signal.h
gfxcard.o: src/core/gfxcard.c config.h string.h lib/fusion/fusion.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/direct/build.h lib/fusion/build.h lib/fusion/arena.h \
  lib/fusion/property.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h src/core/core.h \
  lib/fusion/lock.h lib/direct/messages.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  stdio.h lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/core_parts.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/fonts.h src/core/state.h \
  lib/direct/serial.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/palette.h src/core/surfaces.h src/core/surfacemanager.h \
  src/core/system.h src/gfx/clip.h src/gfx/util.h lib/direct/mem.h \
  stdlib.h lib/direct/tree.h lib/direct/utf8.h src/misc/conf.h signal.h
input.o: src/core/input.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h sys/types.h sys/stat.h \
  fcntl.h lib/fusion/shmalloc.h stddef.h lib/direct/build.h \
  lib/fusion/build.h lib/fusion/types.h lib/direct/types.h \
  include/dfb_types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h lib/fusion/reactor.h lib/direct/list.h \
  lib/direct/debug.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/lock.h lib/fusion/arena.h src/core/core.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  src/core/coretypes.h src/core/coredefs.h src/core/core_parts.h \
  src/core/gfxcard.h lib/direct/modules.h src/core/surfaces.h \
  lib/direct/serial.h src/core/system.h src/core/layer_context.h \
  src/core/layer_control.h src/core/layer_region.h src/core/layers.h \
  src/core/input.h src/core/windows.h src/core/windows_internal.h \
  lib/direct/mem.h string.h lib/direct/memcpy.h lib/direct/trace.h \
  src/misc/conf.h signal.h src/misc/util.h src/gfx/convert.h
layers.o: src/core/layers.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h \
  lib/fusion/shmalloc.h stddef.h lib/direct/build.h lib/fusion/build.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h \
  lib/fusion/arena.h lib/fusion/property.h pthread.h src/core/core.h \
  lib/fusion/lock.h lib/direct/messages.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/core_parts.h src/core/input.h \
  lib/direct/modules.h src/core/gfxcard.h src/core/layer_context.h \
  src/core/layer_control.h src/core/layer_region.h src/core/layers.h \
  src/core/state.h lib/direct/serial.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/palette.h src/core/system.h \
  src/core/surfacemanager.h src/core/windows.h src/gfx/convert.h \
  src/gfx/util.h src/core/surfaces.h lib/direct/mem.h string.h \
  lib/direct/memcpy.h src/misc/conf.h signal.h src/core/layers_internal.h \
  lib/fusion/vector.h limits.h src/core/screens_internal.h \
  src/core/screens.h
wm.o: src/core/wm.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h string.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/list.h lib/direct/types.h lib/direct/debug.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/fusion/build.h lib/fusion/types.h src/core/coredefs.h \
  src/core/coretypes.h src/core/core_parts.h lib/fusion/lock.h \
  src/core/windows_internal.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/windows.h lib/fusion/object.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/wm.h src/misc/conf.h \
  signal.h src/misc/util.h lib/direct/mem.h stdlib.h
layer_context.o: src/core/layer_context.c string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/coredefs.h src/core/coretypes.h src/core/core.h \
  lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  stdio.h lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/layer_context.h \
  src/core/layer_control.h src/core/layer_region.h src/core/layers.h \
  src/core/screen.h src/core/surfaces.h lib/direct/serial.h \
  src/core/gfxcard.h lib/direct/modules.h src/core/system.h \
  src/core/windows.h src/core/windowstack.h src/core/wm.h \
  src/core/layers_internal.h lib/fusion/property.h lib/fusion/vector.h \
  limits.h src/core/state.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/windows_internal.h
layer_control.o: src/core/layer_control.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h errno.h \
  lib/fusion/shmalloc.h stddef.h lib/direct/build.h lib/fusion/build.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h \
  lib/fusion/arena.h lib/fusion/property.h pthread.h src/core/core.h \
  lib/fusion/lock.h lib/direct/messages.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/core_parts.h src/core/input.h \
  lib/direct/modules.h src/core/gfxcard.h src/core/layer_context.h \
  src/core/layer_control.h src/core/layer_region.h src/core/layers.h \
  src/core/screen.h src/core/state.h lib/direct/serial.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/palette.h \
  src/core/system.h src/core/surfacemanager.h src/core/windows.h \
  src/gfx/convert.h src/gfx/util.h src/core/surfaces.h lib/direct/mem.h \
  string.h lib/direct/memcpy.h src/core/layers_internal.h \
  lib/fusion/vector.h limits.h
palette.o: src/core/palette.c config.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/direct/build.h lib/fusion/build.h lib/fusion/types.h \
  lib/direct/types.h src/core/coredefs.h src/core/coretypes.h \
  src/core/core.h lib/fusion/lock.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h stdio.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/surfaces.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/palette.h src/core/colorhash.h src/misc/util.h
screen.o: src/core/screen.c config.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/debug.h lib/direct/build.h stdio.h errno.h lib/direct/conf.h \
  lib/direct/types.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h src/core/screen.h \
  src/core/coretypes.h src/core/screens_internal.h lib/fusion/lock.h \
  lib/fusion/types.h src/core/screens.h
screens.o: src/core/screens.c config.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/fusion/shmalloc.h stddef.h /home/vak/Project/uos/sources/mem/mem.h \
  lib/direct/build.h lib/fusion/build.h lib/fusion/types.h \
  lib/direct/types.h src/core/core_parts.h lib/fusion/lock.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h errno.h lib/direct/conf.h sys/signal.h \
  src/core/coretypes.h src/core/coredefs.h src/core/screen.h \
  src/core/screens.h src/core/screens_internal.h lib/direct/debug.h \
  stdio.h lib/direct/system.h lib/direct/util.h lib/direct/mem.h stdlib.h \
  string.h
state.o: src/core/state.c config.h string.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/debug.h lib/direct/build.h stdio.h errno.h lib/direct/conf.h \
  sys/signal.h lib/direct/messages.h lib/direct/system.h \
  lib/direct/util.h lib/fusion/types.h lib/fusion/lock.h src/core/core.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  src/core/coretypes.h src/core/coredefs.h src/core/state.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/surfaces.h \
  lib/direct/mem.h stdlib.h /home/vak/Project/uos/sources/mem/mem.h
surfaces.o: src/core/surfaces.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h sys/types.h \
  sys/stat.h fcntl.h pthread.h include/directfb.h include/dfb_types.h \
  sys/time.h include/directfb_keyboard.h include/directfb_strings.h \
  lib/direct/debug.h lib/direct/build.h errno.h lib/direct/conf.h \
  lib/direct/types.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/direct/mem.h \
  lib/direct/memcpy.h lib/fusion/shmalloc.h stddef.h lib/fusion/build.h \
  lib/fusion/types.h src/core/core.h lib/fusion/lock.h \
  lib/fusion/object.h lib/direct/list.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/layers.h src/core/layers_internal.h lib/fusion/property.h \
  lib/fusion/vector.h limits.h src/core/layer_region.h src/core/state.h \
  lib/direct/serial.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/palette.h src/core/surfaces.h src/core/surfacemanager.h \
  src/core/system.h src/core/windows_internal.h src/core/windows.h \
  src/gfx/convert.h src/gfx/util.h
system.o: src/core/system.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h string.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/list.h lib/direct/types.h lib/direct/debug.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h src/core/coredefs.h \
  src/core/coretypes.h src/core/core_parts.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/layers.h src/core/palette.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/reactor.h src/core/surfaces.h lib/direct/serial.h \
  src/core/gfxcard.h lib/direct/modules.h src/core/system.h \
  src/gfx/convert.h src/misc/conf.h signal.h lib/direct/mem.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h
windows.o: src/core/windows.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h pthread.h \
  lib/fusion/shmalloc.h stddef.h lib/direct/build.h lib/fusion/build.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h \
  src/core/coredefs.h src/core/coretypes.h src/core/core.h \
  lib/fusion/lock.h lib/direct/messages.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/layers.h \
  src/core/layer_context.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/input.h src/core/palette.h src/core/state.h \
  lib/direct/serial.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/system.h src/core/windows.h src/core/windowstack.h \
  src/core/wm.h src/misc/conf.h signal.h lib/direct/mem.h \
  lib/direct/trace.h src/gfx/util.h src/core/surfaces.h \
  src/core/layers_internal.h lib/fusion/property.h lib/fusion/vector.h \
  limits.h src/core/layer_region.h src/core/windows_internal.h
windowstack.o: src/core/windowstack.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h pthread.h \
  lib/fusion/shmalloc.h stddef.h lib/direct/build.h lib/fusion/build.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h include/directfb_keyboard.h \
  src/core/coredefs.h src/core/coretypes.h src/core/core.h \
  lib/fusion/lock.h lib/direct/messages.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/layer_context.h \
  src/core/layer_region.h src/core/layers.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/input.h src/core/palette.h \
  src/core/state.h lib/direct/serial.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/system.h src/core/windows.h \
  src/core/windowstack.h src/core/wm.h src/misc/conf.h signal.h \
  lib/direct/mem.h src/gfx/convert.h src/gfx/util.h src/core/surfaces.h \
  src/core/layers_internal.h lib/fusion/property.h lib/fusion/vector.h \
  limits.h src/core/windows_internal.h
clipboard.o: src/core/clipboard.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h string.h \
  lib/fusion/shmalloc.h stddef.h lib/direct/build.h lib/fusion/build.h \
  lib/fusion/types.h lib/direct/types.h include/dfb_types.h sys/types.h \
  include/directfb.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/lock.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  lib/direct/messages.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h stdio.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/core_parts.h src/core/clipboard.h \
  lib/direct/memcpy.h
idirectfbpalette.o: src/display/idirectfbpalette.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h math.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  include/directfb_keyboard.h src/core/surfaces.h lib/direct/list.h \
  lib/direct/types.h lib/direct/debug.h lib/direct/build.h errno.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/direct/serial.h \
  lib/fusion/object.h lib/fusion/types.h lib/fusion/lock.h \
  lib/fusion/ref.h lib/fusion/call.h lib/fusion/reactor.h \
  src/core/coretypes.h src/core/coredefs.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/palette.h lib/direct/interface.h \
  lib/direct/mem.h lib/direct/memcpy.h src/gfx/convert.h src/gfx/util.h \
  src/display/idirectfbpalette.h
idirectfbsurface.o: src/display/idirectfbsurface.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h alloca.h math.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  include/directfb_keyboard.h src/core/core.h lib/fusion/types.h \
  lib/direct/types.h lib/fusion/lock.h pthread.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/fonts.h src/core/state.h lib/direct/serial.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/palette.h \
  src/core/surfaces.h src/core/surfacemanager.h src/media/idirectfbfont.h \
  src/display/idirectfbsurface.h src/display/idirectfbpalette.h \
  lib/direct/interface.h lib/direct/mem.h lib/direct/memcpy.h \
  src/gfx/convert.h src/gfx/util.h
idirectfbscreen.o: src/display/idirectfbscreen.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/core/layers.h src/core/coretypes.h src/core/screen.h \
  lib/direct/interface.h lib/direct/build.h lib/direct/types.h \
  lib/direct/debug.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/direct/mem.h src/display/idirectfbscreen.h
idirectfbsurface_layer.o: src/display/idirectfbsurface_layer.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  pthread.h lib/direct/messages.h lib/direct/build.h errno.h \
  lib/direct/conf.h sys/signal.h lib/fusion/object.h lib/direct/list.h \
  lib/direct/debug.h lib/direct/system.h lib/direct/util.h \
  lib/fusion/ref.h lib/fusion/call.h lib/fusion/reactor.h \
  src/core/coretypes.h src/core/coredefs.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/state.h lib/direct/serial.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/layers.h \
  src/core/layer_region.h src/core/surfaces.h src/core/system.h \
  src/display/idirectfbsurface.h src/display/idirectfbsurface_layer.h \
  lib/direct/interface.h lib/direct/mem.h src/gfx/util.h
idirectfbsurface_window.o: src/display/idirectfbsurface_window.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  pthread.h lib/direct/messages.h lib/direct/build.h errno.h \
  lib/direct/conf.h sys/signal.h lib/fusion/object.h lib/direct/list.h \
  lib/direct/debug.h lib/direct/system.h lib/direct/util.h \
  lib/fusion/ref.h lib/fusion/call.h lib/fusion/reactor.h \
  src/core/coretypes.h src/core/coredefs.h src/core/fonts.h \
  src/core/state.h lib/direct/serial.h src/core/gfxcard.h \
  lib/direct/modules.h src/gfx/generic/generic.h src/misc/util.h \
  src/core/layer_region.h src/core/layers.h src/core/surfaces.h \
  src/core/windows.h src/core/windows_internal.h \
  src/display/idirectfbsurface.h src/display/idirectfbsurface_window.h \
  lib/direct/interface.h lib/direct/mem.h src/gfx/util.h
idirectfbdisplaylayer.o: src/display/idirectfbdisplaylayer.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  pthread.h lib/direct/messages.h lib/direct/build.h errno.h \
  lib/direct/conf.h sys/signal.h lib/fusion/object.h lib/direct/list.h \
  lib/direct/debug.h lib/direct/system.h lib/direct/util.h \
  lib/fusion/ref.h lib/fusion/call.h lib/fusion/reactor.h \
  src/core/coretypes.h src/core/coredefs.h src/core/surfaces.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/layers.h src/core/layer_context.h src/core/layer_control.h \
  src/core/layer_region.h src/core/state.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/windows.h src/core/windowstack.h \
  src/windows/idirectfbwindow.h src/gfx/convert.h lib/direct/interface.h \
  lib/direct/mem.h src/display/idirectfbdisplaylayer.h \
  src/display/idirectfbscreen.h src/display/idirectfbsurface.h \
  src/display/idirectfbsurface_layer.h
generic.o: src/gfx/generic/generic.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h \
  include/dfb_types.h sys/types.h pthread.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h src/core/core.h lib/fusion/types.h \
  lib/direct/types.h lib/fusion/lock.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/gfxcard.h lib/direct/modules.h \
  src/core/state.h lib/direct/serial.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/surfacemanager.h src/core/palette.h \
  src/misc/gfx_util.h src/misc/conf.h signal.h lib/direct/mem.h \
  lib/direct/memcpy.h lib/direct/cpu_accel.h src/gfx/convert.h \
  src/gfx/util.h src/core/surfaces.h src/gfx/generic/yuvtbl.h
clip.o: src/gfx/clip.c config.h include/directfb.h include/dfb_types.h \
  sys/types.h sys/time.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/util.h lib/direct/types.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h src/misc/util.h \
  lib/direct/debug.h stdio.h lib/direct/system.h src/gfx/clip.h
util.o: src/gfx/util.c config.h pthread.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/util.h lib/direct/types.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  src/core/coretypes.h src/core/state.h lib/direct/serial.h \
  lib/direct/debug.h stdio.h lib/direct/system.h lib/fusion/reactor.h \
  lib/direct/list.h lib/fusion/types.h lib/fusion/lock.h \
  src/core/coredefs.h src/core/gfxcard.h lib/direct/modules.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/surfacemanager.h \
  src/core/core.h lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  src/gfx/util.h src/core/surfaces.h
convert.o: src/gfx/convert.c config.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/gfx/convert.h lib/direct/util.h lib/direct/types.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h pthread.h /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h
idirectfbinputdevice.o: src/input/idirectfbinputdevice.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h \
  lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h sys/time.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  errno.h lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coredefs.h src/core/coretypes.h \
  src/core/input.h lib/direct/modules.h src/misc/util.h \
  lib/direct/interface.h lib/direct/mem.h \
  src/input/idirectfbinputdevice.h src/input/idirectfbinputbuffer.h
idirectfbinputbuffer.o: src/input/idirectfbinputbuffer.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h sys/time.h \
  sys/socket.h pthread.h lib/fusion/reactor.h lib/direct/list.h \
  lib/direct/types.h include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coredefs.h src/core/coretypes.h \
  src/core/input.h lib/direct/modules.h src/core/windows.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h src/misc/util.h \
  lib/direct/interface.h lib/direct/mem.h lib/direct/thread.h \
  src/input/idirectfbinputbuffer.h
idirectfbdatabuffer.o: src/media/idirectfbdatabuffer.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h sys/time.h \
  pthread.h lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coredefs.h src/core/coretypes.h \
  src/core/input.h lib/direct/modules.h src/core/windows.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h src/misc/util.h \
  lib/direct/interface.h lib/direct/mem.h src/media/idirectfbdatabuffer.h \
  src/media/idirectfbimageprovider.h
idirectfbfont.o: src/media/idirectfbfont.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/core/coretypes.h src/core/fonts.h pthread.h lib/fusion/lock.h \
  lib/fusion/types.h lib/direct/types.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  src/core/state.h lib/direct/serial.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/reactor.h \
  lib/direct/list.h src/core/coredefs.h src/core/gfxcard.h \
  lib/direct/modules.h src/gfx/generic/generic.h src/misc/util.h \
  src/media/idirectfbfont.h lib/direct/interface.h lib/direct/mem.h \
  lib/direct/tree.h lib/direct/utf8.h
idirectfbdatabuffer_file.o: src/media/idirectfbdatabuffer_file.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h \
  sys/types.h sys/time.h sys/stat.h fcntl.h pthread.h \
  lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h include/directfb.h include/directfb_keyboard.h \
  lib/direct/debug.h lib/direct/build.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h \
  lib/fusion/types.h lib/fusion/lock.h src/core/coredefs.h \
  src/core/coretypes.h src/core/input.h lib/direct/modules.h \
  src/core/windows.h lib/fusion/object.h lib/fusion/ref.h \
  lib/fusion/call.h src/misc/util.h lib/direct/interface.h \
  lib/direct/mem.h src/media/idirectfbdatabuffer.h
idirectfbdatabuffer_memory.o: src/media/idirectfbdatabuffer_memory.c \
  config.h stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h sys/time.h \
  pthread.h lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coredefs.h src/core/coretypes.h \
  src/core/input.h lib/direct/modules.h src/core/windows.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  lib/direct/interface.h lib/direct/mem.h lib/direct/memcpy.h \
  src/media/idirectfbdatabuffer.h
idirectfbdatabuffer_streamed.o: src/media/idirectfbdatabuffer_streamed.c \
  config.h stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h sys/time.h \
  pthread.h lib/fusion/reactor.h lib/direct/list.h lib/direct/types.h \
  include/dfb_types.h sys/types.h include/directfb.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coredefs.h src/core/coretypes.h \
  lib/direct/interface.h lib/direct/mem.h lib/direct/memcpy.h \
  src/media/idirectfbdatabuffer.h
idirectfbimageprovider.o: src/media/idirectfbimageprovider.c config.h \
  stddef.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  lib/direct/interface.h lib/direct/build.h lib/direct/types.h \
  lib/direct/debug.h stdio.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/direct/mem.h \
  stdlib.h string.h src/media/idirectfbimageprovider.h \
  src/media/idirectfbdatabuffer.h
conf.o: src/misc/conf.c config.h unistd.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h stdio.h string.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  include/directfb_keyboard.h src/core/coretypes.h src/core/surfaces.h \
  lib/direct/list.h lib/direct/types.h lib/direct/debug.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/direct/serial.h lib/fusion/object.h lib/fusion/types.h \
  lib/fusion/lock.h lib/fusion/ref.h lib/fusion/call.h \
  lib/fusion/reactor.h src/core/coredefs.h src/core/gfxcard.h \
  lib/direct/modules.h src/core/layers.h lib/fusion/conf.h \
  src/misc/conf.h signal.h lib/direct/mem.h lib/direct/memcpy.h
util.o: src/misc/util.c config.h string.h sys/types.h sys/stat.h fcntl.h \
  sys/time.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h time.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  lib/direct/types.h include/dfb_types.h include/directfb.h \
  include/directfb_keyboard.h sys/signal.h lib/direct/util.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h src/misc/util.h \
  lib/direct/debug.h stdio.h lib/direct/system.h
gfx_util.o: src/misc/gfx_util.c config.h stdlib.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h stdio.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/palette.h src/core/surfaces.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  lib/direct/memcpy.h string.h lib/direct/mem.h src/misc/util.h \
  src/misc/gfx_util.h src/gfx/convert.h
idirectfbwindow.o: src/windows/idirectfbwindow.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h sched.h sys/time.h \
  errno.h pthread.h include/directfb.h include/dfb_types.h sys/types.h \
  include/directfb_keyboard.h src/idirectfb.h src/core/coretypes.h \
  src/core/core.h lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h \
  lib/direct/messages.h lib/direct/build.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coredefs.h \
  src/core/layers.h src/core/palette.h src/core/state.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/surfaces.h \
  src/core/windows.h src/core/windowstack.h src/core/windows_internal.h \
  src/display/idirectfbsurface.h src/display/idirectfbsurface_window.h \
  src/input/idirectfbinputbuffer.h src/core/input.h \
  lib/direct/interface.h lib/direct/mem.h src/gfx/convert.h \
  src/windows/idirectfbwindow.h
directfb.o: src/directfb.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  include/directfb_version.h src/misc/conf.h signal.h lib/fusion/types.h \
  lib/direct/types.h src/core/core.h lib/fusion/lock.h pthread.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/input.h lib/direct/modules.h \
  src/core/layer_context.h src/core/layer_control.h src/core/layers.h \
  src/core/state.h lib/direct/serial.h src/core/gfxcard.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/surfaces.h \
  src/core/windows.h src/core/windowstack.h src/gfx/convert.h \
  lib/direct/interface.h lib/direct/mem.h src/display/idirectfbsurface.h \
  src/idirectfb.h
idirectfb.o: src/idirectfb.c config.h stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h sys/types.h sys/stat.h \
  fcntl.h string.h include/directfb.h include/dfb_types.h sys/time.h \
  include/directfb_keyboard.h include/directfb_version.h src/core/core.h \
  lib/fusion/types.h lib/direct/types.h lib/fusion/lock.h pthread.h \
  lib/direct/messages.h lib/direct/build.h errno.h lib/direct/conf.h \
  sys/signal.h lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/clipboard.h src/core/state.h \
  lib/direct/serial.h src/core/gfxcard.h lib/direct/modules.h \
  src/gfx/generic/generic.h src/misc/util.h src/core/input.h \
  src/core/layer_context.h src/core/layer_control.h \
  src/core/layer_region.h src/core/layers.h src/core/palette.h \
  src/core/screen.h src/core/screens.h src/core/surfaces.h \
  src/core/surfacemanager.h src/core/system.h src/core/windows.h \
  src/core/windows_internal.h src/core/windowstack.h \
  src/display/idirectfbpalette.h src/display/idirectfbscreen.h \
  src/display/idirectfbsurface.h src/display/idirectfbsurface_layer.h \
  src/display/idirectfbsurface_window.h \
  src/display/idirectfbdisplaylayer.h src/input/idirectfbinputbuffer.h \
  src/input/idirectfbinputdevice.h src/media/idirectfbfont.h \
  src/media/idirectfbimageprovider.h src/media/idirectfbvideoprovider.h \
  src/media/idirectfbdatabuffer.h src/idirectfb.h src/gfx/convert.h \
  src/misc/conf.h signal.h lib/direct/interface.h lib/direct/mem.h \
  lib/direct/memcpy.h
default.o: wm/default/default.c config.h unistd.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h stdio.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h errno.h limits.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  include/directfb_keyboard.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h lib/direct/types.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/direct/mem.h \
  string.h lib/direct/memcpy.h lib/fusion/shmalloc.h stddef.h \
  lib/fusion/build.h lib/fusion/types.h lib/fusion/vector.h \
  src/core/coredefs.h src/core/coretypes.h src/core/gfxcard.h \
  lib/direct/modules.h lib/direct/list.h lib/fusion/lock.h \
  src/core/layer_context.h lib/fusion/object.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/layer_region.h \
  src/core/layers.h src/core/layers_internal.h lib/fusion/property.h \
  src/core/state.h lib/direct/serial.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/surfaces.h src/core/palette.h \
  src/core/windows.h src/core/windows_internal.h src/core/windowstack.h \
  src/core/wm.h src/misc/conf.h signal.h src/core/wm_module.h
idirectfbfont_default.o:  \
 interfaces/IDirectFBFont/idirectfbfont_default.c config.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  /home/vak/Project/uos/sources/timer/timer.h include/directfb_keyboard.h \
  src/core/fonts.h pthread.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h lib/fusion/lock.h \
  lib/fusion/types.h lib/direct/types.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  src/core/coretypes.h src/core/state.h lib/direct/serial.h \
  lib/direct/debug.h stdio.h lib/direct/system.h lib/direct/util.h \
  lib/fusion/reactor.h lib/direct/list.h src/core/coredefs.h \
  src/core/gfxcard.h lib/direct/modules.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/surfaces.h lib/fusion/object.h \
  lib/fusion/ref.h lib/fusion/call.h src/core/surfacemanager.h \
  src/core/core.h src/gfx/convert.h src/media/idirectfbfont.h \
  lib/direct/tree.h lib/direct/interface.h lib/direct/mem.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h string.h lib/direct/memcpy.h \
  lib/direct/utf8.h interfaces/IDirectFBFont/default_font.h \
  lib/direct/interface_implementation.h
getbits.o: interfaces/IDirectFBImageProvider/mpeg2/getbits.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
getblk.o: interfaces/IDirectFBImageProvider/mpeg2/getblk.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
gethdr.o: interfaces/IDirectFBImageProvider/mpeg2/gethdr.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
getpic.o: interfaces/IDirectFBImageProvider/mpeg2/getpic.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
getvlc.o: interfaces/IDirectFBImageProvider/mpeg2/getvlc.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h interfaces/IDirectFBImageProvider/mpeg2/getvlc.h
idct.o: interfaces/IDirectFBImageProvider/mpeg2/idct.c \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
idctref.o: interfaces/IDirectFBImageProvider/mpeg2/idctref.c math.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
motion.o: interfaces/IDirectFBImageProvider/mpeg2/motion.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
mpeg2dec.o: interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h ctype.h fcntl.h sys/types.h \
  unistd.h /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h
recon.o: interfaces/IDirectFBImageProvider/mpeg2/recon.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
systems.o: interfaces/IDirectFBImageProvider/mpeg2/systems.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h \
  interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
store.o: interfaces/IDirectFBImageProvider/mpeg2/store.c stdio.h \
  /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h fcntl.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h lib/direct/mem.h \
  lib/direct/build.h interfaces/IDirectFBImageProvider/mpeg2/global.h \
  interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h include/dfb_types.h \
  sys/types.h
idirectfbimageprovider_mpeg2.o:  \
 interfaces/IDirectFBImageProvider/idirectfbimageprovider_mpeg2.c \
  config.h errno.h stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h include/directfb.h \
  include/dfb_types.h sys/types.h sys/time.h include/directfb_keyboard.h \
  src/display/idirectfbsurface.h lib/direct/types.h lib/fusion/reactor.h \
  lib/direct/list.h lib/direct/debug.h lib/direct/build.h \
  lib/direct/conf.h sys/signal.h lib/direct/messages.h \
  lib/direct/system.h lib/direct/util.h pthread.h lib/fusion/types.h \
  lib/fusion/lock.h src/core/coretypes.h src/core/state.h \
  lib/direct/serial.h src/core/coredefs.h src/core/gfxcard.h \
  lib/direct/modules.h src/gfx/generic/generic.h src/misc/util.h \
  src/media/idirectfbimageprovider.h src/core/layers.h src/core/palette.h \
  lib/fusion/object.h lib/fusion/ref.h lib/fusion/call.h \
  src/core/surfaces.h src/misc/gfx_util.h lib/direct/interface.h \
  lib/direct/mem.h interfaces/IDirectFBImageProvider/mpeg2/mpeg2dec.h \
  lib/direct/interface_implementation.h
idirectfbimageprovider_gif.o:  \
 interfaces/IDirectFBImageProvider/idirectfbimageprovider_gif.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h errno.h \
  include/directfb.h include/dfb_types.h sys/types.h sys/time.h \
  include/directfb_keyboard.h src/display/idirectfbsurface.h \
  lib/direct/types.h lib/fusion/reactor.h lib/direct/list.h \
  lib/direct/debug.h lib/direct/build.h lib/direct/conf.h sys/signal.h \
  lib/direct/messages.h lib/direct/system.h lib/direct/util.h pthread.h \
  lib/fusion/types.h lib/fusion/lock.h src/core/coretypes.h \
  src/core/state.h lib/direct/serial.h src/core/coredefs.h \
  src/core/gfxcard.h lib/direct/modules.h src/gfx/generic/generic.h \
  src/misc/util.h src/media/idirectfbimageprovider.h src/core/layers.h \
  src/core/surfaces.h lib/fusion/object.h lib/fusion/ref.h \
  lib/fusion/call.h src/misc/gfx_util.h lib/direct/interface.h \
  lib/direct/mem.h lib/direct/memcpy.h \
  lib/direct/interface_implementation.h
idirectfbvideoprovider_v4l.o:  \
 interfaces/IDirectFBVideoProvider/idirectfbvideoprovider_v4l.c config.h \
  stdio.h /home/vak/Project/uos/sources/runtime/lib.h \
  /home/vak/Project/uos/sources/runtime/byteorder.h stdarg.h \
  /home/vak/Project/uos/sources/runtime/i386/types.h \
  /home/vak/Project/uos/sources/runtime/i386/assert.h \
  /home/vak/Project/uos/sources/runtime/i386/string.h \
  /home/vak/Project/uos/sources/runtime/i386/stdlib.h \
  /home/vak/Project/uos/sources/runtime/i386/io.h \
  /home/vak/Project/uos/sources/runtime/ctype.h stdlib.h \
  /home/vak/Project/uos/sources/mem/mem.h \
  /home/vak/Project/uos/sources/kernel/uos.h unistd.h \
  /home/vak/Project/uos/sources/kernel/internal.h \
  /home/vak/Project/uos/sources/kernel/i386/machdep.h \
  /home/vak/Project/uos/sources/timer/timer.h string.h sys/time.h \
  sys/types.h sys/select.h sys/mman.h fcntl.h sys/ioctl.h \
  include/directfb.h include/dfb_types.h include/directfb_keyboard.h \
  src/media/idirectfbvideoprovider.h src/core/core.h lib/fusion/types.h \
  lib/direct/types.h lib/fusion/lock.h pthread.h lib/direct/messages.h \
  lib/direct/build.h errno.h lib/direct/conf.h sys/signal.h \
  lib/fusion/object.h lib/direct/list.h lib/direct/debug.h \
  lib/direct/system.h lib/direct/util.h lib/fusion/ref.h \
  lib/fusion/call.h lib/fusion/reactor.h src/core/coretypes.h \
  src/core/coredefs.h src/core/state.h lib/direct/serial.h \
  src/core/gfxcard.h lib/direct/modules.h src/gfx/generic/generic.h \
  src/misc/util.h src/core/layers.h src/core/layer_control.h \
  src/core/surfaces.h src/core/surfacemanager.h \
  src/display/idirectfbsurface.h lib/direct/interface.h lib/direct/mem.h \
  lib/direct/memcpy.h lib/direct/thread.h \
  lib/direct/interface_implementation.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
