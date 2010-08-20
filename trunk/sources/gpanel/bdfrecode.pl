#!/usr/bin/perl

# Reorder glyphs in a BDF font according to table. Script by Serge Winitzki.
# http://www.linuxstart.com/~winitzki/
#
# Use as filter on BDF files only:
# bdfrecode.pl -table=tablefile -usenames=namefile -printnames -registry=REG-Y -verbose < oldfont.bdf > newfont.bdf

$state= 0;	# 0 - before 1st glyph, 1 - inside glyph; 2 - finished glyph

$totalchars= 0;	# Will count chars

# Font information.

@glyphs= ();

@charpresent=();

$preamble= "";
$postamble= "";
$charsinfont= 0;

@charname= ();	# $charname[32]="space"; $glyphs{"space"} = "STARTCHAR ... ENDCHAR\n"

$current=0;
$currentglyph="";

$want_new_registry= ("@ARGV" =~ /-registry=([^ ]+)/i) ? 1 : 0;
$new_registry = "$1" if ($want_new_registry);

$verbose= ("@ARGV" =~ /-verbose/i) ? 1 : 0;

@precode=();	# Recoding table

if ("@ARGV" =~ /-table=([^ ]+)/i) {	# Read recoding table from file

	open(TABLEFILE, "$1") || die "bdfrecode: Error: can't open table file '$1'.\n";
	while(<TABLEFILE>) {
		chop;
		push(@precode, split(/[ ,;\n\t]/, $_)) unless (/^[ \t]*\#/);
	}

	close(TABLEFILE);

} else { # No recoding data given, do nothing
	print STDERR <<"ENDHELP";
BDF font reshuffling script version 1.0 by S. Winitzki.
Usage:
	bdfrecode [options] < oldfont.bdf > newfont.bdf
Options:
-table=XXX         use recoding table in file 'XXX' (required!)
-usenames=XXX      set character names according to file 'XXX'
-registry=XXX      set font registry to XXX (e.g. 'KOI8-R')
-printnames=XXX    print all character names to file 'XXX'
-verbose           print glyph info on STDERR
ENDHELP
	exit;
}

$current=0;

while ($current < $#precode) {

	print STDERR "recode $precode[$current] to $precode[$current+1]\n";
	$recode{&decimal($precode[$current])}=&decimal($precode[$current+1]);
	$current += 2;
}

undef @precode;

# Read the font now.

while(<STDIN>) {

	chop;
	# Read the font and store its glyphs in a hash, indexed by encoding
	# (since some fonts have non-unique glyph names)

	if ($state==0) {	# Waiting for first glyph

		if (/^STARTCHAR +(.+)$/) {	# Glyph started, named $1
			++$totalchars;
			$currentglyph = "$1";
			# Note that the "STARTCHAR" line is not written to hash!
			$state = 1;

		} elsif (/^CHARS +([0-9]+)/) {	# Number of chars given, store it
			$charsinfont=$1;
			#$preamble .= "$_\n";	# Omitting this line.
		} else { 	# Still waiting for first glyph
			$preamble .= "$_\n";

		}
	} elsif ($state==1) {	# Are inside glyph
		if (/^ENDCHAR/) {	# Char just ended
			$glyphs[$current] .= "$_\n";
			$state= 2;
		} elsif (/^ENCODING +([0-9]+)$/) {
			$current= &decimal($1);
			print STDERR "reading $current\n";
			$charname[$current] = "$currentglyph";	# The ENCODING directive is not written to hash
			$glyphs[$current] = "";	# Define it now
			$charpresent[$current]= 1;
		} else {	# We are inside glyph and need to store this line now
			$glyphs[$current] .= "$_\n";
		}
	} elsif ($state==2) {
		if (/^STARTCHAR +(.+)$/) {	# Glyph started, named $1
			++$totalchars;
			$currentglyph = "$1";
			# Not writing the STARTCHAR line to hash
			$state = 1;
		} else {
			$postamble = "$_\n";
		}

	}

}

if ($totalchars != $charsinfont) {
	print STDERR "The number of characters read, $totalchars, does not match that given in font ($charsinfont).\n";

	}

# OK, we have read everything, now we can reshuffle the font.

# Print the table of character names if necessary

if ("@ARGV" =~ /-printnames=([^ ]+)/) {
    open(NAMETABLE, ">" . "$1");
    $i = 0;
    while($i < 256) {
		print NAMETABLE "$i\t" . $charname[$i] . "\n" if ($charpresent[$i]);
		++$i;
    }
    close(NAMETABLE);
}

# We need to make the @newglyphs array with just the required characters.

@newglyphs = ();

@newcharpresent = ();

@newcharname = ();

$totalchars = 0;

foreach $current (keys %recode) {
	print STDERR "getting $current\n";
	if ($charpresent[$current]) {	# Char exists in old font
		print STDERR "recode $current to $recode{$current}\n";
		$newglyphs[$recode{$current}] = $glyphs[$current];
		$newcharname[$recode{$current}] = $charname[$current];
		$newcharpresent[$recode{$current}] = 1;
		++$totalchars;
	} else {
		printf STDERR "CHAR %d (0x%02x) does not exist in current font.\n", $current, $current if ($verbose);
	}
}

# Transform preamble: new glyph ranges as given by @newcharpresent and new registry

# First deal with the registry

if ($want_new_registry and $new_registry =~ /^([^-\n]+)-([^-\n]+)$/) {

	$charset_registry = "$1";
	$charset_encoding = "$2";

	$preamble =~ s/(FONT +.*-)([^-\n]+-[^\n-]+)\n/$1$charset_registry-$charset_encoding\n/;
	$preamble =~ s/(CHARSET_REGISTRY) +\".*\"/$1 \"$charset_registry\"/;
	$preamble =~ s/(CHARSET_ENCODING) +\".*\"/$1 \"$charset_encoding\"/;
}

# Now deal with new ranges

# Value must be something like "32_126 154 192_255"
# Prepare the new ranges value.

$i = 0;
$ranges_field_value = "";
$previous_value = -1;	# Invalid value.
$are_in = 0;	# Whether we are inside one of the ranges.
while ($i < 256) {
	if ($newcharpresent[$i] + $are_in == 1) {	# State changed.
		if ($are_in) {	# Were in, now not in.
			$ranges_field_value .= sprintf("_%d", $i-1)
				if ($i - $previous_value > 1);	# This cannot happen at $i==0 because we are not yet in, and it can't happen at $previous_value==-1 because the next clause is always first to be run.
		} else {	# Were not in, now in. Store current value.
			$ranges_field_value .= " " if ($previous_value != -1);
			$ranges_field_value .= "$i";
			$previous_value = $i;
		}
		$are_in = 1 - $are_in;
	}
	++$i;
}

$ranges_field_value .= "_255" if ($i == 256 and $are_in);
$ranges_name = "_XFREE86_GLYPH_RANGES";
# Replace old value by new value now.
if ($preamble =~ /($ranges_name) +\".*\"/) {
	$preamble =~ s/($ranges_name) +\".*\"/$1 \"$ranges_field_value\"/;
} else {	# There was no _XFREE86_GLYPH_RANGES field, add one
	$preamble =~ s/(\nENDPROPERTIES\n)/\n$ranges_name \"$ranges_field_value\"$1/;
	$preamble =~ s/(\nSTARTPROPERTIES) +([0-9]+)\n/sprintf("$1 %d\n", $2+1);/e;
}

# OPTIONAL
# Replace something in the preamble -- put this in by hand if needed
# For example: add an extra comment at a certain place.
$preamble =~ s/(\nCOMMENT This software may be used)/\nCOMMENT Changes 1999 by Serge Winitzki.$1/ unless ($preamble =~ /Serge Winitzki/);

# Now print the resulting font

print $preamble . "CHARS $totalchars\n";

$current = 0;	# Index for the @charname array

while ($current <= $#newcharpresent) {
	if ($newcharpresent[$current]==1) {
		$currentglyph = $newcharname[$current];
		print "STARTCHAR $currentglyph\nENCODING $current\n" . $newglyphs[$current];
	}
	++$current;
}

print $postamble;


# Done.

sub decimal {
	my ($number) = (@_);
	sprintf ("%d", $number);

}
