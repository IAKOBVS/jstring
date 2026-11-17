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
	char src[] = {'A','B','C',0,'d','e','f',0};
	/* n=0 no work, no terminator */
	dst[0]='X'; jstr_tolowerstrcpy_len(dst, src, 0);
	assert(dst[0]=='X');
	/* n>0 copies+converts+terminates */
	jstr_tolowerstrcpy_len(dst, src, 4);
	assert(dst[0]=='a' && dst[1]=='b' && dst[2]=='c' && dst[3]=='\0' && dst[4]=='\0');
	/* overlap not allowed but distinct ok */
	char s2[]="HELLO";
	jstr_tolowerstrcpy_len(dst, s2, 5);
	assert(strcmp(dst,"hello")==0);
	return 0;
}
