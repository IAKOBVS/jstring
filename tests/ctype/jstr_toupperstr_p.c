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
	strcpy(buf, "");
	assert(jstr_toupperstr_p(buf) == buf);
	strcpy(buf, "abcXYZ123");
	char *r = jstr_toupperstr_p(buf);
	assert(r == buf + strlen(buf));
	assert(strcmp(buf, "ABCXYZ123") == 0);
	strcpy(buf, "a\0hidden");
	/* stops at first NUL */
	jstr_toupperstr_p(buf);
	assert(buf[0]=='A' && buf[1]=='\0');
	return 0;
}
