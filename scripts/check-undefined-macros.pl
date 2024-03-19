#!/usr/bin/perl
use strict;
use warnings;

do "./jlib.pl";

die("Usage: $0 <filename> <macro prefix>.\n") if (scalar(@ARGV) < 2);

my $fname  = $ARGV[1];
my $prefix = $ARGV[0];
my $file_str   = jl_file_get_str(\$fname);
my @file_lines = split(/\n/, $file_str);
my $undef      = '';
foreach (@file_lines) {
	if ($_ =~ /^[ \t]*#[ \t]*define[ \t]+($prefix\w*)[ \t]+1/) {
		my $macro = $1;
		if ($file_str !~ /$macro[ \t]+0/ && $macro !~ /_H$/) {
			$undef .= "#ifndef $macro\n#\tdefine $macro 0\n#endif\n";
		}
	}
}

my $end = rindex($file_str, "#endif");
$file_str = substr($file_str, 0, $end) . $undef . substr($file_str, $end);
print $file_str;
