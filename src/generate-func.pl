#!/usr/bin/perl
use strict;
use warnings;

usage();

my $G_NMSPC      = 'jstr';
my $G_NMSPC_UPP  = uc($G_NMSPC);
my $G_STR_STRUCT = 'jstr_ty';
my $G_STRUCT_VAR = 'j';

my $G_STRUCT_DATA = 'data';
my $G_STRUCT_SIZE = 'size';
my $G_STRUCT_CAP  = 'capacity';

my $G_CAP_VAR  = 'cap';
my $G_SIZE_VAR = 'sz';
my $G_LEN_VAR  = 'len';

my $G_MCR_INLINE          = $G_NMSPC_UPP . '_INLINE';
my $G_MCR_MAYBE_UNUSED    = $G_NMSPC_UPP . '_MAYBE_UNUSED';
my $G_MCR_RESTRICT        = $G_NMSPC_UPP . '_RESTRICT';
my $G_MCR_WARN_UNUSED     = $G_NMSPC_UPP . '_WARN_UNUSED';
my $G_MCR_RETURNS_NONNULL = $G_NMSPC_UPP . '_RETURNS_NONNULL';

my $G_FN_SUFFIX_RETURNS_END_PTR = '_p';
my $G_FN_SUFFIX_LEN             = '_len';

my $fname     = $ARGV[0];
my $g_in_header = get_file_str($fname);
$g_in_header = tidy_newlines($g_in_header);

my @lines      = gen_nonlen_funcs($g_in_header);
my $out_header = gen_struct_funcs(@lines);

print $out_header;

sub usage {
	if ($#ARGV != 0) {
		print('Usage: ./gen_func.pl <file>');
		exit;
	}
}

sub get_file_str {
	my ($fname) = @_;
	my $in_header = '';
	my @lines;
	open(my $FH, '<', $fname) or die("Can't open $fname\n");
	while (<$FH>) {
		$in_header .= $_;
		push(@lines, $_);
	}
	close($FH);
	my @def;
	my @undef;

	# prefix temporary macros with $G_NMSPC_UPP to avoid naming conflicts
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]*([_A-Z0-9]*)/) {
			my $macro = $1;
			if ($macro !~ /^[Pp]{0,1}[Jj][Ss][Tt][Rr]/) {
				$in_header =~ s/([^'"_0-9A-Za-z]|^)$macro([^'"_0-9A-Za-z]|$)/$1P$G_NMSPC_UPP\_$macro$2/g;
			}
		}
	}
	return $in_header;
}

sub tidy_newlines {
	my ($in_header) = @_;
	$in_header =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$in_header =~ s/\n\n\n*\t/\n\t/g;
	$in_header =~ s/\n\n\n* /\n /g;
	return $in_header;
}

sub gen_nonlen_funcs {
	my ($file_str) = @_;
	my @old_lines = split(/\n\n/, $file_str);
	my @new_lines;
	foreach (@old_lines) {
		my ($decl, $fn_name, $params) = get_fn($_);
		if (   !defined($decl)
			|| !defined($fn_name)
			|| !defined($params)
			|| $fn_name !~ /$G_NMSPC[_0-9_A-Za-z]*$G_FN_SUFFIX_LEN(?:_|$)/o
			|| is_private_fn($fn_name))
		{
			goto CONT;
		}
		my $base_fn_name = $fn_name;
		$base_fn_name =~ s/$G_FN_SUFFIX_LEN//o;
		if ($g_in_header =~ /$base_fn_name\(/) {
			goto CONT;
		}
		$decl = add_inline($decl);

		# my $ptr    = ($decl =~ /\*.*\*/)         ? '&'       : '';
		my $return = (index($_, 'return') != -1) ? 'return ' : '';
		$decl =~ s/($G_NMSPC\_\w*)$G_FN_SUFFIX_LEN(\w*\()/$1$2/o;
		$decl .= "\n{\n\t";
		my $size_ptr_var = get_size_ptr($fn_name, $params);
		$decl .= "$return$fn_name(";
		my $using_len = (index($params, $G_LEN_VAR) != -1) ? 1 : 0;

		foreach (my @args = convert_params_to_array($params)) {
			if ($using_len) {
				if (/(\w*)$G_LEN_VAR/) {
					my $var = $1;
					$decl =~ s/,[ \t\n_0-9A-Za-z]*$G_LEN_VAR//o;
					$_ = "strlen($var)";
				}
			} else {
				if (!$size_ptr_var && /\w*$G_SIZE_VAR/) {
					$decl =~ s/,[ \t\n_0-9A-Za-z]*$G_SIZE_VAR//o;
					$_ = "strlen($args[0])";
				}
			}
			$decl .= "$_, ";
		}
		$decl =~ s/, $//;
		$decl .= ");\n}\n";
		push(@new_lines, $_);
		push(@new_lines, $decl);
		next;
	  CONT:
		push(@new_lines, $_);
	}
	return @new_lines;
}

sub gen_struct_funcs {
	my (@lines) = @_;
	my $out_header;
	foreach (@lines) {
		my ($decl, $fn_name, $params) = get_fn($_);
		if (   !defined($decl)
			|| !defined($fn_name)
			|| !defined($params)
			|| index($params, "...") != -1
			|| is_private_fn($fn_name)
			|| index($g_in_header, "$fn_name\_j") != -1)
		{
			goto CONT;
		}
		my $has_sz  = 0;
		my $has_cap = 0;
		if ($params =~ /$G_SIZE_VAR(,|\))/) {
			my $find = $1;
			my $rplc = ($find eq ',') ? '' : ')';
			$decl =~ s/[^(,]*$G_SIZE_VAR\Q$find\E/$rplc/;
			$has_sz = 1;
		}
		if ($params =~ /\W$G_CAP_VAR(,|\))/) {
			my $find = $1;
			my $rplc = ($find eq ',') ? '' : ')';
			$decl =~ s/[^(,]*\W$G_CAP_VAR\Q$find\E/$rplc/;
			$has_cap = 1;
		}
		if (!$has_sz && !$has_cap) {
			goto CONT;
		}
		my $return          = (index($decl, 'void') != -1) ? '' : 'return ';
		my $returns_end_ptr = 0;
		$decl =~ s/$fn_name/$fn_name\_j/;
		$decl = add_inline($decl);
		if ($fn_name =~ /$G_FN_SUFFIX_RETURNS_END_PTR(?:_|$)/o) {
			$decl =~ s/.*Return value:(?:.|\n)*?(\*\/|\/\/)/$1/;
			$decl =~ s/$G_FN_SUFFIX_RETURNS_END_PTR//o;
			$decl =~ s/static\s*(?:char|void)\s\*/static void /;
			$decl =~ s/[ \t]*$G_MCR_WARN_UNUSED[ \t]*\n//o;
			$return          = '';
			$returns_end_ptr = 1;
		}
		my $ptr    = ($decl   =~ /\([^,)]*\*.*\*/) ? '&'      : '';
		my $const  = ($params =~ /^\s*const/)      ? 'const ' : '';
		my $last_c = (rindex($params, ',') != -1) ? ',' : ')';
		my $rplc   = "($const$G_STR_STRUCT *$G_MCR_RESTRICT j$last_c";
		$decl =~ s/\(.+?$last_c/$rplc/;
		$decl =~ s/,\s*\)/)/g;
		$decl .= "\n{\n\t";
		my $func_body = "$return$fn_name(";

		if (using_str_ptr($params)) {
			$func_body .= $ptr;
		}
		$func_body .= "$G_STRUCT_VAR->$G_STRUCT_DATA, ";
		for (my @args = convert_params_to_array($params), my $i = 1 ; $i <= $#args ; ++$i) {
			if (index($args[$i], $G_SIZE_VAR) != -1) {
				if (index($fn_name, $G_NMSPC) != -1 && using_size_ptr($params)) {
					$args[$i] = "&$G_STRUCT_VAR->$G_STRUCT_SIZE";
				} else {
					$args[$i] = $ptr . "$G_STRUCT_VAR->$G_STRUCT_SIZE";
				}
			} elsif ($args[$i] eq $G_CAP_VAR) {
				$args[$i] = $ptr . "$G_STRUCT_VAR->$G_STRUCT_CAP";
			}
			$func_body .= "$args[$i], ";
		}
		$func_body =~ s/, $//;
		$func_body .= ")";
		if ($returns_end_ptr) {
			$func_body = "$G_STRUCT_VAR->$G_STRUCT_SIZE = $func_body";
			$func_body .= " - $G_STRUCT_VAR->$G_STRUCT_DATA";
		}
		$decl .= "$func_body;\n}\n";
		$decl =~ s/\n\n/\n/g;
		$_          .= "\n\n";
		$decl       .= "\n\n";
		$out_header .= $_;
		$out_header .= $decl;
		next;
	  CONT:
		$_          .= "\n\n";
		$out_header .= $_;
	}
	$out_header =~ s/\n\n\n/\n\n/g;
	$out_header =~ s/\n\n*$/\n/g;
	return $out_header;
}

sub get_size_ptr {
	my ($fn, $params) = @_;
	if (index($fn, $G_NMSPC) != -1) {
		$params =~ /\*.*\*.*(\w*sz)/o;
		return $1;
	}
	return 0;
}

sub using_str_ptr {
	my ($params) = @_;
	return ($params =~ /\*.*\*.*s[,\)]/o) ? 1 : 0;
}

sub using_size_ptr {
	my ($params) = @_;
	return ($params =~ /\*.*sz[,\)]/) ? 1 : 0;
}

sub add_inline {
	my ($decl) = @_;
	if ($decl !~ /$G_MCR_INLINE[^_]/o) {
		$decl =~ s/static/$G_MCR_INLINE\n$G_MCR_MAYBE_UNUSED\nstatic/o;
	}
	return $decl;
}

sub get_fn {
	my ($string) = @_;
	my $decl     = undef;
	my $fn_name  = undef;
	my $params   = undef;
	if ($string =~ /((?:\/\*|\/\/|$G_NMSPC_UPP\_|static)\s+\w+\s+(\w*(?:$G_NMSPC)\_.*?)\(((?:.|\n)*?\)[^(){}]*))/o) {
		$decl    = $1;
		$fn_name = $2;
		$params  = $3;
	}
	return ($decl, $fn_name, $params);
}

sub is_private_fn {
	my ($fn_name) = @_;
	return $fn_name =~ /^p/ || $fn_name =~ /^P/;
}

sub convert_params_to_array {
	my ($params) = @_;
	$params =~ s/\)/,/;
	my @args = split(/\s/, $params);
	my @vars;
	foreach (@args) {
		if (index($_, ',') != -1) {
			$_ =~ s/,//;
			push(@vars, $_);
		}
	}
	return @vars;
}
