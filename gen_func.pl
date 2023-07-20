#!/usr/bin/perl
use strict;
use warnings;

my $namespace = 'jstr';
my $namespace_big = 'JSTR';

while (glob('*.h')) {
	my $fname = $_;
	open(my $FH, '<', $fname) or die "Can't open $fname\n";
	my $s;
	while (<$FH>) {
		$s .= $_;
	}
	close($FH);
	my @funcs = split(/\n\n/, $s);
	my $file;
	foreach (@funcs) {
		/^((?:\/\/|\/\*|$namespace_big|static)[^(){}]+?($namespace\_\w+?)\(((?:.|\n)+?(?:sz|cap)(?:.|\n)+?\)\s*\w*NOEXCEPT))/;
		if (!$1 || !$2 || !$3) {
			next;
		}
		my $decl = $1;
		my $func = $2;
		my $args = $3;
		my $has_sz = 0;
		if (index($args, 'sz') != -1) {
			$has_sz = 1;
		}
		my $has_cap = 0;
		if (index($args, 'cap') != -1) {
			$has_cap = 1;
		}
		if (!$has_sz && !$has_cap) {
			next;
		}
		$args =~ s/\)/,/;
		my @new_args;
		my @types;
		my $argnum = 0;
		{
			my @old_args = split(/\s/, $args);
			my $i = 0;
			foreach (@old_args) {
				if (/,$/) {
					s/,//;
					push(@new_args, $_);
					++$i;
				}
				$types[$i] .= "$_ ";
				++$argnum;
			}
		}
		my $tmp = '';
		if ($has_sz) {
			$decl =~ s/\((?:.|\n)+?sz(,|\))/(/;
			if ($1) {
				$tmp = $1;
			}
		}
		if ($has_cap) {
			$decl =~ s/\((?:.|\n)+?cap(,|\))/(/;
			if ($1 eq ',') {
				$tmp = $1;
			}
		}
		$decl =~ s/\(/($namespace\_t *j$tmp/;
		$decl .= "\n{\n\t$func(";
		my $func_args;
		for (my $i = 0; $i <= $#new_args; ++$i) {
			if (index($new_args[$i], 'sz') != -1) {
				$new_args[$i] = "&j->size";
			} elsif (index($new_args[$i], 'cap') != -1) {
				$new_args[$i] = "&j->cap";
			} elsif ($i == 0) {
				$new_args[$i] = "&j->data";
			}
			$func_args .= "$new_args[$i], ";
		}
		$func_args =~ s/,[^,]*$//;
		$decl .= "$func_args);\n}\n\n";
		$decl =~ s/$func/$func\_j/g;
		$file .= $decl;
	}
	if (!$file) {
		next;
	}
	$file =~ s/$namespace_big\_RST/$namespace_big\_RESTRICT/g;
	$file = "$s\n#if $namespace_big\_NAMESPACE && defined(__cplusplus)\nnamespace $namespace {\n#endif /* $namespace_big\_NAMESPACE */\n\n$file#if $namespace_big\_NAMESPACE && defined(__cplusplus)\n}\n#endif /* $namespace_big\_NAMESPACE */\n";
	my $tmp = $file;
	$tmp =~ s/\.h"/.hpp"/g;
	$tmp =~ s/H_DEF/HPP_DEF/g;
	$tmp =~ s/$namespace\_(\w*)\(/$1(/g;
	$tmp =~ s/EXTERN_C\s*\d/EXTERN_C 0/;
	$tmp =~ s/NAMESPACE\s*\d/NAMESPACE 1/;
	$tmp =~ s/_j\(/(/g;
	open($FH, '>', "cpp/$fname". 'pp') or die "Can't open cpp/$fname"."pp\n";
	print($FH $tmp);
	close($FH);
	# open($FH, '>', $fname) or die "Can't open $fname\n";
	# print($FH $file);
	# close($FH);
}
