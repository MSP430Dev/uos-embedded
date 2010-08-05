# Daemon configuration
#~~~~~~~~~~~~~~~~~~~~~
telnet_port 2001
gdb_port 2000

# Interface
#~~~~~~~~~~
# Bitbang: homebrew adapter based on FT232R chip.
#interface ft232r
#ft232r_device_desc "FT232R USB UART"
#ft232r_vid_pid 0x0403 0x7777

# Olimex OpenOCD JTAG Tiny adapter.
interface ft2232
ft2232_device_desc "Olimex OpenOCD JTAG TINY"
ft2232_vid_pid 0x15ba 0x0004
ft2232_layout olimex-jtag
jtag_khz 1000
jtag_nsrst_delay 100
jtag_ntrst_delay 100
reset_config trst_and_srst srst_pulls_trst

# Target configuration
#~~~~~~~~~~~~~~~~~~~~~
# Milandr 1986BE91 - Cortex M3 with internal flash and RAM.
jtag newtap board cpu -irlen 4 -ircapture 0x1 -irmask 0xf -expected-id 0x4ba00477

target create board.cpu cortex_m3 -endian little -chain-position board.cpu
board.cpu configure -work-area-phys 0x20000000 -work-area-size 0x4000 -work-area-backup 0

#flash bank board.flash stm32x 0 0 0 0 board.cpu

#init #-----------------
#scan_chain
#targets
#flash banks
#poll

#halt
#reg

#mww 0x20000000 0x12345678
#mdw 0x20000000 8

#arm disassemble 0x08000000 0x100
#dump_image memory.bin 0x20000000 0x4000
#dump_image memory.bin 0x08000000 0x40000

#debug_level 3
#proc ocd_init {} {exit}
#echo {*** exit}
