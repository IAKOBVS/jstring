/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for builder functions: reserve variants, asprintf/sprintf,
 * pop, trim, assignnchr, pushbackn/pushfrontn, prepend, cat. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "../include/macros.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_builder2(size_t iter)
{
	char h[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, hmode);

		/* --- reservealways / reserveexact / reserveexactalways / shrink_to_fit --- */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int ret = jstr_assign_len(&s, &sz, &cap, h, hl);
		assert(ret == JSTR_RET_SUCC);

		ret = jstr_reservealways(&s, &sz, &cap, 100);
		assert(ret == JSTR_RET_SUCC);
		assert(cap >= 100);

		ret = jstr_reserveexact(&s, &sz, &cap, 200);
		assert(ret == JSTR_RET_SUCC);
		assert(cap >= 200);

		ret = jstr_reserveexactalways(&s, &sz, &cap, 150);
		assert(ret == JSTR_RET_SUCC);
		assert(cap == 150);

		ret = jstr_shrink_to_fit(&s, &sz, &cap);
		assert(ret == JSTR_RET_SUCC);
		assert(cap == sz + 1);

		/* --- strset --- */
		if (sz > 0) {
			char *copy = malloc(sz + 1);
			memcpy(copy, s, sz);
			copy[sz] = '\0';
			jstr_strset(copy, 'X');
			for (size_t j = 0; j < sz; ++j)
				assert(copy[j] == 'X');
			free(copy);
		}

		/* --- assignnchr_unsafe_p --- */
		char buf_j[FSTR];
		memcpy(buf_j, h, hl + 1);
		size_t nfill = (size_t)(rand() % 10);
		assert(nfill < FSTR);
		int fillc = 'a' + (rand() % 26);
		char *end = jstr_assignnchr_unsafe_p(buf_j, fillc, nfill);

		// The new length of the string will be the larger of the two
		size_t new_len = nfill;
		// The end pointer should point to the null-terminator of the resulting string
		assert(end == buf_j + new_len);
		// The null terminator must be at the end of the new length
		assert(buf_j[new_len] == '\0');
		// Verify the fill characters were actually written
		for (size_t i = 0; i < nfill; i++)
			assert(buf_j[i] == fillc);

		free(s);
		/* --- assignnchr (safe) --- */
		s = NULL;
		sz = 0;
		cap = 0;
		ret = jstr_assign_len(&s, &sz, &cap, h, hl);
		assert(ret == JSTR_RET_SUCC);
		nfill = (size_t)(rand() % 10);
		fillc = 'a' + (rand() % 26);
		ret = jstr_assignnchr(&s, &sz, &cap, fillc, nfill);
		assert(ret == JSTR_RET_SUCC);
		assert(sz >= nfill);
		for (size_t j = 0; j < nfill; ++j)
			assert(s[j] == (char)fillc);
		assert(s[sz] == '\0');

		/* --- pushbackn / pushfrontn --- */
		size_t nrep = (size_t)(rand() % 8);
		int repc = '0' + (rand() % 10);
		size_t sz_before = sz;
		ret = jstr_pushbackn(&s, &sz, &cap, repc, nrep);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_before + nrep);
		for (size_t j = 0; j < nrep; ++j)
			assert(s[sz_before + j] == (char)repc);

		size_t fnrep = (size_t)(rand() % 6);
		int frep_ch = '!' + (rand() % 10);
		sz_before = sz;
		ret = jstr_pushfrontn(&s, &sz, &cap, frep_ch, fnrep);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_before + fnrep);
		for (size_t j = 0; j < fnrep; ++j)
			assert(s[j] == (char)frep_ch);

		/* --- prepend_len_unsafe_p --- */
		char prepend_src[32];
		size_t pl = gen_str(prepend_src, 31, hmode);
		sz = JSTR_MIN(sizeof(buf_j), sz + 1) / 2;
		memcpy(buf_j, s, sz + 1);
		pl = JSTR_MIN(sizeof(buf_j) - pl, pl) / 2;
		char *ep = jstr_prepend_len_unsafe_p(buf_j, sz, prepend_src, pl);
		assert(ep == buf_j + sz + pl);
		assert(memcmp(buf_j, prepend_src, pl) == 0);
		assert(memcmp(buf_j + pl, s, sz) == 0);

		/* --- prepend_len (safe) --- */
		sz_before = sz;
		ret = jstr_prepend_len(&s, &sz, &cap, prepend_src, pl);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_before + pl);
		assert(memcmp(s, prepend_src, pl) == 0);

		/* --- cat (variadic) --- */
		char *t = NULL;
		size_t tsz = 0, tcap = 0;
		ret = jstr_cat(&t, &tsz, &tcap, "AB", "CD", "EF", NULL);
		assert(ret == JSTR_RET_SUCC);
		assert(tsz == 6);
		assert(memcmp(t, "ABCDEF", 6) == 0);
		free(t);

		free(s);
		/* --- asprintf / asprintf_append / asprintf_from --- */
		s = NULL;
		sz = 0;
		cap = 0;
		ret = jstr_assign_len(&s, &sz, &cap, "start", 5);
		assert(ret == JSTR_RET_SUCC);

		ret = jstr_asprintf_append(&s, &sz, &cap, "|%d|", 42);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 9);
		assert(memcmp(s, "start|42|", 9) == 0);

		ret = jstr_asprintf(&s, &sz, &cap, "num=%d", 99);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 6);
		assert(memcmp(s, "num=99", 6) == 0);

		ret = jstr_asprintf_from(&s, &sz, &cap, 2, "test%d", 123);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 2 + 7);
		assert(memcmp(s + 2, "test123", 7) == 0);

		free(s);
		/* --- sprintf / sprintf_j / sprintf_from / sprintf_from_j --- */
		s = NULL;
		sz = 0;
		cap = 0;
		ret = jstr_reserve(&s, &sz, &cap, 100);
		assert(ret == JSTR_RET_SUCC);
		ret = jstr_sprintf(&s, &sz, &cap, "val=%d", 777);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 7);
		assert(memcmp(s, "val=777", 7) == 0);

		ret = jstr_sprintf_from(&s, &sz, &cap, 3, "x%dy", 5);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 3 + 3);
		assert(memcmp(s + 3, "x5y", 3) == 0);

		/* --- popback_p / popfront_p --- */
		if (sz > 0) {
			char *pbp = jstr_popback_p(s, sz);
			assert(pbp == s + sz - 1);
			assert(s[sz - 1] == '\0');
			--sz;
		}
		if (sz > 0) {
			char *pfp = jstr_popfront_p(s, sz);
			assert(pfp == s + sz - 1);
			assert(s[0] == '\0' || s[sz - 1] == '\0');
			--sz;
		}

		/* --- trim functions --- */
		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		char *tep = jstr_trimend_p(s);
		assert(tep == s + 7);
		assert(memcmp(s, "  hello", 7) == 0);

		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		tep = jstr_trimend_len_p(s, sz);
		assert(tep == s + 7);

		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		tep = jstr_trimstart_p(s);
		assert(tep == s + 7);
		assert(memcmp(s, "hello  ", 7) == 0);

		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		tep = jstr_trimstart_len_p(s, sz);
		assert(tep == s + 7);

		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		tep = jstr_trim_p(s);
		assert(tep == s + 5);
		assert(memcmp(s, "hello", 5) == 0);

		ret = jstr_assign_len(&s, &sz, &cap, "  hello  ", 9);
		assert(ret == JSTR_RET_SUCC);
		tep = jstr_trim_len_p(s, sz);
		assert(tep == s + 5);

		free(s);
	}
}

#ifndef FUZZ_LIBFUZZER

int
main(int argc, char **argv)
{
	START();
	unsigned seed = 0;
	if (argc > 1)
		seed = (unsigned)atoi(argv[1]);
	size_t iter = FITER;
#	ifdef JSTR_TEST_SLOW
	iter *= 10;
#	else
	if (iter > 500)
		iter = 500;
#	endif
	if (iter < 1)
		iter = 1;
#	ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#	endif
	{
		srand(seed);
		TESTING(fuzz_builder2);
		fuzz_builder2(iter);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}

#else

int
LLVMFuzzerTestOneInput(const unsigned char *data, size_t size)
{
	if (size < 4)
		return 0;
	unsigned seed = (unsigned)data[0] | ((unsigned)data[1] << 8) | ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24);
	srand(seed);
	data += 4;
	size -= 4;
	size_t iter = size > 0 ? size : 10;
	if (iter > 100)
		iter = 100;
	fuzz_builder2(iter);
	return 0;
}

#endif
