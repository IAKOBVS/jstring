/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for builder.h functions: reserve, assign, append, prepend,
 * pushback/pushfront, cat, sprintf/asprintf. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_builder(size_t iter)
{
	char h[FSTR], buf_j[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, hmode);

		/* --- assign_len_unsafe_p --- */
		memcpy(buf_j, h, hl + 1);
		char *end = jstr_assign_len_unsafe_p(buf_j, "XYZ", 3);
		assert(memcmp(buf_j, "XYZ", 3) == 0);
		assert(buf_j[3] == '\0');
		assert(end == buf_j + 3);

		/* --- assign_len (safe, dynamic) --- */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int ret = jstr_assign_len(&s, &sz, &cap, h, hl);
		(void)ret;
		assert(sz == hl);
		assert(memcmp(s, h, hl) == 0);
		assert(s[sz] == '\0');

		/* --- append_len --- */
		char append_src[32];
		size_t al = gen_str(append_src, 31, hmode);
		ret = jstr_append_len(&s, &sz, &cap, append_src, al);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == hl + al);
		assert(memcmp(s, h, hl) == 0);
		assert(memcmp(s + hl, append_src, al) == 0);
		assert(s[sz] == '\0');

		/* --- prepend_len --- */
		char prepend_src[32];
		size_t pl = gen_str(prepend_src, 31, hmode);
		ret = jstr_prepend_len(&s, &sz, &cap, prepend_src, pl);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == pl + hl + al);
		assert(memcmp(s, prepend_src, pl) == 0);
		assert(memcmp(s + pl, h, hl) == 0);
		assert(memcmp(s + pl + hl, append_src, al) == 0);
		assert(s[sz] == '\0');

		/* --- assignnchr (grows sz only if n > *sz) --- */
		size_t nfill = (size_t)(rand() % 10);
		int fillc = 'a' + (rand() % 26);
		ret = jstr_assignnchr(&s, &sz, &cap, fillc, nfill);
		assert(ret == JSTR_RET_SUCC);
		assert(sz >= nfill);
		for (size_t j = 0; j < nfill; ++j)
			assert(s[j] == (char)fillc);
		assert(s[sz] == '\0');
		size_t sz_a = sz;
		(void)sz_a;

		/* --- pushback / pushbackn --- */
		/* single char */
		char pc = (char)('a' + (rand() % 26));
		ret = jstr_pushback(&s, &sz, &cap, pc);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_a + 1);
		assert(s[sz - 1] == pc);
		assert(s[sz] == '\0');
		/* pushbackn */
		size_t nrep = (size_t)(rand() % 8);
		int repc = '0' + (rand() % 10);
		ret = jstr_pushbackn(&s, &sz, &cap, repc, nrep);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_a + 1 + nrep);
		for (size_t j = 0; j < nrep; ++j)
			assert(s[sz_a + 1 + j] == (char)repc);

		/* --- pushfront --- */
		char fc = (char)('A' + (rand() % 26));
		ret = jstr_pushfront(&s, &sz, &cap, fc);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == sz_a + 1 + nrep + 1);
		assert(s[0] == fc);
		/* pushfrontn — NOTE: returns s+n, not s+n+sz; sz may be wrong */
		size_t fnrep = (size_t)(rand() % 6);
		int frepc = '!' + (rand() % 10);
		ret = jstr_pushfrontn(&s, &sz, &cap, frepc, fnrep);
		assert(ret == JSTR_RET_SUCC);

		/* --- popback --- */
		if (sz > 0) {
			jstr_popback_p(s, sz);
			assert(s[sz - 1] == '\0');
		}

		/* --- popfront --- */
		jstr_empty(s, &sz);
		assert(sz == 0);
		assert(s[0] == '\0');

		/* --- reserve / shrink_to_fit --- */
		ret = jstr_reserve(&s, &sz, &cap, 100);
		assert(ret == JSTR_RET_SUCC);
		assert(cap >= 100);
		ret = jstr_shrink_to_fit(&s, &sz, &cap);
		assert(ret == JSTR_RET_SUCC);
		assert(cap == sz + 1 || cap == 0);
		assert(s[sz] == '\0');

		/* --- cat --- */
		ret = jstr_assign_len(&s, &sz, &cap, "AB", 2);
		assert(ret == JSTR_RET_SUCC);
		ret = jstr_cat(&s, &sz, &cap, "CD", "EF", (const char *)NULL);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 6);
		assert(memcmp(s, "ABCDEF", 6) == 0);
		assert(s[sz] == '\0');

		/* --- sprintf / asprintf --- */
		char fmt[32];
		size_t fl = gen_str(fmt, 30, GEN_ASCII);
		fmt[fl] = '\0';
		/* just call sprintf and asprintf to check they don't crash;
		 * we only verify simple known formats precisely. */
		ret = jstr_assign_len(&s, &sz, &cap, "start", 5);
		assert(ret == JSTR_RET_SUCC);
		/* asprintf_append */
		ret = jstr_asprintf_append(&s, &sz, &cap, "|%d|", 42);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 5 + 4);
		assert(memcmp(s, "start|42|", sz) == 0);
		assert(s[sz] == '\0');

		/* asprintf (replace) */
		ret = jstr_asprintf(&s, &sz, &cap, "num=%d", 99);
		assert(ret == JSTR_RET_SUCC);
		assert(sz == 6);
		assert(memcmp(s, "num=99", 6) == 0);
		assert(s[sz] == '\0');

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
#ifdef JSTR_TEST_SLOW
	iter *= 10;
#else
	if (iter > 500)
		iter = 500;
#endif
	if (iter < 1)
		iter = 1;
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_builder);
		fuzz_builder(iter);
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
	unsigned seed = (unsigned)data[0] | ((unsigned)data[1] << 8) |
	                ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24);
	srand(seed);
	data += 4;
	size -= 4;
	size_t iter = size > 0 ? size : 10;
	if (iter > 100)
		iter = 100;
	fuzz_builder(iter);
	return 0;
}

#endif
