#!/usr/bin/perl
use strict;
use warnings;

usage();

my $G_FNAME      = $ARGV[0];
my $G_FNAME_BASE = $G_FNAME;
$G_FNAME_BASE =~ s/^_//;
my $G_DIR_C = 'c';

# script_needed();

my $G_DIR_CPP            = $G_DIR_C . 'pp';
my $G_OUT_C              = "$G_DIR_C/$G_FNAME_BASE";
my $G_OUT_CPP            = "$G_DIR_CPP/$G_FNAME_BASE" . 'pp';
# my $G_IGNORE_FILE_NONMEM = 'builder.h';
my $G_IGNORE_FILE = 'private';

my $G_NMSPC      = 'jstr';
my $G_NMSPC_UPP  = uc($G_NMSPC);
my $G_STR_STRUCT = $G_NMSPC . '_t';
my $G_STRUCT_VAR = substr($G_STR_STRUCT, 0, 1);

my $G_STRUCT_DATA = 'data';
my $G_STRUCT_SIZE = 'size';
my $G_STRUCT_CAP  = 'cap';

my $G_CAP_PTN  = 'cap';
my $G_SIZE_PTN = 'sz';
my $G_LEN_PTN  = 'len';

my $G_MACRO_INLINE      = $G_NMSPC_UPP . '_INLINE';
my $G_MACRO_RESTRICT    = $G_NMSPC_UPP . '_RST';
my $G_MACRO_WARN_UNUSED = $G_NMSPC_UPP . '_WARN_UNUSED';

my $G_RE_FUNC = qr/[ \t]*((?:\/\*|\/\/|$G_NMSPC_UPP\_|static)[^{}()]*($G_NMSPC\_.*?)\(((?:.|\n)*?\)\s*\w*NOEXCEPT))/;

mkdir($G_DIR_CPP);
mkdir($G_DIR_C);

my $g_in_h = get_file_str($G_FNAME);
$g_in_h = tidy_newlines($g_in_h);

my @LNS = gen_nonmem_funcs($g_in_h);
my ($out_h, $out_hpp) = gen_struct_funcs(@LNS);
print_to_file($G_OUT_C, $out_h, $G_OUT_CPP, $out_hpp);

sub usage
{
	if ($#ARGV != 0) {
		print('Usage: ./gen_func.pl <file>');
		exit;
	}
}

sub script_needed
{
	if (system("test $G_FNAME -nt $G_DIR_C/$G_FNAME || test $0 -nt $G_DIR_C")) {
		exit;
	}
}

sub get_file_str
{
	my ($FILENAME) = @_;
	open(my $FH, '<', $FILENAME)
	  or die("Can't open $FILENAME\n");
	my $g_in_h = '';
	while (<$FH>) {
		$g_in_h .= $_;
	}
	close($FH);
	return $g_in_h;
}

sub tidy_newlines
{
	my ($IN_H) = @_;
	$IN_H =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$IN_H =~ s/\n\n\n*\t/\n\t/g;
	$IN_H =~ s/\n\n\n* /\n /g;
	return $IN_H;
}

sub print_to_file
{
	my ($OUT_C, $OUT_H, $OUT_CPP, $OUT_HPP) = @_;
	open(my $FH, '>', $OUT_C)
	  or die("Can't open $OUT_H\n");
	print($FH $OUT_H);
	close($FH);
	open($FH, '>', $OUT_CPP)
	  or die("Can't open $OUT_HPP\n");
	print($FH $OUT_HPP);
	close($FH);
}

sub gen_nonmem_funcs
{
	my ($FILE_STR) = @_;
	my @OLD_LNS = split(/\n\n/, $FILE_STR);
	my @NEW_LNS;
	foreach (@OLD_LNS) {
		if ($_ !~ $G_RE_FUNC) {
			goto NEXT;
		}
		my $decl      = $1;
		my $FUNC_NAME = $2;
		my $params    = $3;
		if (!$decl && !$FUNC_NAME && !$params) {
			goto NEXT;
		}
		if ($FUNC_NAME !~ /$G_NMSPC\_/) {
			goto NEXT;
		}
		if ($FUNC_NAME !~ /_mem(?:_|$)/) {
			goto NEXT;
		}
		my $tmp = $FUNC_NAME;
		$tmp =~ s/_mem//;
		if ($g_in_h =~ /$tmp\(/) {
			goto NEXT;
		}
		if ($G_FNAME =~ /$G_IGNORE_FILE/) {
			goto NEXT;
		}
		my $PTR    = ($decl =~ /\([^,)]*\*\*/) ? '&'       : '';
		my $RETURN = ($_    =~ /return/)       ? 'return ' : '';
		$params =~ s/\)/,/;
		my @OLD_ARGS = split(/\s/, $params);
		my @new_args;
		foreach (@OLD_ARGS) {
			if (/,$/) {
				s/,//;
				push(@new_args, $_);
			}
		}
		$decl =~ s/($G_NMSPC\_\w*)_mem(\w*\()/$1$2/o;
		my $func_args;
		for (my $i = 0 ; $i <= $#new_args ; ++$i) {
			$func_args .= $PTR . "$new_args[$i], ";
		}
		$decl .= "\n{\n\t$RETURN$FUNC_NAME(";
		my $G_LEN = ($params =~ /$G_LEN_PTN/o) ? 1 : 0;
		foreach (@new_args) {
			if ($G_LEN) {
				if (/(\w*)$G_LEN_PTN/) {
					my $var = $1;
					$decl =~ s/,[^,]*$G_LEN_PTN//o;
					$_ = "strlen($var)";
				}
			} else {
				if (/\w*$G_SIZE_PTN/) {
					$decl =~ s/,[^,]*$G_SIZE_PTN//o;
					$_ = "strlen($new_args[0])";
				}
			}
			$decl .= "$_, ";
		}
		$decl =~ s/, $//;
		$decl .= ");\n}\n";
		push(@NEW_LNS, $_);
		push(@NEW_LNS, $decl);
		next;
	  NEXT:
		push(@NEW_LNS, $_);
	}
	return @NEW_LNS;
}

sub gen_struct_funcs
{
	my (@LINES) = @_;
	my $out_h;
	my $out_hpp;

	# my $skel;
	foreach (@LINES) {
		if ($_ !~ $G_RE_FUNC) {
			goto NEXT;
		}
		my $decl      = $1;
		my $FUNC_NAME = $2;
		my $params    = $3;
		if (!$decl && !$FUNC_NAME && !$params) {
			goto NEXT;
		}

		# if ($G_FNAME !~ /$G_IGNORE_FILE_NONMEM/) {
		# 	$skel .= "$decl;\n\n";
		# }
		$params =~ s/\)/,/;
		my $HAS_SZ  = ($params =~ /$G_SIZE_PTN(?:,|\))/o) ? 1 : 0;
		my $HAS_CAP = ($params =~ /$G_CAP_PTN(?:,|\))/o)  ? 1 : 0;
		if (!$HAS_SZ && !$HAS_CAP) {
			goto NEXT;
		}
		if ($FUNC_NAME !~ /$G_NMSPC\_/) {
			goto NEXT;
		}
		if ($G_FNAME =~ /$G_IGNORE_FILE/) {
			goto NEXT;
		}
		if ($params =~ /\.\.\./) {
			goto NEXT;
		}
		my $RETURN           = ($decl =~ /void/) ? '' : 'return ';
		my $RETURNS_PTR_FUNC = 0;
		$decl =~ s/$FUNC_NAME/$FUNC_NAME\_j/;
		if ($decl !~ /$G_MACRO_INLINE/) {
			$decl =~ s/static/$G_MACRO_INLINE\nstatic/o;
		}
		if ($FUNC_NAME =~ /_p(?:_|$)/) {
			$decl =~ s/.*Return value:(?:.|\n)*?(\*\/|\/\/)/$1/;
			$decl =~ s/_p//;
			$decl =~ s/static\s*(?:char|void)\s\*/static void /;
			$decl =~ s/[ \t]*$G_MACRO_WARN_UNUSED[ \t]*\n//;
			$RETURN           = '';
			$RETURNS_PTR_FUNC = 1;
		}
		my $PTR = ($decl =~ /\([^,)]*\*\*/) ? '&' : '';
		if ($HAS_SZ && $decl =~ /\w*$G_SIZE_PTN(,|\))/o) {
			if ($1 eq ')') {
				$decl =~ s/[^(,]*$G_SIZE_PTN(?:,|\))/)/o;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*$G_SIZE_PTN,//o;
			}
		}
		if ($HAS_CAP && $decl =~ /\w*$G_CAP_PTN(,|\))/o) {
			if ($1 eq ')') {
				$decl =~ s/[^(,]*$G_CAP_PTN(?:,|\))/o)/;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*$G_CAP_PTN,//o;
			}
		}
		my @OLD_ARGS = split(/\s/, $params);
		my $CONST    = ($OLD_ARGS[0] eq 'const') ? 'const ' : '';
		my $LAST     = ($params =~ /,/)          ? ','      : ')';
		my $tmp      = "($G_STR_STRUCT *$G_MACRO_RESTRICT $CONST" . "j$LAST";
		$decl =~ s/\(.+?$LAST/$tmp/;
		$decl =~ s/,\s*\)/)/g;
		$decl .= "\n{\n\t";
		my $body = "$RETURN$FUNC_NAME(";
		my @new_args;

		foreach (@OLD_ARGS) {
			if (/,$/) {
				s/,//;
				push(@new_args, $_);
			}
		}
		$body .= $PTR . "$G_STRUCT_VAR->$G_STRUCT_DATA, ";
		for (my $i = 1 ; $i <= $#new_args ; ++$i) {
			if ($new_args[$i] =~ /$G_SIZE_PTN/o) {
				$new_args[$i] = $PTR . "$G_STRUCT_VAR->$G_STRUCT_SIZE";
			} elsif ($new_args[$i] =~ /$G_CAP_PTN/o) {
				$new_args[$i] = $PTR . "$G_STRUCT_VAR->$G_STRUCT_CAP";
			}
			$body .= "$new_args[$i], ";
		}
		$body =~ s/, $//;
		$body .= ")";
		if ($RETURNS_PTR_FUNC) {
			$body = "$G_STRUCT_VAR->$G_STRUCT_SIZE = $body";
			$body .= " - $G_STRUCT_VAR->$G_STRUCT_DATA";
		}
		$decl    .= "$body;\n}\n";
		$_       .= "\n\n";
		$decl    .= "\n\n";
		$out_hpp .= $_;
		$out_hpp .= $decl;
		$out_h   .= $_;
		$out_h   .= $decl;
		next;
	  NEXT:
		$_ = update_includes($_);
		$_       .= "\n\n";
		$out_h   .= $_;
		$out_hpp .= $_;
	}

	# if ($skel) {
	# 	$out_h =~ s/(namespace(?:.|\n)*?\n\n)/$1$skel/;
	# 	$out_hpp =~ s/(namespace(?:.|\n)*?\n\n)/$1$skel/;
	# }
	$out_hpp =~ s/\.h"/.hpp"/g;
	$out_hpp =~ s/H_DEF/HPP_DEF/g;
	$out_hpp =~ s/$G_NMSPC_UPP\_EXTERN_C\s*\d/$G_NMSPC_UPP\_EXTERN_C 0/;
	$out_hpp =~ s/$G_NMSPC_UPP\_NAMESPACE\s*\d/$G_NMSPC_UPP\_NAMESPACE 1/;
	$out_hpp =~ s/(\W)$G_NMSPC\_(\w*\()/$1$2/go;
	$out_hpp =~ s/\tt\(/\t$G_STR_STRUCT(/go;
	$out_hpp =~ s/\t~t\(/\t$G_STR_STRUCT(/go;
	$out_hpp =~ s/\n#if.*\s*#endif.*/\n/g;
	$out_hpp =~ s/\n\n\n/\n\n/g;
	$out_h   =~ s/\n\n\n/\n\n/g;
	$out_h   =~ s/\n\n*$/\n/g;
	$out_hpp =~ s/\n\n*$/\n/g;
	return ($out_h, $out_hpp);
}

sub update_includes
{
	my ($includes) = @_;
	$includes =~ s/((?:^|\n)[ \t]*#[ \t]*include[ \t]*")_$G_NMSPC/$1$G_NMSPC/g;
	return $includes;
}
