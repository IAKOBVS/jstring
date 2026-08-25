/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

/* jstr_strstr_comp must store the needle-length classification for needles
 * of length 0, 1, 2 (handled directly), and >2 (musl twoway precompute).
 * The stored length decides which path jstr_strstr_exec takes, so a wrong
 * value silently corrupts every exec/count call. */

static void test_comp_len_classification(void) {
	jstr_twoway_ty t;

	jstr_strstr_comp(&t, "");
	assert(jstr_twoway_get_len(&t) == 0);

	jstr_strstr_comp(&t, "a");
	assert(jstr_twoway_get_len(&t) == 1);

	jstr_strstr_comp(&t, "ab");
	assert(jstr_twoway_get_len(&t) == 2);

	jstr_strstr_comp(&t, "abc");
	assert(jstr_twoway_get_len(&t) > 2);
}

static void test_exec_matches_libc(void) {
	static const char *const hss[] = {"",   "a",     "ab",     "abc",
	                                  "xax", "xabx",  "haystack", "aaaa"};
	static const char *const nes[] = {"", "a", "ab", "abc", "ab", "sta"};

	for (size_t i = 0; i < sizeof(hss) / sizeof(hss[0]); ++i) {
		for (size_t j = 0; j < sizeof(nes) / sizeof(nes[0]); ++j) {
			jstr_twoway_ty t;
			jstr_strstr_comp(&t, nes[j]);
			char *r = jstr_strstr_exec(&t, hss[i], nes[j]);
			char *e = strstr(hss[i], nes[j]);
			assert(r == e);
		}
	}
}

/* jstr_count goes through the same comp/exec pair. */
static void test_count_short_needles(void) {
	assert(jstr_count("aaa", "a") == 3);
	assert(jstr_count("ababab", "ab") == 3);
	assert(jstr_count("xyz", "") == 0); /* Empty needle counts as 0 by contract (string.h). */
	assert(jstr_count("", "a") == 0);
}

int main(void) {
	test_comp_len_classification();
	test_exec_matches_libc();
	test_count_short_needles();
	return EXIT_SUCCESS;
}
