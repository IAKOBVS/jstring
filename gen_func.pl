#!/usr/bin/perl
use strict;
use warnings;

my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = 'JSTR';
my $DIR_CPP       = 'cpp';
my $DIR_C         = 'c';

mkdir($DIR_CPP);
mkdir($DIR_C);

my $FNAME = $ARGV[0];
if ($#ARGV != 0) {
	print "Usage: ./gen_func.pl <file>";
	exit 1;
}
my $file;
my $namespace = '';
my $undef     = '';
my $endif     = '';
open(my $FH, '<', $FNAME) or die "Can't open $FNAME\n";
while (<$FH>) {
	if (/#\s*undef/) {
		$undef .= $_;
	} elsif (/#\s*endif\s*(?:\/\/|\/\*).*DEF.*(?:\/\*){0,1}/) {
		$endif .= $_;
	} elsif (/^\s*#if\s*defined.*JSTR_EXTERN_C/) {
		$namespace .= $_;
		while (<$FH>) {
			if (/^$/) {
				last;
			}
			$namespace .= $_;
		}
		if ($namespace =~ /namespace/) {
			$namespace = '';
		}
	} else {
		$file .= $_;
	}
}
close($FH);
my $h;
my $hpp;
my @funcs = split(/\n\n/, $file);
my $skeleton;
foreach (@funcs) {
/^((?:\/\/|\/\*|$NAMESPACE_BIG|static)[^(){}]+?($NAMESPACE\_\w+?)\(((?:.|\n)+?(?:sz|cap)(?:.|\n)+?\)\s*\w*NOEXCEPT))/;
	if (!$1 || !$2 || !$3) {
		next;
	}
	my $decl    = $1;
	my $func    = $2;
	my $args    = $3;
	my $has_sz  = (index($args, 'sz') != -1)  ? 1 : 0;
	my $has_cap = (index($args, 'cap') != -1) ? 1 : 0;
	next if !$has_sz && !$has_cap;
	$args =~ s/\)/,/;
	my $is_const;
	my @new_args;
	{
		my @old_args = split(/\s/, $args);
		my $i        = 0;
		foreach (@old_args) {
			if ($i == 0) {
				$is_const = ($_ =~ /const/) ? 1 : 0;
			}
			if (/,$/) {
				s/,//;
				push(@new_args, $_);
				++$i;
			}
		}
	}
	my $tmp = '';
	if ($has_sz) {
		$decl =~ s/\((?:.|\n)+?sz(,|\))/(/;
		if ($1) {
			$tmp = $1;
		}
	}
	if ($has_cap) {
		$decl =~ s/\((?:.|\n)+?cap(,|\))/(/;
		if ($1 eq ',') {
			$tmp = $1;
		}
	}
	if ($is_const) {
		$decl =~ s/\(/(const $NAMESPACE\_t *$NAMESPACE_BIG\_RST const j$tmp/;
	} else {
		$decl =~ s/\(/($NAMESPACE\_t *$NAMESPACE_BIG\_RST const j$tmp/;
	}
	$decl .= "\n{\n\t$func(";
	my $func_args;
	for (my $i = 0 ; $i <= $#new_args ; ++$i) {
		if (index($new_args[$i], 'sz') != -1) {
			$new_args[$i] = "&j->size";
		} elsif (index($new_args[$i], 'cap') != -1) {
			$new_args[$i] = "&j->cap";
		} elsif ($i == 0) {
			$new_args[$i] = "&j->data";
		}
		$func_args .= "$new_args[$i], ";
	}
	$func_args =~ s/,[^,]*$//;
	$decl .= "$func_args);\n}\n\n";
	if ($decl !~ /$NAMESPACE_BIG\_INLINE/) {
		$decl =~ s/static/$NAMESPACE_BIG\_INLINE\nstatic/;
	}

	$hpp .= $decl;

	$decl =~ s/$func/$func\_j/;
	$h .= $decl;
}
my $end = "$namespace\n$undef\n$endif";
if ($h && $hpp) {
	$h   = "$file\n$h\n$end";
	$hpp = "$file\n$hpp\n$end";
} else {
	$h   = "$file\n$end";
	$hpp = "$file\n$end";
}
$hpp =~ s/\.h"/.hpp"/g;
$hpp =~ s/H_DEF/HPP_DEF/g;
$hpp =~ s/EXTERN_C\s*\d/EXTERN_C 0/;
$hpp =~ s/NAMESPACE\s*\d/NAMESPACE 1/;
$hpp =~ s/$NAMESPACE\_(\w*)mem(\w*\()/$1$2/g;
$hpp =~ s/($NAMESPACE\_\w+)_j(\()/$1$2/g;
$hpp =~ s/$NAMESPACE\_(\w+\()/$1/g;
$hpp =~ s/\tt\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/\t~t\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/alloc_append/alloc/g;
$hpp =~ s/\n\n\n/\n\n/g;
$h   =~ s/\n\n\n/\n\n/g;
open($FH, '>', "$DIR_CPP/$FNAME" . 'pp')
  or die "Can't open $DIR_CPP/$FNAME" . "pp\n";
print($FH $hpp);
close($FH);
open($FH, '>', "$DIR_C/$FNAME") or die "Can't open $DIR_C/$FNAME\n";
print($FH $h);
close($FH);
