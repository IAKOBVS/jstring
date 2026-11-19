/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Shared function-based helpers for the legacy tests in tests/.
 *
 * This header holds the fixture setup and assertion/reporting logic that used
 * to be duplicated as local macros (FILL, T_ASSERT, CHECK_CALL, ...).  The
 * only preprocessor logic left in the tests is the thin location/expression
 * stringification in tests/rigor.h. */

#ifndef JSTR_TESTS_TESTUTIL_H
#define JSTR_TESTS_TESTUTIL_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rigor.h"
#include "../include/jstr.h"

JSTR_ATTR_MAYBE_UNUSED static const char *
test_str(const char *s)
{
	return s ? s : "(null)";
}

JSTR_ATTR_MAYBE_UNUSED static void
test_testing(const char *name)
{
#if defined(VERBOSE) && VERBOSE
	fprintf(stderr, "Testing %s().\n", name);
#else
	(void)name;
#endif
}

/* Fixture setup.  The three variants preserve the allocation patterns of the
 * old FILL macros: plain assignment, assign-then-shrink, and shrink-then-assign. */
JSTR_ATTR_MAYBE_UNUSED static void
test_fill(jstr_ty *buf, const char *str)
{
	assert(!jstr_chk(jstr_assign_len(jstr_struct(buf), str, strlen(str))));
}

JSTR_ATTR_MAYBE_UNUSED static void
test_fill_shrunk(jstr_ty *buf, const char *str)
{
	test_fill(buf, str);
	assert(!jstr_chk(jstr_shrink_to_fit(&buf->data, &buf->size, &buf->capacity)));
}

JSTR_ATTR_MAYBE_UNUSED static void
test_shrink_fill(jstr_ty *buf, const char *str)
{
	assert(!jstr_chk(jstr_shrink_to_fit(&buf->data, &buf->size, &buf->capacity)));
	test_fill(buf, str);
}

JSTR_ATTR_MAYBE_UNUSED static void
test_reset(jstr_ty *buf)
{
	buf->size = 0;
	if (buf->data)
		*buf->data = '\0';
}

JSTR_ATTR_MAYBE_UNUSED static void
test_expect_true(const char *func, const char *what, int cond)
{
	if (!cond) {
		fprintf(stderr, "Assertion failure: %s(): %s\n", func, what);
		abort();
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
test_expect_buf(const char *func, const jstr_ty *buf, const char *expected)
{
	size_t n = strlen(expected);
	if (buf->size != n || (n && memcmp(buf->data, expected, n) != 0)) {
		fprintf(stderr, "Assertion failure: %s().\nResult:%s.\nExpected:%s.\n", func, test_str(buf->data), expected);
		abort();
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
test_expect_call(const char *func, long long got, long long want, const jstr_ty *buf, const char *expected)
{
	if (got != want) {
		fprintf(stderr, "Assertion failure: %s() returned %lld, expected %lld.\n", func, got, want);
		abort();
	}
	test_expect_buf(func, buf, expected);
}

JSTR_ATTR_MAYBE_UNUSED static void
test_builder_expect(const char *func, jstr_ret_ty ret, const jstr_ty *buf, const char *expected)
{
	size_t n = strlen(expected);
	if (jstr_chk(ret) || !buf->data || buf->capacity <= n || buf->size != n || (n && memcmp(buf->data, expected, n) != 0)) {
		fprintf(stderr, "Assertion failure: %s().\nResult:%s.\nExpected:%s.\n", func, test_str(buf->data), expected);
		abort();
	}
}

/* Pair of buffers used by the differential table-driven tests. */
struct test_pair {
	jstr_ty result;
	jstr_ty expected;
};

JSTR_ATTR_MAYBE_UNUSED static void
test_pair_init(struct test_pair *pair)
{
	memset(pair, 0, sizeof(*pair));
}

JSTR_ATTR_MAYBE_UNUSED static void
test_pair_free(struct test_pair *pair)
{
	jstr_free_j(&pair->result);
	jstr_free_j(&pair->expected);
}

#endif /* JSTR_TESTS_TESTUTIL_H */
