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
base_url = "http://uos-embedded.googlecode.com/svn/wiki/"
language = ""
verbose = 0
section_level = 0
inside_para = ""
inside_code = 0
inside_table = 0
re_heading1 = re.compile ("=(.*)=$")
re_heading2 = re.compile ("==(.*)==$")
re_heading3 = re.compile ("===(.*)===$")
re_bullets = re.compile ("  *\* (.*)$")
re_numbering = re.compile ("  *# (.*)$")
re_link = re.compile ("\[([^ ]*)(.*)\]")
re_image = re.compile ("\[([^ ]*\.(png|jpg|gif))\]$")
re_bold_left = re.compile ("\*[^ *]")
re_bold_right = re.compile ("[^ *]\*")
re_italic_left = re.compile ("_[^ _]")
re_italic_right = re.compile ("[^ _]_")
re_literal_left = re.compile ("`[^ `]")
re_literal_right = re.compile ("[^ `]`")

#
# Start/finish a paragraph or list.
#
def set_para (style):
	global inside_para
	if style:
		if inside_para == style:
			return
		if inside_para:
			print "</%s>" % inside_para
		print "<%s>" % style,
		inside_para = style
	else:
		if not inside_para:
			return
		print "</%s>" % inside_para
		inside_para = ""

#
# Close paragraph or list.
#
def para_close():
	print "</para>"
	inside_para = 0

#
# Make a list item.
#
def list_item (text):
	print "<listitem><para>" + text.strip().encode('utf-8') + "</para></listitem>"

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
	print "<title>" + title.strip().encode('utf-8') + "</title>"

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
# Replace left and right emphasis.
#
def replace_emphasis (line, re_left, re_right, tag_open, tag_close):
	match = re_left.search (line)
	if not match:
		return ""
	i = match.start()
	if i > 0 and line[i-1].isalnum():
		return ""
	match = re_right.search (line, i)
	if not match:
		return ""
	k = match.end()
	if k < len(line) and line[k].isalnum():
		return ""
	return line[:i] + tag_open + line[i+1:k-1] + tag_close + line[k:]

#
# Handle bold/italic/code emphasis.
#
def parse_emph (line):
	while 1:
		new = replace_emphasis (line, re_bold_left, re_bold_right,
			"<emphasis role=\"bold\">", "</emphasis>")
		if new:
			line = new
			continue
		new = replace_emphasis (line, re_italic_left,
			re_italic_right, "<emphasis>", "</emphasis>")
		if new:
			line = new
			continue
		new = replace_emphasis (line, re_literal_left,
			re_literal_right, "<literal>", "</literal>")
		if new:
			line = new
			continue
		break
	return line

#
# Convert &, <, > to HTML entities.
#
def parse_entities (line):
	for i in reversed(range(len(line))):
		c = line[i]
		if c == '&':
			line = line[:i] + "&amp;" + line[i+1:]
		elif c == '<':
			line = line[:i] + "&lt;" + line[i+1:]
		elif c == '>':
			line = line[:i] + "&gt;" + line[i+1:]
	return line

#
# Process a single line.
#
def convert_line (line):
	line = parse_links (line)

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

	set_para ("para")
	print line.encode('utf-8')

#
# Make a reference to image file.
#
def put_image (url):
	file = url
	if url.startswith (base_url):
		file = "file:./" + url [len(base_url):]
	print "<graphic fileref=\"" + file + "\"></graphic>"

#
# Make a row of table.
#
def add_table_row (line, nrows):
	cells = line[2:].split ("||")
	print "<row>",
	for i in range (nrows):
		if i < len(cells):
			cell = cells[i].strip()
		else:
			cell = ""
		print "<entry>" + cell.encode('utf-8') + "</entry>",
	print "</row>"

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
print "<article lang=\"%s\">" % language
for filename in args:
	# Process a single wiki page.
	#print "--", filename, "--"
	f = open (filename)
	for line in f.readlines():
		line = unicode (line, "utf-8").rstrip()
		if not line:
			if inside_code:
				print
			else:
				set_para ("")
			continue
		if line[0] == '#':
			continue
		line = parse_entities (line)

		# Blocks of literal text.
		if not inside_code and line == "{{{":
			inside_code = 1
			print "<screen>"
			continue
		if inside_code:
			if line == "}}}":
				inside_code = 0
				print "</screen>"
			else:
				print line.encode('utf-8')
			continue

		# Inline images.
		match = re_image.match (line)
		if match:
			put_image (match.expand ("\\1"))
			continue

		line = parse_emph (line)

		# Tables.
		if not inside_table and line.startswith ("||"):
			set_para ("")
			inside_table = line.count ("||", 2)
			if inside_table < 1:
				inside_table = 1
			print "<table frame='all'><title></title>",
			print "<tgroup cols='%d' align='left' colsep='1' rowsep='1'>" % inside_table,
			print "<tbody>"
			add_table_row (line, inside_table)
			continue
		if inside_table:
			if line.startswith ("||"):
				add_table_row (line, inside_table)
				continue
			inside_table = 0
			print "</tbody></tgroup></table>"

		# Ordinary text/
		convert_line (line)

set_section (0)
print "</article>"
