#
# SAM-BA script for programming binary file into a flash memory.
#

DATAFLASH::SelectDataflash AT91C_SPI0_CS0

send_file "DataFlash AT45DB/DCB" [lindex $::argv 3] 0x8000 0
