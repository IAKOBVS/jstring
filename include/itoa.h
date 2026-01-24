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

#ifndef JSTR_ITOA_H
#	define JSTR_ITOA_H 1

#	include "macros.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#	include "builder.h"
#	include "replace.h"

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

#	define JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(T, name, is_thousep)                       \
		JSTR_FUNC                                                                 \
		static size_t jstr_internal_itoa_countudigits_##name(T number, unsigned int base) \
		JSTR_NOEXCEPT                                                             \
		{                                                                         \
			switch (base) {                                                   \
			case 10:                                                          \
				return sizeof(number) / 10                                \
				       + (is_thousep ? number / 3 : 0);                   \
			case 16:                                                          \
				return sizeof(number) / 16                                \
				       + (is_thousep ? number / 3 : 0);                   \
			case 2:                                                           \
				return sizeof(number) / 2                                 \
				       + (is_thousep ? number / 3 : 0);                   \
			case 8:                                                           \
				return sizeof(number) / 8                                 \
				       + (is_thousep ? number / 3 : 0);                   \
			default:                                                          \
				return sizeof(number) / base                              \
				       + (is_thousep ? number / 3 : 0);                   \
			}                                                                 \
		}

JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned int, utoa, 0)
JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned long, ultoa, 0)
JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned long long, ulltoa, 0)

JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned int, utoa_thousep, 1)
JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned long, ultoa_thousep, 1)
JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(unsigned long long, ulltoa_thousep, 1)

#	undef JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS

/* Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_ATTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p(unsigned long long number, char *R buf, unsigned int base)
JSTR_NOEXCEPT
{
#	define LOOP_BASE(base)                       \
		do                                    \
			*buf++ = number % base + '0'; \
		while ((number /= base) != 0);        \
		break
	char *start = buf;
	switch (base) {
	case 10:
		if (number <= 9) {
			*buf = number + '0';
			*(buf + 1) = '\0';
			return buf + 1;
		}
		LOOP_BASE(10);
	case 16: LOOP_BASE(16);
	case 2: LOOP_BASE(2);
	case 8: LOOP_BASE(8);
	default: LOOP_BASE(base);
	}
	char *end = buf;
	*buf-- = '\0';
	int c;
	for (; start < buf;) {
		c = *start;
		*start++ = *buf;
		*buf-- = c;
	}
	return (char *)end;
#	undef LOOP_BASE
}

/* Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_ATTR_RETURNS_NONNULL
static char *
jstr_lltoa_p(long long number, char *R buf, unsigned int base)
JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*buf++ = '-';
	}
	return jstr_ulltoa_p((unsigned long long)number, buf, base);
}

#	define JSTR_INTERNAL_ULLTOA(type, name, u)                                               \
		/* Return value:                                                          \
		   ptr to '\0' after the last digit in the DEST string. */                \
		JSTR_FUNC                                                                 \
		JSTR_ATTR_RETURNS_NONNULL                                                 \
		static char *jstr_##name##_p(type number, char *R buf, unsigned int base) \
		JSTR_NOEXCEPT                                                             \
		{                                                                         \
			return jstr_##u##lltoa_p(number, buf, base);                      \
		}

JSTR_INTERNAL_ULLTOA(unsigned long, ultoa, u)
JSTR_INTERNAL_ULLTOA(unsigned int, utoa, u)
JSTR_INTERNAL_ULLTOA(long, ltoa, )
JSTR_INTERNAL_ULLTOA(int, itoa, )

#	undef JSTR_INTERNAL_ULLTOA

/* Convert number to string with thousand separator.
 * Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_ATTR_RETURNS_NONNULL
static char *
jstr_ulltoa_thousep_p(unsigned long long number, char *R buf, unsigned int base, char separator)
JSTR_NOEXCEPT
{
#	define CONV(base)               \
		c = number % base + '0'; \
		loop = number /= base;   \
		break
	if (number <= 999)
		return jstr_ulltoa_p(number, buf, base);
	char *start = buf;
	int n = 0;
	int c;
	for (unsigned long long loop;;) {
		switch (base) {
		case 10: CONV(10);
		case 16: CONV(16);
		case 2: CONV(2);
		case 8: CONV(8);
		default: CONV(base);
		}
		if (++n != 3) {
			*buf++ = c;
			if (loop == 0)
				break;
		} else {
			*buf = c;
			if (loop == 0)
				break;
			*(buf + 1) = separator;
			buf += 2;
			n = 0;
		}
	}
	const char *end = buf;
	*buf-- = '\0';
	while (start < buf) {
		c = *start;
		*start++ = *buf;
		*buf-- = c;
	}
	return (char *)end;
#	undef CONV
}

/* Convert number to string with thousand separator.
 * Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
static char *
jstr_lltoa_thousep_p(long long number, char *R buf, unsigned int base, char separator)
JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*buf++ = '-';
	}
	return jstr_ulltoa_thousep_p((unsigned long long)number, buf, base, separator);
}

#	define JSTR_INTERNAL_ULLTOA_SEP(type, name, u)                                                                   \
		/* Convert number to string with thousand separator.                                              \
		   Return value:                                                                                  \
		   ptr to '\0' after the last digit in the DEST string. */                                        \
		JSTR_FUNC                                                                                         \
		JSTR_ATTR_RETURNS_NONNULL                                                                         \
		static char *jstr_##name##_thousep_p(type number, char *R buf, unsigned int base, char separator) \
		JSTR_NOEXCEPT                                                                                     \
		{                                                                                                 \
			return jstr_##u##lltoa_thousep_p(number, buf, base, separator);                           \
		}

JSTR_INTERNAL_ULLTOA_SEP(unsigned long, ultoa, u)
JSTR_INTERNAL_ULLTOA_SEP(unsigned int, utoa, u)
JSTR_INTERNAL_ULLTOA_SEP(long, ltoa, )
JSTR_INTERNAL_ULLTOA_SEP(int, itoa, )

#	undef JSTR_INTERNAL_ULLTOA_SEP

#	define JSTR_INTERNAL_DEFINE_UTOA_SAFE(T, name)                                                                         \
		JSTR_FUNC                                                                                               \
		static jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base)  \
		JSTR_NOEXCEPT                                                                                           \
		{                                                                                                       \
			if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##name(number, base) + 1))) \
				return JSTR_RET_ERR;                                                                    \
			*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base), *s);                                   \
			return JSTR_RET_SUCC;                                                                           \
		}

JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned int, utoa)
JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned long, ultoa)
JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned long long, ulltoa)

#	undef JSTR_INTERNAL_DEFINE_UTOA_SAFE

#	define JSTR_INTERNAL_DEFINE_ITOA_SAFE(T, name, func_name)                                                                                                                      \
		JSTR_FUNC                                                                                                                                                       \
		static jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base)                                                          \
		JSTR_NOEXCEPT                                                                                                                                                   \
		{                                                                                                                                                               \
			if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##func_name((number < 0) ? (unsigned T) - number : (unsigned T)number, base) + 1))) \
				return JSTR_RET_ERR;                                                                                                                            \
			*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base), *s);                                                                                           \
			return JSTR_RET_SUCC;                                                                                                                                   \
		}

JSTR_INTERNAL_DEFINE_ITOA_SAFE(int, itoa, utoa)
JSTR_INTERNAL_DEFINE_ITOA_SAFE(long, ltoa, ultoa)
JSTR_INTERNAL_DEFINE_ITOA_SAFE(long long, lltoa, ulltoa)

#	undef JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE

#	define JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(T, name)                                                                                \
		JSTR_FUNC                                                                                                              \
		static jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator) \
		JSTR_NOEXCEPT                                                                                                          \
		{                                                                                                                      \
			if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##name(number, base) + 1)))                \
				return JSTR_RET_ERR;                                                                                   \
			*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base, separator), *s);                                       \
			return JSTR_RET_SUCC;                                                                                          \
		}

JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned int, utoa_thousep)
JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned long, ultoa_thousep)
JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned long long, ulltoa_thousep)

#	undef JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE

#	define JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(T, name, func_name)                                                                                                              \
		JSTR_FUNC                                                                                                                                                       \
		static jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator)                                          \
		JSTR_NOEXCEPT                                                                                                                                                   \
		{                                                                                                                                                               \
			if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##func_name((number < 0) ? (unsigned T) - number : (unsigned T)number, base) + 1))) \
				return JSTR_RET_ERR;                                                                                                                            \
			*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base, separator), *s);                                                                                \
			return JSTR_RET_SUCC;                                                                                                                                   \
		}

JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(int, itoa_thousep, utoa_thousep)
JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(long, ltoa_thousep, ultoa_thousep)
JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(long long, lltoa_thousep, ulltoa_thousep)

#	undef JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE

#	define JSTR_INTERNAL_DEFINE_ATOU(T, func_name)                             \
		JSTR_FUNC                                                   \
		static T jstr_##func_name##_len(const char *R s, size_t sz) \
		JSTR_NOEXCEPT                                               \
		{                                                           \
			if (jstr_unlikely(sz == 0))                         \
				return 0;                                   \
			T n = (T)(*s++ - '0');                              \
			while (--sz)                                        \
				n = (n * 10) + (T)(*s++ - '0');             \
			return n;                                           \
		}

JSTR_INTERNAL_DEFINE_ATOU(unsigned int, atou)
JSTR_INTERNAL_DEFINE_ATOU(unsigned long, atoul)
JSTR_INTERNAL_DEFINE_ATOU(unsigned long long, atoull)

#	undef JSTR_INTERNAL_DEFINE_ATOU

#	define JSTR_INTERNAL_DEFINE_ATOI(T, func_name)                             \
		JSTR_FUNC                                                   \
		static T jstr_##func_name##_len(const char *R s, size_t sz) \
		JSTR_NOEXCEPT                                               \
		{                                                           \
			if (jstr_unlikely(sz == 0))                         \
				return 0;                                   \
			T n;                                                \
			if (*s == '-') {                                    \
				if (jstr_unlikely(sz == 1))                 \
					return 0;                           \
				--sz;                                       \
				n = -(*s++ - '0');                          \
			} else {                                            \
				n = (T)(*s++ - '0');                        \
			}                                                   \
			while (--sz)                                        \
				n = (n * 10) + (T)(*s++ - '0');             \
			return n;                                           \
		}

JSTR_INTERNAL_DEFINE_ATOI(int, atoi)
JSTR_INTERNAL_DEFINE_ATOI(long, atol)
JSTR_INTERNAL_DEFINE_ATOI(long long, atoll)

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_ITOA_H */
