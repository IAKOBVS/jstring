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
