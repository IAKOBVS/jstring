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
jstr_ultoa_p(char *R nptr,
	     unsigned long number,
	     const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_ulltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *
jstr_utoa_p(char *R nptr,
	    unsigned int number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_ulltoa_p(nptr, number, base);
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

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *
jstr_ltoa_p(char *R nptr,
	    long number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_lltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *
jstr_itoa_p(char *R nptr,
	    int number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_lltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa_sep_p(char *R nptr,
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
jstr_ultoa_sep_p(char *R nptr,
		 unsigned long number,
		 const unsigned int base,
		 const int separator)
{
	return jstr_ulltoa_sep_p(nptr, number, base, separator);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_utoa_sep_p(char *R nptr,
		unsigned int number,
		const unsigned int base,
		const int separator)
{
	return jstr_ulltoa_sep_p(nptr, number, base, separator);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_lltoa_sep_p(char *R nptr,
		 long long number,
		 const unsigned int base,
		 const int separator)
{
	if (number < 0) {
		number = -number;
		*nptr++ = '-';
	}
	return jstr_ulltoa_sep_p(nptr, number, base, separator);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_ltoa_sep_p(char *R nptr,
		long number,
		const unsigned int base,
		const int separator)
{
	return jstr_lltoa_sep_p(nptr, number, base, separator);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_itoa_sep_p(char *R nptr,
		int number,
		const unsigned int base,
		const int separator)
{
	return jstr_lltoa_sep_p(nptr, number, base, separator);
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_ITOA_H */
