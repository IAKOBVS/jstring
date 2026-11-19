/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char tmp[256];
	/* fast path <= 999 */
	memset(tmp, 0xAA, sizeof(tmp));
	char *end = jstr_ulltoa_thousep_p(123, tmp, 10, ',');
	assert(strcmp(tmp, "123") == 0);
	assert(*end == '\0');

	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_ulltoa_thousep_p(0, tmp, 10, ',');
	assert(strcmp(tmp, "0") == 0);

	/* grouped */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_ulltoa_thousep_p(1234567ULL, tmp, 10, ',');
	assert(strcmp(tmp, "1,234,567") == 0);
	assert(*end == '\0');

	/* base 16 */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_ulltoa_thousep_p(1000ULL, tmp, 16, ',');
	assert(*end == '\0');
	assert(strlen(tmp) > 0);

	return 0;
}
