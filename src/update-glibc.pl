#!/usr/bin/perl
use strict;
use warnings;

while (glob('./glibc/sysdeps/*/string-*.h')) {
	/([^\/]*\/[^\/]*)$/;
	my $new_fname = $1;
	$new_fname =~ s/\//-/;
	$new_fname = '_glibc-' . $new_fname;
	my $cmd = "cp -f $_ $new_fname";
	print "$cmd\n";
	system($cmd);
}
