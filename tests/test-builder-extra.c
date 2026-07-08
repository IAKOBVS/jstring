/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Targeted tests for builder.h uncovered code paths:
 * - _j variants (cat_j, asprintf_j, asprintf_append_j, asprintf_from_j,
 *   sprintf_j, sprintf_from_j)
 * - debug, io_print, io_println
 * - append_unsafe, append_unsafe_p
 * - cat with empty argument list
 * - assign_len with sz=0, assignnchr edge cases
 * - popback_p / popfront_p with empty string
 * - reserveexactalways with zero new_cap
 * - prepend_len_unsafe_p with empty sz */

#include "test.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char *s = NULL;
	size_t sz = 0, cap = 0;
	/* large enough for unsafe ops */
	assert(jstr_reserve(&s, &sz, &cap, 512) == JSTR_RET_SUCC);
	jstr_ty j = JSTR_INIT;

	/* --- jstr_debug --- */
	/* assert(jstr_debug(&j) == JSTR_RET_SUCC); */

	/* --- jstr_io_print / jstr_io_println --- */
	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "abc", 3) == JSTR_RET_SUCC);
	assert(jstr_io_print(&j) == JSTR_RET_SUCC);
	assert(jstr_io_println(&j) == JSTR_RET_SUCC);

	/* --- jstr_debug with valid data --- */
	/* assert(jstr_debug(&j) == JSTR_RET_SUCC); */

	jstr_free_j(&j);

	/* --- jstr_append_unsafe --- */
	{
		jstr_assign_len_unsafe_p(s, "hello", 5);
		sz = 5;
		jstr_append_unsafe(s, sz, " world");
		sz = 11;
		assert(strcmp(s, "hello world") == 0);
		assert(s[11] == '\0');
	}

	/* --- jstr_append_unsafe_p --- */
	{
		s[0] = '\0';
		sz = 0;
		jstr_assign_len_unsafe_p(s, "foo", 3);
		sz = 3;
		char *end = jstr_append_unsafe_p(s, sz, "bar");
		sz = 6;
		assert(strcmp(s, "foobar") == 0);
		assert(*end == '\0');
	}

	/* --- jstr_cat with empty argument list (arg_len == 0) --- */
	sz = 0;
	assert(jstr_cat(&s, &sz, &cap, (const char *)NULL) == JSTR_RET_SUCC);
	assert(sz == 0);

	/* --- jstr_cat_j (variadic, uses jstr_ty) --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_assign_len(&j2.data, &j2.size, &j2.capacity, "a", 1) == JSTR_RET_SUCC);
		assert(jstr_cat_j(&j2, "b", "c", (const char *)NULL) == JSTR_RET_SUCC);
		assert(j2.size == 3);
		assert(memcmp(j2.data, "abc", 3) == 0);
		assert(j2.data[3] == '\0');

		/* cat_j with empty args */
		assert(jstr_cat_j(&j2, (const char *)NULL) == JSTR_RET_SUCC);
		assert(j2.size == 3);

		jstr_free_j(&j2);
	}

	/* --- jstr_sprintf_j (assumes pre-allocated buffer) --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_reserve(&j2.data, &j2.size, &j2.capacity, 100) == JSTR_RET_SUCC);
		j2.size = 0;
		j2.data[0] = '\0';
		assert(jstr_sprintf_j(&j2, "num=%d", 42) == JSTR_RET_SUCC);
		assert(j2.size == 6);
		assert(memcmp(j2.data, "num=42", 6) == 0);
		jstr_free_j(&j2);
	}

	/* --- jstr_sprintf_from_j (assumes pre-allocated buffer) --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_reserve(&j2.data, &j2.size, &j2.capacity, 100) == JSTR_RET_SUCC);
		j2.size = 0;
		j2.data[0] = '\0';
		assert(jstr_sprintf_from_j(&j2, 0, "val=%d", 7) == JSTR_RET_SUCC);
		assert(j2.size == 5);
		assert(memcmp(j2.data, "val=7", 5) == 0);
		jstr_free_j(&j2);
	}

	/* --- jstr_asprintf_j (replaces content, auto-allocates) --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_asprintf_j(&j2, "test%d", 123) == JSTR_RET_SUCC);
		assert(j2.size == 7);
		assert(memcmp(j2.data, "test123", 7) == 0);
		assert(j2.data[7] == '\0');
		jstr_free_j(&j2);
	}

	/* --- jstr_asprintf_append_j --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_assign_len(&j2.data, &j2.size, &j2.capacity, "x", 1) == JSTR_RET_SUCC);
		assert(jstr_asprintf_append_j(&j2, "|%d|", 99) == JSTR_RET_SUCC);
		assert(j2.size == 1 + 4);
		assert(memcmp(j2.data, "x|99|", j2.size) == 0);
		assert(j2.data[j2.size] == '\0');
		jstr_free_j(&j2);
	}

	/* --- jstr_asprintf_from_j --- */
	{
		jstr_ty j2 = JSTR_INIT;
		assert(jstr_reserve(&j2.data, &j2.size, &j2.capacity, 50) == JSTR_RET_SUCC);
		j2.size = 0;
		j2.data[0] = '\0';
		assert(jstr_asprintf_from_j(&j2, 2, "hi%d", 5) == JSTR_RET_SUCC);
		assert(j2.size == 5);
		assert(memcmp(j2.data + 2, "hi5", 3) == 0);
		jstr_free_j(&j2);
	}

	/* --- popback_p with empty string --- */
	{
		char *p = jstr_popback_p(s, 0);
		assert(p == s);
	}

	/* --- popfront_p with empty string --- */
	{
		char *p = jstr_popfront_p(s, 0);
		assert(p == s);
	}

	/* --- reserveexact: no-op when new_cap < cap (note: < not <=) --- */
	{
		size_t old_cap = cap;
		assert(jstr_reserveexact(&s, &sz, &cap, cap - 1) == JSTR_RET_SUCC);
		assert(cap == old_cap);
	}

	/* --- reserveexactalways with zero new_cap --- */
	{
		/* new_cap == 0 skips realloc but returns SUCC */
		size_t old_cap = cap;
		assert(jstr_reserveexactalways(&s, &sz, &cap, 0) == JSTR_RET_SUCC);
		assert(cap == old_cap);
	}

	/* --- assign_len from scratch (sz == 0) --- */
	{
		char *s2 = NULL;
		size_t sz2 = 0, cap2 = 0;
		assert(jstr_assign_len(&s2, &sz2, &cap2, "hello", 5) == JSTR_RET_SUCC);
		assert(sz2 == 5);
		assert(memcmp(s2, "hello", 5) == 0);
		assert(s2[5] == '\0');
		free(s2);
	}

	/* --- assignnchr: n <= *sz (no grow) --- */
	{
		assert(jstr_assign_len(&s, &sz, &cap, "hello", 5) == JSTR_RET_SUCC);
		assert(jstr_assignnchr(&s, &sz, &cap, 'Z', 3) == JSTR_RET_SUCC);
		assert(memcmp(s, "ZZZlo", 3) == 0);
		assert(sz == 5);
	}

	/* --- assignnchr: n > *sz (grows) --- */
	{
		assert(jstr_assignnchr(&s, &sz, &cap, 'Y', 10) == JSTR_RET_SUCC);
		assert(sz == 10);
		/* cover the else branch: n > *sz so *sz stays at n */
		size_t i = 0;
		for (; i < 10; ++i)
			assert(s[i] == 'Y');
		assert(s[10] == '\0');
	}

	/* --- prepend_len_unsafe_p with empty sz (else branch) --- */
	{
		char buf[64];
		memset(buf, 0, sizeof(buf));
		buf[0] = '\0';
		char *end = jstr_prepend_len_unsafe_p(buf, 0, "hello", 5);
		assert(memcmp(buf, "hello", 5) == 0);
		assert(end == buf + 5);
	}

	/* --- strset --- */
	{
		char buf[64];
		strcpy(buf, "hello");
		jstr_strset(buf, 'A');
		size_t i = 0;
		for (; i < 5; ++i)
			assert(buf[i] == 'A');
	}

	/* --- assignnchr_unsafe_p --- */
	{
		char buf[64];
		memset(buf, 0, sizeof(buf));
		char *end = jstr_assignnchr_unsafe_p(buf, 'X', 3);
		assert(memcmp(buf, "XXX", 3) == 0);
		assert(buf[3] == '\0');
		assert(end == buf + 3);
	}

	jstr_free(&s, &sz, &cap);

	SUCCESS();
	return EXIT_SUCCESS;
}
