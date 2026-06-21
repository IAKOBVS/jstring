#!/usr/bin/perl
use strict;
use warnings;

# wrap-func-decls.pl — post-process generated .h files to support
# dual-mode inclusion:
#
#   Mode 1 (header-only):
#     No special define needed.  Functions are static (current behavior).
#
#   Mode 2 (declarations-only):
#     Define JSTR_DECL_ONLY before including.  Function bodies are
#     suppressed; only extern prototypes remain.  Link against libjstr.so.
#
#   Mode 3 (implementation / library build):
#     Define JSTR_IMPLEMENTATION in exactly one TU.  Functions become
#     non-static extern definitions (without 'static').

use FindBin;
do "$FindBin::RealBin/jlib.pl";

# Regex to match function definitions, capturing 6 groups:
#   1 — attributes before return type
#   2 — return type (possibly with pointer asterisks)
#   3 — function name
#   4 — argument list
#   5 — attributes between argument list and body (JSTR_NOEXCEPT etc.)
#   6 — function body
our $WRAP_FN_RE = qr/
	^
	([^{}]*(?:^|\W))            # 1 — pre-return attrs
	(\w+\s*[* \t\n]*)           # 2 — return type
	\s+
	(\w+)                       # 3 — name
	\s*\(([^{}]*)\)             # 4 — args
	([^{}]*)                    # 5 — post-args attrs
	\{((?:.|\n)*)\}             # 6 — body
/x;

my $file_str;
{
	local $/;
	$file_str = <>;
}

# Non-function patterns that can falsely match:
#   #define, #if, #else, #elif, for, while, switch, if
my %is_not_func = map { $_ => 1 } qw(define if else elif for while switch);

my $out = '';
foreach (jl_file_to_blocks(\$file_str)) {
	if (/$WRAP_FN_RE/) {
		# Save capture groups BEFORE any other regex match,
		# since ANY subsequent regex clears $1 .. $n.
		my ($pre_attr, $rettype, $name, $arg_str, $post_attr, $body) =
			($1, $2, $3, $4, $5, $6);
		if ($is_not_func{$rettype}) {
			$out .= "$_\n\n";
			next;
		}
		# Only wrap blocks that originated from a JSTR_FUNC annotation.
		next unless /(?:JSTR_FUNC|JSTR_FUNC_VOID|JSTR_FUNC_PURE)/s;
		if (/(?:JSTR_DECL_ONLY|JSTR_AS_LIBRARY)/) {
			$out .= "$_\n\n";
			next;
		}

		# Remove 'static' from pre-attributes
		$pre_attr =~ s/^[ \t]*static[ \t]*\n?//gm;
		$pre_attr =~ s/\n[ \t]*static[ \t]*\n?/\n/g;
		$pre_attr =~ s/^[ \t]*static[ \t]*//g;
		$pre_attr =~ s/[ \t]*static[ \t]*$//g;
		$pre_attr =~ s/^\s+//;
		$pre_attr =~ s/\s+$//;

		# Normalize arg string
		$arg_str =~ s/,\s*$//;

		# Declaration (extern prototype)
		my $decl = $pre_attr ne ''
			? "$pre_attr\nextern $rettype\n${name}($arg_str);"
			: "extern $rettype\n${name}($arg_str);";

		$post_attr =~ s/\s+$//;
		# Definition (with JSTR_API in place of static)
		my $def;
		if ($pre_attr ne '' && $post_attr ne '') {
			$def = "$pre_attr\nJSTR_API $rettype\n${name}($arg_str)$post_attr\n\{ $body }";
		} elsif ($pre_attr ne '') {
			$def = "$pre_attr\nJSTR_API $rettype\n${name}($arg_str)\n\{ $body }";
		} elsif ($post_attr ne '') {
			$def = "JSTR_API $rettype\n${name}($arg_str)$post_attr\n\{ $body }";
		} else {
			$def = "JSTR_API $rettype\n${name}($arg_str)\n\{ $body }";
		}

		$out .= "#if defined(JSTR_AS_LIBRARY) || defined(JSTR_DECL_ONLY)\n$decl\n#else\n$def\n#endif\n\n";
	} else {
		$out .= "$_\n\n";
	}
}

$out =~ s/\n\n*$/\n/;
print $out;
