/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;
	char dst[64];
	char src[] = {'a','b','c',0,'D','E','F',0};
	dst[0]='X'; jstr_toupperstrcpy_len(dst, src, 0);
	assert(dst[0]=='X');
	jstr_toupperstrcpy_len(dst, src, 4);
	assert(dst[0]=='A' && dst[1]=='B' && dst[2]=='C' && dst[3]=='\0' && dst[4]=='\0');
	char s2[]="hello";
	jstr_toupperstrcpy_len(dst, s2, 5);
	assert(strcmp(dst,"HELLO")==0);
	return 0;
}
