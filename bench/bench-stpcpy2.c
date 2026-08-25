/* SPDX-License-Identifier: MIT */

/* stpcpy shootout:
 *   libc stpcpy        - glibc's AVX2 asm
 *   simd stpcpy        - replica of jstr_internal_simd_stpcpy (fixed loop):
 *                        align-up scalar prefix + 16B cmpeq/movemask loop +
 *                        scalar tail
 *   musl-style stpcpy  - musl's stpcpy is a plain byte loop; replica
 *   strlen+memcpy      - the two-call idiom this function replaces
 *   naive              - same as musl-style (kept for labeling clarity)
 *
 * Standalone: no libjstr linkage needed. */

#define _POSIX_C_SOURCE 200809L

#include <emmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFLEN ((size_t)500 << 10)
#define ITERS 300

static char *src;
static char *dst;

static double
now_ms(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec * 1e3 + (double)t.tv_nsec / 1e6;
}

static char *
simd_stpcpy_replica(char *d, const char *s)
{
	unsigned int i = (unsigned)(16u - ((uintptr_t)s & 15u));
	for (; i--;)
		if ((*d++ = *s++) == '\0')
			return d - 1;
	const __m128i zv = _mm_setzero_si128();
	for (;;) {
		const __m128i sv = _mm_loadu_si128((const __m128i *)s);
		if (_mm_movemask_epi8(_mm_cmpeq_epi8(sv, zv)))
			break;
		_mm_storeu_si128((__m128i *)d, sv);
		s += 16;
		d += 16;
	}
	while ((*d++ = *s++)) {}
	return d - 1;
}

/* musl's stpcpy is a plain byte loop. */
static char *
musl_stpcpy_replica(char *d, const char *s)
{
	for (; (*d++ = *s++);) {}
	return d - 1;
}

static size_t
b_libc(void *dummy)
{
	size_t cs = 0;
	char *e = dst;
	for (size_t i = 0; i < ITERS; ++i)
		cs += (size_t)(stpcpy(dst, src) - dst);
	return cs + (size_t)(e != dst);
}

static size_t
b_simd(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; ++i)
		cs += (size_t)(simd_stpcpy_replica(dst, src) - dst);
	return cs;
}

static size_t
b_musl(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; ++i)
		cs += (size_t)(musl_stpcpy_replica(dst, src) - dst);
	return cs;
}

static size_t
b_strlen_memcpy(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; ++i) {
		const size_t n = strlen(src); /* scan */
		memcpy(dst, src, n + 1);      /* copy incl NUL */
		cs += n;
	}
	return cs;
}

int
main(void)
{
	src = malloc(BUFLEN + 1);
	dst = malloc(BUFLEN + 1);
	if (!src || !dst)
		return 1;

	struct {
		const char *label;
		size_t len;
	} cases[] = {
		{"long", BUFLEN - 1},
		{"short31", 31},
	};

	for (size_t c = 0; c < sizeof(cases) / sizeof(cases[0]); ++c) {
		memset(src, 'a', cases[c].len);
		src[cases[c].len] = '\0';

		struct {
			const char *name;
			size_t (*fn)(void *);
		} fns[] = {
			{"libc-stpcpy", b_libc},
			{"simd-stpcpy", b_simd},
			{"musl-style", b_musl},
			{"strlen+memcpy", b_strlen_memcpy},
		};
		double times[4] = {1e30, 1e30, 1e30, 1e30};
		for (int rep = 0; rep < 3; ++rep)
			for (size_t f = 0; f < 4; ++f) {
				/* warm + timed run */
				fns[f].fn(NULL);
				const double t0 = now_ms();
				fns[f].fn(NULL);
				const double dt = now_ms() - t0;
				if (dt < times[f])
					times[f] = dt;
			}
		printf("%-8s ", cases[c].label);
		for (size_t f = 0; f < 4; ++f)
			printf("%s=%.3fms ", fns[f].name, times[f]);
		printf("\n");
	}

	free(src);
	free(dst);
	return 0;
}
