#ifndef JSTR_RAREBYTE_H
#define JSTR_RAREBYTE_H

#include "jstr-ctype-table.h"
#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
static const unsigned char pjstr_rarebyte_table[256] = {
	0, /* 0 */
	0, /* 1 */
	0, /* 2 */
	0, /* 3 */
	0, /* 4 */
	0, /* 5 */
	0, /* 6 */
	0, /* 7 */
	0, /* 8 */
	0, /* 9 */
	0, /* 10 */
	0, /* 11 */
	0, /* 12 */
	0, /* 13 */
	0, /* 14 */
	0, /* 15 */
	0, /* 16 */
	0, /* 17 */
	0, /* 18 */
	0, /* 19 */
	0, /* 20 */
	0, /* 21 */
	0, /* 22 */
	0, /* 23 */
	0, /* 24 */
	0, /* 25 */
	0, /* 26 */
	0, /* 27 */
	0, /* 28 */
	0, /* 29 */
	0, /* 30 */
	0, /* 31 */
	0, /* ' ' */
	1, /* '!' */
	1, /* '"' */
	1, /* '#' */
	1, /* '$' */
	1, /* '%' */
	1, /* '&' */
	1, /* '\'' */
	1, /* '(' */
	1, /* ')' */
	1, /* '*' */
	1, /* '+' */
	1, /* '', */
	1, /* '-' */
	1, /* '.' */
	1, /* '/' */
	1, /* '0' */
	1, /* '1' */
	1, /* '2' */
	1, /* '3' */
	1, /* '4' */
	1, /* '5' */
	1, /* '6' */
	1, /* '7' */
	1, /* '8' */
	1, /* '9' */
	1, /* ':' */
	1, /* ';' */
	1, /* '<' */
	1, /* '=' */
	1, /* '>' */
	1, /* '?' */
	1, /* '@' */
	1, /* 'A' */
	1, /* 'B' */
	1, /* 'C' */
	1, /* 'D' */
	1, /* 'E' */
	1, /* 'F' */
	1, /* 'G' */
	1, /* 'H' */
	1, /* 'I' */
	1, /* 'J' */
	1, /* 'K' */
	1, /* 'L' */
	1, /* 'M' */
	1, /* 'N' */
	1, /* 'O' */
	1, /* 'P' */
	1, /* 'Q' */
	1, /* 'R' */
	1, /* 'S' */
	1, /* 'T' */
	1, /* 'U' */
	1, /* 'V' */
	1, /* 'W' */
	1, /* 'X' */
	1, /* 'Y' */
	1, /* 'Z' */
	1, /* '[' */
	1, /* '\\' */
	1, /* ']' */
	1, /* '^' */
	1, /* '_' */
	1, /* '`' */
	0, /* 'a' */
	0, /* 'b' */
	0, /* 'c' */
	0, /* 'd' */
	0, /* 'e' */
	0, /* 'f' */
	0, /* 'g' */
	0, /* 'h' */
	0, /* 'i' */
	1, /* 'j' */
	0, /* 'k' */
	0, /* 'l' */
	0, /* 'm' */
	0, /* 'n' */
	0, /* 'o' */
	0, /* 'p' */
	1, /* 'q' */
	0, /* 'r' */
	0, /* 's' */
	0, /* 't' */
	0, /* 'u' */
	0, /* 'v' */
	0, /* 'w' */
	1, /* 'x' */
	0, /* 'y' */
	1, /* 'z' */
	1, /* '{' */
	1, /* '|' */
	1, /* '}' */
	1, /* '~' */
	0 /* 127 */
};

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static unsigned char *
pjstr_rarebyteget(const unsigned char *n)
{
	int c;
	for (; (c = *n); ++n)
		if (pjstr_rarebyte_table[c])
			return (unsigned char *)n;
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static unsigned char *
pjstr_rarebytegetcase(const unsigned char *n)
{
	int c;
	for (; (c = *n); ++n)
		if (pjstr_ctype[c] & (JSTR_ISDIGIT | JSTR_ISPUNCT))
			return (unsigned char *)n;
	return NULL;
}

PJSTR_END_DECLS

#endif /* JSTR_RAREBYTE_H */
