/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/string.h"
#include "../include/ctype.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000

char *buf;

static char *
simple_strcasestr(const char *h, const char *n)
{
	if (jstr_unlikely(*n == 0))
		return (char *)h;
	const char *p1, *p2;
	for (; *h; ++h) {
		for (p1 = h + 1, p2 = n + 1; *p1 && jstr_tolower(*p1) == jstr_tolower(*p2); ++p1, ++p2) {}
		if (*p2 == '\0')
			return (char *)h;
		if (jstr_unlikely(*p1 == '\0'))
			break;
	}
	return NULL;
}

static char *
simple_strcasestr_tolower(const char *h, const char *n)
{
	if (jstr_unlikely(*n == 0))
		return (char *)h;
	size_t hl = strlen(h);
	char *tmp = malloc(hl + 1);
	if (!tmp)
		return NULL;
	memcpy(tmp, h, hl + 1);
	jstr_tolowerstr_len(tmp, hl);
	char *p = strstr(tmp, n);
	char *result = p ? (char *)h + (p - tmp) : NULL;
	free(tmp);
	return result;
}

/* needle "abc" */
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_strcasestr_abc(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr(buf, "abc"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_tolower_abc(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr_tolower(buf, "abc"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_abc(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr(buf, "abc"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_len_abc(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr_len(buf, BUFLEN, "abc", 3); buf[0] ^= 1; } return cs; }

/* needle "ABC" */
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_strcasestr_ABC(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr(buf, "ABC"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_tolower_ABC(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr_tolower(buf, "ABC"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_ABC(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr(buf, "ABC"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_len_ABC(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr_len(buf, BUFLEN, "ABC", 3); buf[0] ^= 1; } return cs; }

/* needle "xyz" (not found — worst case) */
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_strcasestr_xyz(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr(buf, "xyz"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_simple_tolower_xyz(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)simple_strcasestr_tolower(buf, "xyz"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_xyz(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr(buf, "xyz"); buf[0] ^= 1; } return cs; }
static JSTR_ATTR_MAYBE_UNUSED size_t b_jstr_strcasestr_len_xyz(void *d) { size_t cs = 0; for (size_t i = 0; i < 50; i++) { cs += (size_t)jstr_strcasestr_len(buf, BUFLEN, "xyz", 3); buf[0] ^= 1; } return cs; }

int
main()
{
	buf = malloc(BUFLEN + 1);
	assert(buf);

	for (size_t i = 0; i < BUFLEN; i++)
		buf[i] = 'a' + (char)(i % 26);
	buf[BUFLEN] = '\0';

	RUN(b_simple_strcasestr_abc, 0);
	RUN(b_simple_tolower_abc, 0);
	RUN(b_jstr_strcasestr_abc, 0);
	RUN(b_jstr_strcasestr_len_abc, 0);

	RUN(b_simple_strcasestr_ABC, 0);
	RUN(b_simple_tolower_ABC, 0);
	RUN(b_jstr_strcasestr_ABC, 0);
	RUN(b_jstr_strcasestr_len_ABC, 0);

	RUN(b_simple_strcasestr_xyz, 0);
	RUN(b_simple_tolower_xyz, 0);
	RUN(b_jstr_strcasestr_xyz, 0);
	RUN(b_jstr_strcasestr_len_xyz, 0);

	free(buf);
	return 0;
}
