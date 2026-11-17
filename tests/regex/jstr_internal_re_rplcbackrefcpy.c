/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: backreference expansion/copy semantics (byte exact, no NUL). */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

#define SENTINEL 0xA5

struct rm_case {
	regoff_t so;
	regoff_t eo;
};

static void
init_rm(regmatch_t rm[10], const struct rm_case *groups, size_t n)
{
	for (size_t i = 0; i < 10; ++i) {
		rm[i].rm_so = -1;
		rm[i].rm_eo = -1;
	}
	for (size_t i = 0; i < n && i < 10; ++i) {
		rm[i].rm_so = groups[i].so;
		rm[i].rm_eo = groups[i].eo;
	}
}

static void
one(const struct rm_case *groups, size_t ngroups, size_t nmatch, const char *tpl, size_t tpl_len,
    const char *expected, size_t expected_len)
{
	regmatch_t rm[10];
	unsigned char dst[128];
	const char *src = "ABCDEFGH";
	memset(dst, SENTINEL, sizeof(dst));
	init_rm(rm, groups, ngroups);
	jstr_internal_re_rplcbackrefcpy(rm, (const unsigned char *)src, dst,
	                                (const unsigned char *)tpl, (const unsigned char *)tpl + tpl_len,
	                                nmatch);
	RCHECK_MEM(dst, expected, expected_len);
	/* Must not write a terminator or anything beyond the produced bytes. */
	RCHECK_EQ(dst[expected_len], SENTINEL);
}

int
main(void)
{
	/* rm[0] = whole "ABCDEFGH", rm[1]="AB", rm[2]="CD", rm[3]="EFGH". */
	static const struct rm_case groups[] = {
		{0, 8}, {0, 2}, {2, 4}, {4, 8},
	};

	one(groups, 4, 10, "xy", 2, "xy", 2);
	one(groups, 4, 10, "\\1-\\2", 5, "AB-CD", 5);
	one(groups, 4, 10, "\\0!", 3, "ABCDEFGH!", 9);
	one(groups, 4, 10, "\\1\\1", 4, "ABAB", 4);
	one(groups, 4, 10, "pre\\3post", 9, "preEFGHpost", 11);

	/* Non-participating group (rm_so < 0): literal backslash+digit. */
	one(groups, 4, 10, "\\4", 2, "\\4", 2);
	one(groups, 4, 10, "a\\4b", 4, "a\\4b", 4);

	/* Index outside nmatch: also literal, no rm[] read. */
	one(groups, 4, 4, "\\4", 2, "\\4", 2);
	one(groups, 4, 2, "\\2", 2, "\\2", 2);

	/* Non-digit escapes keep both bytes. */
	one(groups, 4, 10, "\\x", 2, "\\x", 2);
	one(groups, 4, 10, "a\\zb", 4, "a\\zb", 4);

	/* Trailing lone backslash is emitted literally. */
	one(groups, 4, 10, "ab\\", 3, "ab\\", 3);
	one(groups, 4, 10, "\\", 1, "\\", 1);

	/* No backrefs at all / empty template. */
	one(groups, 4, 10, "", 0, "", 0);
	one(groups, 4, 10, "plain text", 10, "plain text", 10);

	/* Empty participating group expands to nothing. */
	static const struct rm_case empty_group[] = {{0, 1}, {1, 1}};
	one(empty_group, 2, 2, "a\\1b", 4, "ab", 2);

	return EXIT_SUCCESS;
}
