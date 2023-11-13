#!/usr/bin/perl
use strict;
use warnings;

sub replaceall {
	my ($str_ref, $find_ref, $replace_ref) = @_;
	$$str_ref =~ s/(^|\W)$$find_ref($|\W)/$1$$replace_ref$2/g;
}

my $file_str;
{
	local $/;
	open(my $FH, '<', $ARGV[0])
	  or die "Can't open file $ARGV[0].\n";
	$file_str = <$FH>;
	close($FH);
}

if (index($file_str, '"jstr-macros.h"') == -1) {
	$file_str =~ s/(#define.*)/$1\n\n#include "jstr-macros.h"\n/;
}

replaceall(\$file_str, \'if\s*\(__BYTE_ORDER\s*==\s*__LITTLE_ENDIAN\)',              \'if (JSTR_ENDIAN_LITTLE)');
replaceall(\$file_str, \'(?:find_t|op_t)',                                           \'jstr_word_ty');
replaceall(\$file_str, \'(?:_Bool|bool)',                                            \'int');
replaceall(\$file_str, \'OP_T_THRES',                                                \'JSTR_WORD_THRES');
replaceall(\$file_str, \'__attribute__\s*\(\(\s*(?:__may_alias__|may_alias)\s*\)\)', \'JSTR_MAY_ALIAS');
replaceall(\$file_str, \'__always_inline',                                           \'JSTR_INLINE');
$file_str =~ s/(^|\W)((?:repeat_bytes|extractbyte|shift|find_index|has|clz|ctz)\w*)($|\W)/$1jstr_word_$2$3/g;
$file_str =~ s/\n[ \t]*typedef jstr_word_ty find_t;\s*\n/\n/g;
$file_str =~ s/#[ \t]*(ifndef|define)[ \t]*_{,1}(\w*_H)/#$1 PJSTR_$2/g;
$file_str =~ s/include[ \t]*<string\-([-._A-Za-z0-9]*)>/include "_string-$1"/g;
$file_str =~ s/include[ \t]*<sysdeps\/generic\/([-._A-Za-z0-9]*)>/include "_glibc_generic-$1"/g;
$file_str =~ s/\n\n\n/\n\n/g;

print $file_str;
