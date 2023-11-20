#!/usr/bin/perl

# See LICENSE file for copyright and license details.

use strict;
use warnings;

my $curr_year = `date +%Y`;
chomp $curr_year;

my $str = '';
open(my $FH, '<', $ARGV[0]) or die "Can't open $ARGV[0].\n";
while (<$FH>) {
	if (/Copyright \(c\) ([0-9]*)(?:\-([0-9]*)){,1}/) {
		my $first_year = $1;
		my $last_year  = $2;
		if (defined($last_year) && $curr_year gt $last_year
			|| $curr_year gt $first_year)
		{
			s/(Copyright \(c\)) [-0-9]*/$1 $first_year-$curr_year/;
		}
	}
	$str .= $_;
}
close($FH) or die "Can't close $ARGV[0].\n";
print $str;
