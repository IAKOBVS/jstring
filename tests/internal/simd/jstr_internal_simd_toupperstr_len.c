/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "../../rigor.h"
#include "internal/simd.h"

static unsigned char
ref_upper(unsigned char c)
{
	return (c >= 'a' && c <= 'z') ? (unsigned char)(c - ('a' - 'A')) : c;
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
		static const char printable[] = "AZaz09! ~mMnN";
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)printable[i % (sizeof printable - 1)];
		break;
	}
	case 3:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(i % 4 == 0 ? 'a' : i % 4 == 1   ? 'z'
			                                          : i % 4 == 2 ? 'A'
			                                                       : 'Z');
		break;
	case 4:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(0x20 + ((i * 53 + len * 7) % 95));
		break;
	default:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(i == len / 2 ? '\0' : 'q');
		break;
	}
	p[len] = '\0';
}

static void
check_len(size_t len, int pat, size_t off)
{
	enum { PAD = 128,
	       GUARD = 64,
	       TOTAL = 4096 };
	unsigned char orig[2064];
	unsigned char storage[TOTAL];
	unsigned char *s = storage + PAD + off;

	assert(off < 64);
	assert(len + 1 < sizeof orig);
	assert(PAD + off + len + GUARD < TOTAL);

	memset(storage, 0xA5, sizeof storage);
	fill_pattern(orig, len, pat);
	memcpy(s, orig, len);
	memset(s + len, 0xA5, GUARD);

	jstr_internal_simd_toupperstr_len((char *)s, len);

	for (size_t i = 0; i < len; ++i)
		assert((unsigned char)s[i] == ref_upper(orig[i]));
	for (size_t i = 0; i < GUARD; ++i)
		assert((unsigned char)s[len + i] == 0xA5);
	if (off != 0)
		assert((unsigned char)s[-1] == 0xA5);
}

int
main(void)
{

	{
		char buf[128];
		strcpy(buf, "abc123");
		jstr_internal_simd_toupperstr_len(buf, 6);
		assert(memcmp(buf, "ABC123", 7) == 0);

		strcpy(buf, "abc123");
		jstr_internal_simd_toupperstr_len(buf, 0);
		assert(memcmp(buf, "abc123", 7) == 0);
	}

	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		for (int pat = 0; pat < 6; ++pat) {
			for (size_t off = 0; off < 64; ++off)
				check_len(len, pat, off);
		}
	}

	return EXIT_SUCCESS;
}
