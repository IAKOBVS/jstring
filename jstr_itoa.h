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
	  unsigned int _number,
	  const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ultoa(char *_nptr,
	   unsigned long _number,
	   const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULTOA_UNROLLED(_nptr, _number, __base);
}

/* Returns ptr to '\0' after the last digit in the DEST string. */
PJSTR_ITOA_ATTR
static char *
jstr_ulltoa(char *_nptr,
	    unsigned long long _number,
	    const unsigned int _base) JSTR_NOEXCEPT
{
	PJSTR_ULLTOA_UNROLLED(_nptr, _number, __base);
}

#if 0

#	define PJSTR_DEFINE_ITOA_BASE(_num_type, _func_name, _max) \
		PJSTR_ITOA_ATTR                                     \
	char *                                              \
	_func_name(char *_nptr,                             \
		   _num_type _number,

#	define PJSTR_DEFINE_LLTOA_BASE(_num_type, _func_name, _max) \
		PJSTR_ITOA_ATTR                                      \
	char *                                               \
	_func_name(char *_nptr,                              \
		   _num_type _number,

#	define PJSTR_DEFINE_UITOA_BASE(_num_type, _func_name, _max) \
		PJSTR_ITOA_ATTR                                      \
	char *                                               \
	_func_name(char *_nptr,                              \
		   _num_type _number,

#	define PJSTR_DEFINE_ITOAALL_BASE(_base)                                                       \
		PJSTR_DEFINE_ITOA_BASE(int, jstr_itoa##_base, PJSTR_MAX_INT_DIGITS, _base)             \
		PJSTR_DEFINE_ITOA_BASE(long, jstr_ltoa##_base, PJSTR_MAX_INT_DIGITS, _base)            \
		PJSTR_DEFINE_ITOA_BASE(long long, jstr_lltoa##_base, PJSTR_MAX_INT_DIGITS, _base)      \
		PJSTR_DEFINE_UITOA_BASE(unsigned int, jstr_utoa##_base, PJSTR_MAX_INT_DIGITS, _base)   \
		PJSTR_DEFINE_UITOA_BASE(unsigned long, jstr_ultoa##_base, PJSTR_MAX_INT_DIGITS, _base) \
		PJSTR_DEFINE_UITOA_BASE(unsigned long long, jstr_ulltoa##_base, PJSTR_MAX_INT_DIGITS, _base)

PJSTR_DEFINE_ITOAALL_BASE(2);
PJSTR_DEFINE_ITOAALL_BASE(3);
PJSTR_DEFINE_ITOAALL_BASE(4);
PJSTR_DEFINE_ITOAALL_BASE(5);
PJSTR_DEFINE_ITOAALL_BASE(6);
PJSTR_DEFINE_ITOAALL_BASE(7);
PJSTR_DEFINE_ITOAALL_BASE(8);
PJSTR_DEFINE_ITOAALL_BASE(9);
PJSTR_DEFINE_ITOAALL_BASE(10);
PJSTR_DEFINE_ITOAALL_BASE(11);
PJSTR_DEFINE_ITOAALL_BASE(12);
PJSTR_DEFINE_ITOAALL_BASE(13);
PJSTR_DEFINE_ITOAALL_BASE(14);
PJSTR_DEFINE_ITOAALL_BASE(15);
PJSTR_DEFINE_ITOAALL_BASE(16);
PJSTR_DEFINE_ITOAALL_BASE(17);
PJSTR_DEFINE_ITOAALL_BASE(18);
PJSTR_DEFINE_ITOAALL_BASE(19);
PJSTR_DEFINE_ITOAALL_BASE(20);
PJSTR_DEFINE_ITOAALL_BASE(21);
PJSTR_DEFINE_ITOAALL_BASE(22);
PJSTR_DEFINE_ITOAALL_BASE(23);
PJSTR_DEFINE_ITOAALL_BASE(24);
PJSTR_DEFINE_ITOAALL_BASE(25);
PJSTR_DEFINE_ITOAALL_BASE(26);
PJSTR_DEFINE_ITOAALL_BASE(27);
PJSTR_DEFINE_ITOAALL_BASE(28);
PJSTR_DEFINE_ITOAALL_BASE(29);
PJSTR_DEFINE_ITOAALL_BASE(30);
PJSTR_DEFINE_ITOAALL_BASE(31);
PJSTR_DEFINE_ITOAALL_BASE(32);
PJSTR_DEFINE_ITOAALL_BASE(33);
PJSTR_DEFINE_ITOAALL_BASE(34);
PJSTR_DEFINE_ITOAALL_BASE(35);
PJSTR_DEFINE_ITOAALL_BASE(36);

#endif

#undef PJSTR_DEFINE_ITOAALL_BASE
#undef PJSTR_DEFINE_ITOAALL
#undef PJSTR_DEFINE_UITOA_BASE
#undef PJSTR_DEFINE_ITOA_BASE
#undef PJSTR_DEFINE_UITOA
#undef PJSTR_DEFINE_ITOA
#undef PJSTR_ITOA_ATTR

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_ITOA_DEF_H */
