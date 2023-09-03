#ifndef JSTR_ITOA_DEF_H
#define JSTR_ITOA_DEF_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdlib.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#define PJSTR_ITOA_ATTR      \
	JSTR_MAYBE_UNUSED    \
	JSTR_NONNULL_ALL     \
	JSTR_WARN_UNUSED     \
	JSTR_RETURNS_NONNULL \
	JSTR_NOTHROW

#define PJSTR_ULTOA_UNROLLED(_nptr, _number, __base)                                                                              \
	do {                                                                                                                      \
		JSTR_ASSERT_IS_STR(_nptr);                                                                                        \
		JSTR_ASSERT_IS_SIZE(_number);                                                                                     \
		JSTR_ASSERT_IS_SIZE(_base);                                                                                       \
		if (jstr_unlikely(_number <= 1)) {                                                                                \
			_nptr[0] = _number % _base + '0';                                                                         \
			_nptr += 1;                                                                                               \
		} else if (_number <= 10) {                                                                                       \
			_nptr[0] = _number / _base % _base + '0';                                                                 \
			_nptr[1] = _number % _base + '0';                                                                         \
			_nptr += 2;                                                                                               \
		} else if (_number <= 100) {                                                                                      \
			_nptr[0] = _number / _base / _base % _base + '0';                                                         \
			_nptr[1] = _number / _base % _base + '0';                                                                 \
			_nptr[2] = _number % _base + '0';                                                                         \
			_nptr += 3;                                                                                               \
		} else if (_number <= 1000) {                                                                                     \
			_nptr[0] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[1] = _number / _base / _base % _base + '0';                                                         \
			_nptr[2] = _number / _base % _base + '0';                                                                 \
			_nptr[3] = _number % _base + '0';                                                                         \
			_nptr += 4;                                                                                               \
		} else if (_number <= 10000) {                                                                                    \
			_nptr[0] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[1] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[2] = _number / _base / _base % _base + '0';                                                         \
			_nptr[3] = _number / _base % _base + '0';                                                                 \
			_nptr[4] = _number % _base + '0';                                                                         \
			_nptr += 5;                                                                                               \
		} else if (_number <= 100000) {                                                                                   \
			_nptr[0] = _number / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[1] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[2] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[3] = _number / _base / _base % _base + '0';                                                         \
			_nptr[4] = _number / _base % _base + '0';                                                                 \
			_nptr[5] = _number % _base + '0';                                                                         \
			_nptr += 6;                                                                                               \
		} else if (_number <= 1000000) {                                                                                  \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[1] = _number / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[2] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[3] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[4] = _number / _base / _base % _base + '0';                                                         \
			_nptr[5] = _number / _base % _base + '0';                                                                 \
			_nptr[6] = _number % _base + '0';                                                                         \
			_nptr += 7;                                                                                               \
		} else if (_number <= 10000000) {                                                                                 \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[3] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[4] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[5] = _number / _base / _base % _base + '0';                                                         \
			_nptr[6] = _number / _base % _base + '0';                                                                 \
			_nptr[7] = _number % _base + '0';                                                                         \
			_nptr += 8;                                                                                               \
		} else if (_number <= 100000000) {                                                                                \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[3] = _number / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[4] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[5] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[6] = _number / _base / _base % _base + '0';                                                         \
			_nptr[7] = _number / _base % _base + '0';                                                                 \
			_nptr[8] = _number % _base + '0';                                                                         \
			_nptr += 9;                                                                                               \
		} else {                                                                                                          \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0'; \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[5] = _number / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[6] = _number / _base / _base / _base % _base + '0';                                                 \
			_nptr[7] = _number / _base / _base % _base + '0';                                                         \
			_nptr[8] = _number / _base % _base + '0';                                                                 \
			_nptr[9] = _number % _base + '0';                                                                         \
			_nptr += 10;                                                                                              \
		}                                                                                                                 \
		*_nptr = '\0';                                                                                                    \
		return _nptr;                                                                                                     \
	} while (0)

#define PJSTR_ULLTOA_UNROLLED(_nptr, _number, __base)                                                                                                                                                     \
	do {                                                                                                                                                                                              \
		JSTR_ASSERT_IS_STR(_nptr);                                                                                                                                                                \
		JSTR_ASSERT_IS_SIZE(_number);                                                                                                                                                             \
		JSTR_ASSERT_IS_SIZE(_base);                                                                                                                                                               \
		if (jstr_unlikely(_number <= 1)) {                                                                                                                                                        \
			_nptr[0] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 1;                                                                                                                                                                       \
		} else if (_number <= 10) {                                                                                                                                                               \
			_nptr[0] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[1] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 2;                                                                                                                                                                       \
		} else if (_number <= 100) {                                                                                                                                                              \
			_nptr[0] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[1] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[2] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 3;                                                                                                                                                                       \
		} else if (_number <= 1000) {                                                                                                                                                             \
			_nptr[0] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[1] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[2] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[3] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 4;                                                                                                                                                                       \
		} else if (_number <= 10000) {                                                                                                                                                            \
			_nptr[0] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[1] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[2] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[3] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[4] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 5;                                                                                                                                                                       \
		} else if (_number <= 100000) {                                                                                                                                                           \
			_nptr[0] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[1] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[2] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[3] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[4] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[5] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 6;                                                                                                                                                                       \
		} else if (_number <= 1000000) {                                                                                                                                                          \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[2] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[3] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[4] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[5] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[6] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 7;                                                                                                                                                                       \
		} else if (_number <= 10000000) {                                                                                                                                                         \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[3] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[4] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[5] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[6] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[7] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 8;                                                                                                                                                                       \
		} else if (_number <= 100000000) {                                                                                                                                                        \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[4] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[5] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[6] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[7] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[8] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 9;                                                                                                                                                                       \
		} else if (_number <= 1000000000) {                                                                                                                                                       \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[4] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[5] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[6] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[7] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[8] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[9] = _number % _base + '0';                                                                                                                                                 \
			_nptr += 10;                                                                                                                                                                      \
		} else if (_number <= 10000000000) {                                                                                                                                                      \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[6] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[7] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[8] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[9] = _number / _base % _base + '0';                                                                                                                                         \
			_nptr[10] = _number % _base + '0';                                                                                                                                                \
			_nptr += 11;                                                                                                                                                                      \
		} else if (_number <= 100000000000) {                                                                                                                                                     \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[6] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[7] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[8] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[9] = _number / _base / _base % _base + '0';                                                                                                                                 \
			_nptr[10] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[11] = _number % _base + '0';                                                                                                                                                \
			_nptr += 12;                                                                                                                                                                      \
		} else if (_number <= 1000000000000) {                                                                                                                                                    \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[8] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[9] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[10] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[11] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[12] = _number % _base + '0';                                                                                                                                                \
			_nptr += 13;                                                                                                                                                                      \
		} else if (_number <= 10000000000000) {                                                                                                                                                   \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[8] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_nptr[9] = _number / _base / _base / _base % _base + '0';                                                                                                                         \
			_nptr[10] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[11] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[13] = _number % _base + '0';                                                                                                                                                \
			_nptr += 13;                                                                                                                                                                      \
		} else if (_number <= 10000000000000) {                                                                                                                                                   \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[9] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[10] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[11] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[12] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[13] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[14] = _number % _base + '0';                                                                                                                                                \
			_nptr += 14;                                                                                                                                                                      \
		} else if (_number <= 100000000000000) {                                                                                                                                                  \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[9] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_nptr[10] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[11] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[12] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[13] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[14] = _number % _base + '0';                                                                                                                                                \
			_nptr += 15;                                                                                                                                                                      \
		} else if (_number <= 1000000000000000) {                                                                                                                                                 \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[9] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_nptr[10] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
			_nptr[11] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[12] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[13] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[14] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[15] = _number % _base + '0';                                                                                                                                                \
			_nptr += 16;                                                                                                                                                                      \
		} else if (_number <= 10000000000000000) {                                                                                                                                                \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[9] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_nptr[10] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
			_nptr[11] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
			_nptr[12] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[13] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[14] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[15] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[16] = _number % _base + '0';                                                                                                                                                \
			_nptr += 17;                                                                                                                                                                      \
		} else if (_number <= 100000000000000000) {                                                                                                                                               \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[9] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
			_nptr[10] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                        \
			_nptr[11] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
			_nptr[12] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
			_nptr[13] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[14] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[15] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[16] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[17] = _number % _base + '0';                                                                                                                                                \
			_nptr += 18;                                                                                                                                                                      \
		} else {                                                                                                                                                                                  \
			_nptr[0] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0'; \
			_nptr[1] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_nptr[2] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_nptr[3] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_nptr[4] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
			_nptr[5] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
			_nptr[6] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
			_nptr[7] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
			_nptr[8] = _number / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
			_nptr[10] = _number / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                \
			_nptr[11] = _number / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                        \
			_nptr[12] = _number / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
			_nptr[13] = _number / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
			_nptr[14] = _number / _base / _base / _base / _base % _base + '0';                                                                                                                \
			_nptr[15] = _number / _base / _base / _base % _base + '0';                                                                                                                        \
			_nptr[16] = _number / _base / _base % _base + '0';                                                                                                                                \
			_nptr[17] = _number / _base % _base + '0';                                                                                                                                        \
			_nptr[18] = _number % _base + '0';                                                                                                                                                \
			_nptr += 19;                                                                                                                                                                      \
		}                                                                                                                                                                                         \
		*_nptr = '\0';                                                                                                                                                                            \
		return _nptr;                                                                                                                                                                             \
	} while (0)

#define PJSTR_LTOA_UNROLLED(_nptr, _number, __base)           \
	do {                                                  \
		if (_number < 0) {                            \
			_number = -_number;                   \
			*_nptr++ = '-';                       \
		}                                             \
		PJSTR_ULTOA_UNROLLED(_nptr, _number, __base); \
	} while (0)

#define PJSTR_LLTOA_UNROLLED(_nptr, _number, __base)           \
	do {                                                   \
		if (_number < 0) {                             \
			_number = -_number;                    \
			*_nptr++ = '-';                        \
		}                                              \
		PJSTR_ULLTOA_UNROLLED(_nptr, _number, __base); \
	} while (0)

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_itoa(char *_nptr,
	  int _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_LTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ltoa(char *_nptr,
	  long _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_LTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_lltoa(char *_nptr,
	   long long _number,
	   const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_LLTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_utoa(char *_nptr,
	  const unsigned int _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ultoa(char *_nptr,
	   const unsigned long _number,
	   const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ulltoa(char *_nptr,
	    const unsigned long long _number,
	    const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULLTOA_UNROLLED(_nptr, _number, __base);
}

#undef PJSTR_ITOA_ATTR
#undef PJSTR_ULLTOA_UNROLLED
#undef PJSTR_ULTOA_UNROLLED
#undef PJSTR_LLTOA_UNROLLED
#undef PJSTR_LTOA_UNROLLED

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_ITOA_DEF_H */
