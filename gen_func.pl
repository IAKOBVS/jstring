#!/usr/bin/perl
use strict;
use warnings;

my $FNAME         = $ARGV[0];
my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = uc($NAMESPACE);
my $DIR_C         = 'c';
my $DIR_CPP       = $DIR_C . 'pp';
mkdir($DIR_CPP);
mkdir($DIR_C);
if ($#ARGV != 0) {
	print 'Usage: ./gen_func.pl <file>';
	exit 1;
}

# if (system("test $FNAME -nt $DIR_C")) {
# 	exit 1;
# }

my $file          = '';
my $namespace_end = '';
my $undef         = '';
my $endif         = '';
open(my $FH, '<', $FNAME)
  or die "Can't open $FNAME\n";

while (<$FH>) {
	$file .= $_;
}

close($FH);

foreach (split(/\n\n/, $file)) {
	if (!/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*_mem\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/) {
		next;
	}
	my $decl     = $1;
	my $FUNC     = $2;
	my $params   = $3;
	my $FUNCTION = $_;
	if (!$decl && !$FUNC && !$params) {
		next;
	}
	$params =~ s/\)/,/;
	my @OLD_ARGS = split(/\s/, $params);
	my @NEW_ARGS;
	my $PTR = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			if (/(\w*)len/) {
				$_ = "strlen($1)";
			}
			push(@NEW_ARGS, $_);
		}
	}
	$decl =~ s/$NAMESPACE\w*_mem\w*\(/(/;
	$decl =~ s/,\s*\w*len//g;
	my $func_args = $PTR . "j->data, ";
	for (my $i = 1 ; $i <= $#NEW_ARGS ; ++$i) {
		if (index($NEW_ARGS[$i], 'sz') != -1) {
			$NEW_ARGS[$i] = $PTR . "j->size";
		} elsif (index($NEW_ARGS[$i], 'cap') != -1) {
			$NEW_ARGS[$i] = $PTR . "j->cap";
		}
		$func_args .= "$NEW_ARGS[$i], ";
	}
	$decl .= "\n{\n\t";
	$decl .= "$FUNC(";
	foreach (@NEW_ARGS) {
		$decl .= "$_, ";
	}
	$decl =~ s/, $//;
	$decl .= ");\n}\n";
	$file =~ s/\Q$FUNCTION\E/$FUNCTION\n\n$decl/;
}

$file =~ s/\n\n\n/\n\n/g;

my $h         = $file;
my $hpp       = $file;
my $has_funcs = 0;
my $in_ifdef  = 0;

foreach (split(/\n\n/, $file)) {
	if (!/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/) {
		next;
	}
	my $decl   = $1;
	my $FUNC   = $2;
	my $params = $3;
	if (!$decl && !$FUNC && !$params) {
		next;
	}
	$params =~ s/\)/,/;
	my $SZ  = ($params =~ /sz(?:,|\))/)  ? 1 : 0;
	my $CAP = ($params =~ /cap(?:,|\))/) ? 1 : 0;
	if (!$SZ && !$CAP) {
		next;
	}
	my $FUNCTION = $_;
	$has_funcs = 1;
	my $RETURN = (index($decl, 'void') != -1) ? '' : 'return ';
	my $PTR    = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
	if ($SZ && $decl =~ /\w*sz(,|\))/) {
		if ($1 eq ')') {
			$decl =~ s/[^(,]*sz(?:,|\))/)/;
		} elsif ($1 eq ',') {
			$decl =~ s/[^(,]*sz,//;
		}
	}
	if ($CAP && $decl =~ /\w*cap(,|\))/) {
		if ($1 eq ')') {
			$decl =~ s/[^(,]*cap(?:,|\))/)/;
		} elsif ($1 eq ',') {
			$decl =~ s/[^(,]*cap,//;
		}
	}
	my @OLD_ARGS = split(/\s/, $params);
	my $CONST    = ($OLD_ARGS[0] eq 'const')   ? 'const ' : '';
	my $LAST     = (index($params, ',') != -1) ? ','      : ')';
	my $tmp      = "($NAMESPACE\_t *$NAMESPACE_BIG\_RST $CONST" . "j$LAST";
	$decl =~ s/\(.+?\Q$LAST\E/$tmp/;
	$decl .= "\n{\n\t$RETURN$FUNC(";

	my @NEW_ARGS;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@NEW_ARGS, $_);
		}
	}
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
	$decl =~ s/,\s*,/)/g;
	$decl =~ s/,\s*\)/)/g;
	$h    =~ s/\Q$FUNCTION\E/$FUNCTION\n\n$decl/;
	$decl =~ s/$FUNC/$FUNC\_j/;
	$hpp  =~ s/\Q$FUNCTION\E/$FUNCTION\n\n$decl/;
}

$hpp =~ s/\.h"/.hpp"/g;
$hpp =~ s/H_DEF/HPP_DEF/g;
$hpp =~ s/EXTERN_C\s*\d/EXTERN_C 0/;
$hpp =~ s/NAMESPACE\s*\d/NAMESPACE 1/;
$hpp =~ s/($NAMESPACE\_\w*)_mem(\w*\()/$1$2/g;
$hpp =~ s/($NAMESPACE\_\w*)_j(\()/$1$2/g;
$hpp =~ s/$NAMESPACE\_(\w*\()/$1/g;
$hpp =~ s/\tt\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/\t~t\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/alloc_append/alloc/g;
$hpp =~ s/\n#if.*\s*#endif.*/\n/g;
$hpp =~ s/\n\n\n/\n\n/g;
$h   =~ s/\n\n\n/\n\n/g;

open($FH, '>', "$DIR_CPP/$FNAME" . 'pp')
  or die "Can't open $DIR_CPP/$FNAME" . "pp\n";
print($FH $hpp);
close($FH);
open($FH, '>', "$DIR_C/$FNAME")
  or die "Can't open $DIR_C/$FNAME\n";
print($FH $h);
close($FH);
