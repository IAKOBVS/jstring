/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[] = "abac";
	assert(jstr_memnchr(buf, 'a', 4, 4) == buf);
	assert(jstr_memnchr(buf, 'c', 4, 2) == NULL);
	assert(jstr_memnchr(buf, 'a', 4, 1) == buf);
	assert(jstr_memnchr(buf, 'z', 4, 4) == NULL);
	/* n capped by sz */
	assert(jstr_memnchr(buf, 'c', 2, 10) == NULL);
	return 0;
}
