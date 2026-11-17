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
	char buf[64];
	/* n=0 no-op */
	jstr_tolowerstr_len(NULL, 0);
	strcpy(buf, "ABC");
	jstr_tolowerstr_len(buf, 0);
	assert(strcmp(buf, "ABC") == 0);
	/* converts n bytes including embedded NUL */
	buf[0]='A'; buf[1]='\0'; buf[2]='B'; buf[3]='C';
	jstr_tolowerstr_len(buf, 4);
	assert(buf[0]=='a' && buf[1]=='\0' && buf[2]=='b' && buf[3]=='c');
	/* full */
	char b2[8]; strcpy(b2,"ABC"); jstr_tolowerstr_len(b2, 3);
	assert(strcmp(b2,"abc")==0);
	return 0;
}
