/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000

char *buf;

static size_t
simple_memspn(const char *s, const char *c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *a = p;
	if (!c[0])
		return 0;
	if (!c[1]) {
		for (; n && *p == (unsigned char)*c; n--, p++) {}
		return (size_t)(p - a);
	}
	for (; n--; p++)
		if (!strchr(c, *p))
			return (size_t)(p - a);
	return (size_t)(p - a);
}

static size_t
simple_memcspn(const char *s, const char *c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *a = p;
	if (!c[0])
		return n;
	if (!c[1]) {
		p = (const unsigned char *)memchr(p, *c, n);
		return p ? (size_t)(p - a) : n;
	}
	for (; n--; p++)
		if (strchr(c, *p))
			return (size_t)(p - a);
	return (size_t)(p - a);
}

static size_t
simple_memrspn(const char *s, const char *c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s + n;
	if (!c[0])
		return 0;
	if (!c[1]) {
		for (; n && *--p == (unsigned char)*c; n--) {}
		return n;
	}
	size_t cnt = 0;
	for (p--; n--; p--, cnt++)
		if (!strchr(c, *p))
			break;
	return cnt;
}

static size_t
simple_memrcspn(const char *s, const char *c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s + n;
	if (!c[0])
		return n;
	if (!c[1]) {
		for (; n && *--p != (unsigned char)*c; n--) {}
		return n;
	}
	size_t cnt = 0;
	for (p--; n--; p--, cnt++)
		if (strchr(c, *p))
			break;
	return cnt;
}

#define T_DEFINE_SPN(name, impl_func, ...)                   \
	static JSTR_ATTR_MAYBE_UNUSED size_t                  \
	b_##name(void *dummy)                                 \
	{                                                     \
		size_t cs = 0;                                \
		for (size_t i = 0; i < 50; i++) {             \
			cs += impl_func(__VA_ARGS__);           \
			buf[0] ^= 1;                          \
		}                                             \
		return cs;                                    \
	}

/* Multi-char accept/reject set (3 chars) */
T_DEFINE_SPN(simple_memspn3, simple_memspn, buf, "abc", BUFLEN)
T_DEFINE_SPN(jstr_memspn3, jstr_memspn, buf, "abc", BUFLEN)
T_DEFINE_SPN(simple_memcspn3, simple_memcspn, buf, "xyz", BUFLEN)
T_DEFINE_SPN(jstr_memcspn3, jstr_memcspn, buf, "xyz", BUFLEN)
T_DEFINE_SPN(simple_memrspn3, simple_memrspn, buf, "abc", BUFLEN)
T_DEFINE_SPN(jstr_strrspn3, jstr_strrspn_len, buf, "abc", BUFLEN)
T_DEFINE_SPN(simple_memrcspn3, simple_memrcspn, buf, "xyz", BUFLEN)
T_DEFINE_SPN(jstr_strrcspn3, jstr_strrcspn_len, buf, "xyz", BUFLEN)

/* Single-char accept/reject */
T_DEFINE_SPN(simple_memspn1, simple_memspn, buf, "a", BUFLEN)
T_DEFINE_SPN(jstr_memspn1, jstr_memspn, buf, "a", BUFLEN)
T_DEFINE_SPN(simple_memcspn1, simple_memcspn, buf, "z", BUFLEN)
T_DEFINE_SPN(jstr_memcspn1, jstr_memcspn, buf, "z", BUFLEN)

int
main()
{
	buf = malloc(BUFLEN + 1);
	assert(buf);

	for (size_t i = 0; i < BUFLEN; i++)
		buf[i] = 'a' + (char)(i % 26);
	buf[BUFLEN] = '\0';

	/* Multi-char */
	RUN(b_simple_memspn3, 0);
	RUN(b_jstr_memspn3, 0);
	RUN(b_simple_memcspn3, 0);
	RUN(b_jstr_memcspn3, 0);
	RUN(b_simple_memrspn3, 0);
	RUN(b_jstr_strrspn3, 0);
	RUN(b_simple_memrcspn3, 0);
	RUN(b_jstr_strrcspn3, 0);

	/* Single-char */
	RUN(b_simple_memspn1, 0);
	RUN(b_jstr_memspn1, 0);
	RUN(b_simple_memcspn1, 0);
	RUN(b_jstr_memcspn1, 0);

	free(buf);
	return 0;
}
