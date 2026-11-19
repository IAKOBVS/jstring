/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char src[] = "abc";
	char *p = jstr_strdup_len(src, 3);
	assert(p != NULL);
	assert(strcmp(p, "abc") == 0);
	free(p);
	/* n=0 */
	p = jstr_strdup_len(src, 0);
	assert(p != NULL);
	assert(p[0] == '\0');
	free(p);
	return 0;
}
