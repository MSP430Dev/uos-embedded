#!/usr/bin/perl

# A quick perl script that reads the rgb.txt file
# and constructs a header file for embedding into a program.

my($rgbtxt) = "rgb.txt";

#open(FILE, $rgbtxt) or die "Oops, couldn't open $rgbtxt";

print "/* X rgb.txt data file */\n";
print "/* Automatically generated */\n";

while(<>) {
    if (/^\s*(\d+)\s*(\d+)\s*(\d+)\s*(.*)/) {
	my($r) = $1;
	my($g) = $2;
	my($b) = $3;
	my($colorname) =  $4;

	print "{ $r, $g, $b, \"$colorname\" },\n";
    }
}
