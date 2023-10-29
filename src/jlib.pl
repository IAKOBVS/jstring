#!/usr/bin/perl
use strict;
use warnings;

# @param {$} fname
# @returns {$} file_str
sub jl_file_get_str
{
	my ($fname) = @_;
	my $file_str;
	local $/;
	open(my $FH, '<', $fname)
	  or die("Can't open $fname\n");
	$file_str = <$FH>;
	close($FH);
	return $file_str;
}

# @param {$} file_str_ref
# @param {$} prefix
# @param {$} ignore_prefix
# @returns {$}
sub jl_file_namespace_macros
{
	my ($file_str_ref, $prefix, @ignore_prefix) = @_;
	my @lines = split(/\n/, $$file_str_ref);
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)/) {
			my $macro = $1;
			foreach (@ignore_prefix) {
				my $i = index($macro, $_);
				if ($i == -1 || $i != 0) {
					$$file_str_ref =~ s/([^'"_0-9A-Za-z]|^)$macro([^'"_0-9A-Za-z]|$)/$1$prefix$macro$2/g;
				}
			}
		}
	}
}

# @param {$} file_str
# @returns {$}
sub jl_file_tidy_newlines
{
	my ($file_str) = @_;
	$file_str =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$file_str =~ s/\n\n\n*\t/\n\t/g;
	$file_str =~ s/\n\n\n* /\n /g;
	return $file_str;
}

# @param {$} arg
# @returns {@}
sub jl_arg_to_array
{
	my ($arg) = @_;
	my @arg_arr = split(/,/, $arg);
	return @arg_arr;
}

# @param {@} arg_ref
# @returns {$} arg_str
sub jl_arg_to_string
{
	my ($arg_ref) = @_;
	my $arg_str = '';
	$arg_str .= "$_," foreach (@$arg_ref);
	$arg_str =~ s/,$//;
	return $arg_str;
}

# @param {$} arg_ref
# @param {$} add
sub jl_arg_push
{
	my ($arg_ref, $add) = @_;
	push(@$arg_ref, $add);
}

# @param {$} arg_ref
# @param {$} find
# @returns {$}
sub jl_arg_index
{
	my ($arg_ref, $find) = @_;
	for (my $i = 0 ; $i < $#$arg_ref ; ++$i) {
		if (index(@$arg_ref[$i], $find) != -1) {
			return $i;
		}
	}
	return -1;
}

# @param {$} arg_ref
# @param {$} insert
# @param {$} index
sub jl_arg_insert
{
	my ($arg_ref, $insert, $i) = @_;
	for (my $j = $#$arg_ref - 1 ; $i <= $j ; --$j) {
		@$arg_ref[ $j + 1 ] = @$arg_ref[$j];
	}
	@$arg_ref[$i] = $insert;
}

# @param {$} arg_ref
# @param {$} insert
# @param {$} after
sub jl_arg_insert_after
{
	my ($arg_ref, $insert, $after) = @_;
	my $i = jl_arg_index($arg_ref, $after);
	@$arg_ref = jl_arg_insert($arg_ref, $insert, $i + 1) if ($i != -1);
}

# @param {$} arg_ref
# @param {$} find
# @param {$} replace
sub jl_arg_replace
{
	my ($arg_ref, $find, $replace) = @_;
	my $i = jl_arg_index($arg_ref, $find);
	@$arg_ref[$i] = $replace if ($i != -1);
}

# @param {$} arg_ref
# @param {$} remove
sub jl_arg_remove
{
	my ($arg_ref, $remove) = @_;
	my $i = jl_arg_index($arg_ref, $remove);
	splice(@$arg_ref, $i) if ($i != -1);
}

# @param {$} arg
# @returns {$}
sub jl_arg_get_rettype
{
	my ($arg) = @_;
	$arg =~ /^\s*(\w+\s*[* \t\n]*)/;
	return $1;
}

# @param {$} arg
# @returns {$}
sub jl_arg_get_var
{
	my ($arg) = @_;
	$arg =~ /(\w+)\s*$/;
	return $1;
}

# @param {$} arg
# @returns {$}
sub jl_arg_is_ptr_ptr
{
	my ($arg) = @_;
	return $arg =~ /\*(?:.|\n)*\*/;
}

# @param {$} block_str_ref
# @param {$} attr_ref
# @param {$} rettype_ref
# @param {$} name_ref
# @param {$} arg_ref
# @param {$} body_ref
# @returns {$}
sub jl_fn_get
{
	my ($block_str_ref, $attr_ref, $rettype_ref, $name_ref, $arg_ref, $body_ref) = @_;
	if ($$block_str_ref =~ /((?:.|\n)*(?:^|\W))(\w+\s*[* \t\n]*)\s+(\w+)\s*\(((?:.|\n)*?)\)(?:.|\n)*?\{((?:.|\n)*)\}/) {
		$$attr_ref    = $1               if (defined($attr_ref));
		$$rettype_ref = $2               if (defined($rettype_ref));
		$$name_ref    = $3               if (defined($name_ref));
		@$arg_ref     = jl_arg_to_array($4) if (defined($arg_ref));
		$$body_ref    = $5               if (defined($body_ref));
		return 1;
	}
	return 0;
}

# @param {$} rettype
# @param {$} name
# @param {$} arg_ref
# @param {$} body
# @returns {$} fn_string
sub jl_fn_to_string
{
	my ($attr, $rettype, $name, $arg_ref, $body) = @_;
	my $arg    = jl_arg_to_string($arg_ref);
	my $fn_str = "$attr\n$rettype $name($arg)\n{$body}";
	return $fn_str;
}

# @param {$} name
# @param {$} prefix
# @returns {$}
sub jl_name_prepend
{
	my ($name, $prefix) = @_;
	return $prefix . $name;
}

# @param {$} name
# @param {$} suffix
# @returns {$}
sub jl_name_append
{
	my ($name, $suffix) = @_;
	return $name . $suffix;
}

# @param {$} file_string
# @returns {@}
sub jl_file_to_blocks
{
	my ($file_string) = @_;
	return split(/\n\n/, $file_string);
}
