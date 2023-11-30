#!/usr/bin/perl
use strict;
use warnings;

do "./jlib.pl";

my $out = '';
my $fname = $ARGV[0];

my $file_str1 = jl_file_get_str(\$fname);
foreach (jl_file_to_blocks(\$file_str1)) {
	my ($attr, $rettype, $name, @arg_arr);
	if (jl_fn_get(\$_, \$attr, \$rettype, \$name, \@arg_arr, undef)) {
		$out .= jl_fn_to_string(\$attr, \$rettype, \$name, \@arg_arr, undef) . ";";
	} else {
		$out .= $_;
	}
	$out .= "\n\n";
}
print $out;
