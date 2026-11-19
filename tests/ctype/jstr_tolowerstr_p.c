/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[64];
	strcpy(buf, "");
	assert(jstr_tolowerstr_p(buf) == buf);
	strcpy(buf, "ABCxyz123");
	char *r = jstr_tolowerstr_p(buf);
	assert(r == buf + strlen(buf));
	assert(strcmp(buf, "abcxyz123") == 0);
	return 0;
}
