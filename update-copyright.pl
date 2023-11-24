#!/usr/bin/perl

# Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
# This file is part of the jstring library.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
# MIT License (Expat)

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
