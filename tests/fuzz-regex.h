/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Shared fuzz-test logic for regex functions.
 * Each including .c file defines FUZZ_PATTERNS / FUZZ_NPATTERNS before
 * including this header, so patterns are tested in parallel by the runner. */

#ifndef FUZZ_REGEX_H
#define FUZZ_REGEX_H

#include "fuzz-shared.h"

enum { BFUZZ = 256 };

struct regex_pattern {
	const char *pat;
	int cflags;
	int has_capture;
};

static const char *replacements[] = {
	"", "X", "long", "AB", ".", "   "
};
static const size_t nrepl = sizeof(replacements) / sizeof(replacements[0]);

static const char *breplacements[] = {
	"\\1\\1", "\\1", "X\\1Y", "\\1\\2"
};
static const size_t nbrepl = sizeof(breplacements) / sizeof(breplacements[0]);

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_regex_patterns(size_t iter,
                    const struct regex_pattern *patterns,
                    size_t npatterns)
{
	char h[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 3);
		size_t hl = gen_str(h, FSTR - 1, hmode);

		for (size_t pi = 0; pi < npatterns; ++pi) {
			jstr_re_ty preg;
			int comp_ret = jstr_re_comp(&preg, patterns[pi].pat, patterns[pi].cflags);
			if (comp_ret != JSTR_RE_RET_NOERROR) {
				jstr_re_free(&preg);
				continue;
			}

			regmatch_t rm;
			int exec_ret = jstr_re_exec_len(&preg, h, hl, 1, &rm, 0);
			(void)exec_ret;
			if (exec_ret == JSTR_RE_RET_NOERROR) {
				assert((size_t)rm.rm_so <= hl);
				assert((size_t)rm.rm_eo <= hl);
				assert(rm.rm_so <= rm.rm_eo);
			}

			int match_ret = jstr_re_match_len(&preg, h, hl, 0);
			assert(match_ret == JSTR_RE_RET_NOERROR || match_ret == JSTR_RE_RET_NOMATCH);

			regmatch_t rm_search;
			int search_ret = jstr_re_search_len(&preg, h, hl, &rm_search, 0);
			if (search_ret == JSTR_RE_RET_NOERROR) {
				assert((size_t)rm_search.rm_so <= hl);
				assert((size_t)rm_search.rm_eo <= hl);
				assert(rm_search.rm_so <= rm_search.rm_eo);
			}

			char *buf = (char *)malloc(BFUZZ + 1);
			size_t cap = BFUZZ;
			size_t sz;
			(void)sz;
			for (int ei = 0; ei < 8; ++ei) {
				int eflags = ((ei & 1) ? JSTR_RE_EF_NOTBOL : 0) |
				             ((ei & 2) ? JSTR_RE_EF_NOTEOL : 0) |
				             ((ei & 4) ? JSTR_RE_EF_NOSUB : 0);

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rm_exec(&preg, &buf, &sz, &cap, eflags);
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rmall_exec(&preg, &buf, &sz, &cap, eflags);
				assert(buf[sz] == '\0');

				size_t n = (size_t)(rand() % 5);
				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rmn_exec(&preg, &buf, &sz, &cap, eflags, n);
				assert(buf[sz] == '\0');

				size_t start_idx = (size_t)(rand() % (hl + 5));
				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rm_from_exec(&preg, &buf, &sz, &cap, start_idx, eflags);
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rmn_from_exec(&preg, &buf, &sz, &cap, start_idx, eflags, n);
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				jstr_re_rmall_from_exec(&preg, &buf, &sz, &cap, start_idx, eflags);
				assert(buf[sz] == '\0');
			}

			for (size_t ri = 0; ri < nrepl; ++ri) {
				const char *rplc = replacements[ri];
				size_t rplc_len = strlen(rplc);

				for (int ei = 0; ei < 8; ++ei) {
					int eflags = ((ei & 1) ? JSTR_RE_EF_NOTBOL : 0) |
					             ((ei & 2) ? JSTR_RE_EF_NOTEOL : 0) |
					             ((ei & 4) ? JSTR_RE_EF_NOSUB : 0);

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplc_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags);(void)_r;}
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplcall_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags);(void)_r;}
				assert(buf[sz] == '\0');

				size_t n = (size_t)(rand() % 5);
				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplcn_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags, n);(void)_r;}
				assert(buf[sz] == '\0');

				size_t start_idx = (size_t)(rand() % (hl + 5));
				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplc_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags);(void)_r;}
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplcn_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags, n);(void)_r;}
				assert(buf[sz] == '\0');

				memcpy(buf, h, hl + 1);
				sz = hl;
				cap = BFUZZ;
				{int _r = jstr_re_rplcall_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags);(void)_r;}
				assert(buf[sz] == '\0');
				}
			}

			if (patterns[pi].has_capture) {
				for (size_t ri = 0; ri < nbrepl; ++ri) {
					const char *rplc = breplacements[ri];
					size_t rplc_len = strlen(rplc);
					size_t nmatch = 10;

				for (int ei = 0; ei < 8; ++ei) {
					int eflags = ((ei & 1) ? JSTR_RE_EF_NOTBOL : 0) |
					             ((ei & 2) ? JSTR_RE_EF_NOTEOL : 0) |
					             ((ei & 4) ? JSTR_RE_EF_NOSUB : 0);

					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplc_backref_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags, nmatch);(void)_r;}
					assert(buf[sz] == '\0');

					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplcall_backref_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags, nmatch);(void)_r;}
					assert(buf[sz] == '\0');

					size_t n = (size_t)(rand() % 5);
					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplcn_backref_len_exec(&preg, &buf, &sz, &cap, rplc, rplc_len, eflags, nmatch, n);(void)_r;}
					assert(buf[sz] == '\0');

					size_t start_idx = (size_t)(rand() % (hl + 5));
					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplc_backref_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags, nmatch);(void)_r;}
					assert(buf[sz] == '\0');

					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplcn_backref_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags, nmatch, n);(void)_r;}
					assert(buf[sz] == '\0');

					memcpy(buf, h, hl + 1);
					sz = hl;
					cap = BFUZZ;
					{int _r = jstr_re_rplcall_backref_len_from_exec(&preg, &buf, &sz, &cap, start_idx, rplc, rplc_len, eflags, nmatch);(void)_r;}
					assert(buf[sz] == '\0');
					}
				}
			}

			free(buf);
			jstr_re_free(&preg);
		}
	}
}

#endif /* FUZZ_REGEX_H */
