#ifndef JSTR_ITOA_H
#define JSTR_ITOA_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#include "jstr-replace.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p(char *R nptr,
	      unsigned long long number,
	      const unsigned int base) JSTR_NOEXCEPT
{
	char *start = nptr;
	do
		*nptr++ = number % base + '0';
	while (number /= base);
	const char *const end = nptr;
	*nptr-- = '\0';
	int c;
	while (start < nptr) {
		c = *start;
		*start++ = *nptr;
		*nptr-- = c;
	}
	return (char *)end;
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *
jstr_lltoa_p(char *R nptr,
	     long long number,
	     const unsigned int base) JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*nptr++ = '-';
	}
	return jstr_ulltoa_p(nptr, number, base);
}

#define P_JSTR_ULLTOA(type, name, func)                                   \
	/* Return ptr to '\0' after the last digit in the DEST string. */ \
	JSTR_FUNC                                                         \
	JSTR_RETURNS_NONNULL                                              \
	JSTR_INLINE                                                       \
	static char *                                                     \
	jstr_##name(char *R nptr,                                         \
		    type number,                                          \
		    const unsigned int base) JSTR_NOEXCEPT                \
	{                                                                 \
		return func(nptr, number, base);                          \
	}

P_JSTR_ULLTOA(unsigned long, ultoa_p, jstr_ulltoa_p)
P_JSTR_ULLTOA(unsigned int, utoa_p, jstr_ulltoa_p)
P_JSTR_ULLTOA(long, ltoa_p, jstr_lltoa_p)
P_JSTR_ULLTOA(int, itoa_p, jstr_lltoa_p)

#undef P_JSTR_ULLTOA

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_p_sep(char *R nptr,
		  unsigned long long number,
		  const unsigned int base,
		  const int separator)
{
	char *start = nptr;
	int n = 0;
	int c;
	for (unsigned long long loop;;) {
		c = number % base + '0';
		loop = number /= base;
		if (++n != 3) {
			*nptr++ = c;
			if (jstr_unlikely(loop == 0))
				break;
		} else {
			if (jstr_likely(loop)) {
				*(nptr) = c;
				*(nptr + 1) = separator;
				nptr += 2;
			} else {
				*nptr++ = c;
				break;
			}
			n = 0;
		}
	}
	const char *const end = nptr;
	*nptr-- = '\0';
	while (start < nptr) {
		c = *start;
		*start++ = *nptr;
		*nptr-- = c;
	}
	return (char *)end;
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_lltoa_p_sep(char *R nptr,
		 long long number,
		 const unsigned int base,
		 const int separator)
{
	if (number < 0) {
		number = -number;
		*nptr++ = '-';
	}
	return jstr_ulltoa_p_sep(nptr, number, base, separator);
}

#define P_JSTR_ULLTOA_SEP(type, name, func)                               \
	/* Return ptr to '\0' after the last digit in the DEST string. */ \
	JSTR_FUNC                                                         \
	JSTR_RETURNS_NONNULL                                              \
	JSTR_INLINE                                                       \
	static char *                                                     \
	jstr_##name(char *R nptr,                                         \
		    type number,                                          \
		    const unsigned int base,                              \
		    const int separator) JSTR_NOEXCEPT                    \
	{                                                                 \
		return func(nptr, number, base, separator);               \
	}

P_JSTR_ULLTOA_SEP(unsigned long, ultoa_p_sep, jstr_ulltoa_p_sep)
P_JSTR_ULLTOA_SEP(unsigned int, utoa_p_sep, jstr_ulltoa_p_sep)
P_JSTR_ULLTOA_SEP(long, ltoa_p_sep, jstr_lltoa_p_sep)
P_JSTR_ULLTOA_SEP(int, itoa_p_sep, jstr_lltoa_p_sep)

#undef P_JSTR_ULLTOA_SEP

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_ITOA_H */
