#!/usr/bin/perl
use strict;
use warnings;
# use diagnostics;

my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = uc($NAMESPACE);
my $DIR_C         = 'c';
my $DIR_CPP       = $DIR_C . 'pp';

mkdir($DIR_CPP);
mkdir($DIR_C);

my $FNAME = $ARGV[0];
if ($#ARGV != 0) {
	print "Usage: ./gen_func.pl <file>";
	exit 1;
}
open(my $FH, '<', $FNAME)
  or die "Can't open $FNAME\n";
my $file;
while (<$FH>) {
	$file .= $_;
}
close($FH);
my @funcs = split(/\n\n/, $file);
my $skel;
foreach (@funcs) {
	if (/^((?:\/\/|\/\*|$NAMESPACE_BIG|static)(?:.|\n)*?NOEXCEPT)/) {
		$skel .= "$1;\n\n";
	}
}
$file =~ s/(.*&& $NAMESPACE_BIG\_EXTERN_C)/$skel$1/;
print $skel;
# open($FH, '>', $FNAME)
#   or die "Can't open $FNAME\n";
# print $FH $file;
# close($FH);
