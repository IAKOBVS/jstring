#!/usr/bin/perl
use strict;
use warnings;

do "./jlib.pl";

my $fname = $ARGV[0];
my $file_str = jl_file_get_str(\$fname);
my @lines = split(/\n/, $file_str);
foreach (@lines) {
	if (/^[ \t]*#[ \t]*define[ \t]*([A-Z][A-Z0-9_]*)/) {
		my $macro = $1;
		if ($file_str !~ /(?:^|\n)[ \t]*#[ \t]*undef[ \t]+$macro/) {
			print "$macro is not undefined in $fname.\n";
		}
	}
}
