/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for string manipulation functions: trim, camelCase/snake_case,
 * strtok, thousep, unescape, rev, basename, starts/ends, count, line. */

#define __STDC_NO_VLA__ 1
#define _GNU_SOURCE

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"
#include <ctype.h>

/* Simple reference implementations */
static void
simple_trimend_len_p(char *s, size_t sz)
{
	unsigned char *p = (unsigned char *)s + sz;
	for (; sz && jstr_isspace(*(p - 1)); --p, --sz) {}
	*p = '\0';
}

static void
simple_trimstart_len_p(char *s, size_t sz)
{
	const unsigned char *p = (const unsigned char *)s;
	for (; sz && jstr_isspace(*p); ++p, --sz) {}
	if (p != (unsigned char *)s) {
		memmove(s, p, sz + 1);
	}
}

static size_t
simple_count_len(const char *s, size_t sz, const char *find, size_t find_len)
{
	if (find_len == 0 || sz == 0 || find_len > sz)
		return 0;
	size_t cnt = 0;
	for (size_t i = 0; i <= sz - find_len;) {
		if (memcmp(s + i, find, find_len) == 0) {
			++cnt;
			i += find_len;
		} else {
			++i;
		}
	}
	return cnt;
}

static size_t
simple_thousep_len_p(char *nptr, size_t sz, char separator)
{
	if (sz == 0)
		return 0;

	char *orig = nptr;
	char *end = nptr + sz;
	if (*nptr == '-') {
		++nptr;
		--sz;
	}
	if (sz < 4)
		return (size_t)(end - orig);

	size_t dif = (sz - 1) / 3;
	end += dif;
	char *start = nptr;
	nptr += sz - 1;
	int n = 0;
	for (; nptr >= start;) {
		*(nptr + dif) = *nptr;
		--nptr;
		if (++n == 3) {
			*(nptr + dif) = separator;
			if (--dif == 0)
				break;
			n = 0;
		}
	}
	*end = '\0';
	return (size_t)(end - orig);
}

static void
simple_unescape_p(char *s)
{
	char *dst = s;
	for (; *s; ++s) {
		if (*s != '\\') {
			*dst++ = *s;
			continue;
		}
		if (!s[1])
			break;
		++s;
		switch (*s) {
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		{
			*dst = '\0';
			for (int o = 3; o-- && *s >= '0' && *s <= '7'; ++s)
				*dst = (*dst << 3) + (*s - '0');
			++dst;
			--s;
			break;
		}
		case 'b': *dst++ = '\b'; break;
		case 'f': *dst++ = '\f'; break;
		case 'n': *dst++ = '\n'; break;
		case 'r': *dst++ = '\r'; break;
		case 't': *dst++ = '\t'; break;
		case 'v': *dst++ = '\v'; break;
		default:  *dst++ = *s;   break;
		}
	}
	*dst = '\0';
}

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_string(size_t iter)
{
	char h[FSTR], buf_j[FSTR], buf_s[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		if (hl == 0)
			continue;

		/* --- trimend_len_p / trimend_p --- */
		memcpy(buf_j, h, hl + 1);
		memcpy(buf_s, h, hl + 1);
		char *j_end = jstr_trimend_len_p(buf_j, hl);
		simple_trimend_len_p(buf_s, hl);
		assert(strcmp(buf_j, buf_s) == 0);
		assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));

		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			j_end = jstr_trimend_p(buf_j);
			simple_trimend_len_p(buf_s, hl);
			assert(strcmp(buf_j, buf_s) == 0);
			assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));
		}

		/* --- trimstart_len_p / trimstart_p --- */
		memcpy(buf_j, h, hl + 1);
		memcpy(buf_s, h, hl + 1);
		j_end = jstr_trimstart_len_p(buf_j, hl);
		simple_trimstart_len_p(buf_s, hl);
		assert(strcmp(buf_j, buf_s) == 0);
		assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));

		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			j_end = jstr_trimstart_p(buf_j);
			simple_trimstart_len_p(buf_s, hl);
			assert(strcmp(buf_j, buf_s) == 0);
			assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));
		}

		/* --- trim_len_p / trim_p --- */
		memcpy(buf_j, h, hl + 1);
		memcpy(buf_s, h, hl + 1);
		j_end = jstr_trim_len_p(buf_j, hl);
		simple_trimend_len_p(buf_s, hl);
		simple_trimstart_len_p(buf_s, strlen(buf_s));
		assert(strcmp(buf_j, buf_s) == 0);
		assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));

		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			j_end = jstr_trim_p(buf_j);
			simple_trimend_len_p(buf_s, hl);
			simple_trimstart_len_p(buf_s, strlen(buf_s));
			assert(strcmp(buf_j, buf_s) == 0);
			assert(j_end - buf_j == (ptrdiff_t)strlen(buf_s));
		}

		/* --- countchr / countchr_len --- */
		if (hl > 0) {
			unsigned char c = (unsigned char)h[rand() % hl];
			size_t j_cnt = jstr_countchr_len(h, c, hl);
			size_t s_cnt = simple_countchr_len(h, c, hl);
			assert(j_cnt == s_cnt);

			if (hl == strlen(h)) {
				j_cnt = jstr_countchr(h, c);
				s_cnt = simple_countchr_len(h, c, hl);
				assert(j_cnt == s_cnt);
			}
		}

		/* --- count_len / count --- */
		if (hl > 1) {
			char find[8];
			size_t fl = gen_str(find, 7, GEN_ASCII);
			if (fl > 0) {
				find[fl] = '\0';
				size_t j_cnt = jstr_count_len(h, hl, find, fl);
				size_t s_cnt = simple_count_len(h, hl, find, fl);
				assert(j_cnt == s_cnt);

				if (hl == strlen(h)) {
					j_cnt = jstr_count(h, find);
					s_cnt = simple_count_len(h, hl, find, fl);
					assert(j_cnt == s_cnt);
				}
			}
		}


		/* --- thousep --- */
		if (hl > 0 && hl < 180) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			char sep = ',';
			char *j_ep = jstr_thousep_len_p(buf_j, hl, sep);
			size_t s_len = simple_thousep_len_p(buf_s, hl, sep);
			assert(strcmp(buf_j, buf_s) == 0);
			assert((size_t)(j_ep - buf_j) == s_len);

			if (hl == strlen(h)) {
				memcpy(buf_j, h, hl + 1);
				memcpy(buf_s, h, hl + 1);
				j_ep = jstr_thousep_p(buf_j, sep);
				s_len = simple_thousep_len_p(buf_s, hl, sep);
				assert(strcmp(buf_j, buf_s) == 0);
				assert((size_t)(j_ep - buf_j) == s_len);
			}
		}

		/* --- unescape --- */
		if (hl == strlen(h) && hl < FSTR - 1) {
			/* Add some escape sequences */
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			char *j_ep = jstr_unescape_p(buf_j);
			simple_unescape_p(buf_s);
			assert(strcmp(buf_j, buf_s) == 0);
			assert(j_ep == buf_j + strlen(buf_s));
		}

		/* --- starts_len / starts / endscase_len / ends --- */
		if (hl > 1) {
			char find[8];
			size_t fl = gen_str(find, 7, GEN_ASCII);
			if (fl > 0 && fl <= hl) {
				find[fl] = '\0';
				int j_starts = jstr_starts_len(h, hl, find, fl);
				int s_starts = (hl >= fl && memcmp(h, find, fl) == 0);
				assert(!!j_starts == !!s_starts);

				if (hl == strlen(h)) {
					j_starts = jstr_starts(h, find);
					s_starts = (strlen(h) >= fl && strncmp(h, find, fl) == 0);
					assert(!!j_starts == !!s_starts);
				}

				int j_ends = jstr_ends_len(h, hl, find, fl);
				int s_ends = (hl >= fl && memcmp(h + hl - fl, find, fl) == 0);
				assert(!!j_ends == !!s_ends);

				if (hl == strlen(h)) {
					j_ends = jstr_ends(h, find);
					s_ends = (strlen(h) >= fl && strcmp(h + strlen(h) - fl, find) == 0);
					assert(!!j_ends == !!s_ends);
				}

				/* case-insensitive */
				j_starts = jstr_startscase_len(h, hl, find, fl);
				s_starts = (hl >= fl && jstr_strcasecmpeq_len(h, find, fl) == 0);
				assert(!!j_starts == !!s_starts);

				j_ends = jstr_endscase_len(h, hl, find, fl);
				s_ends = (hl >= fl && jstr_strcasecmpeq_len(h + hl - fl, find, fl) == 0);
				assert(!!j_ends == !!s_ends);
			}
		}

		/* --- rev / revcpy --- */
		memcpy(buf_j, h, hl + 1);
		memcpy(buf_s, h, hl + 1);
		jstr_rev_len(buf_j, hl);
		for (size_t j = 0; j < hl / 2; ++j) {
			char tmp = buf_s[j];
			buf_s[j] = buf_s[hl - 1 - j];
			buf_s[hl - 1 - j] = tmp;
		}
		assert(memcmp(buf_j, buf_s, hl) == 0);
		assert(buf_j[hl] == '\0');

		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			char *j_ep = jstr_rev_p(buf_j);
			for (size_t j = 0; j < hl / 2; ++j) {
				char tmp = buf_s[j];
				buf_s[j] = buf_s[hl - 1 - j];
				buf_s[hl - 1 - j] = tmp;
			}
			assert(strcmp(buf_j, buf_s) == 0);
			assert(j_ep == buf_j + hl);
		}

		/* --- basename --- */
		if (hl == strlen(h)) {
			char *j_bn = jstr_basename(h);
			char *s_bn = strrchr(h, '/');
			s_bn = s_bn ? s_bn + 1 : h;
			assert(strcmp(j_bn, s_bn) == 0);
		}

		/* --- linestart / linenext / linenumber --- */
		if (hl > 0) {
			char *p = (char *)memchr(h, '\n', hl);
			if (p) {
				char *j_ls = jstr_linestart(h, p);
				char *s_ls = h;
				for (char *q = p; q >= h; --q) {
					if (*q == '\n') {
						s_ls = q + 1;
						break;
					}
				}
				assert(j_ls == s_ls);
			}

			size_t j_ln = jstr_linenumber(h, h + hl);
			size_t s_ln = simple_countchr_len(h, '\n', hl) + 1;
			assert(j_ln == s_ln);
		}
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
		TESTING(fuzz_string);
		fuzz_string(iter);
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
	fuzz_string(iter);
	return 0;
}

#endif
