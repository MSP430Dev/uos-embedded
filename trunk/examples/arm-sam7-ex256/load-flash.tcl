#
# SAM-BA script for programming binary file into a flash memory.
#
send_file Flash [lindex $::argv 3] 0x100000 0

# Boot from Flash (GPNVM2)
FLASH::ScriptGPNMV 4
