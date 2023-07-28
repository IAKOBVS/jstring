#!/usr/bin/perl
use strict;
use warnings;

usage();

my $FNAME = $ARGV[0];
my $DIR_C = 'c';

update_needed();

my $DIR_CPP = $DIR_C . 'pp';
my $OUT_C   = "$DIR_C/$FNAME";
my $OUT_CPP = "$DIR_CPP/$FNAME" . 'pp';

my $NAMESPACE     = 'jstr';
my $NAMESPACE_BIG = uc($NAMESPACE);

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

my $file    = get_file_buf();
my $out_h   = '';
my $out_hpp = '';

my $RE_FUNC   = qr/^((?:\/\*|\/\/|$NAMESPACE_BIG\_|static)[^(){}]*($NAMESPACE\_\w*)\(([^\)]*\)[ \t]*\w*NOEXCEPT))/;
my @NEW_LINES = gen_nonmem_funcs(split(/\n\n/, $file));
gen_struct_funcs(@NEW_LINES);
print_to_file($OUT_C, $OUT_CPP);

sub usage
{
	if ($#ARGV != 0) {
		print('Usage: ./gen_func.pl <file>');
		exit(1);
	}
}

sub update_needed
{
	if (system("test $FNAME -nt $DIR_C/$FNAME || test $0 -nt $DIR_C")) {
		exit(1);
	}
}

sub get_file_buf
{
	open(my $FH, '<', $FNAME)
	  or die("Can't open $FNAME\n");
	my $file = '';
	while (<$FH>) {
		$file .= $_;
	}
	close($FH);
	return $file;
}

sub print_to_file
{
	my ($h, $hpp) = @_;
	open(my $FH, '>', $h)
	  or die("Can't open $out_h\n");
	print($FH $out_h);
	close($FH);
	open($FH, '>', $hpp)
	  or die("Can't open $out_hpp\n");
	print($FH $out_hpp);
	close($FH);
}

sub gen_nonmem_funcs
{
	my (@OLD_LINES) = @_;
	if ($FNAME =~ /builder/) {
		return @OLD_LINES;
	} else {
		my @NEW_LINES;
		foreach (split(/\n\n/, $file)) {
			if ($_ !~ $RE_FUNC) {
				goto NEXT;
			}
			my $decl      = $1;
			my $FUNC_NAME = $2;
			my $params    = $3;
			if (!$decl && !$FUNC_NAME && !$params) {
				goto NEXT;
			}
			if ($FUNC_NAME !~ /_mem[^0-9A-Za-z]/) {
				goto NEXT;
			}
			my $tmp = $FUNC_NAME;
			$tmp =~ s/_mem//;
			if ($file =~ /$tmp\(/) {
				goto NEXT;
			}
			$params =~ s/\)/,/;
			my $PTR      = ($decl =~ /\([^,)]*\*\*/) ? '&'       : '';
			my $RETURN   = ($_    =~ /return/)       ? 'return ' : '';
			my @OLD_ARGS = split(/\s/, $params);
			my @NEW_ARGS;
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
			$decl .= "\n{\n\t$RETURN$FUNC_NAME(";
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
		return @NEW_LINES;
	}
}

sub gen_struct_funcs
{
	my (@LINES) = @_;
	foreach (@LINES) {
		if ($_ !~ $RE_FUNC) {
			goto NEXT;
		}
		my $decl      = $1;
		my $FUNC_NAME = $2;
		my $params    = $3;
		if (!$decl && !$FUNC_NAME && !$params) {
			goto NEXT;
		}
		$params =~ s/\)/,/;
		my $HAS_SZ  = ($params =~ /$SIZE_PTN(?:,|\))/) ? 1 : 0;
		my $HAS_CAP = ($params =~ /$CAP_PTN(?:,|\))/)  ? 1 : 0;
		if (!$HAS_SZ && !$HAS_CAP) {
			goto NEXT;
		}
		my $RETURN = ($decl =~ /void/)         ? ''  : 'return ';
		my $PTR    = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
		if ($HAS_SZ && $decl =~ /\w*$SIZE_PTN(,|\))/) {
			if ($1 eq ')') {
				$decl =~ s/[^(,]*$SIZE_PTN(?:,|\))/)/;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*$SIZE_PTN,//;
			}
		}
		if ($HAS_CAP && $decl =~ /\w*$CAP_PTN(,|\))/) {
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
		$decl .= "\n{\n\t$RETURN$FUNC_NAME(";
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
		$decl =~ s/$FUNC_NAME/$FUNC_NAME\_j/;
		$out_hpp .= $_;
		$out_hpp .= $decl;
		$out_h   .= $_;
		$out_h   .= $decl;
		next;
	  NEXT:
		$_       .= "\n\n";
		$out_h   .= $_;
		$out_hpp .= $_;
	}
	$out_hpp =~ s/\.h"/.hpp"/g;
	$out_hpp =~ s/H_DEF/HPP_DEF/g;
	$out_hpp =~ s/EXTERN_C\s*\d/EXTERN_C 0/;
	$out_hpp =~ s/NAMESPACE\s*\d/NAMESPACE 1/;

	# $out_hpp =~ s/($NAMESPACE\_\w*)_mem(\w*\()/$1$2/g;
	$out_hpp =~ s/$NAMESPACE\_(\w*\()/$1/g;
	$out_hpp =~ s/\tt\(/\t$STR_STRUCT(/g;
	$out_hpp =~ s/\t~t\(/\t$STR_STRUCT(/g;
	$out_hpp =~ s/\n#if.*\s*#endif.*/\n/g;
	$out_hpp =~ s/\n\n\n/\n\n/g;
	$out_h   =~ s/\n\n\n/\n\n/g;
}
