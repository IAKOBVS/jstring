#ifndef JSTR_ITOA_H
#define JSTR_ITOA_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-macros.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

#define PJSTR_ITOA_ATTR      \
	JSTR_MAYBE_UNUSED    \
	JSTR_NONNULL_ALL     \
	JSTR_WARN_UNUSED     \
	JSTR_RETURNS_NONNULL \
	JSTR_NOTHROW
#define PJSTR_ULTOA_UNROLLED(nptr, number, base)                                                                      \
	do {                                                                                                          \
		JSTR_ASSERT_IS_STR(nptr);                                                                             \
		JSTR_ASSERT_IS_SIZE(number);                                                                          \
		JSTR_ASSERT_IS_SIZE(base);                                                                            \
		if (number <= 9) {                                                                                    \
			nptr[0] = number % base + '0';                                                                \
			nptr += 1;                                                                                    \
		} else if (number <= 99) {                                                                            \
			nptr[0] = number / base % base + '0';                                                         \
			nptr[1] = number % base + '0';                                                                \
			nptr += 2;                                                                                    \
		} else if (number <= 999) {                                                                           \
			nptr[0] = number / base / base % base + '0';                                                  \
			nptr[1] = number / base % base + '0';                                                         \
			nptr[2] = number % base + '0';                                                                \
			nptr += 3;                                                                                    \
		} else if (number <= 9999) {                                                                          \
			nptr[0] = number / base / base / base % base + '0';                                           \
			nptr[1] = number / base / base % base + '0';                                                  \
			nptr[2] = number / base % base + '0';                                                         \
			nptr[3] = number % base + '0';                                                                \
			nptr += 4;                                                                                    \
		} else if (number <= 99999) {                                                                         \
			nptr[0] = number / base / base / base / base % base + '0';                                    \
			nptr[1] = number / base / base / base % base + '0';                                           \
			nptr[2] = number / base / base % base + '0';                                                  \
			nptr[3] = number / base % base + '0';                                                         \
			nptr[4] = number % base + '0';                                                                \
			nptr += 5;                                                                                    \
		} else if (number <= 999999) {                                                                        \
			nptr[0] = number / base / base / base / base / base % base + '0';                             \
			nptr[1] = number / base / base / base / base % base + '0';                                    \
			nptr[2] = number / base / base / base % base + '0';                                           \
			nptr[3] = number / base / base % base + '0';                                                  \
			nptr[4] = number / base % base + '0';                                                         \
			nptr[5] = number % base + '0';                                                                \
			nptr += 6;                                                                                    \
		} else if (number <= 9999999) {                                                                       \
			nptr[0] = number / base / base / base / base / base / base % base + '0';                      \
			nptr[1] = number / base / base / base / base / base % base + '0';                             \
			nptr[2] = number / base / base / base / base % base + '0';                                    \
			nptr[3] = number / base / base / base % base + '0';                                           \
			nptr[4] = number / base / base % base + '0';                                                  \
			nptr[5] = number / base % base + '0';                                                         \
			nptr[6] = number % base + '0';                                                                \
			nptr += 7;                                                                                    \
		} else if (number <= 99999999) {                                                                      \
			nptr[0] = number / base / base / base / base / base / base / base % base + '0';               \
			nptr[1] = number / base / base / base / base / base / base % base + '0';                      \
			nptr[2] = number / base / base / base / base / base % base + '0';                             \
			nptr[3] = number / base / base / base / base % base + '0';                                    \
			nptr[4] = number / base / base / base % base + '0';                                           \
			nptr[5] = number / base / base % base + '0';                                                  \
			nptr[6] = number / base % base + '0';                                                         \
			nptr[7] = number % base + '0';                                                                \
			nptr += 8;                                                                                    \
		} else if (number <= 999999999) {                                                                     \
			nptr[0] = number / base / base / base / base / base / base / base / base % base + '0';        \
			nptr[1] = number / base / base / base / base / base / base / base % base + '0';               \
			nptr[2] = number / base / base / base / base / base / base % base + '0';                      \
			nptr[3] = number / base / base / base / base / base % base + '0';                             \
			nptr[4] = number / base / base / base / base % base + '0';                                    \
			nptr[5] = number / base / base / base % base + '0';                                           \
			nptr[6] = number / base / base % base + '0';                                                  \
			nptr[7] = number / base % base + '0';                                                         \
			nptr[8] = number % base + '0';                                                                \
			nptr += 9;                                                                                    \
		} else {                                                                                              \
			nptr[0] = number / base / base / base / base / base / base / base / base / base % base + '0'; \
			nptr[1] = number / base / base / base / base / base / base / base / base % base + '0';        \
			nptr[2] = number / base / base / base / base / base / base / base % base + '0';               \
			nptr[3] = number / base / base / base / base / base / base % base + '0';                      \
			nptr[4] = number / base / base / base / base / base % base + '0';                             \
			nptr[5] = number / base / base / base / base % base + '0';                                    \
			nptr[6] = number / base / base / base % base + '0';                                           \
			nptr[7] = number / base / base % base + '0';                                                  \
			nptr[8] = number / base % base + '0';                                                         \
			nptr[9] = number % base + '0';                                                                \
			nptr += 10;                                                                                   \
		}                                                                                                     \
		*nptr = '\0';                                                                                         \
		return nptr;                                                                                          \
	} while (0)
#if 0

#	define PJSTR_ULLTOA_UNROLLED(nptr, number, base)                                                                                                                                    \
		do {                                                                                                                                                                         \
			JSTR_ASSERT_IS_STR(nptr);                                                                                                                                            \
			JSTR_ASSERT_IS_SIZE(number);                                                                                                                                         \
			JSTR_ASSERT_IS_SIZE(base);                                                                                                                                           \
			if (number <= 1) {                                                                                                                                                   \
				nptr[0] = number % base + '0';                                                                                                                               \
				nptr += 1;                                                                                                                                                   \
			} else if (number <= 10) {                                                                                                                                           \
				nptr[0] = number / base % base + '0';                                                                                                                        \
				nptr[1] = number % base + '0';                                                                                                                               \
				nptr += 2;                                                                                                                                                   \
			} else if (number <= 100) {                                                                                                                                          \
				nptr[0] = number / base / base % base + '0';                                                                                                                 \
				nptr[1] = number / base % base + '0';                                                                                                                        \
				nptr[2] = number % base + '0';                                                                                                                               \
				nptr += 3;                                                                                                                                                   \
			} else if (number <= 1000) {                                                                                                                                         \
				nptr[0] = number / base / base / base % base + '0';                                                                                                          \
				nptr[1] = number / base / base % base + '0';                                                                                                                 \
				nptr[2] = number / base % base + '0';                                                                                                                        \
				nptr[3] = number % base + '0';                                                                                                                               \
				nptr += 4;                                                                                                                                                   \
			} else if (number <= 10000) {                                                                                                                                        \
				nptr[0] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[1] = number / base / base / base % base + '0';                                                                                                          \
				nptr[2] = number / base / base % base + '0';                                                                                                                 \
				nptr[3] = number / base % base + '0';                                                                                                                        \
				nptr[4] = number % base + '0';                                                                                                                               \
				nptr += 5;                                                                                                                                                   \
			} else if (number <= 100000) {                                                                                                                                       \
				nptr[0] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[1] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[2] = number / base / base / base % base + '0';                                                                                                          \
				nptr[3] = number / base / base % base + '0';                                                                                                                 \
				nptr[4] = number / base % base + '0';                                                                                                                        \
				nptr[5] = number % base + '0';                                                                                                                               \
				nptr += 6;                                                                                                                                                   \
			} else if (number <= 1000000) {                                                                                                                                      \
				nptr[0] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[1] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[2] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[3] = number / base / base / base % base + '0';                                                                                                          \
				nptr[4] = number / base / base % base + '0';                                                                                                                 \
				nptr[5] = number / base % base + '0';                                                                                                                        \
				nptr[6] = number % base + '0';                                                                                                                               \
				nptr += 7;                                                                                                                                                   \
			} else if (number <= 10000000) {                                                                                                                                     \
				nptr[0] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[1] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[2] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[3] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[4] = number / base / base / base % base + '0';                                                                                                          \
				nptr[5] = number / base / base % base + '0';                                                                                                                 \
				nptr[6] = number / base % base + '0';                                                                                                                        \
				nptr[7] = number % base + '0';                                                                                                                               \
				nptr += 8;                                                                                                                                                   \
			} else if (number <= 100000000) {                                                                                                                                    \
				nptr[0] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[1] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[2] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[3] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[4] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[5] = number / base / base / base % base + '0';                                                                                                          \
				nptr[6] = number / base / base % base + '0';                                                                                                                 \
				nptr[7] = number / base % base + '0';                                                                                                                        \
				nptr[8] = number % base + '0';                                                                                                                               \
				nptr += 9;                                                                                                                                                   \
			} else if (number <= 1000000000) {                                                                                                                                   \
				nptr[0] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[1] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[2] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[3] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[4] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[5] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[6] = number / base / base / base % base + '0';                                                                                                          \
				nptr[7] = number / base / base % base + '0';                                                                                                                 \
				nptr[8] = number / base % base + '0';                                                                                                                        \
				nptr[9] = number % base + '0';                                                                                                                               \
				nptr += 10;                                                                                                                                                  \
			} else if (number <= 10000000000) {                                                                                                                                  \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[1] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[2] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[3] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[4] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[5] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[6] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[7] = number / base / base / base % base + '0';                                                                                                          \
				nptr[8] = number / base / base % base + '0';                                                                                                                 \
				nptr[9] = number / base % base + '0';                                                                                                                        \
				nptr[10] = number % base + '0';                                                                                                                              \
				nptr += 11;                                                                                                                                                  \
			} else if (number <= 100000000000) {                                                                                                                                 \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[2] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[3] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[4] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[5] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[6] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[7] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[8] = number / base / base / base % base + '0';                                                                                                          \
				nptr[9] = number / base / base % base + '0';                                                                                                                 \
				nptr[10] = number / base % base + '0';                                                                                                                       \
				nptr[11] = number % base + '0';                                                                                                                              \
				nptr += 12;                                                                                                                                                  \
			} else if (number <= 1000000000000) {                                                                                                                                \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[3] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[4] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[5] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[6] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[7] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[8] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[9] = number / base / base / base % base + '0';                                                                                                          \
				nptr[10] = number / base / base % base + '0';                                                                                                                \
				nptr[11] = number / base % base + '0';                                                                                                                       \
				nptr[12] = number % base + '0';                                                                                                                              \
				nptr += 13;                                                                                                                                                  \
			} else if (number <= 10000000000000) {                                                                                                                               \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[3] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[4] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[5] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[6] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[7] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[8] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[9] = number / base / base / base % base + '0';                                                                                                          \
				nptr[10] = number / base / base % base + '0';                                                                                                                \
				nptr[11] = number / base % base + '0';                                                                                                                       \
				nptr[13] = number % base + '0';                                                                                                                              \
				nptr += 13;                                                                                                                                                  \
			} else if (number <= 10000000000000) {                                                                                                                               \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[5] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[6] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[7] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[8] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[9] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[10] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[11] = number / base / base / base % base + '0';                                                                                                         \
				nptr[12] = number / base / base % base + '0';                                                                                                                \
				nptr[13] = number / base % base + '0';                                                                                                                       \
				nptr[14] = number % base + '0';                                                                                                                              \
				nptr += 14;                                                                                                                                                  \
			} else if (number <= 100000000000000) {                                                                                                                              \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[5] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[6] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[7] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[8] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[9] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[10] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[11] = number / base / base / base % base + '0';                                                                                                         \
				nptr[12] = number / base / base % base + '0';                                                                                                                \
				nptr[13] = number / base % base + '0';                                                                                                                       \
				nptr[14] = number % base + '0';                                                                                                                              \
				nptr += 15;                                                                                                                                                  \
			} else if (number <= 1000000000000000) {                                                                                                                             \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                      \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[5] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[6] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[7] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[8] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[9] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[10] = number / base / base / base / base / base % base + '0';                                                                                           \
				nptr[11] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[12] = number / base / base / base % base + '0';                                                                                                         \
				nptr[13] = number / base / base % base + '0';                                                                                                                \
				nptr[14] = number / base % base + '0';                                                                                                                       \
				nptr[15] = number % base + '0';                                                                                                                              \
				nptr += 16;                                                                                                                                                  \
			} else if (number <= 10000000000000000) {                                                                                                                            \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';               \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                      \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[5] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[6] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[7] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[8] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[9] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[10] = number / base / base / base / base / base / base % base + '0';                                                                                    \
				nptr[11] = number / base / base / base / base / base % base + '0';                                                                                           \
				nptr[12] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[13] = number / base / base / base % base + '0';                                                                                                         \
				nptr[14] = number / base / base % base + '0';                                                                                                                \
				nptr[15] = number / base % base + '0';                                                                                                                       \
				nptr[16] = number % base + '0';                                                                                                                              \
				nptr += 17;                                                                                                                                                  \
			} else if (number <= 100000000000000000) {                                                                                                                           \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';        \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';               \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                      \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[5] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[6] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[7] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[8] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[9] = number / base / base / base / base / base / base / base / base % base + '0';                                                                       \
				nptr[10] = number / base / base / base / base / base / base / base % base + '0';                                                                             \
				nptr[11] = number / base / base / base / base / base / base % base + '0';                                                                                    \
				nptr[12] = number / base / base / base / base / base % base + '0';                                                                                           \
				nptr[13] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[14] = number / base / base / base % base + '0';                                                                                                         \
				nptr[15] = number / base / base % base + '0';                                                                                                                \
				nptr[16] = number / base % base + '0';                                                                                                                       \
				nptr[17] = number % base + '0';                                                                                                                              \
				nptr += 18;                                                                                                                                                  \
			} else {                                                                                                                                                             \
				nptr[0] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0'; \
				nptr[1] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';        \
				nptr[2] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';               \
				nptr[3] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                      \
				nptr[4] = number / base / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                             \
				nptr[5] = number / base / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                    \
				nptr[6] = number / base / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                           \
				nptr[7] = number / base / base / base / base / base / base / base / base / base / base / base % base + '0';                                                  \
				nptr[8] = number / base / base / base / base / base / base / base / base / base / base % base + '0';                                                         \
				nptr[8] = number / base / base / base / base / base / base / base / base / base % base + '0';                                                                \
				nptr[10] = number / base / base / base / base / base / base / base / base % base + '0';                                                                      \
				nptr[11] = number / base / base / base / base / base / base / base % base + '0';                                                                             \
				nptr[12] = number / base / base / base / base / base / base % base + '0';                                                                                    \
				nptr[13] = number / base / base / base / base / base % base + '0';                                                                                           \
				nptr[14] = number / base / base / base / base % base + '0';                                                                                                  \
				nptr[15] = number / base / base / base % base + '0';                                                                                                         \
				nptr[16] = number / base / base % base + '0';                                                                                                                \
				nptr[17] = number / base % base + '0';                                                                                                                       \
				nptr[18] = number % base + '0';                                                                                                                              \
				nptr += 19;                                                                                                                                                  \
			}                                                                                                                                                                    \
			*nptr = '\0';                                                                                                                                                        \
			return nptr;                                                                                                                                                         \
		} while (0)
#endif

#define PJSTR_LTOA_UNROLLED(nptr, number, base)           \
	do {                                              \
		if (number < 0) {                         \
			number = -number;                 \
			*nptr++ = '-';                    \
		}                                         \
		PJSTR_ULTOA_UNROLLED(nptr, number, base); \
	} while (0)
#define PJSTR_ULLTOA_UNROLLED(nptr, number, base)            \
	do {                                                 \
		enum { _len = PJSTR_MAX_ULONG_LONG_DIGITS }; \
		char _s[_len + 1];                           \
		_s[_len] = '\0';                             \
		char *_p = _s + _len - 1;                    \
		do                                           \
			*_p-- = number % base + '0';         \
		while (number /= base);                      \
		while (*++_p)                                \
			*nptr++ = *_p;                       \
		*nptr = '\0';                                \
		return nptr;                                 \
	} while (0)
#define PJSTR_LLTOA_UNROLLED(nptr, number, base)           \
	do {                                               \
		if (number < 0) {                          \
			number = -number;                  \
			*nptr++ = '-';                     \
		}                                          \
		PJSTR_ULLTOA_UNROLLED(nptr, number, base); \
	} while (0)
/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_itoa(char *R _nptr,
	  int _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_LTOA_UNROLLED(_nptr, _number, _base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ltoa(char *R _nptr,
	  long _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_LTOA_UNROLLED(_nptr, _number, _base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_lltoa(char *R _nptr,
	   long long _number,
	   const unsigned int _base) JSTR_NOEXCEPT
{
	if (_number < 0) {
		_number = -_number;
		*_nptr++ = '-';
	}
	if (jstr_likely(_number <= 9999999999))
		PJSTR_ULTOA_UNROLLED(_nptr, _number, _base);
	PJSTR_ULLTOA_UNROLLED(_nptr, _number, _base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_utoa(char *R _nptr,
	  const unsigned int _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, _base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ultoa(char *R _nptr,
	   const unsigned long _number,
	   const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, _base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ulltoa(char *R _nptr,
	    unsigned long long _number,
	    const unsigned int _base) JSTR_NOEXCEPT
{
	if (jstr_likely(_number <= 9999999999))
		PJSTR_ULTOA_UNROLLED(_nptr, _number, _base);
	PJSTR_ULLTOA_UNROLLED(_nptr, _number, _base);
}

PJSTR_END_DECLS

#undef R

#undef PJSTR_ITOA_ATTR
#undef PJSTR_ULLTOA_UNROLLED
#undef PJSTR_ULTOA_UNROLLED
#undef PJSTR_LLTOA_UNROLLED
#undef PJSTR_LTOA_UNROLLED

#endif /* JSTR_ITOA_H */
