#!/usr/bin/perl
use strict;
use warnings;

sub usage {
	if ($#ARGV == -1) {
		print('Usage: ./' . $0 . ' <file>');
		exit;
	}
}

# @param {$} fname
# @return {$} file_str
sub file_get_str {
	my ($fname) = @_;
	my $file_str;
	local $/;
	open(my $FH, '<', $fname)
	  or die("Can't open $fname\n");
	$file_str = <$FH>;
	close($FH);
	return $file_str;
}

# @param {$} file_str
# @param {$} prefix
# @param {@} ignore_prefix
# @returns {$}
sub file_namespace_macros {
	my ($file_str, $prefix, @ignore_prefix) = @_;
	my @lines = split(/\n/, $file_str);
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)/) {
			my $macro = $1;
			foreach (@ignore_prefix) {
				my $i = index($macro, $_);
				if ($i == -1 || $i != 0) {
					$file_str =~ s/([^'"_0-9A-Za-z]|^)$macro([^'"_0-9A-Za-z]|$)/$1$prefix$macro$2/g;
				}
			}
		}
	}
	return $file_str;
}

# @param {$} file_str
# @returns {$}
sub file_tidy_newlines {
	my ($file_str) = @_;
	$file_str =~ s/(\n[ \t].*)\n\n\n*/$1\n/g;
	$file_str =~ s/\n\n\n*\t/\n\t/g;
	$file_str =~ s/\n\n\n* /\n /g;
	return $file_str;
}

# @param {$} arg
# @returns {@}
sub arg_to_array {
	my ($arg) = @_;
	my @arg_arr = split(/,/, $arg);
	return @arg_arr;
}

# @param {@} arg
# @returns {$} arg_str
sub arg_to_string {
	my (@arg) = @_;
	my $arg_str = '';
	$arg_str .= "$_," foreach (@arg);
	$arg_str =~ s/,$//;
	return $arg_str;
}

# @param {$} arg
# @param {$} add
# @returns {@}
sub arg_push_arr {
	my ($arg, $add) = @_;
	my @arg_arr = arg_to_array($arg);
	push(@arg_arr, $add);
	return @arg_arr;
}

# @param {$} arg
# @param {$} add
# @returns {$}
sub arg_push {
	my ($arg, $add) = @_;
	return arg_to_string(arg_push_arr($arg, $add));
}

# @param {$} arg
# @param {$} find
# @returns {$}
sub arg_index {
	my ($arg, $find) = @_;
	my @arg_arr = arg_to_array($arg);
	for (my $i = 0 ; $i < $#arg_arr ; ++$i) {
		if (index($arg_arr[$i], $find) != -1) {
			return $i;
		}
	}
	return -1;
}

# @param {$} arg
# @param {$} insert
# @param {$} index
# @returns {@}
sub arg_insert_arr {
	my ($arg, $insert, $i) = @_;
	my @arg_arr = arg_to_array($arg);
	my $tmp;
	for (my $j = $#arg_arr - 1 ; $i <= $j ; --$j) {
		$arg_arr[ $j + 1 ] = $arg_arr[$j];
	}
	$arg_arr[$i] = $insert;
	return @arg_arr;
}

# @param {$} arg
# @param {$} insert
# @param {$} index
# @returns {$}
sub arg_insert {
	my ($arg, $insert, $i) = @_;
	return arg_to_string(arg_insert_arr($arg, $insert, $i));
}

# @param {$} arg
# @param {$} insert
# @param {$} after
# @returns {@}
sub arg_insert_after_arr {
	my ($arg, $insert, $after) = @_;
	my @arg_arr = arg_to_array($arg);
	my $i       = arg_index($arg, $after);
	@arg_arr = arg_insert_arr($arg, $insert, $i + 1) if ($i != -1);
	return @arg_arr;
}

# @param {$} arg
# @param {$} insert
# @param {$} after
# @returns {$}
sub arg_insert_after {
	my ($arg, $insert, $after) = @_;
	return arg_to_string(arg_insert_after_arr($arg, $insert, $after));
}

# @param {$} arg
# @param {$} find
# @param {$} replace
# @returns {@}
sub arg_replace_arr {
	my ($arg, $find, $replace) = @_;
	my @arg_arr = arg_to_array($arg);
	my $i       = arg_index($arg, $find);
	$arg_arr[$i] = $replace if ($i != -1);
	return @arg_arr;
}

# @param {$} arg
# @param {$} find
# @param {$} replace
# @returns {$}
sub arg_replace {
	my ($arg, $find, $replace) = @_;
	return arg_to_string(arg_replace_arr($arg, $find, $replace));
}

# @param {$} arg
# @param {$} remove
# @returns {@}
sub arg_remove_arr {
	my ($arg, $remove) = @_;
	my @arg_arr = arg_to_array($arg);
	my $i       = arg_index($arg, $remove);
	splice(@arg_arr, $i) if ($i != -1);
	return @arg_arr;
}

# @param {$} arg
# @param {$} remove
# @returns {$}
sub arg_remove {
	my ($arg, $remove) = @_;
	return arg_to_string(arg_remove_arr($arg, $remove));
}

# @param {$} block_str
# @returns {$} rettype
# @returns {$} name
# @returns {$} arg
# @returns {$} body
sub fn_get {
	my ($block_str) = @_;
	if ($block_str =~ /((?:.|\n)*(?:^|\W))(\w+\s*[* \t\n]*)\s+(\w+)\s*\(((?:.|\n)*?)\)(?:.|\n)*?\{((?:.|\n)*)\}/) {
		my $attr    = $1;
		my $rettype = $2;
		my $name    = $3;
		my $arg     = $4;
		my $body    = $5;
		return ($attr, $rettype, $name, $arg, $body);
	}
	return;
}

# @param {$} rettype
# @param {$} name
# @param {$} arg
# @param {$} body
# @returns {$} fn_string
sub fn_to_string {
	my ($attr, $rettype, $name, $arg, $body) = @_;
	my $fn_str = "$attr\n$rettype $name($arg)\n{$body}";
	return $fn_str;
}

# @param {$} name
# @param {$} prefix
# @returns {$}
sub name_prepend {
	my ($name, $prefix) = @_;
	return $prefix . $name;
}

# @param {$} name
# @param {$} suffix
# @returns {$}
sub name_append {
	my ($name, $suffix) = @_;
	return $name . $suffix;
}

# @param {$} file_string
# @returns {@}
sub file_to_blocks {
	my ($file_string) = @_;
	return split(/\n\n/, $file_string);
}

# usage();

my $file_str    = file_get_str("./jstr-string.h");
my @file_blocks = file_to_blocks($file_str);
foreach (@file_blocks) {
	my ($attr, $rettype, $name, $arg, $body) = fn_get($_);
	if (defined($attr)) {
		print fn_to_string($attr, $rettype, $name, $arg, $body);
		print "\n";
	}
}
