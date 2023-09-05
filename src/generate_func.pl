#!/usr/bin/perl
use strict;
use warnings;

usage();

my $G_FNAME      = $ARGV[0];
my $G_FNAME_BASE = $G_FNAME;
$G_FNAME_BASE =~ s/^_//;
my $G_DIR_C = 'jstr';

script_needed();

my $G_OUT_C = "$G_DIR_C/$G_FNAME_BASE";

my $G_IGNORE_FILE = 'private';

my $G_NMSPC_REGEX = 'jreg';
my $G_NMSPC       = 'jstr';
my $G_NMSPC_UPP   = uc($G_NMSPC);
my $G_STR_STRUCT  = 'jstr_ty';
my $G_STRUCT_VAR  = 'j';

my $G_STRUCT_DATA = 'data';
my $G_STRUCT_SIZE = 'size';
my $G_STRUCT_CAP  = 'cap';

my $G_CAP_PTN  = 'cap';
my $G_SIZE_PTN = 'sz';
my $G_LEN_PTN  = 'len';

my $G_MACRO_INLINE          = $G_NMSPC_UPP . '_INLINE';
my $G_MACRO_MAYBE_UNUSED    = $G_NMSPC_UPP . '_MAYBE_UNUSED';
my $G_MACRO_RESTRICT        = $G_NMSPC_UPP . '_RST';
my $G_MACRO_WARN_UNUSED     = $G_NMSPC_UPP . '_WARN_UNUSED';
my $G_MACRO_RETURNS_NONNULL = $G_NMSPC_UPP . '_RETURNS_NONNULL';
my $G_MACRO_LONG_FUNCTION   = $G_NMSPC_UPP . '_LONG_FUNCTION';

my $G_RE_FUNC   = qr/[ \t]*((?:\/\*|\/\/|$G_NMSPC_UPP\_|static)\s+\w+\s+(\w*(?:$G_NMSPC|$G_NMSPC_REGEX)\_.*?)\(((?:.|\n)*?\)\s*\w*NOEXCEPT))/;
my $G_RE_DEFINE = qr/\([^)]*\)[^{]*{[^}]*}/;

my $G_LEN_FUNC_SUFFIX = '_len';

# mkdir($G_DIR_CPP);
mkdir($G_DIR_C);

my $g_in_h = get_file_str($G_FNAME);
$g_in_h = tidy_newlines($g_in_h);

my @LNS = gen_nonmem_funcs($g_in_h);
my ($out_h) = gen_struct_funcs(@LNS);

print_to_file($G_OUT_C, $out_h);

sub usage
{
	if ($#ARGV != 0) {
		print('Usage: ./gen_func.pl <file>');
		exit;
	}
}

sub script_needed
{
	if (system("test $G_FNAME -nt $G_DIR_C/$G_FNAME")) {
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
	my ($OUT_C, $BUF) = @_;
	open(my $FH, '>', $OUT_C)
	  or die("Can't open $OUT_C\n");
	print $FH $BUF;
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
		if ($FUNC_NAME !~ /$G_LEN_FUNC_SUFFIX(?:_|$)/) {
			goto NEXT;
		}
		my $tmp = $FUNC_NAME;
		$tmp =~ s/$G_LEN_FUNC_SUFFIX//;
		if ($g_in_h =~ /$tmp\(/) {
			goto NEXT;
		}
		if ($G_FNAME =~ /$G_IGNORE_FILE/o) {
			goto NEXT;
		}
		if ($_ !~ $G_RE_DEFINE) {
			goto NEXT;
		}
		if ($FUNC_NAME =~ /^p/) {
			goto NEXT;
		}
		if ($decl !~ /$G_MACRO_INLINE[^_]/o) {
			$decl =~ s/static/$G_MACRO_INLINE\n$G_MACRO_MAYBE_UNUSED\nstatic/o;
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
		my $nmspc = ($decl =~ /$G_NMSPC/o) ? $G_NMSPC : $G_NMSPC_REGEX;
		$decl =~ s/($nmspc\_\w*)$G_LEN_FUNC_SUFFIX(\w*\()/$1$2/o;
		$decl .= "\n{\n\t";
		my $size_ptr_var = get_regex_size_ptr($FUNC_NAME, $params);
		$decl .= "$RETURN$FUNC_NAME(";
		my $G_LEN = ($params =~ /$G_LEN_PTN/o) ? 1 : 0;
		foreach (@new_args) {
			if ($G_LEN) {
				if (/(\w*)$G_LEN_PTN/) {
					my $var = $1;
					$decl =~ s/,[^,]*$G_LEN_PTN//o;
					$_ = "strlen($var)";
				}
			} else {
				if (!$size_ptr_var && /\w*$G_SIZE_PTN/) {
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
		$params =~ s/\)/,/;
		my $HAS_SZ  = ($params =~ /$G_SIZE_PTN(?:,|\))/o) ? 1 : 0;
		my $HAS_CAP = ($params =~ /$G_CAP_PTN(?:,|\))/o)  ? 1 : 0;
		if (!$HAS_SZ && !$HAS_CAP) {
			goto NEXT;
		}
		if ($G_FNAME =~ /$G_IGNORE_FILE/o) {
			goto NEXT;
		}
		if ($params =~ /\.\.\./) {
			goto NEXT;
		}
		if ($_ !~ $G_RE_DEFINE) {
			goto NEXT;
		}
		if ($FUNC_NAME =~ /^p/) {
			goto NEXT;
		}
		if ($g_in_h =~ /$FUNC_NAME/) {
			goto NEXT;
		}
		my $RETURN = ($decl =~ /void/) ? '' : 'return ';
		if ($RETURN) {
			$decl =~ s/$G_MACRO_RETURNS_NONNULL//;
		}
		my $RETURNS_END_PTR = 0;
		$decl =~ s/$FUNC_NAME/$FUNC_NAME\_j/;
		if ($decl !~ /$G_MACRO_INLINE[^_]/o) {
			$decl =~ s/static/$G_MACRO_INLINE\n$G_MACRO_MAYBE_UNUSED\nstatic/o;
		}
		if ($FUNC_NAME =~ /_p(?:_|$)/) {
			$decl =~ s/.*Return value:(?:.|\n)*?(\*\/|\/\/)/$1/;
			$decl =~ s/_p//;
			$decl =~ s/static\s*(?:char|void)\s\*/static void /;
			$decl =~ s/[ \t]*$G_MACRO_WARN_UNUSED[ \t]*\n//o;
			$RETURN          = '';
			$RETURNS_END_PTR = 1;
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
				$decl =~ s/[^(,]*$G_CAP_PTN(?:,|\))/)/o;
			} elsif ($1 eq ',') {
				$decl =~ s/[^(,]*$G_CAP_PTN,//o;
			}
		}
		my @OLD_ARGS         = split(/\s/, $params);
		my $CONST            = ($OLD_ARGS[0] =~ 'const')                     ? 'const ' : '';
		my $CONST_STRUCT_PTR = ($params      =~ /[^\n]*const[^\n]*(?:,|\))/) ? 'const ' : '';
		my $LAST             = ($params      =~ /,/)                         ? ','      : ')';
		my $tmp              = "($CONST$G_STR_STRUCT *$G_MACRO_RESTRICT $CONST_STRUCT_PTR" . "j$LAST";
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
		if (is_str_ptr($params)) {
			$body .= $PTR;
		}
		$body .= "$G_STRUCT_VAR->$G_STRUCT_DATA, ";
		for (my $i = 1 ; $i <= $#new_args ; ++$i) {
			if ($new_args[$i] =~ /$G_SIZE_PTN/o) {
				if ($FUNC_NAME =~ /$G_NMSPC_REGEX/ && is_size_ptr($params)) {
					$new_args[$i] = "&$G_STRUCT_VAR->$G_STRUCT_SIZE";
				} else {
					$new_args[$i] = $PTR . "$G_STRUCT_VAR->$G_STRUCT_SIZE";
				}
			} elsif ($new_args[$i] =~ /$G_CAP_PTN/o) {
				$new_args[$i] = $PTR . "$G_STRUCT_VAR->$G_STRUCT_CAP";
			}
			$body .= "$new_args[$i], ";
		}
		$body =~ s/, $//;
		$body .= ")";
		if ($RETURNS_END_PTR) {
			$body = "$G_STRUCT_VAR->$G_STRUCT_SIZE = $body";
			$body .= " - $G_STRUCT_VAR->$G_STRUCT_DATA";
		}
		$decl .= "$body;\n}\n";
		$decl =~ s/\n\n/\n/g;
		$_     .= "\n\n";
		$decl  .= "\n\n";
		$out_h .= $_;
		$out_h .= $decl;
		next;
	  NEXT:
		$_     .= "\n\n";
		$out_h .= $_;
	}
	$out_h =~ s/\n\n\n/\n\n/g;
	$out_h =~ s/\n\n*$/\n/g;
	return ($out_h);
}

# sub update_includes
# {
# 	my ($includes) = @_;
# 	$includes =~ s/((?:^|\n)[ \t]*#[ \t]*include[ \t]*")_/$1/go;
# 	return $includes;
# }

sub get_regex_size_ptr
{
	my ($FUNC_NAME, $params) = @_;
	if ($FUNC_NAME =~ /$G_NMSPC_REGEX/o) {
		$params =~ /size_t[ \t]*\*[ \t]*$G_MACRO_RESTRICT[ \t][^\n]*(\w*sz)/o;
		return $1;
	}
	return 0;
}

sub is_str_ptr
{
	my ($params) = @_;
	return ($params =~ /[^,\)]*\*\*[^,\)]*s[,\)]/o) ? 1 : 0;
}

sub is_size_ptr
{
	my ($params) = @_;
	return ($params =~ /size_t[^,\)]*\*[^,\)]*sz[,\)]/) ? 1 : 0;
}
