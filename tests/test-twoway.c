/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

/* Cover twoway.h line 188-189: shift-based k < memory clamp.
 *
 * Needle "ababab" is periodic (period=2, memory0=4).
 * At haystack position 0: left-half mismatch sets memory=4.
 * At position 2: hs[needle_len-1]='a' gives shift-based k=1.
 * Since 1 < 4, k is clamped to memory (4).
 * Match found at position 7. */
static void test_k_less_than_memory(void) {
	const char *hs = "xbababaabababab";
	size_t hl = 14;
	const char *ne = "ababab";
	size_t nl = 6;
	char *r = jstr_memmem(hs, hl, ne, nl);
	assert(r == hs + 7);
}

/* Cover twoway.h line 189 again but through jstr_strstr_exec path.
 * Use jstr_strstr_comp + jstr_strstr_exec with a long periodic needle
 * to reach the memory > 0 path. */
static void test_strstr_exec_memory(void) {
	jstr_twoway_ty t;
	jstr_strstr_comp(&t, "ababab");
	char *r = jstr_strstr_exec(&t, "xbababaabababab", "ababab");
	assert(r != NULL);
}

/* Cover jstr_internal_memmem_musl wrapper (twoway.h line 218 + 240)
 * by calling jstr_memmem directly with a >4 byte needle. */
static void test_memmem_musl_wrapper(void) {
	const char *hs = "xhelloxhello";
	const char *ne = "hello";
	char *r = jstr_memmem(hs, strlen(hs), ne, strlen(ne));
	assert(r == hs + 1);
}

/* Cover memmem periodic needle k < memory clamp at line 189.
 * Use a periodic needle and haystack that triggers partial match then clamp. */
static void test_memmem_k_less_memory(void) {
	const char *hs = "xbabaababababab";
	const char *ne = "ababab";
	char *r = jstr_memmem(hs, strlen(hs), ne, strlen(ne));
	assert(r == hs + 5);
}

int main(void) {
	test_k_less_than_memory();
	test_strstr_exec_memory();
	test_memmem_musl_wrapper();
	test_memmem_k_less_memory();
	return EXIT_SUCCESS;
}
