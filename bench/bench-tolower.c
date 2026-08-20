/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/string.h"
#include "../include/ctype.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000

char *buf;
char *dst;

static void
simple_tolower_len(char *s, size_t n)
{
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		if (*p >= 'A' && *p <= 'Z')
			*p += 'a' - 'A';
}

static void
simple_toupper_len(char *s, size_t n)
{
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		if (*p >= 'a' && *p <= 'z')
			*p -= 'a' - 'A';
}

static void
simple_tolowerstrcpy_len(char *d, char *s, size_t n)
{
	unsigned char *ud = (unsigned char *)d;
	const unsigned char *us = (unsigned char *)s;
	while (n--) {
		unsigned char c = *us++;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		*ud++ = c;
	}
	*ud = '\0';
}

static void
simple_toupperstrcpy_len(char *d, char *s, size_t n)
{
	unsigned char *ud = (unsigned char *)d;
	const unsigned char *us = (unsigned char *)s;
	while (n--) {
		unsigned char c = *us++;
		if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';
		*ud++ = c;
	}
	*ud = '\0';
}

static char *
simple_tolowerstr_p(char *s)
{
	unsigned char *p = (unsigned char *)s;
	for (; *p; ++p)
		if (*p >= 'A' && *p <= 'Z')
			*p += 'a' - 'A';
	return (char *)p;
}

static char *
simple_toupperstr_p(char *s)
{
	unsigned char *p = (unsigned char *)s;
	for (; *p; ++p)
		if (*p >= 'a' && *p <= 'z')
			*p -= 'a' - 'A';
	return (char *)p;
}

#define T_DEFINE_TOLOWER(impl_func, ...)                       \
	static JSTR_ATTR_MAYBE_UNUSED size_t                  \
	b_##impl_func(void *dummy)                            \
	{                                                     \
		size_t cs = 0;                                \
		for (size_t i = 0; i < 50; i++) {             \
			impl_func(__VA_ARGS__);                 \
			cs += (size_t)buf[0];                   \
		}                                             \
		return cs;                                    \
	}

T_DEFINE_TOLOWER(simple_tolower_len, buf, BUFLEN)
T_DEFINE_TOLOWER(simple_toupper_len, buf, BUFLEN)
T_DEFINE_TOLOWER(jstr_tolowerstr_len, buf, BUFLEN)
T_DEFINE_TOLOWER(jstr_toupperstr_len, buf, BUFLEN)

T_DEFINE_TOLOWER(simple_tolowerstrcpy_len, dst, buf, BUFLEN)
T_DEFINE_TOLOWER(simple_toupperstrcpy_len, dst, buf, BUFLEN)
T_DEFINE_TOLOWER(jstr_tolowerstrcpy_len, dst, buf, BUFLEN)
T_DEFINE_TOLOWER(jstr_toupperstrcpy_len, dst, buf, BUFLEN)

T_DEFINE_TOLOWER(simple_tolowerstr_p, buf)
T_DEFINE_TOLOWER(simple_toupperstr_p, buf)
T_DEFINE_TOLOWER(jstr_tolowerstr_p, buf)
T_DEFINE_TOLOWER(jstr_toupperstr_p, buf)

#define T_SETUP(buf, BUFLEN)                                       \
	do {                                                       \
		for (size_t i = 0; i < BUFLEN; i++)                \
			buf[i] = 'A' + (char)(i % 26);            \
		buf[BUFLEN] = '\0';                                \
	} while (0)

int
main()
{
	buf = malloc(BUFLEN + 1);
	dst = malloc(BUFLEN + 1);
	assert(buf && dst);
	T_SETUP(buf, BUFLEN);

	RUN(b_jstr_tolowerstr_len, 0);
	RUN(b_jstr_toupperstr_len, 0);
	RUN(b_simple_tolower_len, 0);
	RUN(b_simple_toupper_len, 0);

	RUN(b_jstr_tolowerstrcpy_len, 0);
	RUN(b_jstr_toupperstrcpy_len, 0);
	RUN(b_simple_tolowerstrcpy_len, 0);
	RUN(b_simple_toupperstrcpy_len, 0);

	RUN(b_jstr_tolowerstr_p, 0);
	RUN(b_jstr_toupperstr_p, 0);
	RUN(b_simple_tolowerstr_p, 0);
	RUN(b_simple_toupperstr_p, 0);

	free(buf);
	free(dst);
	return 0;
}
