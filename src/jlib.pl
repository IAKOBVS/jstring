#!/usr/bin/perl
use strict;
use warnings;

# @param {$} fname_ref
# @returns {$} file_str
sub jl_file_get_str {
	my ($fname_ref) = @_;
	local $/;
	open(my $FH, '<', $$fname_ref)
	  or die("Can't open $$fname_ref\n");
	my $file_str = <$FH>;
	close($FH);
	return $file_str;
}

# @param {$} file_str_ref
# @param {$} prefix_ref
# @param {$} ignore_prefix_ref
# @returns {$}
sub jl_file_namespace_macros {
	my ($file_str_ref, $prefix_ref, $ignore_prefix_ref) = @_;
	my @lines = split(/\n/, $$file_str_ref);
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)/) {
			my $macro = $1;
			foreach (@$ignore_prefix_ref) {
				my $i = index($macro, $_);
				if ($i == -1 || $i != 0) {
					$$file_str_ref =~ s/([^'"_0-9A-Za-z]|^)$macro([^'"_0-9A-Za-z]|$)/$1$$prefix_ref$macro$2/g;
				}
			}
		}
	}
}

# @param {$} file_str_ref
sub jl_file_tidy_newlines {
	my ($file_str_ref) = @_;
	$$file_str_ref =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$$file_str_ref =~ s/\n\n\n*\t/\n\t/g;
	$$file_str_ref =~ s/\n\n\n* /\n /g;
}

# @param {$} arg_str_ref
# @returns {@}
sub jl_arg_to_array {
	my ($arg_str_ref) = @_;
	my @arg_arr = split(/,/, $$arg_str_ref);
	return @arg_arr;
}

# @param {$} arg_arr_ref
# @returns {$} arg_str
sub jl_arg_to_string {
	my ($arg_arr_ref) = @_;
	my $arg_str = '';
	$arg_str .= "$_," foreach (@$arg_arr_ref);
	$arg_str =~ s/,$//;
	return $arg_str;
}

# @param {$} arg_arr_ref
# @param {$} add_ref
sub jl_arg_push {
	my ($arg_arr_ref, $add_ref) = @_;
	push(@$arg_arr_ref, $$add_ref);
}

# @param {$} arg_arr_ref
# @param {$} find_ref
# @returns {$}
sub jl_arg_index {
	my ($arg_arr_ref, $find_ref) = @_;
	for (my $i = 0 ; $i < $#$arg_arr_ref ; ++$i) {
		if (index(@$arg_arr_ref[$i], $$find_ref) != -1) {
			return $i;
		}
	}
	return -1;
}

# @param {$} arg_arr_ref
# @param {$} insert_ref
# @param {$} index
sub jl_arg_insert {
	my ($arg_arr_ref, $insert_ref, $i) = @_;
	for (my $j = $#$arg_arr_ref - 1 ; $i <= $j ; --$j) {
		@$arg_arr_ref[ $j + 1 ] = @$arg_arr_ref[$j];
	}
	@$arg_arr_ref[$i] = $$insert_ref;
}

# @param {$} arg_arr_ref
# @param {$} insert_ref
# @param {$} after_ref
sub jl_arg_insert_after {
	my ($arg_arr_ref, $insert_ref, $after_ref) = @_;
	my $i = jl_arg_index($arg_arr_ref, $$after_ref);
	@$arg_arr_ref = jl_arg_insert($arg_arr_ref, $$insert_ref, $i + 1) if ($i != -1);
}

# @param {$} arg_arr_ref
# @param {$} find_ref
# @param {$} replace_ref
sub jl_arg_replace {
	my ($arg_arr_ref, $find_ref, $replace_ref) = @_;
	my $i = jl_arg_index($arg_arr_ref, $$find_ref);
	@$arg_arr_ref[$i] = $$replace_ref if ($i != -1);
}

# @param {$} arg_arr_ref
# @param {$} remove_ref
sub jl_arg_remove {
	my ($arg_arr_ref, $remove_ref) = @_;
	my $i = jl_arg_index($arg_arr_ref, $$remove_ref);
	splice(@$arg_arr_ref, $i) if ($i != -1);
}

# @param {$} arg_arr_ref
# @returns {$}
sub jl_arg_get_rettype {
	my ($arg_arr_ref) = @_;
	$$arg_arr_ref =~ /^\s*(\w+\s*[* \t\n]*)/;
	return $1;
}

# @param {$} arg_arr_ref
# @returns {$}
sub jl_arg_get_var {
	my ($arg_arr_ref) = @_;
	$$arg_arr_ref =~ /(\w+)\s*$/;
	return $1;
}

# @param {$} arg_arr_ref
# @returns {$}
sub jl_arg_is_ptr_ptr {
	my ($arg_arr_ref) = @_;
	return $$arg_arr_ref =~ /\*(?:.|\n)*\*/;
}

# @param {$} block_str_ref
# @param {$} attr_ref
# @param {$} rettype_ref
# @param {$} name_ref
# @param {$} arg_arr_ref
# @param {$} body_ref
# @returns {$}
sub jl_fn_get {
	my ($block_str_ref, $attr_ref, $rettype_ref, $name_ref, $arg_arr_ref, $body_ref) = @_;
	if ($$block_str_ref =~ /((?:.|\n)*(?:^|\W))(\w+\s*[* \t\n]*)\s+(\w+)\s*\(((?:.|\n)*?)\)(?:.|\n)*?\{((?:.|\n)*)\}/) {
		$$attr_ref    = $1                   if (defined($attr_ref));
		$$rettype_ref = $2                   if (defined($rettype_ref));
		$$name_ref    = $3                   if (defined($name_ref));
		@$arg_arr_ref = jl_arg_to_array(\$4) if (defined($arg_arr_ref));
		$$body_ref    = $5                   if (defined($body_ref));
		return 1;
	}
	return 0;
}

# @param {$} rettype_ref
# @param {$} name_ref
# @param {$} arg_arr_ref
# @param {$} body_ref
# @returns {$} fn_string
sub jl_fn_to_string {
	my ($attr_ref, $rettype_ref, $name_ref, $arg_arr_ref, $body_ref) = @_;
	my $arg = jl_arg_to_string($arg_arr_ref);
	return $$attr_ref . "\n" . $$rettype_ref . ' ' . $$name_ref . '(' . $arg . ")\n{" . $$body_ref . "}";
}

# @param {$} name_ref
# @param {$} prefix_ref
# @returns {$}
sub jl_name_prepend {
	my ($name_ref, $prefix_ref) = @_;
	return $$prefix_ref . $$name_ref;
}

# @param {$} name_ref
# @param {$} suffix_ref
# @returns {$}
sub jl_name_append {
	my ($name_ref, $suffix_ref) = @_;
	return $$name_ref . $$suffix_ref;
}

# @param {$} file_str_ref
# @returns {@}
sub jl_file_to_blocks {
	my ($file_str_ref) = @_;
	return split(/\n\n/, $$file_str_ref);
}