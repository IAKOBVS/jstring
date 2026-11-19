/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "../../rigor.h"
#include "internal/simd.h"

#ifndef JSTR_HAVENT_STRCASECHRNUL_SIMD

static char *
ref_strcasechrnul(const char *s, int c)
{
	const unsigned char lo = (unsigned char)jstr_tolower(c);
	const unsigned char up = (unsigned char)jstr_toupper(c);
	const unsigned char *p = (const unsigned char *)s;
	for (;; ++p) {
		if (*p == '\0' || *p == lo || *p == up)
			return (char *)p;
	}
}

static void
fill_pattern(unsigned char *p, size_t len, int pat)
{
	switch (pat) {
	case 0:
		memset(p, 0x00, len);
		break;
	case 1:
		memset(p, 0xFF, len);
		break;
	case 2: {
		static const char printable[] = "aZ09! ~mMnN";
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)printable[i % (sizeof printable - 1)];
		break;
	}
	case 3:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(i % 4 == 0 ? 'A' : i % 4 == 1   ? 'z'
			                                          : i % 4 == 2 ? '0'
			                                                       : '~');
		break;
	case 4:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(0x20 + ((i * 37 + len * 11) % 95));
		break;
	default:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(i == len / 2 ? '\0' : 'm');
		break;
	}
	p[len] = '\0';
}

static void
check_one(size_t len, int pat, size_t off, int c)
{
	enum { PAD = 128,
	       TOTAL = 4096 };
	unsigned char storage[TOTAL];
	unsigned char *s = storage + PAD + off;

	assert(off < 64);
	assert(PAD + off + len + 1 < TOTAL);
	memset(storage, 0xA5, sizeof storage);
	fill_pattern(s, len, pat);

	char *const want = ref_strcasechrnul((const char *)s, c);
	char *const got = jstr_internal_simd_strcasechrnul((const char *)s, c);
	assert(got == want);
	assert(*got == '\0' || (unsigned char)*got == (unsigned char)jstr_tolower(c) || (unsigned char)*got == (unsigned char)jstr_toupper(c));
}

#endif

int
main(void)
{

#ifndef JSTR_HAVENT_STRCASECHRNUL_SIMD
	{
		char buf[128];
		strcpy(buf, "abcABC");
		assert(jstr_internal_simd_strcasechrnul(buf, 'd') == buf + strlen(buf));
		assert(jstr_internal_simd_strcasechrnul(buf, 'b') == buf + 1);
		assert(jstr_internal_simd_strcasechrnul(buf, 'B') == buf + 1);
		assert(jstr_internal_simd_strcasechrnul(buf, '\0') == buf + strlen(buf));
	}

	{
		static const int chars[] = {
			0,
			'a',
			'A',
			'z',
			'Z',
			'q',
			'Q',
			'0',
			' ',
			'~',
			0x7F,
			0x80,
			0xFF
		};
		for (size_t li = 0; li < RIGOR_NLENS; ++li) {
			const size_t len = rigor_lens[li];
			if (!JSTR_TEST_SLOW && len > 128)
				continue;
			for (int pat = 0; pat < 6; ++pat) {
				for (size_t off = 0; off < 64; ++off) {
					for (size_t ci = 0; ci < sizeof chars / sizeof chars[0]; ++ci)
						check_one(len, pat, off, chars[ci]);
				}
			}
		}
	}
#endif

	return EXIT_SUCCESS;
}
