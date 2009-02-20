#!/usr/bin/python
# -*- coding: utf-8 -*-
# Compute divisor and modulation values for MSP430 USART baud rate generator.

import sys, string, getopt

#
# Default options.
#
verbose = 0

def usage ():
	print """doxy2wiki.py: Create wiki page from Doxygen XML data.

Usage:
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
	tab = [9600, 19200, 38400, 57600, 76800, 115200]
else:
	tab = []
	for arg in args[1:]:
		tab.append (string.atoi (arg))

#
# Compute sum of J+1 modulation bits.
#
def sum_m (m, j):
	sum = 0
	for i in range(j+1):
		if i < 8:
			bit = (m >> i) & 1
		else:
			bit = (m >> (i-8)) & 1
		if bit != 0:
			sum = sum + 1
	return sum

#
# Compute transmit error for bit N.
#
def tx_error (hz, baud, div, m, j):
	err = ((j+1) * div + sum_m (m, j)) * baud * 1.0 / hz - j - 1
	return err * 100

#
# Compute receive error for bit N.
#
def rcv_error (hz, baud, div, m, j):
	if j == 0:
		err = ((int(div/2) + (m&1)) * 2) * baud * 1.0 / hz - j - 1
	else:
		err = (int(div/2) * 2 + (m&1) + j*div + sum_m (m, j)) * baud * 1.0 / hz - j - 1
	return err * 100

#
# Sequentially compute M value.
#
def compute_modulation (hz, baud, div):
	m = 0
	maxerr = 0
	for i in range(8):
		err0 = tx_error (hz, baud, div, m, i)
		rerr0 = rcv_error (hz, baud, div, m, i)
		m = m | (1 << i)
		err1 = tx_error (hz, baud, div, m, i)
		rerr1 = rcv_error (hz, baud, div, m, i)
		#print i, "(%02x)" % (m & ~(1 << i)), "--", "%.2f" % err0, "%.2f" % err1, "%.2f" % rerr0, "%.2f" % rerr1
		err0 = abs (err0)
		err1 = abs (err1)
		if err0 <= err1:
			m = m & ~(1 << i)
			if err0 > maxerr:
				maxerr = err0
		else:
			if err1 > maxerr:
				maxerr = err1
	return (m, maxerr)

def print_bit_errors (hz, baud, div, m):
	print "Start bit error =", "%.2f" % tx_error (hz, baud, div, m, 0), "%.2f" % rcv_error (hz, baud, div, m, 0)
	for i in range(1,9):
		print "Data bit D%d error =" % (i-1), "%.2f" % tx_error (hz, baud, div, m, i), "%.2f" % rcv_error (hz, baud, div, m, i)
	print "Stop bit error =", "%.2f" % tx_error (hz, baud, div, m, 9), "%.2f" % rcv_error (hz, baud, div, m, 9)

print "HZ       Baud    UxBR    UxMCTL  Max error"
print "------------------------------------------"
for baud in tab:
	div = hz / baud
	(m, maxerr) = compute_modulation (hz, baud, div)
	print "%-08d" % hz,
	print "%-07d" % baud,
	print "0x%04x " % div,
	print "0x%02x   " % m,
	print "%.2f" % maxerr, "%"
	#print_bit_errors ()
