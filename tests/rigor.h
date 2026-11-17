/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Shared helpers for rigorous (property/differential) tests.
 *
 * Provides deterministic pseudo-random input generation, a boundary-length
 * table covering scalar and SIMD widths, and check macros that print the
 * failing expression before aborting (so ASAN/assert output is actionable).
 *
 * Include as "rigor.h" from a top-level test or "../rigor.h" from a test in
 * a subdirectory.
 */

#ifndef JSTR_TESTS_RIGOR_H
#define JSTR_TESTS_RIGOR_H

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
#	define RIGOR_UNUSED __attribute__((unused))
#else
#	define RIGOR_UNUSED
#endif

/* Lengths that exercise scalar paths, SSE2 (16), AVX2 (32) and AVX-512 (64)
 * vector widths plus their neighbours, odd tails, and multi-vector runs. */
RIGOR_UNUSED static const size_t rigor_lens[] = {
	0,    1,    2,    3,    4,    7,    8,    9,    15,   16,   17,
	23,   24,   31,   32,   33,   47,   48,   63,   64,   65,   79,
	80,   95,   96,   127,  128,  129,  255,  256,  257,  511,  512,
	513,  1023, 1024, 1025,
};
#define RIGOR_NLENS (sizeof(rigor_lens) / sizeof(rigor_lens[0]))

/* xorshift64*: deterministic, seedable, no dependency on rand(). */
RIGOR_UNUSED static uint64_t rigor_rng_state = 0x9e3779b97f4a7c15ull;

RIGOR_UNUSED static void
rigor_srand(uint64_t seed)
{
	rigor_rng_state = seed ? seed : 0x9e3779b97f4a7c15ull;
}

RIGOR_UNUSED static uint64_t
rigor_rand(void)
{
	uint64_t x = rigor_rng_state;
	x ^= x >> 12;
	x ^= x << 25;
	x ^= x >> 27;
	rigor_rng_state = x;
	return x * 0x2545f4914f6cdd1dull;
}

RIGOR_UNUSED static size_t
rigor_randn(size_t n)
{
	return n ? (size_t)(rigor_rand() % (uint64_t)n) : 0;
}

RIGOR_UNUSED static unsigned char
rigor_randbyte(void)
{
	return (unsigned char)(rigor_rand() & 0xffu);
}

/* Fill buf with len pseudo-random bytes; always NUL-terminates at buf[len]. */
RIGOR_UNUSED static void
rigor_fill(void *buf, size_t len, int allow_nul)
{
	unsigned char *p = (unsigned char *)buf;
	for (size_t i = 0; i < len; ++i) {
		unsigned char b;
		do
			b = rigor_randbyte();
		while (!allow_nul && b == 0);
		p[i] = b;
	}
	p[len] = 0;
}

RIGOR_UNUSED static void
rigor_fill_printable(void *buf, size_t len)
{
	unsigned char *p = (unsigned char *)buf;
	for (size_t i = 0; i < len; ++i)
		p[i] = (unsigned char)(' ' + (rigor_rand() % 95u));
	p[len] = 0;
}

#define RCHECK(cond)                                                              \
	do {                                                                      \
		if (!(cond)) {                                                    \
			fprintf(stderr,                                           \
			        "RCHECK failed at %s:%d: %s\n",                   \
			        __FILE__,                                         \
			        __LINE__,                                         \
			        #cond);                                           \
			abort();                                                  \
		}                                                                 \
	} while (0)

#define RCHECK_EQ(a, b)                                                           \
	do {                                                                      \
		long long _ra = (long long)(a);                                   \
		long long _rb = (long long)(b);                                   \
		if (_ra != _rb) {                                                 \
			fprintf(stderr,                                           \
			        "RCHECK_EQ failed at %s:%d: %s (%lld) != %s "     \
			        "(%lld)\n",                                       \
			        __FILE__,                                         \
			        __LINE__,                                         \
			        #a,                                               \
			        _ra,                                              \
			        #b,                                               \
			        _rb);                                             \
			abort();                                                  \
		}                                                                 \
	} while (0)

#define RCHECK_MEM(a, b, n)                                                       \
	do {                                                                      \
		const void *_ma = (a);                                            \
		const void *_mb = (b);                                            \
		size_t _mn = (size_t)(n);                                         \
		if (memcmp(_ma, _mb, _mn) != 0) {                                 \
			fprintf(stderr,                                           \
			        "RCHECK_MEM failed at %s:%d: %s vs %s "           \
			        "(%zu bytes)\n",                                  \
			        __FILE__,                                         \
			        __LINE__,                                         \
			        #a,                                               \
			        #b,                                               \
			        _mn);                                             \
			abort();                                                  \
		}                                                                 \
	} while (0)

#define RCHECK_STR(a, b) RCHECK_MEM((a), (b), strlen(b) + 1)

#endif /* JSTR_TESTS_RIGOR_H */
