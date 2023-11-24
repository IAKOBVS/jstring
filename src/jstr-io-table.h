/* See LICENSE file for copyright and license details. */

#ifndef JSTRIO_TABLE_H
#define JSTRIO_TABLE_H 1

#include "jstr-macros.h"

JSTR_MAYBE_UNUSED
static const unsigned char pjstrio_binary_table[256] = {
	1, /* 0 */
	1, /* 1 */
	1, /* 2 */
	1, /* 3 */
	1, /* 4 */
	1, /* 5 */
	1, /* 6 */
	1, /* 7 */
	1, /* 8 */
	0, /* 9 */
	0, /* 10 */
	1, /* 11 */
	1, /* 12 */
#if JSTR_OS_WINDOWS || JSTR_OS_WINDOWSCE
	/* Windows uses \r\n as newline. */
	0, /* 13 */
#else
	1, /* 13 */
#endif
	1, /* 14 */
	1, /* 15 */
	1, /* 16 */
	1, /* 17 */
	1, /* 18 */
	1, /* 19 */
	1, /* 20 */
	1, /* 21 */
	1, /* 22 */
	1, /* 23 */
	1, /* 24 */
	1, /* 25 */
	1, /* 26 */
	1, /* 27 */
	1, /* 28 */
	1, /* 29 */
	1, /* 30 */
	1 /* 31 */
};

#endif /* JSTRIO_TABLE_H */
