/* SPDX-License-Identifier: MIT */

/* Micro-bench: how fast is a 16-wide SSE2 equality compare (+movemask)
 * versus a one-char-at-a-time compare, per byte scanned?
 * Standalone: no libjstr linkage needed. */

#define _POSIX_C_SOURCE 199309L
#include <emmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N ((size_t)1 << 20)
#define ITERS 100

static unsigned char *buf;

static double
now_ms(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec * 1e3 + (double)t.tv_nsec / 1e6;
}

/* one-char compare */
static size_t
scalar_cmp(unsigned char c)
{
	size_t cnt = 0;
	for (size_t i = 0; i < N; ++i)
		cnt += (buf[i] == c);
	return cnt;
}

/* 16-wide compare + mask extraction */
static size_t
vec_cmp(unsigned char c)
{
	const __m128i cv = _mm_set1_epi8((char)c);
	size_t cnt = 0;
	for (size_t i = 0; i < N; i += 16) {
		const __m128i v = _mm_loadu_si128((const __m128i *)(buf + i));
		cnt += (unsigned)_mm_movemask_epi8(_mm_cmpeq_epi8(v, cv));
	}
	return cnt;
}

static void
bench(const char *label, int fill, unsigned char c)
{
	memset(buf, fill, N);
	double best_s = 1e30, best_v = 1e30;
	size_t sink = 0;
	for (int i = 0; i < ITERS; ++i) {
		const double t0 = now_ms();
		sink += scalar_cmp(c);
		const double t1 = now_ms();
		best_s = t1 - t0 < best_s ? t1 - t0 : best_s;
	}
	for (int i = 0; i < ITERS; ++i) {
		const double t0 = now_ms();
		sink += vec_cmp(c);
		const double t1 = now_ms();
		best_v = t1 - t0 < best_v ? t1 - t0 : best_v;
	}
	printf("%-22s scalar %7.3f ms (%5.2f ns/B)  vector %7.3f ms (%5.4f ns/B)  vec %5.1fx faster   [sink %zu]\n",
	       label, best_s, best_s * 1e6 / N, best_v, best_v * 1e6 / N,
	       best_s / best_v, sink & 1);
}

int
main(void)
{
	buf = malloc(N);
	if (!buf)
		return 1;

	bench("no-hit ('a' buf, find 'n')", 'a', 'n');
	bench("all-hit ('n' buf, find 'n')", 'n', 'n');

	/* alternating: even bytes 'n', odd 'a' */
	for (size_t i = 0; i < N; ++i)
		buf[i] = (i & 1) ? 'a' : 'n';
	{
		double best_s = 1e30, best_v = 1e30;
		size_t sink = 0;
		for (int i = 0; i < ITERS; ++i) {
			double t0 = now_ms();
			sink += scalar_cmp('n');
			double t1 = now_ms();
			best_s = t1 - t0 < best_s ? t1 - t0 : best_s;
			t0 = now_ms();
			sink += vec_cmp('n');
			t1 = now_ms();
			best_v = t1 - t0 < best_v ? t1 - t0 : best_v;
		}
		printf("%-22s scalar %7.3f ms (%5.2f ns/B)  vector %7.3f ms (%5.4f ns/B)  vec %5.1fx faster   [sink %zu]\n",
		       "half-hit alternating", best_s, best_s * 1e6 / N, best_v, best_v * 1e6 / N, best_s / best_v, sink & 1);
	}

	free(buf);
	return 0;
}
