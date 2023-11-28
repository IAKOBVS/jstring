#!/usr/bin/perl

use strict;
use warnings;

do "./jlib.pl";

my $file_str;
{
	local $/;
	if (scalar(@ARGV) == 1) {
		$file_str = jl_file_get_str(\$ARGV[0]);
	} else {
		$file_str = <>;
	}
}
my @ignore_prefix = ("PJSTR", "pjstr", "JSTR", "jstr");
jl_file_namespace_macros(\$file_str, \"PJSTR_", \@ignore_prefix);
print $file_str;
