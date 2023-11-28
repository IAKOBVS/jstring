#!/usr/bin/perl

use strict;
use warnings;

my $file_str;
{
	local $/;
	open(my $FH, '<', $ARGV[0]) or die "Can't open $ARGV[0].\n";
	$file_str = <$FH>;
	close($FH) or die "Can't close $ARGV[0].\n";
}

$file_str =~ s/(#[ \t]*define[ \t]*JSTR_USE_LGPL[ \t]*)1/$1 0/ if ($ARGV[0] =~ 'jstr-config.h');
$file_str =~ s/\n.*#.*include.*"_lgpl.*"//g;
print $file_str;
