/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strcasecmp_len("abc", "ABC", 3) == 0);
	assert(jstr_strcasecmp_len("abc\0zzz", "ABC\0yyy", 4) == 0); /* exact n includes NULs? tolower('\0') */
	/* differing at last */
	assert(jstr_strcasecmp_len("abx", "aby", 3) != 0);
	return 0;
}
