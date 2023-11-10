#!/usr/bin/perl

# See LICENSE file for copyright and license details.

use strict;
use warnings;

my $file_str;
{
	local $/;
	open(my $FH, '<', $ARGV[0])
	  or die "Can't open file $ARGV[0].\n";
	$file_str = <$FH>;
	close($FH);
}

$file_str =~ s/#[ \t]*include[ \t]*<(.*string.*)>/#include "$1"/g;
$file_str =~ s/(^|\W)find_t($|\W)/$1jstr_word_ty$2/g;
$file_str =~ s/(^|\W)op_t($|\W)/$1jstr_word_ty$2/g;
$file_str =~ s/(^|\W)((?:repeat_bytes|extractbyte|shift|find_index|has|clz|ctz)\w*)($|\W)/$1jstr_word_$2$3/g;
$file_str =~ s/(^|\W)_Bool($|\W)/$1int$2/g;
$file_str =~ s/(^|\W)OP_T_THRES($|\W)/$1JSTR_WORD_THRES$2/g;
$file_str =~ s/.*typedef jstr_word_ty jstr_word_ty;.*//g;
$file_str =~ s/__always_inline/JSTR_INLINE/g;
$file_str =~ s/sysdeps\/generic\///;
$file_str =~ s/(#define.*)/$1\n\n#include "jstr-macros.h"\n/ if (index($file_str, "\"jstr-macros.h\"") == -1);
$file_str =~ s/#\s*(ifndef|define)[ \t]*([^P]{,1}[^J][^S][^T][^R]\w*_H)/#$1 PJSTR_$2/g;
$file_str =~ s/PJSTR__/PJSTR_/g;
$file_str =~ s/if (__BYTE_ORDER == __LITTLE_ENDIAN)/if (JSTR_ENDIAN_LITTLE)/g;
$file_str =~ s/include[ \t]*"string\-/include "_string-/g;
$file_str =~ s/__attribute__\(\(__may_alias__\)\)/JSTR_MAY_ALIAS/g;
$file_str =~ s/__attribute__\(\(may_alias\)\)/JSTR_MAY_ALIAS/g;

print $file_str;
