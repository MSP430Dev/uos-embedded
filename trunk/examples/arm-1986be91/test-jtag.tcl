# Daemon configuration
#~~~~~~~~~~~~~~~~~~~~~
#telnet_port 2001
#gdb_port 2000

# Interface
#~~~~~~~~~~
# We use homebrew usbjtag-compatible schematics
# based on FT232R Breakout board from SparkFun.com.
interface ft232r
ft232r_device_desc "FT232R USB UART"
ft232r_vid_pid 0x0403 0x7777
#reset_config trst_and_srst srst_pulls_trst

# Target configuration
#~~~~~~~~~~~~~~~~~~~~~
# Milandr 1986BE91 - Cortex M3 with internal flash and RAM.
set _BOARD 1986BE91
jtag newtap $_BOARD cpu -irlen 4 -ircapture 0x1 -irmask 0xf -expected-id 0x4ba00477

scan_chain

target create $_BOARD.cpu cortex_m3 -endian little -chain-position $_BOARD.cpu
$_BOARD.cpu configure -work-area-phys 0x20000000 -work-area-size 0x4000 -work-area-backup 0

targets

flash bank $_BOARD.flash stm32x 0 0 0 0 $_BOARD.cpu

flash banks
flash list

init
poll
exit 0
