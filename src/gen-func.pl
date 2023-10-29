#!/usr/bin/perl
use strict;
use warnings;
do "./jlib.pl";

sub usage {
	if ($#ARGV == -1) {
		print('Usage: ./' . $0 . ' <file>');
		exit;
	}
}

usage();
my $file_str = jl_file_get_str($ARGV[0]);
jl_file_namespace_macros(\$file_str, "PJSTR_", ("PJSTR", "pjstr", "JSTR", "jstr"));
my @file_blocks = jl_file_to_blocks($file_str);
my $out_str     = '';
foreach (@file_blocks) {
	$out_str .= "$_\n\n";
	my ($attr, $rettype, $name, @arg, $body);
	if (jl_fn_get(\$_, \$attr, \$rettype, \$name, \@arg, undef)) {
		my $base_name = $name;
		$base_name =~ s/_len(_|$)/$1/;
		if ($name =~ /^jstr_/ && $name ne $base_name && index($file_str, $base_name . '(') == -1) {
			$name = $base_name;
			$body = (($rettype eq 'void') ? '' : 'return ') . '_len(';
			for (my $i = 0 ; $i < $#arg ; ++$i) {
				my $var = jl_arg_get_var($arg[$i]);
				my $ret = jl_arg_get_rettype($arg[$i]);
				if (index($ret, 'size_t') != -1 && index($var, 'len') != -1
					|| $var =~ /sz$/)
				{
					my $base_var = $var;
					$base_var =~ s/(?:z|_*len)\s*$//;
					my $str_i   = jl_arg_index(\@arg, $base_var);
					my $str_var = jl_arg_get_var($arg[$str_i]);
					my $deref   = jl_arg_is_ptr_ptr($str_var) ? '*' : '';
					$body .= 'strlen(' . $deref . $str_var . ')';
					splice(@arg, $i);
				} else {
					$body .= $var;
				}
				$body .= ', ';
			}
			$body =~ s/, $//;
			$body    .= ");";
			$out_str .= jl_fn_to_string($attr, $rettype, $name, \@arg, $body) . "\n";
		}
	}
}
print $out_str;
