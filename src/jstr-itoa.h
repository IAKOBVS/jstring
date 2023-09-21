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

#define P_JSTR_ITOA_ATTR     \
	JSTR_MAYBE_UNUSED    \
	JSTR_NONNULL_ALL     \
	JSTR_WARN_UNUSED     \
	JSTR_RETURNS_NONNULL \
	JSTR_NOTHROW

/* Return ptr to '\0' after the last digit in the DEST string. */
P_JSTR_ITOA_ATTR
static char *
jstr_ulltoa_p(char *R nptr,
	      unsigned long long number,
	      const unsigned int base) JSTR_NOEXCEPT
{
	char *start = nptr;
	do
		*nptr++ = number % base + '0';
	while (number /= base);
	*nptr = '\0';
	const char *const end = nptr;
	int c;
	while (start < --nptr) {
		c = *start;
		*start++ = *nptr;
		*nptr-- = c;
	}
	return (char *)end;
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_ultoa_p(char *R nptr,
	     unsigned long number,
	     const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_ulltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_utoa_p(char *R nptr,
	    unsigned int number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_ulltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
P_JSTR_ITOA_ATTR
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
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_ltoa_p(char *R nptr,
	    long number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_lltoa_p(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_itoa_p(char *R nptr,
	    int number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_lltoa_p(nptr, number, base);
}

P_JSTR_END_DECLS

#undef R

#undef P_JSTR_ITOA_ATTR

#endif /* JSTR_ITOA_H */
