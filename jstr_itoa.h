#ifndef JSTR_ITOA_DEF_H
#define JSTR_ITOA_DEF_H 1

#include "jstr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#define PJSTR_UITOA_UNROLLED(_max_digits, _base)                                                                            \
	do {                                                                                                                \
		if (jstr_unlikely(_num <= 1)) {                                                                             \
			_s[0] = _num % _base + '0';                                                                         \
			_s += 1;                                                                                            \
		} else if (_num <= 10) {                                                                                    \
			_s[0] = _num / _base % _base + '0';                                                                 \
			_s[1] = _num % _base + '0';                                                                         \
			_s += 2;                                                                                            \
		} else if (_num <= 100) {                                                                                   \
			_s[0] = _num / _base / _base % _base + '0';                                                         \
			_s[1] = _num / _base % _base + '0';                                                                 \
			_s[2] = _num % _base + '0';                                                                         \
			_s += 3;                                                                                            \
		} else if (_num <= 1000) {                                                                                  \
			_s[0] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[1] = _num / _base / _base % _base + '0';                                                         \
			_s[2] = _num / _base % _base + '0';                                                                 \
			_s[3] = _num % _base + '0';                                                                         \
			_s += 4;                                                                                            \
		} else if (_num <= 10000) {                                                                                 \
			_s[0] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[1] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[2] = _num / _base / _base % _base + '0';                                                         \
			_s[3] = _num / _base % _base + '0';                                                                 \
			_s[4] = _num % _base + '0';                                                                         \
			_s += 5;                                                                                            \
		} else if (_num <= 100000) {                                                                                \
			_s[0] = _num / _base / _base / _base / _base / _base % _base + '0';                                 \
			_s[1] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[2] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[3] = _num / _base / _base % _base + '0';                                                         \
			_s[4] = _num / _base % _base + '0';                                                                 \
			_s[5] = _num % _base + '0';                                                                         \
			_s += 6;                                                                                            \
		} else if (_num <= 1000000) {                                                                               \
			_s[0] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_s[1] = _num / _base / _base / _base / _base / _base % _base + '0';                                 \
			_s[2] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[3] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[4] = _num / _base / _base % _base + '0';                                                         \
			_s[5] = _num / _base % _base + '0';                                                                 \
			_s[6] = _num % _base + '0';                                                                         \
			_s += 7;                                                                                            \
		} else if (_num <= 10000000) {                                                                              \
			_s[0] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_s[1] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_s[2] = _num / _base / _base / _base / _base / _base % _base + '0';                                 \
			_s[3] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[4] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[5] = _num / _base / _base % _base + '0';                                                         \
			_s[6] = _num / _base % _base + '0';                                                                 \
			_s[7] = _num % _base + '0';                                                                         \
			_s += 8;                                                                                            \
		} else if (_num <= 100000000) {                                                                             \
			_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_s[1] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_s[2] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_s[3] = _num / _base / _base / _base / _base / _base % _base + '0';                                 \
			_s[4] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[5] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[6] = _num / _base / _base % _base + '0';                                                         \
			_s[7] = _num / _base % _base + '0';                                                                 \
			_s[8] = _num % _base + '0';                                                                         \
			_s += 9;                                                                                            \
		} else {                                                                                                    \
			_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0'; \
			_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
			_s[2] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
			_s[3] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                         \
			_s[4] = _num / _base / _base / _base / _base / _base % _base + '0';                                 \
			_s[5] = _num / _base / _base / _base / _base % _base + '0';                                         \
			_s[6] = _num / _base / _base / _base % _base + '0';                                                 \
			_s[7] = _num / _base / _base % _base + '0';                                                         \
			_s[8] = _num / _base % _base + '0';                                                                 \
			_s[9] = _num % _base + '0';                                                                         \
			_s += 10;                                                                                           \
		}                                                                                                           \
		*_s = '\0';                                                                                                 \
		return _s;                                                                                                  \
	} while (0)

#define PJSTR_ULLTOA_UNROLLED(_max_digits, _base)                                                                                                                                                           \
	do {                                                                                                                                                                                                \
		if (jstr_unlikely(_num <= 1)) {                                                                                                                                                             \
			_s[0] = _num % _base + '0';                                                                                                                                                         \
			_s += 1;                                                                                                                                                                            \
		} else if (_num <= 10) {                                                                                                                                                                    \
			_s[0] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[1] = _num % _base + '0';                                                                                                                                                         \
			_s += 2;                                                                                                                                                                            \
		} else if (_num <= 100) {                                                                                                                                                                   \
			_s[0] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[1] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[2] = _num % _base + '0';                                                                                                                                                         \
			_s += 3;                                                                                                                                                                            \
		} else if (_num <= 1000) {                                                                                                                                                                  \
			_s[0] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[1] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[2] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[3] = _num % _base + '0';                                                                                                                                                         \
			_s += 4;                                                                                                                                                                            \
		} else if (_num <= 10000) {                                                                                                                                                                 \
			_s[0] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                         \
			_s[1] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[2] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[3] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[4] = _num % _base + '0';                                                                                                                                                         \
			_s += 5;                                                                                                                                                                            \
		} else if (_num <= 100000) {                                                                                                                                                                \
			_s[0] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_s[1] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                         \
			_s[2] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[3] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[4] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[5] = _num % _base + '0';                                                                                                                                                         \
			_s += 6;                                                                                                                                                                            \
		} else if (_num <= 1000000) {                                                                                                                                                               \
			_s[0] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_s[1] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_s[2] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                         \
			_s[3] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[4] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[5] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[6] = _num % _base + '0';                                                                                                                                                         \
			_s += 7;                                                                                                                                                                            \
		} else if (_num <= 10000000) {                                                                                                                                                              \
			_s[0] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_s[1] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_s[2] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_s[3] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                         \
			_s[4] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[5] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[6] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[7] = _num % _base + '0';                                                                                                                                                         \
			_s += 8;                                                                                                                                                                            \
		} else if (_num <= 100000000) {                                                                                                                                                             \
			_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
			_s[1] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
			_s[2] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
			_s[3] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                                 \
			_s[4] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                         \
			_s[5] = _num / _base / _base / _base % _base + '0';                                                                                                                                 \
			_s[6] = _num / _base / _base % _base + '0';                                                                                                                                         \
			_s[7] = _num / _base % _base + '0';                                                                                                                                                 \
			_s[8] = _num % _base + '0';                                                                                                                                                         \
			_s += 9;                                                                                                                                                                            \
		} else {                                                                                                                                                                                    \
			if (_num <= 1000000000) {                                                                                                                                                           \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[3] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[4] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[5] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                 \
				_s[6] = _num / _base / _base / _base % _base + '0';                                                                                                                         \
				_s[7] = _num / _base / _base % _base + '0';                                                                                                                                 \
				_s[8] = _num / _base % _base + '0';                                                                                                                                         \
				_s[9] = _num % _base + '0';                                                                                                                                                 \
				_s += 10;                                                                                                                                                                   \
			} else if (_num <= 10000000000) {                                                                                                                                                   \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[6] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                 \
				_s[7] = _num / _base / _base / _base % _base + '0';                                                                                                                         \
				_s[8] = _num / _base / _base % _base + '0';                                                                                                                                 \
				_s[9] = _num / _base % _base + '0';                                                                                                                                         \
				_s[10] = _num % _base + '0';                                                                                                                                                \
				_s += 11;                                                                                                                                                                   \
			} else if (_num <= 100000000000) {                                                                                                                                                  \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[5] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[6] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[7] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                 \
				_s[8] = _num / _base / _base / _base % _base + '0';                                                                                                                         \
				_s[9] = _num / _base / _base % _base + '0';                                                                                                                                 \
				_s[10] = _num / _base % _base + '0';                                                                                                                                        \
				_s[11] = _num % _base + '0';                                                                                                                                                \
				_s += 12;                                                                                                                                                                   \
			} else if (_num <= 1000000000000) {                                                                                                                                                 \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[8] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                 \
				_s[9] = _num / _base / _base / _base % _base + '0';                                                                                                                         \
				_s[10] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[11] = _num / _base % _base + '0';                                                                                                                                        \
				_s[12] = _num % _base + '0';                                                                                                                                                \
				_s += 13;                                                                                                                                                                   \
			} else if (_num <= 10000000000000) {                                                                                                                                                \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[8] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                 \
				_s[9] = _num / _base / _base / _base % _base + '0';                                                                                                                         \
				_s[10] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[11] = _num / _base % _base + '0';                                                                                                                                        \
				_s[13] = _num % _base + '0';                                                                                                                                                \
				_s += 13;                                                                                                                                                                   \
			} else if (_num <= 10000000000000) {                                                                                                                                                \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[8] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[9] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[10] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[11] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[12] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[13] = _num / _base % _base + '0';                                                                                                                                        \
				_s[14] = _num % _base + '0';                                                                                                                                                \
				_s += 14;                                                                                                                                                                   \
			} else if (_num <= 100000000000000) {                                                                                                                                               \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[8] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[9] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                         \
				_s[10] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[11] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[12] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[13] = _num / _base % _base + '0';                                                                                                                                        \
				_s[14] = _num % _base + '0';                                                                                                                                                \
				_s += 15;                                                                                                                                                                   \
			} else if (_num <= 1000000000000000) {                                                                                                                                              \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[8] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[9] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                 \
				_s[10] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
				_s[11] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[12] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[13] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[14] = _num / _base % _base + '0';                                                                                                                                        \
				_s[15] = _num % _base + '0';                                                                                                                                                \
				_s += 16;                                                                                                                                                                   \
			} else if (_num <= 10000000000000000) {                                                                                                                                             \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[8] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[9] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                         \
				_s[10] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
				_s[11] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
				_s[12] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[13] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[14] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[15] = _num / _base % _base + '0';                                                                                                                                        \
				_s[16] = _num % _base + '0';                                                                                                                                                \
				_s += 17;                                                                                                                                                                   \
			} else if (_num <= 100000000000000000) {                                                                                                                                            \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[8] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[9] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                 \
				_s[10] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                        \
				_s[11] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
				_s[12] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
				_s[13] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[14] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[15] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[16] = _num / _base % _base + '0';                                                                                                                                        \
				_s[17] = _num % _base + '0';                                                                                                                                                \
				_s += 18;                                                                                                                                                                   \
			} else {                                                                                                                                                                            \
				_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0'; \
				_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';         \
				_s[2] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                 \
				_s[3] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                         \
				_s[4] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                 \
				_s[5] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                         \
				_s[6] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                 \
				_s[7] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                         \
				_s[8] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                 \
				_s[8] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                         \
				_s[10] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                \
				_s[11] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                        \
				_s[12] = _num / _base / _base / _base / _base / _base / _base % _base + '0';                                                                                                \
				_s[13] = _num / _base / _base / _base / _base / _base % _base + '0';                                                                                                        \
				_s[14] = _num / _base / _base / _base / _base % _base + '0';                                                                                                                \
				_s[15] = _num / _base / _base / _base % _base + '0';                                                                                                                        \
				_s[16] = _num / _base / _base % _base + '0';                                                                                                                                \
				_s[17] = _num / _base % _base + '0';                                                                                                                                        \
				_s[18] = _num % _base + '0';                                                                                                                                                \
				_s += 19;                                                                                                                                                                   \
			}                                                                                                                                                                                   \
		}                                                                                                                                                                                           \
		*_s = '\0';                                                                                                                                                                                 \
		return _s;                                                                                                                                                                                  \
	} while (0)

#define PJSTR_ITOA_UNROLLED(_max_digits, _base)           \
	do {                                              \
		if (_num < 0) {                           \
			_num = -_num;                     \
			*_s++ = '-';                      \
		}                                         \
		PJSTR_UITOA_UNROLLED(_max_digits, _base); \
	} while (0)

#define PJSTR_LLTOA_UNROLLED(_max_digits, _base)          \
	do {                                              \
		if (_num < 0) {                           \
			_num = -_num;                     \
			*_s++ = '-';                      \
		}                                         \
		PJSTR_UITOA_UNROLLED(_max_digits, _base); \
	} while (0)

#define PJSTR_ITOA_ATTR      \
	JSTR_MAYBE_UNUSED    \
	JSTR_NONNULL_ALL     \
	JSTR_WARN_UNUSED     \
	JSTR_RETURNS_NONNULL \
	JSTR_NOTHROW

#define PJSTR_DEFINE_ITOA(_num_type, _func_name, _max)     \
	PJSTR_ITOA_ATTR                                    \
	static char *                                      \
	_func_name(char *JSTR_RST _s,                      \
		   _num_type _num,                         \
		   const unsigned int _base) JSTR_NOEXCEPT \
	{                                                  \
		PJSTR_ITOA_UNROLLED(_max, _base);          \
	}

#define PJSTR_DEFINE_ITOA_BASE(_num_type, _func_name, _max, _base) \
	PJSTR_ITOA_ATTR                                            \
	static char *                                              \
	_func_name(char *JSTR_RST _s,                              \
		   _num_type _num) JSTR_NOEXCEPT                   \
	{                                                          \
		PJSTR_ITOA_UNROLLED(_max, _base);                  \
	}

#define PJSTR_DEFINE_UITOA(_num_type, _func_name, _max)    \
	PJSTR_ITOA_ATTR                                    \
	static char *                                      \
	_func_name(char *JSTR_RST _s,                      \
		   _num_type _num,                         \
		   const unsigned int _base) JSTR_NOEXCEPT \
	{                                                  \
		PJSTR_UITOA_UNROLLED(_max, _base);         \
	}

#define PJSTR_DEFINE_UITOA_BASE(_num_type, _func_name, _max, _base) \
	PJSTR_ITOA_ATTR                                             \
	static char *                                               \
	_func_name(char *JSTR_RST _s,                               \
		   _num_type _num) JSTR_NOEXCEPT                    \
	{                                                           \
		PJSTR_UITOA_UNROLLED(_max, _base);                  \
	}

#define PJSTR_DEFINE_LLTOA(_num_type, _func_name, _max)    \
	PJSTR_ITOA_ATTR                                    \
	static char *                                      \
	_func_name(char *JSTR_RST _s,                      \
		   _num_type _num,                         \
		   const unsigned int _base) JSTR_NOEXCEPT \
	{                                                  \
		PJSTR_LLTOA_UNROLLED(_max, _base);         \
	}

#define PJSTR_DEFINE_LLTOA_BASE(_num_type, _func_name, _max, _base) \
	PJSTR_ITOA_ATTR                                             \
	static char *                                               \
	_func_name(char *JSTR_RST _s,                               \
		   _num_type _num) JSTR_NOEXCEPT                    \
	{                                                           \
		PJSTR_LLTOA_UNROLLED(_max, _base);                  \
	}

#define PJSTR_DEFINE_ULLTOA(_num_type, _func_name, _max)   \
	PJSTR_ITOA_ATTR                                    \
	static char *                                      \
	_func_name(char *JSTR_RST _s,                      \
		   _num_type _num,                         \
		   const unsigned int _base) JSTR_NOEXCEPT \
	{                                                  \
		PJSTR_ULLTOA_UNROLLED(_max, _base);        \
	}

#define PJSTR_DEFINE_ULLTOA_BASE(_num_type, _func_name, _max, _base) \
	PJSTR_ITOA_ATTR                                              \
	static char *                                                \
	_func_name(char *JSTR_RST _s,                                \
		   _num_type _num) JSTR_NOEXCEPT                     \
	{                                                            \
		PJSTR_ULLTOA_UNROLLED(_max, _base);                  \
	}

PJSTR_DEFINE_ITOA(int, jstr_itoa, PJSTR_MAX_INT_DIGITS)
PJSTR_DEFINE_ITOA(long, jstr_ltoa, PJSTR_MAX_INT_DIGITS)
PJSTR_DEFINE_LLTOA(long long, jstr_lltoa, PJSTR_MAX_INT_DIGITS)
PJSTR_DEFINE_UITOA(unsigned int, jstr_utoa, PJSTR_MAX_INT_DIGITS)
PJSTR_DEFINE_UITOA(unsigned long, jstr_ultoa, PJSTR_MAX_INT_DIGITS)
PJSTR_DEFINE_ULLTOA(unsigned long long, jstr_ulltoa, PJSTR_MAX_INT_DIGITS)

#define PJSTR_DEFINE_ITOAALL_BASE(_base)                                                       \
	PJSTR_DEFINE_ITOA_BASE(int, jstr_itoa##_base, PJSTR_MAX_INT_DIGITS, _base)             \
	PJSTR_DEFINE_ITOA_BASE(long, jstr_ltoa##_base, PJSTR_MAX_INT_DIGITS, _base)            \
	PJSTR_DEFINE_ITOA_BASE(long long, jstr_lltoa##_base, PJSTR_MAX_INT_DIGITS, _base)      \
	PJSTR_DEFINE_UITOA_BASE(unsigned int, jstr_utoa##_base, PJSTR_MAX_INT_DIGITS, _base)   \
	PJSTR_DEFINE_UITOA_BASE(unsigned long, jstr_ultoa##_base, PJSTR_MAX_INT_DIGITS, _base) \
	PJSTR_DEFINE_UITOA_BASE(unsigned long long, jstr_ulltoa##_base, PJSTR_MAX_INT_DIGITS, _base)

/* PJSTR_DEFINE_ITOAALL_BASE(2); */
/* PJSTR_DEFINE_ITOAALL_BASE(3); */
/* PJSTR_DEFINE_ITOAALL_BASE(4); */
/* PJSTR_DEFINE_ITOAALL_BASE(5); */
/* PJSTR_DEFINE_ITOAALL_BASE(6); */
/* PJSTR_DEFINE_ITOAALL_BASE(7); */
/* PJSTR_DEFINE_ITOAALL_BASE(8); */
/* PJSTR_DEFINE_ITOAALL_BASE(9); */
/* PJSTR_DEFINE_ITOAALL_BASE(10); */
/* PJSTR_DEFINE_ITOAALL_BASE(11); */
/* PJSTR_DEFINE_ITOAALL_BASE(12); */
/* PJSTR_DEFINE_ITOAALL_BASE(13); */
/* PJSTR_DEFINE_ITOAALL_BASE(14); */
/* PJSTR_DEFINE_ITOAALL_BASE(15); */
/* PJSTR_DEFINE_ITOAALL_BASE(16); */
/* PJSTR_DEFINE_ITOAALL_BASE(17); */
/* PJSTR_DEFINE_ITOAALL_BASE(18); */
/* PJSTR_DEFINE_ITOAALL_BASE(19); */
/* PJSTR_DEFINE_ITOAALL_BASE(20); */
/* PJSTR_DEFINE_ITOAALL_BASE(21); */
/* PJSTR_DEFINE_ITOAALL_BASE(22); */
/* PJSTR_DEFINE_ITOAALL_BASE(23); */
/* PJSTR_DEFINE_ITOAALL_BASE(24); */
/* PJSTR_DEFINE_ITOAALL_BASE(25); */
/* PJSTR_DEFINE_ITOAALL_BASE(26); */
/* PJSTR_DEFINE_ITOAALL_BASE(27); */
/* PJSTR_DEFINE_ITOAALL_BASE(28); */
/* PJSTR_DEFINE_ITOAALL_BASE(29); */
/* PJSTR_DEFINE_ITOAALL_BASE(30); */
/* PJSTR_DEFINE_ITOAALL_BASE(31); */
/* PJSTR_DEFINE_ITOAALL_BASE(32); */
/* PJSTR_DEFINE_ITOAALL_BASE(33); */
/* PJSTR_DEFINE_ITOAALL_BASE(34); */
/* PJSTR_DEFINE_ITOAALL_BASE(35); */
/* PJSTR_DEFINE_ITOAALL_BASE(36); */

#undef PJSTR_DEFINE_ITOAALL_BASE
#undef PJSTR_DEFINE_ITOAALL
#undef PJSTR_DEFINE_UITOA_BASE
#undef PJSTR_DEFINE_ITOA_BASE
#undef PJSTR_DEFINE_UITOA
#undef PJSTR_DEFINE_ITOA
#undef PJSTR_ITOA_ATTR
#undef PJSTR_UITOA_UNROLLED
#undef PJSTR_ITOA_UNROLLED

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_ITOA_DEF_H */
