/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;
	char tmp[256];
	memset(tmp, 0xAA, sizeof(tmp));
	char *end = jstr_itoa_p(0, tmp, 10);
	assert(strcmp(tmp, "0") == 0);
	assert(*end == '\0');

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_itoa_p(-1, tmp, 10);
	assert(strcmp(tmp, "-1") == 0);
	assert(*end == '\0');

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_itoa_p(-9, tmp, 10);
	assert(strcmp(tmp, "-9") == 0);

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_itoa_p(INT_MIN, tmp, 10);
	assert(tmp[0] == '-');
	assert(*end == '\0');

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_itoa_p(123, tmp, 16);
	assert(strcmp(tmp, "7b") == 0);

	return 0;
}
