/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#ifndef JSTR_ITOA_H
#	define JSTR_ITOA_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <limits.h>
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#	ifdef JSTR_IMPLEMENTATION
#		undef JSTR_IMPLEMENTATION
#		include "builder.h"
#		include "replace.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

/* Render digit D (0..base-1, base <= 36) as a lowercase alnum char. */
JSTR_FUNC
static char
jstr_internal_itoa_digit(unsigned long long d) JSTR_NOEXCEPT
{
	return (char)((d < 10) ? '0' + (int)d : 'a' + (int)d - 10);
}

/* Maximum digit count for T in BASE (worst case value), plus thousand
 * separator slots when IS_THOUSEP. NUMBER is unused; kept so call sites
 * can pass the value through unchanged.
 * digits_max = ceil(bits / log2(base)) is upper-bounded by bits/floor_log2(base) + 1. */
#	define JSTR_INTERNAL_DEFINE_ITOA_COUNTUDIGITS(T, name, is_thousep)                                       \
		JSTR_FUNC                                                                                         \
		static size_t jstr_internal_itoa_countudigits_##name(T number, unsigned int base)                 \
		JSTR_NOEXCEPT                                                                                     \
		{                                                                                                 \
			size_t bits = (size_t)sizeof(T) * (size_t)CHAR_BIT;                                       \
			size_t l2;                                                                                \
			size_t dmax;                                                                              \
			(void)number;                                                                             \
			switch (base) {                                                                           \
			case 2: l2 = 1; break;                                                                    \
			case 8: l2 = 3; break;                                                                    \
			case 10: l2 = 3; break;                                                                   \
			case 16: l2 = 4; break;                                                                   \
			default:                                                                                  \
				l2 = 0;                                                                           \
				while ((((size_t)1 << (l2 + 1)) <= (size_t)base) && (l2 < bits)) ++l2;            \
				if (jstr_unlikely(l2 == 0)) l2 = 1;                                               \
				break;                                                                            \
			}                                                                                         \
			dmax = bits / l2 + 1;                                                                     \
			return dmax + (is_thousep ? dmax / 3 : 0);                                                \
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
char *
jstr_ulltoa_p(unsigned long long number, char *R buf, unsigned int base) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		define LOOP_BASE(base)                                          \
			do                                                        \
				*buf++ = jstr_internal_itoa_digit(number % base); \
			while ((number /= base) != 0);                            \
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
#		undef LOOP_BASE
}
#	else
;
#	endif

/* Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_ATTR_RETURNS_NONNULL
char *
jstr_lltoa_p(long long number, char *R buf, unsigned int base) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (number < 0) {
		number = (long long)-(unsigned long long)number;
		*buf++ = '-';
	}
	return jstr_ulltoa_p((unsigned long long)number, buf, base);
}
#	else
;
#	endif

#	define JSTR_INTERNAL_ULLTOA(type, name, u)                                         \
		/* Return value:                                                          \
		   ptr to '\0' after the last digit in the DEST string. */ \
		JSTR_FUNC                                                                   \
		JSTR_ATTR_RETURNS_NONNULL                                                   \
		static char *jstr_##name##_p(type number, char *R buf, unsigned int base)   \
		JSTR_NOEXCEPT                                                               \
		{                                                                           \
			return jstr_##u##lltoa_p(number, buf, base);                        \
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
char *
jstr_ulltoa_thousep_p(unsigned long long number, char *R buf, unsigned int base, char separator) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		define CONV(base)                                        \
			c = (int)jstr_internal_itoa_digit(number % base); \
			loop = number /= base;                            \
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
			*buf++ = c;
			if (loop == 0)
				break;
			*buf++ = separator;
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
#		undef CONV
}
#	else
;
#	endif

/* Convert number to string with thousand separator.
 * Return value:
 * ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
char *
jstr_lltoa_thousep_p(long long number, char *R buf, unsigned int base, char separator) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (number < 0) {
		number = (long long)-(unsigned long long)number;
		*buf++ = '-';
	}
	return jstr_ulltoa_thousep_p((unsigned long long)number, buf, base, separator);
}
#	else
;
#	endif

#	define JSTR_INTERNAL_ULLTOA_SEP(type, name, u)                                                             \
		/* Convert number to string with thousand separator.                                              \
		   Return value:                                                                                  \
		   ptr to '\0' after the last digit in the DEST string. */ \
		JSTR_FUNC                                                                                           \
		JSTR_ATTR_RETURNS_NONNULL                                                                           \
		static char *jstr_##name##_thousep_p(type number, char *R buf, unsigned int base, char separator)   \
		JSTR_NOEXCEPT                                                                                       \
		{                                                                                                   \
			return jstr_##u##lltoa_thousep_p(number, buf, base, separator);                             \
		}

JSTR_INTERNAL_ULLTOA_SEP(unsigned long, ultoa, u)
JSTR_INTERNAL_ULLTOA_SEP(unsigned int, utoa, u)
JSTR_INTERNAL_ULLTOA_SEP(long, ltoa, )
JSTR_INTERNAL_ULLTOA_SEP(int, itoa, )

#	undef JSTR_INTERNAL_ULLTOA_SEP

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_itoa_chk_base(base)                          \
			do {                                                        \
				if (jstr_unlikely((base) < 2 || (base) > 36))       \
					JSTR_RETURN_ERR(JSTR_RET_ERR);              \
			} while (0)
#		define JSTR_INTERNAL_DEFINE_UTOA_SAFE(T, name)                                                                         \
			JSTR_FUNC                                                                                                       \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base) JSTR_NOEXCEPT   \
			{                                                                                                               \
				JSTR_INTERNAL_itoa_chk_base(base);                                                                      \
				if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##name(number, base) + 1))) \
					JSTR_RETURN_ERR(JSTR_RET_ERR);                                                                            \
				*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base), *s);                                           \
				return JSTR_RET_SUCC;                                                                                   \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_UTOA_SAFE(T, name) \
			JSTR_FUNC                               \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned int, utoa)
JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned long, ultoa)
JSTR_INTERNAL_DEFINE_UTOA_SAFE(unsigned long long, ulltoa)

#	undef JSTR_INTERNAL_DEFINE_UTOA_SAFE

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_DEFINE_ITOA_SAFE(T, name, func_name)                                                                                                                      \
			JSTR_FUNC                                                                                                                                                               \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base) JSTR_NOEXCEPT                                                    \
			{                                                                                                                                                                       \
				JSTR_INTERNAL_itoa_chk_base(base);                                                                                                                              \
				if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##func_name((number < 0) ? ((unsigned T)0 - (unsigned T)number) : (unsigned T)number, base) + 1))) \
					JSTR_RETURN_ERR(JSTR_RET_ERR);                                                                                                                                    \
				*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base), *s);                                                                                                   \
				return JSTR_RET_SUCC;                                                                                                                                           \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_ITOA_SAFE(T, name, func_name) \
			JSTR_FUNC                                          \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_ITOA_SAFE(int, itoa, utoa)
JSTR_INTERNAL_DEFINE_ITOA_SAFE(long, ltoa, ultoa)
JSTR_INTERNAL_DEFINE_ITOA_SAFE(long long, lltoa, ulltoa)

#	undef JSTR_INTERNAL_DEFINE_ITOA_SAFE

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(T, name)                                                                                      \
			JSTR_FUNC                                                                                                                            \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator) JSTR_NOEXCEPT \
			{                                                                                                                                    \
				JSTR_INTERNAL_itoa_chk_base(base);                                                                                           \
				if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##name(number, base) + 1)))                      \
					JSTR_RETURN_ERR(JSTR_RET_ERR);                                                                                                 \
				*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base, separator), *s);                                                     \
				return JSTR_RET_SUCC;                                                                                                        \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(T, name) \
			JSTR_FUNC                                       \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned int, utoa_thousep)
JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned long, ultoa_thousep)
JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE(unsigned long long, ulltoa_thousep)

#	undef JSTR_INTERNAL_DEFINE_UTOA_THOUSEP_SAFE

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(T, name, func_name)                                                                                                              \
			JSTR_FUNC                                                                                                                                                               \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator) JSTR_NOEXCEPT                                    \
			{                                                                                                                                                                       \
				JSTR_INTERNAL_itoa_chk_base(base);                                                                                                                              \
				if (jstr_chk(jstr_reserve(s, sz, cap, *sz + jstr_internal_itoa_countudigits_##func_name((number < 0) ? ((unsigned T)0 - (unsigned T)number) : (unsigned T)number, base) + 1))) \
					JSTR_RETURN_ERR(JSTR_RET_ERR);                                                                                                                                    \
				*sz = JSTR_DIFF(jstr_##name##_p(number, *s + *sz, base, separator), *s);                                                                                        \
				return JSTR_RET_SUCC;                                                                                                                                           \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(T, name, func_name) \
			JSTR_FUNC                                                  \
			jstr_ret_ty jstr_##name(char *R *R s, size_t *R sz, size_t *R cap, T number, unsigned int base, char separator) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(int, itoa_thousep, utoa_thousep)
JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(long, ltoa_thousep, ultoa_thousep)
JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE(long long, lltoa_thousep, ulltoa_thousep)

#	undef JSTR_INTERNAL_DEFINE_ITOA_THOUSEP_SAFE
#	undef JSTR_INTERNAL_itoa_chk_base

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_DEFINE_ATOU(T, func_name)                                   \
			JSTR_FUNC                                                                 \
			T jstr_##func_name##_len(const char *R s, size_t sz) JSTR_NOEXCEPT \
			{                                                                         \
				if (jstr_unlikely(sz == 0))                                       \
					return 0;                                                 \
				T n = (T)(*s++ - '0');                                            \
				while (--sz)                                                      \
					n = (n * 10) + (T)(*s++ - '0');                           \
				return n;                                                         \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_ATOU(T, func_name) \
			JSTR_FUNC                               \
			T jstr_##func_name##_len(const char *R s, size_t sz) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_ATOU(unsigned int, atou)
JSTR_INTERNAL_DEFINE_ATOU(unsigned long, atoul)
JSTR_INTERNAL_DEFINE_ATOU(unsigned long long, atoull)

#	undef JSTR_INTERNAL_DEFINE_ATOU

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_INTERNAL_DEFINE_ATOI(T, func_name)                                   \
			JSTR_FUNC                                                                 \
			T jstr_##func_name##_len(const char *R s, size_t sz) JSTR_NOEXCEPT \
			{                                                                         \
				if (jstr_unlikely(sz == 0))                                       \
					return 0;                                                 \
				T n;                                                              \
				if (*s == '-') {                                                  \
					if (jstr_unlikely(sz == 1))                               \
						return 0;                                         \
					--sz;                                                     \
					n = -(*s++ - '0');                                        \
				} else {                                                          \
					n = (T)(*s++ - '0');                                      \
				}                                                                 \
				while (--sz)                                                      \
					n = (n * 10) + (T)(*s++ - '0');                           \
				return n;                                                         \
			}
#	else
#		define JSTR_INTERNAL_DEFINE_ATOI(T, func_name) \
			JSTR_FUNC                               \
			T jstr_##func_name##_len(const char *R s, size_t sz) JSTR_NOEXCEPT;
#	endif

JSTR_INTERNAL_DEFINE_ATOI(int, atoi)
JSTR_INTERNAL_DEFINE_ATOI(long, atol)
JSTR_INTERNAL_DEFINE_ATOI(long long, atoll)

#	undef JSTR_INTERNAL_DEFINE_ATOI

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_ITOA_H */
