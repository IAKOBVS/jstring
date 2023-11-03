#!/usr/bin/perl
use strict;
use warnings;
do "./jlib.pl";

sub usage {
	if ($#ARGV == -1) {
		print('Usage: ./' . $0 . ' <file>');
		exit;
	}
}

my $fname = $ARGV[0];
my $ATTR_INLINE   = 'JSTR_INLINE';
my $ATTR_RESTRICT = 'JSTR_RESTRICT';
my $VAR_JSTRING = 'j';
my $JSTRING     = 'jstr_ty';
my $DATA = 'data';
my $SIZE = 'size';
my $CAP  = 'capacity';
my $VAR_DATA = 's';
my $VAR_SIZE = 'sz';
my $VAR_CAP  = 'cap';
my $J_PREFIX = '_j';

sub add_inline {
	my ($attr_str_ref, $inline_attr_ref) = @_;
	$$attr_str_ref .= "\n" . $$inline_attr_ref
	  if (index($$attr_str_ref, $$inline_attr_ref) == -1);
}

sub grepped {
	my ($arr_ref, $str_ref) = @_;
	foreach (@$arr_ref) {
		return 1 if ($_ eq $$str_ref);
	}
	return 0;
}

usage();

my $file_str      = jl_file_get_str(\$fname);
my @ignore_prefix = ("PJSTR", "pjstr", "JSTR", "jstr");
jl_file_namespace_macros(\$file_str, \"PJSTR_", \@ignore_prefix);
my $file_str2 = '';
my @func_arr;

foreach (jl_file_to_blocks(\$file_str)) {
	$file_str2 .= $_ . "\n\n";
	my ($attr, $rettype, $name, @arg, $body);
	next
	  if (!jl_fn_get(\$_, \$attr, \$rettype, \$name, \@arg, undef)
		|| $name !~ /^jstr_/);
	push(@func_arr, $name);
	next if ($name !~ /_len/
		|| $name =~ /_j$/);
	my $base_name = s/_len(_|$)/$1/;
	$base_name =~ s/_len//;
	next if (grepped(\@func_arr, \$base_name));
	add_inline(\$attr, \$ATTR_INLINE);
	$body = (($rettype eq 'void') ? '' : 'return ') . $name . '_len(';
	for (my $i = 0; $i < scalar(@arg); ++$i) {
		my $var = jl_arg_get_var(\$arg[$i]);
		if (
			index($arg[$i], 'size_t') != -1
			&& (index($var, 'len') != -1
				|| $var =~ /sz$/)
		  )
		{
			my $base_var = $var;
			$base_var =~ s/z\s*$//;
			$base_var =~ s/_len\s*$//;
			my $str_i   = jl_arg_index(\@arg, \$base_var);
			my $str_var = jl_arg_get_var(\$arg[$str_i]);
			my $deref   = jl_arg_is_ptr_ptr(\$str_var) ? '*' : '';
			$body .= 'strlen(' . $deref . $str_var . ')';
			splice(@arg, $i--, 1);
		} else {
			$body .= $var;
		}
		$body .= ', ';
	}
	$body =~ s/, $//;
	$body .= ");";
	$file_str2 .=
	  jl_fn_to_string(\$attr, \$rettype, \$name, \@arg, \$body) . "\n\n";
}

my $file_str3 = '';

foreach (jl_file_to_blocks(\$file_str2)) {
	$file_str3 .= $_ . "\n\n";
	my ($attr, $rettype, $name, @arg, $body);
	next
	  if (!jl_fn_get(\$_, \$attr, \$rettype, \$name, \@arg, undef)
		|| $name !~ /^jstr_/);
	next if ($name =~ /_j$/
		|| index($name, 'unsafe') != -1);
	my $base_name = $name;
	$base_name =~ s/_len(_|$)/$1/;
	next if (grepped(\@func_arr, \($base_name . $J_PREFIX)));
	$body = (($rettype eq 'void') ? '' : 'return ') . $name . '(';
	$name = $base_name . $J_PREFIX;
	my $need_j = 0;
	for (my $i = 0; $i < scalar(@arg); ++$i) {
		my $var = jl_arg_get_var(\$arg[$i]);
		if ($var eq $VAR_DATA) {
			my $deref = (jl_arg_is_ptr_ptr(\$arg[$i]) ? '&' : '');
			$body .= $deref . $VAR_JSTRING . '->' . $DATA;
			$arg[$i] = jl_arg_is_const(\$arg[$i]) ? 'const ' : '';
			$arg[$i] .= $JSTRING . ' *' . $ATTR_RESTRICT . ' j';
		} elsif ($var eq $VAR_SIZE) {
			$need_j = 1;
			my $deref = (jl_arg_is_ptr(\$arg[$i]) ? '&' : '');
			$body .= $VAR_JSTRING . '->' . $SIZE;
			splice(@arg, $i--, 1);
		} elsif ($var eq $VAR_CAP) {
			$need_j = 1;
			my $deref = (jl_arg_is_ptr(\$arg[$i]) ? '&' : '');
			$body .= $deref . $VAR_JSTRING . '->' . $CAP;
			splice(@arg, $i--, 1);
		} else {
			$body .= $var;
		}
		$body .= ', ';
	}
	next if ($need_j == 0);
	$body =~ s/, $//;
	$body .= ");";
	add_inline(\$attr, \$ATTR_INLINE);
	$file_str3 .=
	  jl_fn_to_string(\$attr, \$rettype, \$name, \@arg, \$body) . "\n\n";
	push(@func_arr, $name);
}

print($file_str3);
