#!/usr/bin/python
# -*- coding: utf-8 -*-
# Compute divisor and modulation values for MSP430 USART baud rate generator.

import sys, string, getopt

#
# Default options.
#
verbose = 0

def usage ():
	print """Usage:
	compute-baud-rate.py [-v] clk [baud...]
Options:
	-v           verbose mode
        clk          source clock frequency in Hz
        baud         required baud rate
"""

try:
	opts, args = getopt.getopt (sys.argv[1:], "hv", ["help"])
except getopt.GetoptError:
	usage()
	sys.exit(2)

for opt, arg in opts:
	if opt in ("-h", "--help"):
		usage()
		sys.exit()
	elif opt == '-v':
		verbose = 1

if len(args) < 1:
	usage()
	sys.exit(2)
hz = string.atoi (args[0])

if len(args) == 1:
	tab = [1200, 2400, 4800, 9600, 19200, 38400, 57600, 76800, 115200]
else:
	tab = []
	for arg in args[1:]:
		tab.append (string.atoi (arg))

#
# Sequentially compute M value.
#
def compute_modulation (hz, baud, div):
	a = baud * div
	b = hz
	m = 0
	maxerr = 0
	for i in range(8):
		if 2 * (b - a) >= baud:
			m = m | 1<<i
			a = a + baud
		err = abs (b - a)
		if err > maxerr:
			maxerr = err
		a = a + baud * div
		b = b + hz
	return (m, maxerr * 100.0 / hz)

print "HZ       Baud    UxBR    UxMCTL  Max error"
print "------------------------------------------"
for baud in tab:
	div = hz / baud
	(m, maxerr) = compute_modulation (hz, baud, div)
	print "%-08d" % hz,
	print "%-07d" % baud,
	print "0x%04x " % div,
	print "0x%02x   " % m,
	print "%.2f %%" % maxerr
