#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Convert Googlecode wiki pages to Docbook format.
# Copyright (C) 2010 Serge Vakulenko <serge@vak.ru>
#
import sys, string, getopt, re

#
# Default options.
#
language = "en"
verbose = 0
section_level = 0
para_inside = ""
re_heading1 = re.compile ("=(.*)=$")
re_heading2 = re.compile ("==(.*)==$")
re_heading3 = re.compile ("===(.*)===$")
re_bullets = re.compile ("  *\* (.*)$")
re_numbering = re.compile ("  *# (.*)$")
re_link = re.compile ("\[([^ ]*)(.*)\]")

#
# Start/finish a paragraph or list.
#
def set_para (style):
	global para_inside
	if style:
		if para_inside == style:
			return
		if para_inside:
			print "</%s>" % para_inside
		print "<%s>" % style
		para_inside = style
	else:
		if not para_inside:
			return
		print "</%s>" % para_inside
		para_inside = ""

#
# Close paragraph or list.
#
def para_close():
	print "</para>"
	para_inside = 0

#
# Make a list item.
#
def list_item (text):
	print "<listitem> <para>", text.strip().encode('utf-8'), "</para> </listitem>"

#
# Start/finish a section.
#
def set_section (level, title=""):
	global section_level
	set_para ("")
	while section_level > level:
		print "</sect%d>" % section_level
		section_level -= 1
	if level == 0:
		return
	if section_level == level:
		print "</sect%d>" % section_level,
		section_level -= 1
	while section_level < level:
		section_level += 1
		print "<sect%d>" % section_level,
	print "<title>", title.strip().encode('utf-8'), "</title>"

#
# Make URL links.
#
def parse_links (line):
	result = ""
	while 1:
		match = re_link.search (line)
		if not match:
			break
		url = match.group(1)
		text = match.group(2).strip()
		#print ">>>", match.start(), match.end(), url.encode('utf-8'), text.encode('utf-8')
		result += line[:match.start()]
		result += "<ulink url=\"" + url + "\">" + text + "</ulink>"
		#print "---", result.encode('utf-8')
		line = line[match.end():]
		#print "---", line.encode('utf-8')
	result += line
	#print "result =", result.encode('utf-8')
	return result

#
# Process a single line.
#
def convert_line (line):
	match = re_heading3.match (line)
	if match:
		set_section (3, match.expand ("\\1"))
		return
	match = re_heading2.match (line)
	if match:
		set_section (2, match.expand ("\\1"))
		return
	match = re_heading1.match (line)
	if match:
		set_section (1, match.expand ("\\1"))
		return
	match = re_bullets.match (line)
	if match:
		set_para ("itemizedlist")
		list_item (match.expand ("\\1"))
		return
	match = re_numbering.match (line)
	if match:
		set_para ("orderedlist")
		list_item (match.expand ("\\1"))
		return

	line = parse_links (line)
	set_para ("para")
	print "   ", line.encode('utf-8')


#
# Print usage info.
#
def usage ():
	print """gwiki2docbook.py: Convert Googlecode wiki pages to Docbook format.

Usage:
	gwiki2docbook.py [-v] [-l language] page.wiki...
Options:
	-v           verbose mode
	-l           language: en, ru, etc.
         <module>    name of module
"""

try:
	opts, args = getopt.getopt (sys.argv[1:], "hl:v", ["help", "language="])
except getopt.GetoptError:
	usage()
	sys.exit(2)
for opt, arg in opts:
	if opt in ("-h", "--help"):
		usage()
		sys.exit()

	elif opt == '-v':
		verbose = 1

	elif opt in ("-l", "--language"):
		language = arg

if args == []:
	usage()
	sys.exit(2)

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
print "<!DOCTYPE article"
print "  PUBLIC \"-//OASIS//DTD DocBook XML V4.1.2//EN\""
print "  \"http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd\">"
print "<article lang=\"\">"
for filename in args:
	# Process a single wiki page.
	#print "--", filename, "--"
	f = open (filename)
	for line in f.readlines():
		line = unicode (line, "utf-8").rstrip()
		if not line:
			set_para ("")
			continue
		if line[0] == '#':
			continue
		convert_line (line)

set_section (0)
print "</article>"
