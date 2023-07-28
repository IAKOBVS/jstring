#!/usr/bin/perl
use strict;
use warnings;

if ($#ARGV != 0) {
	print "Usage: ./$0 <file>";
	exit 1;
}

my $FNAME         = $ARGV[0];
my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = uc($NAMESPACE);
my $DIR_C         = 'c';
my $DIR_CPP       = $DIR_C . 'pp';
mkdir($DIR_CPP);
mkdir($DIR_C);

# if (system("test $0 -ot $FNAME && test $FNAME -nt $DIR_C/$FNAME")) {
# 	exit 1;
# }

open(my $FH, '<', $FNAME)
  or die "Can't open $FNAME\n";
my $file = '';
while (<$FH>) {
	$file .= $_;
}
close($FH);

my $namespace_end = '';
my $undef         = '';
my $endif         = '';

my @OLD_LINES = split(/\n\n/, $file);
my @NEW_LINES;

foreach (@OLD_LINES) {
	if ($FNAME =~ /$NAMESPACE\_builder/) {
		goto NEXT;
	}
	if (!/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*_mem\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/) {
		goto NEXT;
	}
	my $decl   = $1;
	my $FUNC   = $2;
	my $params = $3;
	if (!$decl && !$FUNC && !$params) {
		goto NEXT;
	}
	my $FUNC_BASENAME = $FUNC;
	$FUNC_BASENAME =~ s/_mem//;
	if ($file =~ /$FUNC_BASENAME\(/) {
		goto NEXT;
	}
	$params =~ s/\)/,/;
	my @OLD_ARGS = split(/\s/, $params);
	my @NEW_ARGS;
	my $PTR    = get_ptr_str($decl);
	my $RETURN = get_return_str($decl);
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
	my $LEN = has_arg($params, 'len');
	foreach (@NEW_ARGS) {
		if ($LEN) {
			if (/(\w*)len/) {
				my $var = $1;
				$decl =~ s/,[^,]*len//;
				$_ = "strlen($var)";
			}
		} else {
			if (/\w*sz/) {
				$decl =~ s/,[^,]*sz//;
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

my $h;
my $hpp;
my $has_funcs = 0;
my $in_ifdef  = 0;

foreach (@NEW_LINES) {
	if (!/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/) {
		goto NEXT;
	}
	my $decl   = $1;
	my $FUNC   = $2;
	my $params = $3;
	if (!$decl && !$FUNC && !$params) {
		goto NEXT;
	}
	$params =~ s/\)/,/;
	my $SZ  = has_arg($params, 'sz');
	my $CAP = has_arg($params, 'cap');
	if (!$SZ && !$CAP) {
		goto NEXT;
	}
	$has_funcs = 1;
	my $RETURN = get_return_str($decl);
	my $PTR    = get_ptr_str($decl);
	$decl = handle_sz_cap($SZ, $CAP, $decl);
	my @OLD_ARGS = split(/\s/, $params);
	my $CONST    = get_const_str(@OLD_ARGS);
	my $LAST     = ($params =~ /,/) ? ',' : ')';
	my $tmp      = "($NAMESPACE\_t *$NAMESPACE_BIG\_RST $CONST" . "j$LAST";
	$decl =~ s/\(.+?$LAST/$tmp/;
	$decl .= "\n{\n\t$RETURN$FUNC(";

	my @NEW_ARGS;
	foreach (@OLD_ARGS) {
		if (/,$/) {
			s/,//;
			push(@NEW_ARGS, $_);
		}
	}
	my $func_args = $PTR . "j->data,";
	for (my $i = 1 ; $i <= $#NEW_ARGS ; ++$i) {
		if ($NEW_ARGS[$i] =~ /sz/) {
			$NEW_ARGS[$i] = $PTR . "j->size";
		} elsif ($NEW_ARGS[$i] =~ /cap/) {
			$NEW_ARGS[$i] = $PTR . "j->cap";
		}
		$func_args .= "$NEW_ARGS[$i],";
	}
	$func_args =~ s/,$//;
	$decl .= "$func_args);\n}\n";
	$decl = add_inline($decl);
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
$hpp =~ s/\tt\(/\t$NAMESPACE\_t(/g;
$hpp =~ s/\t~t\(/\t$NAMESPACE\_t(/g;

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

sub handle_sz_cap {
	my ($SZ, $CAP, $decl) = @_;
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
	return $decl;
}

sub has_arg {
	my ($params, $argname) = @_;
	return ($params =~ /$argname(?:,|\))/) ? 1 : 0;
}

sub add_inline {
	my ($decl) = @_;
	if ($decl !~ /$NAMESPACE_BIG\_INLINE/) {
		$decl =~ s/static/$NAMESPACE_BIG\_INLINE\nstatic/;
	}
	return $decl;
}
