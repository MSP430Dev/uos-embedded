#!/usr/bin/python
# -*- coding: utf-8 -*-
# Compute divisor and modulation values for MSP430 USCI baud rate generator.

import sys, string, getopt

#
# Default options.
#
verbose = 0

def usage ():
	print """Usage:
	compute-usci.py [-v] clk [baud...]
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

# Low-Frequency Baud Rate Mode Setting
# N = hz / baud
# UCBRSx = round( ( N − INT(N) ) * 8 )
def compute_low_freq (hz, baud):
	n = hz / baud
	best_n = n+1
	best_s = 0
	min_err = compute_low_freq_error (hz, baud, best_n, best_s)
	for s in range(8):
		err = compute_low_freq_error (hz, baud, n, s)
		#print "low freq s=%#02x error=%.2f%%" % (s, err)
		if err < min_err:
			min_err = err
			best_n = n
			best_s = s
	return (best_n, best_s, min_err)

def compute_low_freq_error (hz, baud, s):
	n = (hz + baud/2) / baud
	m = [0, 0x40, 0x44, 0x54, 0x55, 0x75, 0x77, 0x7f] [s]
	a = baud * n
	b = hz
	maxerr = 0
	for i in range(8):
		if m & 0x80:
			a = a + baud
		err = abs (b - a)
		if err > maxerr:
			maxerr = err
		a = a + baud * n
		b = b + hz
		m <<= 1
	return maxerr * 100.0 / hz

# Oversampling Baud Rate Mode Setting
# N = hz / baud
# UCBRx = INT(N/16)
# UCBRFx = round( ( (N/16) − INT(N/16) ) * 16 )
def compute_oversamp (hz, baud):
	best_s = 0
	min_err = 100
	for s in range(16):
		err = compute_oversamp_error (hz, baud, s)
		#print "oversamp s=%#02x error=%.2f%%" % (s, err)
		if err < min_err:
			min_err = err
			best_s = s
	return (hz/baud/16, best_s, min_err)

def compute_oversamp_error (hz, baud, s):
	n = hz / baud / 16
	m = [0x0000, 0x4000, 0x4001, 0x6001,
	     0x6003, 0x7003, 0x7007, 0x7407,
	     0x740f, 0x7c0f, 0x7c1f, 0x7e1f,
	     0x7e3f, 0x7f3f, 0x7f7f, 0x7fff] [s]
	a = baud * n * 16
	b = hz
	maxerr = 0
	for i in range(16):
		if m & 0x8000:
			a = a + baud * 16
		err = abs (b - a)
		if err > maxerr:
			maxerr = err
		a = a + baud * n * 16
		b = b + hz
		m <<= 1
	return maxerr * 100.0 / hz

print "                 -- Low-frequency --    --- Oversampling ---"
print "HZ       Baud    UCBR   UCBRS  Error    UCBR    UCBRF  Error"
print "------------------------------------------------------------"
for baud in tab:
	(lf_div, lf_s, lf_maxerr) = compute_low_freq (hz, baud)
	(os_div, os_s, os_maxerr) = compute_oversamp (hz, baud)
	print "%-08d" % hz,
	print "%-07d" % baud,
	print "0x%04x  " % lf_div,
	print "%d   " % lf_s,
	print "%.2f%%   " % lf_maxerr,
	print "0x%04x  " % os_div,
	print "0x%x  " % os_s,
	print "%.2f%%" % os_maxerr
