/* SPDX-License-Identifier: MIT */
/* Taken from or based on musl's string functions with modifications.
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef JSTR_MUSL_H
#define JSTR_MUSL_H

#include "jstr-macros.h"

#ifndef JSTR_HAVE_SIMD
#	if defined __AVX512BW__ || defined __AVX2__ || defined __SSE2__
#		define JSTR_HAVE_SIMD 1
#		include "_simd.h"
#	endif
#endif

JSTR__BEGIN_DECLS
#include <limits.h>
JSTR__END_DECLS

#include "jstr-ctype.h"
#include "jstr-stdstring.h"

#define SS         (sizeof(size_t))
#define ALIGN      (sizeof(size_t) - 1)
#define ONES       ((size_t)-1 / UCHAR_MAX)
#define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x) - ONES) & ~(x) & HIGHS)

JSTR__BEGIN_DECLS

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
jstr__memrchr_musl(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s + n - 1;
	c = (unsigned char)c;
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)(p + 1) & ALIGN; n--, p--) {
		if (jstr_unlikely(n == 0))
			return NULL;
		if (*p == c)
			return (char *)p;
	}
	if (n >= SS && *p != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const word *ws = (const word *)(p - SS + 1);
		const size_t k = ONES * (unsigned char)c;
		for (word w = *ws; n >= SS && !HASZERO(w ^ k); w = *--ws, n -= SS) {}
		p = (unsigned char *)ws + SS - 1;
	}
#endif
	for (; n--; p--)
		if (*p == c)
			return (char *)p;
	return NULL;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
jstr__strcasechrnul_musl(const char *s, int c)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)p % ALIGN; p++)
		if (jstr_unlikely(*p == '\0') || jstr_tolower(*p) == c)
			return (char *)p;
	const size_t k = ONES * (unsigned char)c;
	const size_t l = ONES * (unsigned char)jstr_toupper(c);
	const word *ws = (word *)p;
	for (word w = *ws; !HASZERO(w) && !HASZERO(w ^ k) && !HASZERO(w ^ l); w = *++ws) {}
	p = (unsigned char *)ws;
#endif
	for (; *p && jstr_tolower(*p) != c; p++) {}
	return (char *)p;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static void *
jstr__memcasechr_musl(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	if (n >= sizeof(size_t) && jstr_tolower(*p) != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		for (; (uintptr_t)p % ALIGN; n--, ++p) {
			if (jstr_unlikely(n == 0))
				return NULL;
			if (jstr_tolower(*p) == c)
				return (char *)p;
		}
		const size_t k = ONES * (unsigned char)c;
		const size_t l = ONES * (unsigned char)jstr_toupper(c);
		const word *ws = (word *)p;
		for (word w = *ws; n >= sizeof(size_t) && !HASZERO(w ^ k) && !HASZERO(w ^ l); n -= sizeof(size_t), w = *++ws) {}
		p = (unsigned char *)ws;
	}
#endif
	for (; n && jstr_tolower(*p) != c; n--, ++p) {}
	return n ? (void *)p : NULL;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
jstr__strnchr_musl(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)s & ALIGN; n--, s++) {
		if (jstr_unlikely(*s == '\0') || jstr_unlikely(n == 0))
			return NULL;
		if (*s == (char)c)
			return (char *)s;
	}
	if (n >= SS && *s != (char)c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const size_t k = ONES * (unsigned char)c;
		const word *ws = (const word *)s;
		for (word w = *ws; n >= SS && !HASZERO(w) && !HASZERO(w ^ k); w = *++ws, n -= SS) {}
		s = (const char *)ws;
	}
#endif
	for (; n && *s && *s != (char)c; s++, n--) {}
	return n ? (char *)s : NULL;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
jstr__strncasechr_musl(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)p & ALIGN; n--, p++) {
		if (jstr_unlikely(*p == '\0') || jstr_unlikely(n == 0))
			return NULL;
		if (jstr_tolower(*p) == c)
			return (char *)p;
	}
	if (n >= SS && jstr_tolower(*p) != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const size_t k = ONES * (unsigned char)c;
		const size_t l = ONES * (unsigned char)jstr_toupper(c);
		const word *ws = (const word *)p;
		for (word w = *ws; n >= SS && !HASZERO(w) && !HASZERO(w ^ k) && !HASZERO(w ^ l); w = *++ws, n -= SS) {}
		p = (const unsigned char *)ws;
	}
#endif
	for (; n && *p && jstr_tolower(*p) != c; p++, n--) {}
	return n ? (char *)p : NULL;
}

#define BITOP(a, b, op) \
	((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 << ((size_t)(b) % (8 * sizeof *(a))))

JSTR_FUNC_PURE
static size_t
jstr__memspn_musl(const void *s, const char *c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *a = p;
	if (!c[0])
		return 0;
	if (!c[1]) {
		for (; *p == *c; p++) {}
		return (size_t)(p - a);
	}
	size_t byteset[32 / sizeof(size_t)];
	memset(byteset, 0, sizeof byteset);
	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++) {}
	for (; n-- && BITOP(byteset, *(unsigned char *)p, &); p++) {}
	return (size_t)(p - a);
}

JSTR_FUNC_PURE
static size_t
jstr__memcspn_musl(const void *s, const char *c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *a = p;
	if (!c[0] || !c[1]) {
		p = (const unsigned char *)memchr(p, *c, n);
		return (p ? (size_t)(p - a) : n);
	}
	size_t byteset[32 / sizeof(size_t)];
	memset(byteset, 0, sizeof byteset);
	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++) {}
	for (; n-- && !BITOP(byteset, *(unsigned char *)p, &); p++) {}
	return (size_t)(p - a);
}

JSTR_FUNC_PURE
static size_t
jstr__memrspn_musl(const void *s, const char *c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	if (!c[0])
		return 0;
	p += (n - 1);
	if (!c[1]) {
		for (; n && *p == *c; n--, p--) {}
		return n;
	}
	size_t byteset[32 / sizeof(size_t)];
	memset(byteset, 0, sizeof byteset);
	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++) {}
	for (; n && BITOP(byteset, *(unsigned char *)p, &); n--, p--) {}
	return n;
}

JSTR_FUNC_PURE
static size_t
jstr__memrcspn_musl(const void *s, const char *c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	if (!c[0])
		return n;
	if (!c[1]) {
#if 1
		p += (n - 1);
		for (; n && *p != *c; n--, p--) {}
		return n;
		/* TODO: figure out why using memrchr fails test */
#else
		const unsigned char *end = p + n;
#	if JSTR_HAVE_MEMRCHR
		p = (const unsigned char *)memrchr(p, *c, n);
#	elif JSTR_HAVE_SIMD
		p = (const unsigned char *)jstr__simd_memrchr(p, *c, n);
#	else
		p = (const unsigned char *)jstr__memrchr_musl(p, *c, n);
#	endif
		return p ? (size_t)(end - p) : 0;
#endif
	}
	p += (n - 1);
	size_t byteset[32 / sizeof(size_t)];
	memset(byteset, 0, sizeof byteset);
	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++) {}
	for (; n && !BITOP(byteset, *(unsigned char *)p, &); n--, p--) {}
	return n;
}

#undef BITOP

JSTR__END_DECLS

#undef SS
#undef ALIGN
#undef ONES
#undef HIGHS
#undef HASZERO

#endif /* JSTR_MUSL_H */
