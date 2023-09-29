#!/usr/bin/perl
use strict;
use warnings;

usage();

my $G_FNAME = $ARGV[0];
my $G_DIR_C = 'jstr';

my $G_NMSPC      = 'jstr';
my $G_NMSPC_UPP  = uc($G_NMSPC);
my $G_STR_STRUCT = 'jstr_ty';
my $G_STRUCT_VAR = 'j';

my $G_STRUCT_DATA = 'data';
my $G_STRUCT_SIZE = 'size';
my $G_STRUCT_CAP  = 'capacity';

my $G_CAP_PTN  = 'cap';
my $G_SIZE_PTN = 'sz';
my $G_LEN_PTN  = 'len';

my $G_MCR_INLINE          = $G_NMSPC_UPP . '_INLINE';
my $G_MCR_MAYBE_UNUSED    = $G_NMSPC_UPP . '_MAYBE_UNUSED';
my $G_MCR_RESTRICT        = $G_NMSPC_UPP . '_RESTRICT';
my $G_MCR_WARN_UNUSED     = $G_NMSPC_UPP . '_WARN_UNUSED';
my $G_MCR_RETURNS_NONNULL = $G_NMSPC_UPP . '_RETURNS_NONNULL';

my $G_RE_FN     = qr/[ \t]*((?:\/\*|\/\/|$G_NMSPC_UPP\_|static)\s+\w+\s+(\w*(?:$G_NMSPC)\_.*?)\(((?:.|\n)*?\)\s*\w*NOEXCEPT))/;
my $G_RE_DEFINE = qr/\([^)]*\)[^{]*{[^}]*}/;

my $G_LEN_FN_SUFFIX = '_len';

my $g_in_h = get_file_str($G_FNAME);
$g_in_h = tidy_newlines($g_in_h);

my @LNS = gen_nonlen_funcs($g_in_h);
my ($out_h) = gen_struct_funcs(@LNS);

print $out_h;

sub usage
{
	if ($#ARGV != 0) {
		print('Usage: ./gen_func.pl <file>');
		exit;
	}
}

sub get_file_str
{
	my ($FILENAME) = @_;
	my $g_in_h = '';
	my @lines;
	open(my $FH, '<', $FILENAME)
	  or die("Can't open $FILENAME\n");
	while (<$FH>) {
		$g_in_h .= $_;
		push(@lines, $_);
	}
	close($FH);
	my @def;
	my @undef;

	# prefix temporary macros with P_JSTR_ to avoid naming conflicts
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]*([_A-Z0-9]*)/) {
			my $macro = $1;
			if ($macro !~ /^(?:P_JSTR|JSTR|pjstr|jstr)/) {
				$g_in_h =~ s/([^'"_0-9A-Za-z]|^)$macro([^'"_0-9A-Za-z]|$)/$1P_JSTR_$macro$2/g;
			}
		}
	}
	return $g_in_h;
}

sub tidy_newlines
{
	my ($in_h) = @_;
	$in_h =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$in_h =~ s/\n\n\n*\t/\n\t/g;
	$in_h =~ s/\n\n\n* /\n /g;
	return $in_h;
}

sub gen_nonlen_funcs
{
	my ($FILE_STR) = @_;
	my @OLD_LNS = split(/\n\n/, $FILE_STR);
	my @new_lns;
	foreach (@OLD_LNS) {
		my $decl   = $1;
		my $FN     = $2;
		my $params = $3;
		if (   ($_ !~ $G_RE_FN)
			|| (!$decl && !$FN && !$params)
			|| ($FN !~ /$G_NMSPC[_0-9_A-Za-z]*$G_LEN_FN_SUFFIX(?:_|$)/o))
		{
			goto CONT;
		}
		my $tmp = $FN;
		$tmp =~ s/$G_LEN_FN_SUFFIX//o;
		if (   ($g_in_h =~ /$tmp\(/)
			|| ($_  !~ $G_RE_DEFINE)
			|| ($FN =~ /^p/))
		{
			goto CONT;
		}
		if ($decl !~ /$G_MCR_INLINE[^_]/o) {
			$decl =~ s/static/$G_MCR_INLINE\n$G_MCR_MAYBE_UNUSED\nstatic/o;
		}
		my $PTR    = ($decl =~ /\*.*\*/) ? '&'       : '';
		my $RETURN = ($_    =~ /return/) ? 'return ' : '';
		$params =~ s/\)/,/;
		my @OLD_ARGS = split(/\s/, $params);
		my @new_args;
		foreach (@OLD_ARGS) {
			if (/,$/) {
				s/,//;
				push(@new_args, $_);
			}
		}
		$decl =~ s/($G_NMSPC\_\w*)$G_LEN_FN_SUFFIX(\w*\()/$1$2/o;
		$decl .= "\n{\n\t";
		my $size_ptr_var = get_regex_size_ptr($FN, $params);
		$decl .= "$RETURN$FN(";
		my $G_LEN = ($params =~ /$G_LEN_PTN/o) ? 1 : 0;
		foreach (@new_args) {
			if ($G_LEN) {
				if (/(\w*)$G_LEN_PTN/) {
					my $var = $1;
					$decl =~ s/,[ \t\n_0-9A-Za-z]*$G_LEN_PTN//o;
					$_ = "strlen($var)";
				}
			} else {
				if (!$size_ptr_var && /\w*$G_SIZE_PTN/) {
					$decl =~ s/,[ \t\n_0-9A-Za-z]*$G_SIZE_PTN//o;
					$_ = "strlen($new_args[0])";
				}
			}
			$decl .= "$_, ";
		}
		$decl =~ s/, $//;
		$decl .= ");\n}\n";
		push(@new_lns, $_);
		push(@new_lns, $decl);
		next;
	  CONT:
		push(@new_lns, $_);
	}
	return @new_lns;
}

sub gen_struct_funcs
{
	my (@LINES) = @_;
	my $out_h;
	foreach (@LINES) {
		my $decl   = $1;
		my $FN     = $2;
		my $params = $3;
		if (   ($_ !~ $G_RE_FN)
			|| (!$decl && !$FN && !$params))
		{
			goto CONT;
		}
		$params =~ s/\)/,/;
		my $HAS_SZ  = ($params =~ /$G_SIZE_PTN(?:,|\))/o) ? 1 : 0;
		my $HAS_CAP = ($params =~ /$G_CAP_PTN(?:,|\))/o)  ? 1 : 0;
		if (   (!$HAS_SZ && !$HAS_CAP)
			|| ($params =~ /\.\.\./)
			|| ($_      !~ $G_RE_DEFINE)
			|| ($FN     =~ /^p/)
			|| ($g_in_h =~ /$FN\_j/))
		{
			goto CONT;
		}
		my $RETURN = ($decl =~ /void/) ? '' : 'return ';
		if ($RETURN) {
			$decl =~ s/$G_MCR_RETURNS_NONNULL//;
		}
		my $RETURNS_END_PTR = 0;
		$decl =~ s/$FN/$FN\_j/;
		if ($decl !~ /$G_MCR_INLINE[^_]/o) {
			$decl =~ s/static/$G_MCR_INLINE\n$G_MCR_MAYBE_UNUSED\nstatic/o;
		}
		if ($FN =~ /_p(?:_|$)/) {
			$decl =~ s/.*Return value:(?:.|\n)*?(\*\/|\/\/)/$1/;
			$decl =~ s/_p//;
			$decl =~ s/static\s*(?:char|void)\s\*/static void /;
			$decl =~ s/[ \t]*$G_MCR_WARN_UNUSED[ \t]*\n//o;
			$RETURN          = '';
			$RETURNS_END_PTR = 1;
		}
		my $PTR = ($decl =~ /\([^,)]*\*.*\*/) ? '&' : '';
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
		my $tmp              = "($CONST$G_STR_STRUCT *$G_MCR_RESTRICT $CONST_STRUCT_PTR" . "j$LAST";
		$decl =~ s/\(.+?$LAST/$tmp/;
		$decl =~ s/,\s*\)/)/g;
		$decl .= "\n{\n\t";
		my $body = "$RETURN$FN(";
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
				if ($FN =~ /$G_NMSPC/ && is_size_ptr($params)) {
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
	  CONT:
		$_     .= "\n\n";
		$out_h .= $_;
	}
	$out_h =~ s/\n\n\n/\n\n/g;
	$out_h =~ s/\n\n*$/\n/g;
	return ($out_h);
}

sub get_regex_size_ptr
{
	my ($fn, $params) = @_;
	if ($fn =~ /$G_NMSPC/o) {
		$params =~ /\*.*\*.*(\w*sz)/o;
		return $1;
	}
	return 0;
}

sub is_str_ptr
{
	my ($params) = @_;
	return ($params =~ /\*.*\*.*s[,\)]/o) ? 1 : 0;
}

sub is_size_ptr
{
	my ($params) = @_;
	return ($params =~ /\*.*sz[,\)]/) ? 1 : 0;
}
