/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[] = "hello";
	assert(jstr_strnlen(buf, 10) == 5);
	assert(jstr_strnlen(buf, 3) == 3);
	assert(jstr_strnlen("", 5) == 0);
	char buf2[4];
	buf2[0] = 'a';
	buf2[1] = 'b';
	buf2[2] = 'c';
	buf2[3] = '\0';
	assert(jstr_strnlen(buf2, 2) == 2);
	return 0;
}
