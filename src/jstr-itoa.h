/* See LICENSE file for copyright and license details. */

#ifndef JSTR_ITOA_H
#define JSTR_ITOA_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <math.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-replace.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
static const char pjstr_itoa_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

#define PJSTR_DEFINE_ITOA_COUNTUDIGITS(T, name)                 \
	JSTR_FUNC                                               \
	JSTR_INLINE                                             \
	static int                                              \
	pjstr_itoa_countudigits_##name(T number,                \
	                               const unsigned int base) \
	{                                                       \
		switch (base) {                                 \
		case 10: return sizeof(number) / 10;            \
		case 16: return sizeof(number) / 16;            \
		case 2: return sizeof(number) / 2;              \
		case 8: return sizeof(number) / 8;              \
		default: return sizeof(number) / base;          \
		}                                               \
	}

PJSTR_DEFINE_ITOA_COUNTUDIGITS(unsigned int, utoa)
PJSTR_DEFINE_ITOA_COUNTUDIGITS(unsigned long, ultoa)
PJSTR_DEFINE_ITOA_COUNTUDIGITS(unsigned long long, ulltoa)

#undef PJSTR_DEFINE_ITOA_COUNTUDIGITS

/*
   Return value:
   ptr to '\0' after the last digit in the DEST string.
*/
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p(unsigned long long number,
              char *R buf,
              const unsigned int base)
JSTR_NOEXCEPT
{
#define LOOP_BASE(base)                                    \
	do                                                 \
		*buf++ = pjstr_itoa_digits[number % base]; \
	while ((number /= base) != 0);                     \
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
	const char *const end = buf;
	*buf-- = '\0';
	int c;
	while (start < buf) {
		c = *start;
		*start++ = *buf;
		*buf-- = c;
	}
	return (char *)end;
#undef LOOP_BASE
}

/*
   Return value:
   ptr to '\0' after the last digit in the DEST string.
*/
JSTR_FUNC
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *
jstr_lltoa_p(long long number,
             char *R buf,
             const unsigned int base)
JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*buf++ = '-';
	}
	return jstr_ulltoa_p(number, buf, base);
}

#define PJSTR_ULLTOA(type, name, u)                             \
	/*                                                      \
	   Return value:                                        \
	   ptr to '\0' after the last digit in the DEST string. \
	*/                                                      \
	JSTR_FUNC                                               \
	JSTR_RETURNS_NONNULL                                    \
	JSTR_INLINE                                             \
	static char *                                           \
	jstr_##name##_p(type number,                            \
	                char *R buf,                            \
	                const unsigned int base)                \
	JSTR_NOEXCEPT                                           \
	{                                                       \
		return jstr_##u##lltoa_p(number, buf, base);    \
	}

PJSTR_ULLTOA(unsigned long, ultoa, u)
PJSTR_ULLTOA(unsigned int, utoa, u)
PJSTR_ULLTOA(long, ltoa, )
PJSTR_ULLTOA(int, itoa, )

#define PJSTR_DEFINE_UTOA_SAFE(T, name)                                                                         \
	JSTR_FUNC_VOID                                                                                          \
	JSTR_INLINE                                                                                             \
	static jstr_ret_ty                                                                                      \
	jstr_##name(char *R *R s,                                                                               \
	            size_t *R sz,                                                                               \
	            size_t *R cap,                                                                              \
	            T number,                                                                                   \
	            const unsigned int base)                                                                    \
	{                                                                                                       \
		PJSTR_RESERVE(s, sz, cap, *sz + pjstr_itoa_countudigits_##name(number, base), return JSTR_RET_ERR); \
		*sz = jstr_ulltoa_p(number, *s + *sz, base) - *s;                                               \
		return JSTR_RET_SUCC;                                                                               \
	}

PJSTR_DEFINE_UTOA_SAFE(unsigned int, utoa)
PJSTR_DEFINE_UTOA_SAFE(unsigned long, ultoa)
PJSTR_DEFINE_UTOA_SAFE(unsigned long long, ulltoa)

#undef PJSTR_DEFINE_UTOA_SAFE

#define PJSTR_DEFINE_ITOA_SAFE(T, name, fn_name)                                                                                            \
	JSTR_FUNC_VOID                                                                                                                      \
	JSTR_INLINE                                                                                                                         \
	static jstr_ret_ty                                                                                                                  \
	jstr_##name(char *R *R s,                                                                                                           \
	            size_t *R sz,                                                                                                           \
	            size_t *R cap,                                                                                                          \
	            T number,                                                                                                               \
	            const unsigned int base)                                                                                                \
	{                                                                                                                                   \
		PJSTR_RESERVE(s, sz, cap, *sz + pjstr_itoa_countudigits_##fn_name((number < 0) ? -number : number, base), return JSTR_RET_ERR); \
		*sz = jstr_lltoa_p(number, *s + *sz, base) - *s;                                                                            \
		return JSTR_RET_SUCC;                                                                                                           \
	}

PJSTR_DEFINE_ITOA_SAFE(int, itoa, utoa)
PJSTR_DEFINE_ITOA_SAFE(long, ltoa, ultoa)
PJSTR_DEFINE_ITOA_SAFE(long long, lltoa, ulltoa)

#undef PJSTR_DEFINE_ITOA_SAFE

#undef PJSTR_ULLTOA

/*
   Convert number to string with thousand separator.
   Return value:
   ptr to '\0' after the last digit in the DEST string.
*/
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p_thousep(unsigned long long number,
                      char *R buf,
                      const unsigned int base,
                      const int separator)
JSTR_NOEXCEPT
{
#define CONV(base)                            \
	c = pjstr_itoa_digits[number % base]; \
	loop = number /= base;                \
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
	const char *const end = buf;
	*buf-- = '\0';
	while (start < buf) {
		c = *start;
		*start++ = *buf;
		*buf-- = c;
	}
	return (char *)end;
#undef CONV
}

/*
   Convert number to string with thousand separator.
   Return value:
   ptr to '\0' after the last digit in the DEST string.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_lltoa_p_thousep(long long number,
                     char *R buf,
                     const unsigned int base,
                     const int separator)
JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*buf++ = '-';
	}
	return jstr_ulltoa_p_thousep(number, buf, base, separator);
}

#define PJSTR_ULLTOA_SEP(type, name, u)                                         \
	/*                                                                      \
	   Convert number to string with thousand separator.                    \
	   Return value:                                                        \
	   ptr to '\0' after the last digit in the DEST string.                 \
	*/                                                                      \
	JSTR_FUNC                                                               \
	JSTR_RETURNS_NONNULL                                                    \
	JSTR_INLINE                                                             \
	static char *                                                           \
	jstr_##name##_p_thousep(type number,                                    \
	                        char *R buf,                                    \
	                        const unsigned int base,                        \
	                        const int separator)                            \
	JSTR_NOEXCEPT                                                           \
	{                                                                       \
		return jstr_##u##lltoa_p_thousep(number, buf, base, separator); \
	}

PJSTR_ULLTOA_SEP(unsigned long, ultoa, u)
PJSTR_ULLTOA_SEP(unsigned int, utoa, u)
PJSTR_ULLTOA_SEP(long, ltoa, )
PJSTR_ULLTOA_SEP(int, itoa, )

#undef PJSTR_ULLTOA_SEP

PJSTR_END_DECLS

#undef R

#endif /* JSTR_ITOA_H */
