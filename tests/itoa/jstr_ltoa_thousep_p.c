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
	char tmp[256];
	memset(tmp, 0xAA, sizeof(tmp));
	char *end = jstr_ltoa_thousep_p(-999, tmp, 10, ',');
	assert(strcmp(tmp, "-999") == 0);
	assert(*end == '\0');

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_ltoa_thousep_p(-12345, tmp, 10, ',');
	assert(tmp[0] == '-');
	assert(*end == '\0');
	assert(strlen(tmp) > 4);

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_ltoa_thousep_p(0, tmp, 10, ',');
	assert(strcmp(tmp, "0") == 0);

	return 0;
}
