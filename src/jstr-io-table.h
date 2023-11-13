/* See LICENSE file for copyright and license details. */

#ifndef JSTR_IO_TABLE_H
#define JSTR_IO_TABLE_H 1

#include "jstr-macros.h"

JSTR_MAYBE_UNUSED
static const unsigned char jstrp_io_reject_table[256] = {
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
/* Windows uses \r\n as newline. */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
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
	1, /* 31 */
	0, /* 32 */
	0, /* 33 */
	0, /* 34 */
	0, /* 35 */
	0, /* 36 */
	0, /* 37 */
	0, /* 38 */
	0, /* 39 */
	0, /* 40 */
	0, /* 41 */
	0, /* 42 */
	0, /* 43 */
	0, /* 44 */
	0, /* 45 */
	0, /* 46 */
	0, /* 47 */
	0, /* 48 */
	0, /* 49 */
	0, /* 50 */
	0, /* 51 */
	0, /* 52 */
	0, /* 53 */
	0, /* 54 */
	0, /* 55 */
	0, /* 56 */
	0, /* 57 */
	0, /* 58 */
	0, /* 59 */
	0, /* 60 */
	0, /* 61 */
	0, /* 62 */
	0, /* 63 */
	0, /* 64 */
	0, /* 65 */
	0, /* 66 */
	0, /* 67 */
	0, /* 68 */
	0, /* 69 */
	0, /* 70 */
	0, /* 71 */
	0, /* 72 */
	0, /* 73 */
	0, /* 74 */
	0, /* 75 */
	0, /* 76 */
	0, /* 77 */
	0, /* 78 */
	0, /* 79 */
	0, /* 80 */
	0, /* 81 */
	0, /* 82 */
	0, /* 83 */
	0, /* 84 */
	0, /* 85 */
	0, /* 86 */
	0, /* 87 */
	0, /* 88 */
	0, /* 89 */
	0, /* 90 */
	0, /* 91 */
	0, /* 92 */
	0, /* 93 */
	0, /* 94 */
	0, /* 95 */
	0, /* 96 */
	0, /* 97 */
	0, /* 98 */
	0, /* 99 */
	0, /* 100 */
	0, /* 101 */
	0, /* 102 */
	0, /* 103 */
	0, /* 104 */
	0, /* 105 */
	0, /* 106 */
	0, /* 107 */
	0, /* 108 */
	0, /* 109 */
	0, /* 110 */
	0, /* 111 */
	0, /* 112 */
	0, /* 113 */
	0, /* 114 */
	0, /* 115 */
	0, /* 116 */
	0, /* 117 */
	0, /* 118 */
	0, /* 119 */
	0, /* 120 */
	0, /* 121 */
	0, /* 122 */
	0, /* 123 */
	0, /* 124 */
	0, /* 125 */
	0, /* 126 */
	1, /* 127 */
};

#endif /* JSTR_IO_TABLE_H */
