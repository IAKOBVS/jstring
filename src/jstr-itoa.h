#ifndef JSTR_ITOA_H
#define JSTR_ITOA_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

#define P_JSTR_ITOA_ATTR      \
	JSTR_MAYBE_UNUSED    \
	JSTR_NONNULL_ALL     \
	JSTR_WARN_UNUSED     \
	JSTR_RETURNS_NONNULL \
	JSTR_NOTHROW
#define P_JSTR_UTOA_UNROLLED(nptr, number, base)                                                                       \
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

#if 1
#	define P_JSTR_ULTOA_UNROLLED(nptr, number, base)                                                                                                                                     \
		do {                                                                                                                                                                         \
			JSTR_ASSERT_IS_STR(nptr);                                                                                                                                            \
			JSTR_ASSERT_IS_SIZE(number);                                                                                                                                         \
			JSTR_ASSERT_IS_SIZE(base);                                                                                                                                           \
			if (number <= 9) {                                                                                                                                                   \
				nptr[0] = number % base + '0';                                                                                                                               \
				nptr += 1;                                                                                                                                                   \
			} else if (number <= 99) {                                                                                                                                           \
				nptr[0] = number / base % base + '0';                                                                                                                        \
				nptr[1] = number % base + '0';                                                                                                                               \
				nptr += 2;                                                                                                                                                   \
			} else if (number <= 999) {                                                                                                                                          \
				nptr[0] = number / base / base % base + '0';                                                                                                                 \
				nptr[1] = number / base % base + '0';                                                                                                                        \
				nptr[2] = number % base + '0';                                                                                                                               \
				nptr += 3;                                                                                                                                                   \
			} else if (number <= 9999) {                                                                                                                                         \
				nptr[0] = number / base / base / base % base + '0';                                                                                                          \
				nptr[1] = number / base / base % base + '0';                                                                                                                 \
				nptr[2] = number / base % base + '0';                                                                                                                        \
				nptr[3] = number % base + '0';                                                                                                                               \
				nptr += 4;                                                                                                                                                   \
			} else if (number <= 99999) {                                                                                                                                        \
				nptr[0] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[1] = number / base / base / base % base + '0';                                                                                                          \
				nptr[2] = number / base / base % base + '0';                                                                                                                 \
				nptr[3] = number / base % base + '0';                                                                                                                        \
				nptr[4] = number % base + '0';                                                                                                                               \
				nptr += 5;                                                                                                                                                   \
			} else if (number <= 999999) {                                                                                                                                       \
				nptr[0] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[1] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[2] = number / base / base / base % base + '0';                                                                                                          \
				nptr[3] = number / base / base % base + '0';                                                                                                                 \
				nptr[4] = number / base % base + '0';                                                                                                                        \
				nptr[5] = number % base + '0';                                                                                                                               \
				nptr += 6;                                                                                                                                                   \
			} else if (number <= 9999999) {                                                                                                                                      \
				nptr[0] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[1] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[2] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[3] = number / base / base / base % base + '0';                                                                                                          \
				nptr[4] = number / base / base % base + '0';                                                                                                                 \
				nptr[5] = number / base % base + '0';                                                                                                                        \
				nptr[6] = number % base + '0';                                                                                                                               \
				nptr += 7;                                                                                                                                                   \
			} else if (number <= 99999999) {                                                                                                                                     \
				nptr[0] = number / base / base / base / base / base / base / base % base + '0';                                                                              \
				nptr[1] = number / base / base / base / base / base / base % base + '0';                                                                                     \
				nptr[2] = number / base / base / base / base / base % base + '0';                                                                                            \
				nptr[3] = number / base / base / base / base % base + '0';                                                                                                   \
				nptr[4] = number / base / base / base % base + '0';                                                                                                          \
				nptr[5] = number / base / base % base + '0';                                                                                                                 \
				nptr[6] = number / base % base + '0';                                                                                                                        \
				nptr[7] = number % base + '0';                                                                                                                               \
				nptr += 8;                                                                                                                                                   \
			} else if (number <= 999999999) {                                                                                                                                    \
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
			} else if (number <= 9999999999) {                                                                                                                                   \
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
			} else if (number <= 99999999999) {                                                                                                                                  \
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
			} else if (number <= 999999999999) {                                                                                                                                 \
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
			} else if (number <= 9999999999999) {                                                                                                                                \
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
			} else if (number <= 99999999999999) {                                                                                                                               \
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
			} else if (number <= 99999999999999) {                                                                                                                               \
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
			} else if (number <= 999999999999999) {                                                                                                                              \
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
			} else if (number <= 9999999999999999) {                                                                                                                             \
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
			} else if (number <= 99999999999999999) {                                                                                                                            \
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
			} else if (number <= 999999999999999999) {                                                                                                                           \
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
#else
#	define P_JSTR_ULTOA_UNROLLED(nptr, number, base)                 \
		do {                                                     \
			if (jstr_likely(number <= 9999999999)) {         \
				P_JSTR_UTOA_UNROLLED(nptr, number, base); \
				break;                                   \
			}                                                \
			enum { len = JSTR_MAX_ULONG_LONG_DIGITS };       \
			char s[len + 1];                                 \
			s[len] = '\0';                                   \
			char *p = s + len - 1;                           \
			do                                               \
				*p-- = number % base + '0';              \
			while (number /= base);                          \
			while (*++p)                                     \
				*nptr++ = *p;                            \
			*nptr = '\0';                                    \
			return nptr;                                     \
		} while (0)
#endif

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_itoa_p(char *R nptr,
	  int number,
	  const unsigned int base) JSTR_NOEXCEPT
{
	if (number < 0) {
		number = -number;
		*nptr++ = '-';
	}
	P_JSTR_UTOA_UNROLLED(nptr, number, base);
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
	P_JSTR_ULTOA_UNROLLED(nptr, number, base);
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
jstr_utoa_p(char *R nptr,
	  const unsigned int number,
	  const unsigned int base) JSTR_NOEXCEPT
{
	P_JSTR_UTOA_UNROLLED(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
P_JSTR_ITOA_ATTR
static char *
jstr_ulltoa_p(char *R nptr,
	    unsigned long long number,
	    const unsigned int base) JSTR_NOEXCEPT
{
	P_JSTR_ULTOA_UNROLLED(nptr, number, base);
}

/* Return ptr to '\0' after the last digit in the DEST string. */
JSTR_INLINE
P_JSTR_ITOA_ATTR
static char *
jstr_ultoa_p(char *R nptr,
	   const unsigned long number,
	   const unsigned int base) JSTR_NOEXCEPT
{
	return jstr_ulltoa_p(nptr, number, base);
}

P_JSTR_END_DECLS

#undef R

#undef P_JSTR_ITOA_ATTR
#undef P_JSTR_ULTOA_UNROLLED
#undef P_JSTR_ULTOA_UNROLLED
#undef P_JSTR_LTOA_UNROLLED

#endif /* JSTR_ITOA_H */
