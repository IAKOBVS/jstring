#!/usr/bin/perl
use strict;
use warnings;

if ($#ARGV != 0) {
	print "Usage: ./$0 <file>";
	exit 1;
}

my $FNAME     = $ARGV[0];
my $NMSPC     = 'jstr';
my $NMSPC_BIG = uc($NMSPC);
my $DIR_C     = 'c';
my $DIR_CPP   = $DIR_C . 'pp';
mkdir($DIR_CPP);
mkdir($DIR_C);

if (system("test $0 -ot $FNAME && test $FNAME -nt $DIR_C/$FNAME")) {
	exit 1;
}

my $STRUCT_VAR  = 'j';
my $STRUCT_SIZE = 'size';
my $STRUCT_CAP  = 'cap';
my $STRUCT_DATA = 'data';

my $SIZE_PTN = 'sz';
my $CAP_PTN  = 'cap';
my $LEN_PTN  = 'len';

my $STR_STRUCT = $NMSPC . '_t';

open(my $FH, '<', $FNAME)
  or die "Can't open $FNAME\n";
my $file = '';
while (<$FH>) {
	$file .= $_;
}
close($FH);

my $RE_FUNC = qr/^((?:\/\*|\/\/|$NMSPC_BIG\_|static)[^(){}]*($NMSPC\_\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/;

my @lines;
foreach (split(/\n\n/, $file)) {
	if ($FNAME =~ /$NMSPC\_builder/) {
		goto NEXT;
	}
	if (!$RE_FUNC) {
		goto NEXT;
	}
	my $DECL      = $1;
	my $FUNC_NAME = $2;
	my $params    = $3;
	if (!$DECL && !$FUNC_NAME && !$params) {
		goto NEXT;
	}
	$params =~ s/\)/,/;
	my $decl = $DECL;
	if ($FUNC_NAME !~ /_mem/) {
		next;
	}
	my $tmp = $FUNC_NAME;
	$tmp =~ s/_mem//;
	if ($file =~ /$tmp\(/) {
		goto NEXT;
	}
	my $PTR      = get_ptr_str($DECL);
	my $RETURN   = get_return_str($DECL);
	my @OLD_ARGS = split(/\s/, $params);
	my @NEW_ARGS;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@NEW_ARGS, $_);
		}
	}
	my $func_args;
	for (my $i = 0 ; $i <= $#NEW_ARGS ; ++$i) {
		$func_args .= $PTR . "$NEW_ARGS[$i],";
	}
	$decl =~ s/($NMSPC\_\w*)_mem(\w*\()/$1$2/;
	$decl .= "\n{\n\t$RETURN$FUNC_NAME(";
	my $LEN = has_arg($params, $LEN_PTN);
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
	push(@lines, $_);
	push(@lines, $decl);
	next;
  NEXT:
	push(@lines, $_);
}

my $h   = '';
my $hpp = '';

foreach (@lines) {
	if (!$RE_FUNC) {
		goto NEXT;
	}
	my $DECL   = $1;
	my $FUNC   = $2;
	my $params = $3;
	if (!$DECL && !$FUNC && !$params) {
		goto NEXT;
	}
	my $decl = $DECL;
	$params =~ s/\)/,/;
	my $SZ  = has_arg($params, 'sz');
	my $CAP = has_arg($params, 'cap');
	if (!$SZ && !$CAP) {
		goto NEXT;
	}
	my @OLD_ARGS = split(/\s/, $params);
	my $CONST    = get_const_str(@OLD_ARGS);
	my $RETURN   = get_return_str($DECL);
	my $PTR      = get_ptr_str($DECL);
	my $LAST     = ($params =~ /,/) ? ',' : ')';
	my $tmp      = "($STR_STRUCT *$NMSPC_BIG\_RST $CONST" . "$STRUCT_VAR$LAST";
	$decl = rm_arg($decl, $SZ,  $SIZE_PTN);
	$decl = rm_arg($decl, $CAP, $CAP_PTN);
	$decl = add_inline($decl);
	$decl =~ s/\(.+?$LAST/$tmp/;
	$decl .= "\n{\n\t$RETURN$FUNC(";

	my @new_args;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@new_args, $_);
		}
	}
	my $func_args = $PTR . "$STRUCT_VAR->$STRUCT_DATA,";
	for (my $i = 1 ; $i <= $#new_args ; ++$i) {
		if ($new_args[$i] =~ /$SIZE_PTN/) {
			$new_args[$i] = $PTR . "$STRUCT_VAR->$STRUCT_SIZE";
		} elsif ($new_args[$i] =~ /$CAP_PTN/) {
			$new_args[$i] = $PTR . "$STRUCT_VAR->$STRUCT_CAP";
		}
		$func_args .= "$new_args[$i],";
	}
	$func_args =~ s/,$//;
	$decl .= "$func_args);\n}\n";
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
$hpp =~ s/NMSPC\s*\d/NMSPC 1/;

# $hpp =~ s/($NMSPC\_\w*)_mem(\w*\()/$1$2/g;
$hpp =~ s/$NMSPC\_(\w*\()/$1/g;
$hpp =~ s/\tt\(/\t$NMSPC\_t(/g;
$hpp =~ s/\t~t\(/\t$NMSPC\_t(/g;

# $hpp =~ s/alloc_append/alloc/g;
$hpp =~ s/\n#if.*\s*#endif.*/\n/g;
$hpp =~ s/\n\n\n/\n\n/g;
$h   =~ s/\n\n\n/\n\n/g;

open($FH, '>', "$DIR_C/$FNAME")
  or die "Can't open $DIR_C/$FNAME\n";
print($FH $h);
close($FH);
open($FH, '>', "$DIR_CPP/$FNAME" . 'pp')
  or die "Can't open $DIR_CPP/$FNAME" . "pp\n";
print($FH $hpp);
close($FH);

sub get_return_str {
	my ($decl) = @_;
	return ($decl =~ /void/) ? '' : 'return ';
}

sub get_ptr_str {
	my ($decl) = @_;
	return ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
}

sub get_const_str {
	my (@OLD_ARGS) = @_;
	($OLD_ARGS[0] eq 'const') ? 'const ' : '';
}

sub rm_arg {
	my ($decl, $has_arg, $arg) = @_;
	if ($has_arg && $decl =~ /\w*$arg(,|\))/) {
		if ($1 eq ')') {
			$decl =~ s/[^(,]*$arg(?:,|\))/)/;
		} elsif ($1 eq ',') {
			$decl =~ s/[^(,]*$arg,//;
		}
	}
}

sub add_inline {
	my ($decl) = @_;
	if ($decl !~ /$NMSPC_BIG\_INLINE/) {
		$decl =~ s/static/$NMSPC_BIG\_INLINE\nstatic/;
	}
	return $decl;
}
