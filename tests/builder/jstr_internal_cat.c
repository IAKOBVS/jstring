/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

static jstr_ret_ty
cat_wrap(char **s, size_t *sz, size_t *cap, size_t arg_len, ...)
{
	va_list ap;
	va_start(ap, arg_len);
	jstr_ret_ty r = jstr_internal_cat(s, sz, cap, ap, arg_len);
	va_end(ap);
	return r;
}

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* append two strings */
	assert(cat_wrap(&s, &sz, &cap, 11, "hello", " ", "world", NULL) == JSTR_RET_SUCC);
	/* arg_len must equal total length of non-NULL? function loops until NULL; arg_len used reserve */
	assert(sz == 11);
	assert(strcmp(s, "hello world") == 0);

	/* empty total? arg_len 0 reserves sz+1; va list first NULL -> loop ends; sz unchanged? function does *sz += arg_len (0) then loop; returns succ */
	char *s2 = NULL;
	size_t sz2 = 0, cap2 = 0;
	assert(cat_wrap(&s2, &sz2, &cap2, 0, NULL) == JSTR_RET_SUCC);
	assert(sz2 == 0);
	assert(s2 != NULL || cap2 == 0);

	jstr_free(&s, &sz, &cap);
	jstr_free(&s2, &sz2, &cap2);
	return EXIT_SUCCESS;
}
