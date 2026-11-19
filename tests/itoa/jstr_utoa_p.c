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
	/* zero */
	memset(tmp, 0xAA, sizeof(tmp));
	char *end = jstr_utoa_p(0, tmp, 10);
	assert(strcmp(tmp, "0") == 0);
	assert(*end == '\0');
	/* single digit */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(5, tmp, 10);
	assert(strcmp(tmp, "5") == 0);
	assert(*end == '\0');
	/* base 16 */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(255, tmp, 16);
	assert(strcmp(tmp, "ff") == 0);
	assert(*end == '\0');
	/* base 2 */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(255, tmp, 2);
	assert(strcmp(tmp, "11111111") == 0);
	/* base 8 */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(255, tmp, 8);
	assert(strcmp(tmp, "377") == 0);
	/* base 36 */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(35, tmp, 36);
	assert(strcmp(tmp, "z") == 0);
	/* large value */
	memset(tmp, 0xAA, sizeof(tmp));
	end = jstr_utoa_p(1234567U, tmp, 10);
	assert(*end == '\0');
	assert(strlen(tmp) > 0);
	return 0;
}
