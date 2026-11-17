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
	char buf[8] = "abcd";
	char *r = jstr_stpset(buf, 'X');
	assert(r == buf + 4);
	assert(memcmp(buf, "XXXX", 4) == 0);
	assert(buf[4] == '\0');
	return 0;
}
