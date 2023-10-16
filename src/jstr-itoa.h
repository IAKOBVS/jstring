#ifndef JSTR_ITOA_H
#define JSTR_ITOA_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <math.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-replace.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
static const char *R const pjstr_itoa_digits = "0123456789abcdefghijklmnopqrstuvwxyz";

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
	case 10: LOOP_BASE(10);
	case 2: LOOP_BASE(2);
	case 8: LOOP_BASE(8);
	case 16: LOOP_BASE(16);
	default: LOOP_BASE(base);
	}
#undef LOOP_BASE
	const char *const end = buf;
	*buf-- = '\0';
	int c;
	while (start < buf) {
		c = *start;
		*start++ = *buf;
		*buf-- = c;
	}
	return (char *)end;
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

#undef PJSTR_ULLTOA

/*
   Convert number to string with thousand separator.
   Return value:
   ptr to '\0' after the last digit in the DEST string.
*/
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p_sep(unsigned long long number,
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
		case 2: CONV(2);
		case 8: CONV(8);
		case 16: CONV(16);
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
jstr_lltoa_p_sep(long long number,
		 char *R buf,
		 const unsigned int base,
		 const int separator)
JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*buf++ = '-';
	}
	return jstr_ulltoa_p_sep(number, buf, base, separator);
}

#define PJSTR_ULLTOA_SEP(type, name, u)                                     \
	/*                                                                  \
	   Convert number to string with thousand separator.                \
	   Return value:                                                    \
	   ptr to '\0' after the last digit in the DEST string.             \
	*/                                                                  \
	JSTR_FUNC                                                           \
	JSTR_RETURNS_NONNULL                                                \
	JSTR_INLINE                                                         \
	static char *                                                       \
	jstr_##name##_p_sep(type number,                                    \
			    char *R buf,                                    \
			    const unsigned int base,                        \
			    const int separator)                            \
	JSTR_NOEXCEPT                                                       \
	{                                                                   \
		return jstr_##u##lltoa_p_sep(number, buf, base, separator); \
	}

PJSTR_ULLTOA_SEP(unsigned long, ultoa, u)
PJSTR_ULLTOA_SEP(unsigned int, utoa, u)
PJSTR_ULLTOA_SEP(long, ltoa, )
PJSTR_ULLTOA_SEP(int, itoa, )

#undef PJSTR_ULLTOA_SEP

PJSTR_END_DECLS

#undef R

#endif /* JSTR_ITOA_H */
