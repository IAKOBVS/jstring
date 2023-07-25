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
my $has_funcs = 0;
my $in_if = 0;
foreach (split(/\n\n/, $file)) {
	if (/^[ \t]*#[ \t]*(?:if|elif|else)/) {
		if (/#[ \t]*ifndef.*(?:H|DEF)/ || /#if[ \t]*.*(?:NAMESPACE)/) {
			$in_if = 0;
			next;
		}
		$in_if = 1;
		$h .= "$_\n\n";
		$hpp .= "$_\n\n";
		next;
	} elsif (/^[ \t]*#[ \t]*endif/) {
		if ($in_if) {
			$h .= "$_\n\n";
			$hpp .= "$_\n\n";
		}
		next;
	}
	/^((?:#if|\/\/|\/\*|$NAMESPACE_BIG|static)[^(){}]+?($NAMESPACE\_\w+?)\(((?:.|\n)+?(?:sz|cap)(?:.|\n)+?\)\s*\w*NOEXCEPT))/;
	if (!$1 && !$2 && !$3) {
		next;
	}
	my $decl   = $1;
	my $FUNC   = $2;
	my $PARAMS = $3;
	$PARAMS =~ s/\)/,/;
	my $RETURN = (index($decl,   'void') != -1) ? '' : 'return ';
	my $SZ     = (index($PARAMS, 'sz') != -1)   ? 1  : 0;
	my $CAP    = (index($PARAMS, 'cap') != -1)  ? 1  : 0;

	if (!$SZ && !$CAP) {
		next;
	}
	$has_funcs = 1;
	my $PTR = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
	if ($SZ) {
		if ($decl =~ /\w*sz(,|\))/) {
			if ($1 eq ')') {
				$decl =~ s/[^(,]*sz(?:,|\))/)/;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*sz,//;
			}
		}
	}
	if ($CAP) {
		if ($decl =~ /\w*cap(,|\))/) {
			if ($1 eq ')') {
				$decl =~ s/[^(,]*cap(?:,|\))/)/;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*cap,//;
			}
		}
	}
	my @OLD_ARGS = split(/\s/, $PARAMS);
	my $CONST    = ($OLD_ARGS[0] eq 'const') ? 'const ' : '';
	my @NEW_ARGS;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@NEW_ARGS, $_);
		}
	}
	my $LAST    = (index($PARAMS, ',') != -1) ? ',' : ')';
	my $replace = "($NAMESPACE\_t *$NAMESPACE_BIG\_RST $CONST" . "j$LAST";
	$decl =~ s/\(.+?$LAST/$replace/;
	$decl .= "\n{\n\t$RETURN$FUNC(";
	my $func_args = $PTR . "j->data, ";
	for (my $i = 1 ; $i <= $#NEW_ARGS ; ++$i) {
		if (index($NEW_ARGS[$i], 'sz') != -1) {
			$NEW_ARGS[$i] = $PTR . "j->size";
		} elsif (index($NEW_ARGS[$i], 'cap') != -1) {
			$NEW_ARGS[$i] = $PTR . "j->cap";
		}
		$func_args .= "$NEW_ARGS[$i], ";
	}
	$func_args =~ s/,[^,]*$//;
	$decl .= "$func_args);\n}\n\n";
	if ($decl !~ /$NAMESPACE_BIG\_INLINE/) {
		$decl =~ s/static/$NAMESPACE_BIG\_INLINE\nstatic/;
	}
	$hpp .= $decl;
	$decl =~ s/$FUNC/$FUNC\_j/;
	$h .= $decl;
}
my $end = "$namespace\n$undef\n$endif";
if ($has_funcs) {
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
$hpp =~ s/$NAMESPACE\_(\w*)_mem(\w*\()/$1$2/g;
$hpp =~ s/($NAMESPACE\_\w+)_j(\()/$1$2/g;
$hpp =~ s/$NAMESPACE\_(\w+\()/$1/g;
$hpp =~ s/\tt\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/\t~t\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/alloc_append/alloc/g;
# $hpp =~ s/\n(#ifndef.*HPP_DEF)/$1\n/g;
# $h =~ s/\n(#ifndef.*H_DEF)/$1\n/g;
$hpp =~ s/\n\n\n/\n\n/g;
$h   =~ s/\n\n\n/\n\n/g;
open($FH, '>', "$DIR_CPP/${FNAME}pp") or die "Can't open $DIR_CPP/${FNAME}pp\n";
print($FH $hpp);
close($FH);
open($FH, '>', "$DIR_C/$FNAME") or die "Can't open $DIR_C/$FNAME\n";
print($FH $h);
close($FH);
