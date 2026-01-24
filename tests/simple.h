/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef SIMPLE_H
#define SIMPLE_H

#define JSTR_BUILT
#include "../include/string.h"

JSTR_ATTR_MAYBE_UNUSED
JSTR_ATTR_INLINE
static int
TOLOWER(int c)
{
	c = (char)c;
	return (unsigned char)((c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c);
}

JSTR_ATTR_MAYBE_UNUSED
JSTR_ATTR_INLINE
static int
TOUPPER(int c)
{
	c = (char)c;
	return (unsigned char)((c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c);
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strrstr_len(const char *h, size_t hl, const char *n, size_t nl)
{
	if (nl == 0)
		return (char *)h + hl;
	if (hl < nl)
		return NULL;
	const char *p = h + hl - nl;
	hl -= (nl - 1);
	for (; hl--; --p)
		if (*p == *n && !memcmp(p, n, nl))
			return (char *)p;
	return NULL;
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strrstr(const char *h, const char *n)
{
#if JSTR_HAVE_STRSTR && !JSTR_USE_SIMPLE
	return strstr(h, n);
#else
	return simple_strrstr_len(h, strlen(h), n, strlen(n));
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_memmem(const void *hs, const size_t hl, const void *ne, const size_t nl)
{
#if JSTR_HAVE_MEMMEM && !JSTR_USE_SIMPLE
	return (char *)memmem(hs, hl, ne, nl);
#else
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	if (nl == 0)
		return (char *)h;
	if (hl < nl)
		return NULL;
	const unsigned char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (*h == *n && !memcmp(h, n, nl))
			return (char *)h;
	return NULL;
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strnstr(const char *hs, const char *ne, size_t n)
{
	return (char *)simple_memmem(hs, jstr_strnlen(hs, n), ne, strlen(ne));
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strstr(const char *h, const char *n)
{
#if JSTR_HAVE_STRSTR && !JSTR_USE_SIMPLE
	return strstr(h, n);
#else
	return simple_memmem(h, strlen(h), n, strlen(n));
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static int
simple_strcasecmp(const char *s1, const char *s2)
{
#if JSTR_HAVE_STRCASECMP && !JSTR_USE_SIMPLE
	return strcasecmp(s1, s2);
#else
	int ret;
	for (; (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1; ++s1, ++s2)
		;
	return ret;
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static int
simple_strncasecmp(const char *s1, const char *s2, size_t n)
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_USE_SIMPLE
	return strncasecmp(s1, s2, n);
#else
	if (n == 0)
		return 0;
	int ret = TOLOWER(*s1) - TOLOWER(*s2);
	for (; n && (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1; --n, ++s1, ++s2)
		;
	if (n == 0)
		return 0;
	return ret;
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strcasestr_len(const char *h, const size_t hl, const char *n, const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl < nl)
		return NULL;
	const char *end = h + hl - nl;
	for (; h <= end; ++h)
		if (!simple_strncasecmp(h, n, nl))
			return (char *)h;
	return NULL;
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strncasestr(const char *hs, const char *ne, const size_t limit)
{
	if (*ne == '\0')
		return (char *)hs;
	const size_t ne_len = strlen(ne);
	if (ne_len < limit)
		return NULL;
	const char *end = hs + limit;
	for (; *hs && hs <= end; ++hs)
		if (!simple_strncasecmp(hs, ne, ne_len))
			return (char *)hs;
	return NULL;
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strcasestr(const char *h, const char *n)
{
#if JSTR_HAVE_STRCASESTR && !JSTR_USE_SIMPLE
	return (char *)strcasestr(h, n);
#else
	return simple_strcasestr_len(h, strlen(h), n, strlen(n));
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_stpcpy(char *d, const char *s)
{
#if JSTR_HAVE_STPCPY && !JSTR_USE_SIMPLE
	return stpcpy(d, s);
#else
	const size_t len = strlen(s);
	*((char *)memcpy(d, s, len) + len) = '\0';
	return d + len;
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_revcpy_p(char *dst, const char *src)
{
	size_t src_len = strlen(src);
	src += src_len - 1;
	for (; src_len--; *dst++ = *src--) {}
	*dst = '\0';
	return dst;
}

static JSTR_ATTR_MAYBE_UNUSED void *
simple_memrchr(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR && !JSTR_USE_SIMPLE
	return (char *)memrchr(s, c, n);
#else
	const unsigned char *p = (const unsigned char *)s + n;
	for (; n--;)
		if (*--p == (unsigned char)c)
			return (void *)p;
	return NULL;
#endif
}

/*
   strchr() before s + T.
*/
static JSTR_ATTR_MAYBE_UNUSED char *
simple_strnchr(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
	for (; n--; ++s) {
		if (*s == (char)c)
			return (char *)s;
		if (jstr_unlikely(*s == '\0'))
			break;
	}
	return NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strchrnul(const char *s, int c)
{
#if JSTR_HAVE_STRCHRNUL && !JSTR_USE_SIMPLE
	return (char *)strchrnul(s, c);
#else
	const char *const p = strchr(s, c);
	return (char *)(p ? p : s + strlen(s));
#endif
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strcasechrnul(const char *s, int c)
{
	c = TOLOWER(c);
	for (;; ++s)
		if (TOLOWER(*s) == c || *s == '\0')
			break;
	return (char *)s;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strcasechr(const char *s, int c)
{
	s = simple_strcasechrnul(s, c);
	return TOLOWER(*s) == TOLOWER(c) ? (char *)s : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strncasechr(const char *s, int c, size_t n)
{
	const char *p = simple_strcasechrnul(s, c);
	return (p < s + n && TOLOWER(*p) == TOLOWER(c)) ? (char *)p : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_memcasechr(const void *s, int c, size_t n)
{
	c = TOLOWER(c);
	const unsigned char *p = (const unsigned char *)s;
	for (; n && TOLOWER(*p) != c; --n, ++p) {}
	return n ? (char *)p : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_countchr_len(const char *s, int c, size_t sz)
{
	size_t cnt = 0;
	for (; sz--; cnt += *s++ == (char)c) {}
	return cnt;
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_countchr(const char *s, int c)
{
	return simple_countchr_len(s, c, strlen(s));
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_memrspn(const char *s, const char *accept, size_t n)
{
	if (*accept == '\0')
		return 0;
	s += (n - 1);
	for (; n; --s, --n)
		if (!strchr(accept, *s))
			break;
	return n;
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_memrcspn(const char *s, const char *reject, size_t n)
{
	if (*reject == '\0')
		return n;
	s += (n - 1);
	for (; n; --s, --n)
		if (strchr(reject, *s))
			break;
	return n;
}


#endif /* SIMPLE_H */
