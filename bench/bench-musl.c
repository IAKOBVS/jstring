/* SPDX-License-Identifier: MIT */

/* Benchmark: vendored musl implementations vs jstr's normal dispatched
 * functions vs scalar baselines.
 *
 * Run the SAME binary against three library builds:
 *   normal : build/lib          (glibc shortcuts + SIMD dispatch)
 *   musl   : build-musl/lib     (-DJSTR_TEST=1, no SIMD -> vendored musl paths)
 *   scalar : in-file naive baselines (identical across all runs)
 */

#include "bench.h"

#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN (1 << 20) /* 1MB haystack */
#define ITERS 20

char *hs;
size_t hs_len;

static char needle[1024];
static size_t needle_len;

enum {
	N4 = 0,
	N8,
	L1,
	L2,
	L3,
	L5,
	L6,
	L7,
	L9,
	L10,
	L11,
	L12,
	L13,
	L14,
	L15,
	N16,
	N32,
	N256,
	N_NOMATCH,
	R_MID,
	R_ABSENT
};

/* --- scalar baselines --- */

static void *
scalar_memmem(const void *h, size_t hl, const void *n, size_t nl)
{
	if (!nl)
		return (void *)h;
	if (nl > hl)
		return NULL;
	const unsigned char *hb = (const unsigned char *)h;
	const unsigned char *nb = (const unsigned char *)n;
	const unsigned char *end = hb + hl - nl;
	for (const unsigned char *p = hb; p <= end; ++p)
		if (*p == *nb && memcmp(p, nb, nl) == 0)
			return (void *)p;
	return NULL;
}

static void *
scalar_memrchr(const void *h, size_t hl, int c)
{
	const unsigned char *hb = (const unsigned char *)h;
	for (size_t i = hl; i-- > 0;)
		if (hb[i] == (unsigned char)c)
			return (void *)(hb + i);
	return NULL;
}

/* --- bench fns: params selects the case --- */

static void
setup_case(void *params)
{
	switch ((int)(size_t)params) {
	case N4:
		needle_len = 4;
		memcpy(needle, "wxyz", 4);
		break;
	case N8:
		needle_len = 8;
		memcpy(needle, "qrstuvwx", 8);
		break;
	case L1:
	case L2:
	case L3:
	case L5:
	case L6:
	case L7:
	case L9:
	case L10:
	case L11:
	case L12:
	case L13:
	case L14:
	case L15:
	case N16:
		needle_len = (size_t)(int)(size_t)params;
		memset(needle, 'n', needle_len - 1);
		needle[needle_len - 1] = 'm';
		break;
	case N32:
		needle_len = 32;
		memset(needle, 'n', 31);
		needle[31] = '\0';
		break;
	case N256:
		needle_len = 256;
		memset(needle, 'm', 255);
		needle[255] = '\0';
		break;
	case N_NOMATCH:
		needle_len = 32;
		memset(needle, 'q', 31);
		needle[31] = '\0';
		break;
	case R_MID:
	case R_ABSENT:
	default:
		needle_len = 1;
		needle[0] = (int)(size_t)params == R_MID ? '~' : '\x01';
		break;
	}
}

#define MAKE_MEMMEM(fn_name, impl)                                        \
	static size_t fn_name(void *params)                               \
	{                                                                 \
		size_t cs = 0;                                            \
		setup_case(params);                                       \
		int at_end = ((int)(size_t)params != N_NOMATCH);          \
		if (at_end && needle_len <= hs_len)                       \
			memcpy(hs + hs_len - needle_len, needle, needle_len); \
		for (size_t i = 0; i < ITERS; ++i)                        \
			cs += impl(hs, hs_len, needle, needle_len) != NULL; \
		return cs;                                                \
	}

MAKE_MEMMEM(b_jstr_memmem, jstr_memmem)
MAKE_MEMMEM(b_scalar_memmem, scalar_memmem)

#if defined(__GLIBC__)
static size_t
b_libc_memmem(void *params)
{
	size_t cs = 0;
	setup_case(params);
	int at_end = ((int)(size_t)params != N_NOMATCH);
	if (at_end && needle_len <= hs_len)
		memcpy(hs + hs_len - needle_len, needle, needle_len);
	for (size_t i = 0; i < ITERS; ++i)
		cs += memmem(hs, hs_len, needle, needle_len) != NULL;
	return cs;
}

#define NEED_LIBC_MEMMEM 1
#else
#define NEED_LIBC_MEMMEM 0
#endif

static size_t
b_jstr_memrchr(void *params)
{
	size_t cs = 0;
	setup_case(params);
	const int c = ((int)(size_t)params == R_ABSENT) ? '\x01' : '~';
	if ((int)(size_t)params == R_MID)
		hs[hs_len / 2] = '~';
	for (size_t i = 0; i < ITERS; ++i)
		cs += jstr_memrchr(hs, hs_len, c) != NULL;
	return cs;
}

static size_t
b_scalar_memrchr(void *params)
{
	size_t cs = 0;
	setup_case(params);
	const int c = ((int)(size_t)params == R_ABSENT) ? '\x01' : '~';
	if ((int)(size_t)params == R_MID)
		hs[hs_len / 2] = '~';
	for (size_t i = 0; i < ITERS; ++i)
		cs += scalar_memrchr(hs, hs_len, c) != NULL;
	return cs;
}

int main(void)
{
	hs = (char *)malloc(BUFLEN + 1);
	if (!hs)
		return 1;
	memset(hs, 'a', BUFLEN);
	hs_len = BUFLEN;

	RUN(b_jstr_memmem, N4);
	RUN(b_jstr_memmem, N8);
	RUN(b_jstr_memmem, L1);
	RUN(b_jstr_memmem, L2);
	RUN(b_jstr_memmem, L3);
	RUN(b_jstr_memmem, L5);
	RUN(b_jstr_memmem, L6);
	RUN(b_jstr_memmem, L7);
	RUN(b_jstr_memmem, L9);
	RUN(b_jstr_memmem, L10);
	RUN(b_jstr_memmem, L11);
	RUN(b_jstr_memmem, L12);
	RUN(b_jstr_memmem, L13);
	RUN(b_jstr_memmem, L14);
	RUN(b_jstr_memmem, L15);
	RUN(b_jstr_memmem, N16);
	RUN(b_jstr_memmem, N32);
	RUN(b_jstr_memmem, N256);
	RUN(b_jstr_memmem, N_NOMATCH);
	RUN(b_scalar_memmem, N4);
	RUN(b_scalar_memmem, N8);
	RUN(b_scalar_memmem, L1);
	RUN(b_scalar_memmem, L2);
	RUN(b_scalar_memmem, L3);
	RUN(b_scalar_memmem, L5);
	RUN(b_scalar_memmem, L6);
	RUN(b_scalar_memmem, L7);
	RUN(b_scalar_memmem, L9);
	RUN(b_scalar_memmem, L10);
	RUN(b_scalar_memmem, L11);
	RUN(b_scalar_memmem, L12);
	RUN(b_scalar_memmem, L13);
	RUN(b_scalar_memmem, L14);
	RUN(b_scalar_memmem, L15);
	RUN(b_scalar_memmem, N16);
	RUN(b_scalar_memmem, N32);
	RUN(b_scalar_memmem, N256);
	RUN(b_scalar_memmem, N_NOMATCH);
#if NEED_LIBC_MEMMEM
	RUN(b_libc_memmem, N4);
	RUN(b_libc_memmem, N8);
	RUN(b_libc_memmem, L1);
	RUN(b_libc_memmem, L2);
	RUN(b_libc_memmem, L3);
	RUN(b_libc_memmem, L5);
	RUN(b_libc_memmem, L6);
	RUN(b_libc_memmem, L7);
	RUN(b_libc_memmem, L9);
	RUN(b_libc_memmem, L10);
	RUN(b_libc_memmem, L11);
	RUN(b_libc_memmem, L12);
	RUN(b_libc_memmem, L13);
	RUN(b_libc_memmem, L14);
	RUN(b_libc_memmem, L15);
	RUN(b_libc_memmem, N16);
	RUN(b_libc_memmem, N32);
	RUN(b_libc_memmem, N256);
	RUN(b_libc_memmem, N_NOMATCH);
#endif

	RUN(b_jstr_memrchr, R_MID);
	RUN(b_jstr_memrchr, R_ABSENT);
	RUN(b_scalar_memrchr, R_MID);
	RUN(b_scalar_memrchr, R_ABSENT);

	free(hs);
	return 0;
}
