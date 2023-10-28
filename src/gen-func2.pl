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
# @param {$} ignore_prefix_ref
# @returns {$}
sub file_namespace_macros {
	my ($file_str, $prefix, $ignore_prefix_ref) = @_;
	my @lines = split(/\n/, $file_str);
	foreach (@lines) {
		if (/^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)/) {
			my $macro = $1;
			foreach (@$ignore_prefix_ref) {
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
	my ($arg_ref) = @_;
	my $arg_str = '';
	$arg_str .= "$_," foreach (@$arg_ref);
	$arg_str =~ s/,$//;
	return $arg_str;
}

# @param {$} arg
# @param {$} add
# @returns {@}
sub arg_push {
	my ($argRef, $add) = @_;
	push(@$argRef, $add);
}

# @param {$} arg
# @param {$} find
# @returns {$}
sub arg_index {
	my ($arg_ref, $find) = @_;
	for (my $i = 0 ; $i < $#$arg_ref ; ++$i) {
		if (index(@$arg_ref[$i], $find) != -1) {
			return $i;
		}
	}
	return -1;
}

# @param {$} arg
# @param {$} insert
# @param {$} index
# @returns {@}
sub arg_insert {
	my ($arg_ref, $insert, $i) = @_;
	for (my $j = $#$arg_ref - 1 ; $i <= $j ; --$j) {
		@$arg_ref[ $j + 1 ] = @$arg_ref[$j];
	}
	@$arg_ref[$i] = $insert;
}


# @param {$} arg
# @param {$} insert
# @param {$} after
# @returns {@}
sub arg_insert_after {
	my ($arg_ref, $insert, $after) = @_;
	my $i = arg_index($arg_ref, $after);
	@$arg_ref = arg_insert($arg_ref, $insert, $i + 1) if ($i != -1);
}

# @param {$} arg
# @param {$} find
# @param {$} replace
# @returns {@}
sub arg_replace {
	my ($arg_ref, $find, $replace) = @_;
	my $i = arg_index($arg_ref, $find);
	@$arg_ref[$i] = $replace if ($i != -1);
}

# @param {$} arg
# @param {$} remove
# @returns {@}
sub arg_remove {
	my ($arg_ref, $remove) = @_;
	my $i = arg_index($arg_ref, $remove);
	splice(@$arg_ref, $i) if ($i != -1);
}

# @param {$} block_str
# @returns {$} rettype
# @returns {$} name
# @returns {$} arg
# @returns {$} body
sub fn_get {
	my ($arg_ref, $block_str) = @_;
	if ($block_str =~ /((?:.|\n)*(?:^|\W))(\w+\s*[* \t\n]*)\s+(\w+)\s*\(((?:.|\n)*?)\)(?:.|\n)*?\{((?:.|\n)*)\}/) {
		my $attr    = $1;
		my $rettype = $2;
		my $name    = $3;
		@$arg_ref = arg_to_array($4);
		my $body = $5;
		return ($attr, $rettype, $name, $body);
	}
	return;
}

# @param {$} rettype
# @param {$} name
# @param {$} arg
# @param {$} body
# @returns {$} fn_string
sub fn_to_string {
	my ($attr, $rettype, $name, $arg_ref, $body) = @_;
	my $arg    = arg_to_string($arg_ref);
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
	my @arg;
	my ($attr, $rettype, $name, $body) = fn_get(\@arg, $_);
	if (defined($attr)) {
		print fn_to_string($attr, $rettype, $name, \@arg, $body);
		print "\n";
	}
}
