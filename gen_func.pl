#!/usr/bin/perl
use strict;
use warnings;

my $FNAME         = $ARGV[0];
my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = uc($NAMESPACE);
my $DIR_C         = 'c';
my $DIR_CPP       = $DIR_C . 'pp';

my $STRUCT_VAR  = 'j';
my $STR_STRUCT  = $NAMESPACE . '_t';
my $STRUCT_DATA = 'data';
my $STRUCT_SIZE = 'size';
my $STRUCT_CAP  = 'cap';

my $CAP_PTN  = 'cap';
my $SIZE_PTN = 'sz';
my $LEN_PTN  = 'len';

my $INLINE_MACRO   = $NAMESPACE_BIG . '_INLINE';
my $RESTRICT_MACRO = $NAMESPACE_BIG . '_RST';

mkdir($DIR_CPP);
mkdir($DIR_C);

if ($#ARGV != 0) {
	print 'Usage: ./gen_func.pl <file>';
	exit 1;
}

# if (system("test $FNAME -nt $DIR_C/$FNAME")) {
# 	exit 1;
# }

open(my $FH, '<', $FNAME)
  or die "Can't open $FNAME\n";
my $file = '';
while (<$FH>) {
	$file .= $_;
}
close($FH);

my @OLD_LINES = split(/\n\n/, $file);
my @NEW_LINES;

my $RE_FUNC = qr/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/;

if ($FNAME =~ /builder/) {
	@NEW_LINES = @OLD_LINES;
} else {
	foreach (@OLD_LINES) {
		if ($_ !~ $RE_FUNC) {
			goto NEXT;
		}
		my $decl   = $1;
		my $FUNC   = $2;
		my $params = $3;
		if (!$decl && !$FUNC && !$params) {
			goto NEXT;
		}
		if ($FUNC !~ /_mem[^0-9A-Za-z]/) {
			goto NEXT;
		}
		my $tmp = $FUNC;
		$tmp =~ s/_mem//;
		if ($file =~ /$tmp\(/) {
			goto NEXT;
		}
		$params =~ s/\)/,/;
		my @OLD_ARGS = split(/\s/, $params);
		my @NEW_ARGS;
		my $PTR    = ($decl =~ /\([^,)]*\*\*/) ? '&'       : '';
		my $RETURN = ($_    =~ /return/)       ? 'return ' : '';
		foreach (@OLD_ARGS) {
			if (/,$/) {
				s/,//;
				push(@NEW_ARGS, $_);
			}
		}
		$decl =~ s/($NAMESPACE\_\w*)_mem(\w*\()/$1$2/;
		my $func_args;
		for (my $i = 0 ; $i <= $#NEW_ARGS ; ++$i) {
			$func_args .= $PTR . "$NEW_ARGS[$i],";
		}
		$decl .= "\n{\n\t$RETURN$FUNC(";
		my $LEN = ($params =~ /$LEN_PTN/) ? 1 : 0;
		foreach (@NEW_ARGS) {
			if ($LEN) {
				if (/(\w*)$LEN_PTN/) {
					my $var = $1;
					$decl =~ s/,[^,]*$LEN_PTN//;
					$_ = "strlen($var)";
				}
			} else {
				if (/\w*$SIZE_PTN/) {
					$decl =~ s/,[^,]*$SIZE_PTN//;
					$_ = "strlen($NEW_ARGS[0])";
				}
			}
			$decl .= "$_,";
		}
		$decl =~ s/,$//;
		$decl .= ");\n}\n";
		push(@NEW_LINES, $_);
		push(@NEW_LINES, $decl);
		next;
	  NEXT:
		push(@NEW_LINES, $_);
	}
}

my $h   = '';
my $hpp = '';

foreach (@NEW_LINES) {
	if ($_ !~ $RE_FUNC) {
		goto NEXT;
	}
	my $decl   = $1;
	my $FUNC   = $2;
	my $params = $3;
	if (!$decl && !$FUNC && !$params) {
		goto NEXT;
	}
	$params =~ s/\)/,/;
	my $SZ  = ($params =~ /$SIZE_PTN(?:,|\))/) ? 1 : 0;
	my $CAP = ($params =~ /$CAP_PTN(?:,|\))/)  ? 1 : 0;
	if (!$SZ && !$CAP) {
		goto NEXT;
	}
	my $RETURN = ($decl =~ /void/)         ? ''  : 'return ';
	my $PTR    = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
	if ($SZ && $decl =~ /\w*$SIZE_PTN(,|\))/) {
		if ($1 eq ')') {
			$decl =~ s/[^(,]*$SIZE_PTN(?:,|\))/)/;
		} elsif ($1 eq ',') {
			$decl =~ s/[^(,]*$SIZE_PTN,//;
		}
	}
	if ($CAP && $decl =~ /\w*$CAP_PTN(,|\))/) {
		if ($1 eq ')') {
			$decl =~ s/[^(,]*$CAP_PTN(?:,|\))/)/;
		} elsif ($1 eq ',') {
			$decl =~ s/[^(,]*$CAP_PTN,//;
		}
	}
	my @OLD_ARGS = split(/\s/, $params);
	my $CONST    = ($OLD_ARGS[0] eq 'const') ? 'const ' : '';
	my $LAST     = ($params =~ /,/)          ? ','      : ')';
	my $tmp      = "($STR_STRUCT *$RESTRICT_MACRO $CONST" . "j$LAST";
	$decl =~ s/\(.+?$LAST/$tmp/;
	$decl .= "\n{\n\t$RETURN$FUNC(";

	my @NEW_ARGS;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@NEW_ARGS, $_);
		}
	}
	my $func_args = $PTR . "$STRUCT_VAR->$STRUCT_DATA,";
	for (my $i = 1 ; $i <= $#NEW_ARGS ; ++$i) {
		if ($NEW_ARGS[$i] =~ /$SIZE_PTN/) {
			$NEW_ARGS[$i] = $PTR . "$STRUCT_VAR->$STRUCT_SIZE";
		} elsif ($NEW_ARGS[$i] =~ /$CAP_PTN/) {
			$NEW_ARGS[$i] = $PTR . "$STRUCT_VAR->$STRUCT_CAP";
		}
		$func_args .= "$NEW_ARGS[$i],";
	}
	$func_args =~ s/,$//;
	$decl .= "$func_args);\n}\n";
	if ($decl !~ /$INLINE_MACRO/) {
		$decl =~ s/static/$INLINE_MACRO\nstatic/;
	}
	$decl =~ s/,\s*\)/)/g;
	$_    .= "\n\n";
	$decl .= "\n\n";
	$decl =~ s/$FUNC/$FUNC\_j/;
	$hpp .= $_;
	$hpp .= $decl;
	$h   .= $_;
	$h   .= $decl;
	next;
  NEXT:
	$_   .= "\n\n";
	$h   .= $_;
	$hpp .= $_;
}

$hpp =~ s/\.h"/.hpp"/g;
$hpp =~ s/H_DEF/HPP_DEF/g;
$hpp =~ s/EXTERN_C\s*\d/EXTERN_C 0/;
$hpp =~ s/NAMESPACE\s*\d/NAMESPACE 1/;

# $hpp =~ s/($NAMESPACE\_\w*)_mem(\w*\()/$1$2/g;
$hpp =~ s/$NAMESPACE\_(\w*\()/$1/g;
$hpp =~ s/\tt\(/\t$STR_STRUCT(/g;
$hpp =~ s/\t~t\(/\t$STR_STRUCT(/g;

# $hpp =~ s/alloc_append/alloc/g;
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
