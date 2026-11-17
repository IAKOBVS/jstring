/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: expanded-length accounting for backreference replacement
 * templates; must agree with jstr_internal_re_rplcbackrefcpy. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

static void
init_rm(regmatch_t rm[10])
{
	for (size_t i = 0; i < 10; ++i) {
		rm[i].rm_so = -1;
		rm[i].rm_eo = -1;
	}
	rm[0].rm_so = 0;
	rm[0].rm_eo = 8; /* "ABCDEFGH" */
	rm[1].rm_so = 0;
	rm[1].rm_eo = 2; /* "AB" */
	rm[2].rm_so = 2;
	rm[2].rm_eo = 7; /* "CDEFG" */
}

static size_t
len_of(const regmatch_t rm[10], const char *tpl, size_t tpl_len, size_t nmatch)
{
	return jstr_internal_re_rplcbackrefstrlen(rm, (const unsigned char *)tpl,
	                                          (const unsigned char *)tpl + tpl_len, tpl_len, nmatch);
}

int
main(void)
{
	regmatch_t rm[10];
	init_rm(rm);

	RCHECK_EQ(len_of(rm, "", 0, 10), 0);
	RCHECK_EQ(len_of(rm, "abc", 3, 10), 3);
	RCHECK_EQ(len_of(rm, "\\1", 2, 10), 2);
	RCHECK_EQ(len_of(rm, "\\0", 2, 10), 8);
	RCHECK_EQ(len_of(rm, "\\2", 2, 10), 5);
	RCHECK_EQ(len_of(rm, "x\\1y", 4, 10), 4);
	RCHECK_EQ(len_of(rm, "\\1\\2", 4, 10), 7);
	RCHECK_EQ(len_of(rm, "\\0\\0", 4, 10), 16);

	/* Non-digit escapes are copied verbatim (length unchanged). */
	RCHECK_EQ(len_of(rm, "\\x", 2, 10), 2);
	RCHECK_EQ(len_of(rm, "a\\zb", 4, 10), 4);

	/* Out-of-range / non-participating groups stay literal. */
	RCHECK_EQ(len_of(rm, "\\3", 2, 10), 2);
	RCHECK_EQ(len_of(rm, "\\9", 2, 10), 2);
	RCHECK_EQ(len_of(rm, "\\2", 2, 2), 2);

	/* Trailing lone backslash is an error. */
	RCHECK_EQ(len_of(rm, "\\", 1, 10), (size_t)-1);
	RCHECK_EQ(len_of(rm, "abc\\", 4, 10), (size_t)-1);
	RCHECK_EQ(len_of(rm, "\\1\\", 3, 10), (size_t)-1);

	/* Empty participating group shrinks by two bytes per reference. */
	regmatch_t empty[10];
	init_rm(empty);
	empty[1].rm_so = empty[1].rm_eo = 0;
	RCHECK_EQ(len_of(empty, "\\1", 2, 10), 0);
	RCHECK_EQ(len_of(empty, "a\\1b", 4, 10), 2);

	/* Length must match bytes produced by rplcbackrefcpy. */
	const char *tpl = "a\\1-\\2-z";
	const size_t tpl_len = strlen(tpl);
	regmatch_t rm2[10];
	init_rm(rm2);
	const size_t want = len_of(rm2, tpl, tpl_len, 10);
	unsigned char dst[64];
	jstr_internal_re_rplcbackrefcpy(rm2, (const unsigned char *)"ABCDEFGH", dst,
	                                (const unsigned char *)tpl,
	                                (const unsigned char *)tpl + tpl_len, 10);
	RCHECK_EQ(want, strlen("aAB-CDEFG-z"));
	RCHECK_MEM(dst, "aAB-CDEFG-z", want);

	return EXIT_SUCCESS;
}
