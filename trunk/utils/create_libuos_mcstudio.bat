@echo off

set SRC_ROOT=..\sources
set DST_ROOT=libuos

set CUR_SRC_DIR=%SRC_ROOT%\buf
set CUR_DST_DIR=%DST_ROOT%\buf
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\buf-chksum.c %CUR_DST_DIR%\buf-chksum.c
copy /Y %CUR_SRC_DIR%\buf-chksum32.c %CUR_DST_DIR%\buf-chksum32.c
copy /Y %CUR_SRC_DIR%\buf-clen.c %CUR_DST_DIR%\buf-clen.c
copy /Y %CUR_SRC_DIR%\buf-copy.c %CUR_DST_DIR%\buf-copy.c
copy /Y %CUR_SRC_DIR%\buf-print.c %CUR_DST_DIR%\buf-print.c
copy /Y %CUR_SRC_DIR%\buf-prio.c %CUR_DST_DIR%\buf-prio.c
copy /Y %CUR_SRC_DIR%\buf-prio.h %CUR_DST_DIR%\buf-prio.h
copy /Y %CUR_SRC_DIR%\buf-queue.c %CUR_DST_DIR%\buf-queue.c
copy /Y %CUR_SRC_DIR%\buf-queue.h %CUR_DST_DIR%\buf-queue.h
copy /Y %CUR_SRC_DIR%\buf.c %CUR_DST_DIR%\buf.c
copy /Y %CUR_SRC_DIR%\buf.h %CUR_DST_DIR%\buf.h

set CUR_SRC_DIR=%SRC_ROOT%\crc
set CUR_DST_DIR=%DST_ROOT%\crc
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\crc-rmon.c %CUR_DST_DIR%\crc-rmon.c
copy /Y %CUR_SRC_DIR%\crc-rmon.h %CUR_DST_DIR%\crc-rmon.h
copy /Y %CUR_SRC_DIR%\crc16-ccitt.c %CUR_DST_DIR%\crc16-ccitt.c
copy /Y %CUR_SRC_DIR%\crc16-ccitt.h %CUR_DST_DIR%\crc16-ccitt.h
copy /Y %CUR_SRC_DIR%\crc16-inet.c %CUR_DST_DIR%\crc16-inet.c
copy /Y %CUR_SRC_DIR%\crc16-inet.h %CUR_DST_DIR%\crc16-inet.h
copy /Y %CUR_SRC_DIR%\crc16.c %CUR_DST_DIR%\crc16.c
copy /Y %CUR_SRC_DIR%\crc16.h %CUR_DST_DIR%\crc16.h
copy /Y %CUR_SRC_DIR%\crc32-vak.c %CUR_DST_DIR%\crc32-vak.c
copy /Y %CUR_SRC_DIR%\crc32-vak.h %CUR_DST_DIR%\crc32-vak.h
copy /Y %CUR_SRC_DIR%\crc8-atm.c %CUR_DST_DIR%\crc8-atm.c
copy /Y %CUR_SRC_DIR%\crc8-atm.h %CUR_DST_DIR%\crc8-atm.h
copy /Y %CUR_SRC_DIR%\crc8-dallas.c %CUR_DST_DIR%\crc8-dallas.c
copy /Y %CUR_SRC_DIR%\crc8-dallas.h %CUR_DST_DIR%\crc8-dallas.h

set CUR_SRC_DIR=%SRC_ROOT%\elvees
set CUR_DST_DIR=%DST_ROOT%\elvees
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\eth-mcb.c %CUR_DST_DIR%\eth-mcb.c
copy /Y %CUR_SRC_DIR%\eth-mcb.h %CUR_DST_DIR%\eth-mcb.h
copy /Y %CUR_SRC_DIR%\eth.c %CUR_DST_DIR%\eth.c
copy /Y %CUR_SRC_DIR%\eth.h %CUR_DST_DIR%\eth.h
copy /Y %CUR_SRC_DIR%\i2c.c %CUR_DST_DIR%\i2c.c
copy /Y %CUR_SRC_DIR%\i2c.h %CUR_DST_DIR%\i2c.h
copy /Y %CUR_SRC_DIR%\ks8721bl.h %CUR_DST_DIR%\ks8721bl.h
copy /Y %CUR_SRC_DIR%\lport.c %CUR_DST_DIR%\lport.c
copy /Y %CUR_SRC_DIR%\lport.h %CUR_DST_DIR%\lport.h
copy /Y %CUR_SRC_DIR%\lxt971a.h %CUR_DST_DIR%\lxt971a.h
copy /Y %CUR_SRC_DIR%\mcb-01.h %CUR_DST_DIR%\mcb-01.h
copy /Y %CUR_SRC_DIR%\mcb-03.h %CUR_DST_DIR%\mcb-03.h
copy /Y %CUR_SRC_DIR%\mcb.c %CUR_DST_DIR%\mcb.c
copy /Y %CUR_SRC_DIR%\mct-02.h %CUR_DST_DIR%\mct-02.h
copy /Y %CUR_SRC_DIR%\pci.c %CUR_DST_DIR%\pci.c
copy /Y %CUR_SRC_DIR%\pci.h %CUR_DST_DIR%\pci.h
copy /Y %CUR_SRC_DIR%\queue.c %CUR_DST_DIR%\queue.c
copy /Y %CUR_SRC_DIR%\queue.h %CUR_DST_DIR%\queue.h
copy /Y %CUR_SRC_DIR%\spi.c %CUR_DST_DIR%\spi.c
copy /Y %CUR_SRC_DIR%\spi.h %CUR_DST_DIR%\spi.h
copy /Y %CUR_SRC_DIR%\spw-mcb.c %CUR_DST_DIR%\spw-mcb.c
copy /Y %CUR_SRC_DIR%\spw-mcb.h %CUR_DST_DIR%\spw-mcb.h
copy /Y %CUR_SRC_DIR%\uartx.c %CUR_DST_DIR%\uartx.c
copy /Y %CUR_SRC_DIR%\uartx.h %CUR_DST_DIR%\uartx.h

set CUR_SRC_DIR=%SRC_ROOT%\flash
set CUR_DST_DIR=%DST_ROOT%\flash
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\at45dbxx.c %CUR_DST_DIR%\at45dbxx.c
copy /Y %CUR_SRC_DIR%\at45dbxx.h %CUR_DST_DIR%\at45dbxx.h
copy /Y %CUR_SRC_DIR%\flash-interface.h %CUR_DST_DIR%\flash-interface.h
copy /Y %CUR_SRC_DIR%\m25pxx.c %CUR_DST_DIR%\m25pxx.c
copy /Y %CUR_SRC_DIR%\m25pxx.h %CUR_DST_DIR%\m25pxx.h
copy /Y %CUR_SRC_DIR%\sd-private.h %CUR_DST_DIR%\sd-private.h
copy /Y %CUR_SRC_DIR%\sdhc-spi.c %CUR_DST_DIR%\sdhc-spi.c
copy /Y %CUR_SRC_DIR%\sdhc-spi.h %CUR_DST_DIR%\sdhc-spi.h

set CUR_SRC_DIR=%SRC_ROOT%\iic
set CUR_DST_DIR=%DST_ROOT%\iic
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\miic.h %CUR_DST_DIR%\miic.h

set CUR_SRC_DIR=%SRC_ROOT%\kernel
set CUR_DST_DIR=%DST_ROOT%\kernel
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\halt.c %CUR_DST_DIR%\halt.c
copy /Y %CUR_SRC_DIR%\iattach.c %CUR_DST_DIR%\iattach.c
copy /Y %CUR_SRC_DIR%\internal.h %CUR_DST_DIR%\internal.h
copy /Y %CUR_SRC_DIR%\irelease.c %CUR_DST_DIR%\irelease.c
copy /Y %CUR_SRC_DIR%\itake.c %CUR_DST_DIR%\itake.c
copy /Y %CUR_SRC_DIR%\main.c %CUR_DST_DIR%\main.c
copy /Y %CUR_SRC_DIR%\mgroup.c %CUR_DST_DIR%\mgroup.c
copy /Y %CUR_SRC_DIR%\msignal.c %CUR_DST_DIR%\msignal.c
copy /Y %CUR_SRC_DIR%\mtry.c %CUR_DST_DIR%\mtry.c
copy /Y %CUR_SRC_DIR%\mutex.c %CUR_DST_DIR%\mutex.c
copy /Y %CUR_SRC_DIR%\tcreate.c %CUR_DST_DIR%\tcreate.c
copy /Y %CUR_SRC_DIR%\tdebug.c %CUR_DST_DIR%\tdebug.c
copy /Y %CUR_SRC_DIR%\tdelete.c %CUR_DST_DIR%\tdelete.c
copy /Y %CUR_SRC_DIR%\texit.c %CUR_DST_DIR%\texit.c
copy /Y %CUR_SRC_DIR%\tfpucontrol.c %CUR_DST_DIR%\tfpucontrol.c
copy /Y %CUR_SRC_DIR%\tname.c %CUR_DST_DIR%\tname.c
copy /Y %CUR_SRC_DIR%\tprio.c %CUR_DST_DIR%\tprio.c
copy /Y %CUR_SRC_DIR%\tprivate.c %CUR_DST_DIR%\tprivate.c
copy /Y %CUR_SRC_DIR%\tsetprio.c %CUR_DST_DIR%\tsetprio.c
copy /Y %CUR_SRC_DIR%\tsetprivate.c %CUR_DST_DIR%\tsetprivate.c
copy /Y %CUR_SRC_DIR%\tstack.c %CUR_DST_DIR%\tstack.c
copy /Y %CUR_SRC_DIR%\twait.c %CUR_DST_DIR%\twait.c
copy /Y %CUR_SRC_DIR%\tyield.c %CUR_DST_DIR%\tyield.c
copy /Y %CUR_SRC_DIR%\uos.h %CUR_DST_DIR%\uos.h

set CUR_SRC_DIR=%SRC_ROOT%\kernel\mips
set CUR_DST_DIR=%DST_ROOT%\kernel\mips
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\machdep.c %CUR_DST_DIR%\machdep.c
copy /Y %CUR_SRC_DIR%\machdep.h %CUR_DST_DIR%\machdep.h

set CUR_SRC_DIR=%SRC_ROOT%\max3421e
set CUR_DST_DIR=%DST_ROOT%\max3421e
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\max3421e.h %CUR_DST_DIR%\max3421e.h
copy /Y %CUR_SRC_DIR%\usbdevhal.c %CUR_DST_DIR%\usbdevhal.c
copy /Y %CUR_SRC_DIR%\usbdevhal.h %CUR_DST_DIR%\usbdevhal.h

set CUR_SRC_DIR=%SRC_ROOT%\mem
set CUR_DST_DIR=%DST_ROOT%\mem
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\alloc-must.c %CUR_DST_DIR%\alloc-must.c
copy /Y %CUR_SRC_DIR%\check32.c %CUR_DST_DIR%\check32.c
copy /Y %CUR_SRC_DIR%\mem-queue.h %CUR_DST_DIR%\mem-queue.h
copy /Y %CUR_SRC_DIR%\mem.c %CUR_DST_DIR%\mem.c
copy /Y %CUR_SRC_DIR%\mem.h %CUR_DST_DIR%\mem.h
copy /Y %CUR_SRC_DIR%\strdup.c %CUR_DST_DIR%\strdup.c
copy /Y %CUR_SRC_DIR%\strndup.c %CUR_DST_DIR%\strndup.c

set CUR_SRC_DIR=%SRC_ROOT%\net
set CUR_DST_DIR=%DST_ROOT%\net
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\arp.c %CUR_DST_DIR%\arp.c
copy /Y %CUR_SRC_DIR%\arp.h %CUR_DST_DIR%\arp.h
copy /Y %CUR_SRC_DIR%\bridge.c %CUR_DST_DIR%\bridge.c
copy /Y %CUR_SRC_DIR%\bridge.h %CUR_DST_DIR%\bridge.h
copy /Y %CUR_SRC_DIR%\icmp.c %CUR_DST_DIR%\icmp.c
copy /Y %CUR_SRC_DIR%\ip.c %CUR_DST_DIR%\ip.c
copy /Y %CUR_SRC_DIR%\ip.h %CUR_DST_DIR%\ip.h
copy /Y %CUR_SRC_DIR%\netif.c %CUR_DST_DIR%\netif.c
copy /Y %CUR_SRC_DIR%\netif.h %CUR_DST_DIR%\netif.h
copy /Y %CUR_SRC_DIR%\route.c %CUR_DST_DIR%\route.c
copy /Y %CUR_SRC_DIR%\route.h %CUR_DST_DIR%\route.h
copy /Y %CUR_SRC_DIR%\tcp-in.c %CUR_DST_DIR%\tcp-in.c
copy /Y %CUR_SRC_DIR%\tcp-out.c %CUR_DST_DIR%\tcp-out.c
copy /Y %CUR_SRC_DIR%\tcp-stream.c %CUR_DST_DIR%\tcp-stream.c
copy /Y %CUR_SRC_DIR%\tcp-user.c %CUR_DST_DIR%\tcp-user.c
copy /Y %CUR_SRC_DIR%\tcp.c %CUR_DST_DIR%\tcp.c
copy /Y %CUR_SRC_DIR%\tcp.h %CUR_DST_DIR%\tcp.h
copy /Y %CUR_SRC_DIR%\telnet.c %CUR_DST_DIR%\telnet.c
copy /Y %CUR_SRC_DIR%\telnet.h %CUR_DST_DIR%\telnet.h
copy /Y %CUR_SRC_DIR%\udp.c %CUR_DST_DIR%\udp.c
copy /Y %CUR_SRC_DIR%\udp.h %CUR_DST_DIR%\udp.h

set CUR_SRC_DIR=%SRC_ROOT%\random
set CUR_DST_DIR=%DST_ROOT%\random
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\rand15.c %CUR_DST_DIR%\rand15.c
copy /Y %CUR_SRC_DIR%\rand15.h %CUR_DST_DIR%\rand15.h

set CUR_SRC_DIR=%SRC_ROOT%\runtime
set CUR_DST_DIR=%DST_ROOT%\runtime
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\arch.h %CUR_DST_DIR%\arch.h
copy /Y %CUR_SRC_DIR%\assert.c %CUR_DST_DIR%\assert.c
copy /Y %CUR_SRC_DIR%\assert.h %CUR_DST_DIR%\assert.h
copy /Y %CUR_SRC_DIR%\byteorder.h %CUR_DST_DIR%\byteorder.h
copy /Y %CUR_SRC_DIR%\ctype.c %CUR_DST_DIR%\ctype.c
copy /Y %CUR_SRC_DIR%\ctype.h %CUR_DST_DIR%\ctype.h
copy /Y %CUR_SRC_DIR%\debug-dump.c %CUR_DST_DIR%\debug-dump.c
copy /Y %CUR_SRC_DIR%\debug-printf.c %CUR_DST_DIR%\debug-printf.c
copy /Y %CUR_SRC_DIR%\halt.c %CUR_DST_DIR%\halt.c
copy /Y %CUR_SRC_DIR%\lib.h %CUR_DST_DIR%\lib.h
copy /Y %CUR_SRC_DIR%\list.h %CUR_DST_DIR%\list.h
copy /Y %CUR_SRC_DIR%\math.h %CUR_DST_DIR%\math.h
copy /Y %CUR_SRC_DIR%\memchr-fast.c %CUR_DST_DIR%\memchr-fast.c
copy /Y %CUR_SRC_DIR%\memcmp-fast.c %CUR_DST_DIR%\memcmp-fast.c
copy /Y %CUR_SRC_DIR%\memcpy-fast.c %CUR_DST_DIR%\memcpy-fast.c
copy /Y %CUR_SRC_DIR%\memmove-fast.c %CUR_DST_DIR%\memmove-fast.c
copy /Y %CUR_SRC_DIR%\memset-fast.c %CUR_DST_DIR%\memset-fast.c
copy /Y %CUR_SRC_DIR%\strcasecmp.c %CUR_DST_DIR%\strcasecmp.c
copy /Y %CUR_SRC_DIR%\strcat-fast.c %CUR_DST_DIR%\strcat-fast.c
copy /Y %CUR_SRC_DIR%\strchr-fast.c %CUR_DST_DIR%\strchr-fast.c
copy /Y %CUR_SRC_DIR%\strcmp-fast.c %CUR_DST_DIR%\strcmp-fast.c
copy /Y %CUR_SRC_DIR%\strcpy-fast.c %CUR_DST_DIR%\strcpy-fast.c
copy /Y %CUR_SRC_DIR%\strcspn.c %CUR_DST_DIR%\strcspn.c
copy /Y %CUR_SRC_DIR%\strlen-fast.c %CUR_DST_DIR%\strlen-fast.c
copy /Y %CUR_SRC_DIR%\strmatch.c %CUR_DST_DIR%\strmatch.c
copy /Y %CUR_SRC_DIR%\strncasecmp.c %CUR_DST_DIR%\strncasecmp.c
copy /Y %CUR_SRC_DIR%\strncat-fast.c %CUR_DST_DIR%\strncat-fast.c
copy /Y %CUR_SRC_DIR%\strncmp-fast.c %CUR_DST_DIR%\strncmp-fast.c
copy /Y %CUR_SRC_DIR%\strncpy-fast.c %CUR_DST_DIR%\strncpy-fast.c
copy /Y %CUR_SRC_DIR%\strnlen.c %CUR_DST_DIR%\strnlen.c
copy /Y %CUR_SRC_DIR%\strrchr-fast.c %CUR_DST_DIR%\strrchr-fast.c
copy /Y %CUR_SRC_DIR%\strrchr-fast.c %CUR_DST_DIR%\strrchr-fast.c
copy /Y %CUR_SRC_DIR%\strspn.c %CUR_DST_DIR%\strspn.c
copy /Y %CUR_SRC_DIR%\strstr.c %CUR_DST_DIR%\strstr.c
copy /Y %CUR_SRC_DIR%\strtod.c %CUR_DST_DIR%\strtod.c
copy /Y %CUR_SRC_DIR%\strtoul.c %CUR_DST_DIR%\strtoul.c
copy /Y %CUR_SRC_DIR%\time.h %CUR_DST_DIR%\time.h
copy /Y %CUR_SRC_DIR%\tz-parse.c %CUR_DST_DIR%\tz-parse.c
copy /Y %CUR_SRC_DIR%\tz-time.c %CUR_DST_DIR%\tz-time.c

set CUR_SRC_DIR=%SRC_ROOT%\runtime\math
set CUR_DST_DIR=%DST_ROOT%\runtime\math
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\isinf.c %CUR_DST_DIR%\isinf.c
copy /Y %CUR_SRC_DIR%\isnan.c %CUR_DST_DIR%\isnan.c
copy /Y %CUR_SRC_DIR%\modf.c %CUR_DST_DIR%\modf.c
copy /Y %CUR_SRC_DIR%\pow.c %CUR_DST_DIR%\pow.c
copy /Y %CUR_SRC_DIR%\private.h %CUR_DST_DIR%\private.h

md %DST_ROOT%\runtime\mips
set CUR_SRC_DIR=%SRC_ROOT%\runtime\mips
set CUR_DST_DIR=%DST_ROOT%\runtime\mips
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\debug.c %CUR_DST_DIR%\debug.c
copy /Y %CUR_SRC_DIR%\init-elvees.c %CUR_DST_DIR%\init-elvees.c
copy /Y %CUR_SRC_DIR%\io-elvees.h %CUR_DST_DIR%\io-elvees.h
copy /Y %CUR_SRC_DIR%\io-mc0226.h %CUR_DST_DIR%\io-mc0226.h
copy /Y %CUR_SRC_DIR%\io-mc0428.h %CUR_DST_DIR%\io-mc0428.h
copy /Y %CUR_SRC_DIR%\io-mc24.h %CUR_DST_DIR%\io-mc24.h
copy /Y %CUR_SRC_DIR%\io-mc24r2.h %CUR_DST_DIR%\io-mc24r2.h
copy /Y %CUR_SRC_DIR%\io-mct02.h %CUR_DST_DIR%\io-mct02.h
copy /Y %CUR_SRC_DIR%\io-nvcom01.h %CUR_DST_DIR%\io-nvcom01.h
copy /Y %CUR_SRC_DIR%\io-nvcom02.h %CUR_DST_DIR%\io-nvcom02.h
copy /Y %CUR_SRC_DIR%\io.h %CUR_DST_DIR%\io.h
copy /Y %CUR_SRC_DIR%\mathinline.h %CUR_DST_DIR%\mathinline.h
copy /Y %CUR_SRC_DIR%\mdelay.c %CUR_DST_DIR%\mdelay.c
copy /Y %CUR_SRC_DIR%\mips-dump-stack.c %CUR_DST_DIR%\mips-dump-stack.c
copy /Y %CUR_SRC_DIR%\startup-mcstudio.S %CUR_DST_DIR%\startup-mcstudio.S
copy /Y %CUR_SRC_DIR%\stdlib.h %CUR_DST_DIR%\stdlib.h
copy /Y %CUR_SRC_DIR%\string.h %CUR_DST_DIR%\string.h
copy /Y %CUR_SRC_DIR%\types.h %CUR_DST_DIR%\types.h
copy /Y %CUR_SRC_DIR%\udelay.c %CUR_DST_DIR%\udelay.c

set CUR_SRC_DIR=%SRC_ROOT%\snmp
set CUR_DST_DIR=%DST_ROOT%\snmp
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\asn-cnt.c %CUR_DST_DIR%\asn-cnt.c
copy /Y %CUR_SRC_DIR%\asn-print.c %CUR_DST_DIR%\asn-print.c
copy /Y %CUR_SRC_DIR%\asn.c %CUR_DST_DIR%\asn.c
copy /Y %CUR_SRC_DIR%\asn.h %CUR_DST_DIR%\asn.h
copy /Y %CUR_SRC_DIR%\snmp-icmp.c %CUR_DST_DIR%\snmp-icmp.c
copy /Y %CUR_SRC_DIR%\snmp-icmp.h %CUR_DST_DIR%\snmp-icmp.h
copy /Y %CUR_SRC_DIR%\snmp-ip.c %CUR_DST_DIR%\snmp-ip.c
copy /Y %CUR_SRC_DIR%\snmp-ip.h %CUR_DST_DIR%\snmp-ip.h
copy /Y %CUR_SRC_DIR%\snmp-netif.c %CUR_DST_DIR%\snmp-netif.c
copy /Y %CUR_SRC_DIR%\snmp-netif.h %CUR_DST_DIR%\snmp-netif.h
copy /Y %CUR_SRC_DIR%\snmp-snmp.c %CUR_DST_DIR%\snmp-snmp.c
copy /Y %CUR_SRC_DIR%\snmp-snmp.h %CUR_DST_DIR%\snmp-snmp.h
copy /Y %CUR_SRC_DIR%\snmp-system.c %CUR_DST_DIR%\snmp-system.c
copy /Y %CUR_SRC_DIR%\snmp-system.h %CUR_DST_DIR%\snmp-system.h
copy /Y %CUR_SRC_DIR%\snmp-udp.c %CUR_DST_DIR%\snmp-udp.c
copy /Y %CUR_SRC_DIR%\snmp-udp.h %CUR_DST_DIR%\snmp-udp.h
copy /Y %CUR_SRC_DIR%\snmp-var.h %CUR_DST_DIR%\snmp-var.h
copy /Y %CUR_SRC_DIR%\snmp-vardecl.h %CUR_DST_DIR%\snmp-vardecl.h
copy /Y %CUR_SRC_DIR%\snmp-vardef.h %CUR_DST_DIR%\snmp-vardef.h
copy /Y %CUR_SRC_DIR%\snmp.c %CUR_DST_DIR%\snmp.c
copy /Y %CUR_SRC_DIR%\snmp.h %CUR_DST_DIR%\snmp.h
copy /Y %CUR_SRC_DIR%\trap-defer.c %CUR_DST_DIR%\trap-defer.c
copy /Y %CUR_SRC_DIR%\trap-defer.h %CUR_DST_DIR%\trap-defer.h
copy /Y %CUR_SRC_DIR%\trap-v2c.c %CUR_DST_DIR%\trap-v2c.c
copy /Y %CUR_SRC_DIR%\trap.c %CUR_DST_DIR%\trap.c

set CUR_SRC_DIR=%SRC_ROOT%\spi
set CUR_DST_DIR=%DST_ROOT%\spi
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\spi-master-interface.h %CUR_DST_DIR%\spi-master-interface.h

set CUR_SRC_DIR=%SRC_ROOT%\stream
set CUR_DST_DIR=%DST_ROOT%\stream
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\drain-input.c %CUR_DST_DIR%\drain-input.c
copy /Y %CUR_SRC_DIR%\gets.c %CUR_DST_DIR%\gets.c
copy /Y %CUR_SRC_DIR%\pipe.c %CUR_DST_DIR%\pipe.c
copy /Y %CUR_SRC_DIR%\pipe.h %CUR_DST_DIR%\pipe.h
copy /Y %CUR_SRC_DIR%\printf.c %CUR_DST_DIR%\printf.c
copy /Y %CUR_SRC_DIR%\puts.c %CUR_DST_DIR%\puts.c
copy /Y %CUR_SRC_DIR%\scanf.c %CUR_DST_DIR%\scanf.c
copy /Y %CUR_SRC_DIR%\snprintf.c %CUR_DST_DIR%\snprintf.c
copy /Y %CUR_SRC_DIR%\sscanf.c %CUR_DST_DIR%\sscanf.c
copy /Y %CUR_SRC_DIR%\stream.h %CUR_DST_DIR%\stream.h
copy /Y %CUR_SRC_DIR%\stropen.c %CUR_DST_DIR%\stropen.c
copy /Y %CUR_SRC_DIR%\vprintf.c %CUR_DST_DIR%\vprintf.c
copy /Y %CUR_SRC_DIR%\vscanf.c %CUR_DST_DIR%\vscanf.c
copy /Y %CUR_SRC_DIR%\vsnprintf.c %CUR_DST_DIR%\vsnprintf.c

set CUR_SRC_DIR=%SRC_ROOT%\timer
set CUR_DST_DIR=%DST_ROOT%\timer
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\timer.c %CUR_DST_DIR%\timer.c
copy /Y %CUR_SRC_DIR%\timer.h %CUR_DST_DIR%\timer.h

set CUR_SRC_DIR=%SRC_ROOT%\uart
set CUR_DST_DIR=%DST_ROOT%\uart
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\elvees.h %CUR_DST_DIR%\elvees.h
copy /Y %CUR_SRC_DIR%\slip.c %CUR_DST_DIR%\slip.c
copy /Y %CUR_SRC_DIR%\slip.h %CUR_DST_DIR%\slip.h
copy /Y %CUR_SRC_DIR%\uart.c %CUR_DST_DIR%\uart.c
copy /Y %CUR_SRC_DIR%\uart.h %CUR_DST_DIR%\uart.h

set CUR_SRC_DIR=%SRC_ROOT%\usb
set CUR_DST_DIR=%DST_ROOT%\usb
md %CUR_DST_DIR%
copy /Y %CUR_SRC_DIR%\hiddev.c %CUR_DST_DIR%\hiddev.c
copy /Y %CUR_SRC_DIR%\hiddev.h %CUR_DST_DIR%\hiddev.h
copy /Y %CUR_SRC_DIR%\hid_const.h %CUR_DST_DIR%\hid_const.h
copy /Y %CUR_SRC_DIR%\usbdev.c %CUR_DST_DIR%\usbdev.c
copy /Y %CUR_SRC_DIR%\usbdev.h %CUR_DST_DIR%\usbdev.h
copy /Y %CUR_SRC_DIR%\usbdev_dfl_settings.h %CUR_DST_DIR%\usbdev_dfl_settings.h
copy /Y %CUR_SRC_DIR%\usb_const.h %CUR_DST_DIR%\usb_const.h
copy /Y %CUR_SRC_DIR%\usb_struct.h %CUR_DST_DIR%\usb_struct.h
