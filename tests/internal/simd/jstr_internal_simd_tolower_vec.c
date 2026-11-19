/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

#if defined(CMPGT8)

static unsigned char
ref_lower(unsigned char c)
{
	return (c >= 'A' && c <= 'Z') ? (unsigned char)(c + ('a' - 'A')) : c;
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
			p[i] = (unsigned char)(i % 4 == 0 ? 'A' : i % 4 == 1   ? 'Z'
			                                          : i % 4 == 2 ? 'a'
			                                                       : 'z');
		break;
	case 4:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(0x20 + ((i * 53 + len * 7) % 95));
		break;
	default:
		for (size_t i = 0; i < len; ++i)
			p[i] = (unsigned char)(i % 3 == 0 ? '@' : i % 3 == 1 ? '['
			                                                     : '`');
		break;
	}
}

static void
check_vec(const unsigned char *src, size_t off)
{
	unsigned char out[VEC_SIZE];
	VEC v;
	VEC r;

	memcpy(&v, src + off, sizeof v);
	r = jstr_internal_simd_tolower_vec(v);
	memcpy(out, &r, sizeof r);
	for (size_t i = 0; i < VEC_SIZE; ++i)
		assert(out[i] == ref_lower(src[off + i]));
}

#endif

int
main(void)
{

#if defined(CMPGT8)
	{
		char in[VEC_SIZE];
		char out[VEC_SIZE];

		for (int c = 0; c < 256; c++) {
			memset(in, c, VEC_SIZE);
			VEC v;
			memcpy(&v, in, sizeof(v));
			VEC r = jstr_internal_simd_tolower_vec(v);
			memcpy(out, &r, sizeof(r));
			for (size_t i = 0; i < VEC_SIZE; i++)
				assert(out[i] == (char)tolower((unsigned char)c));
		}
	}

	{
		unsigned char storage[VEC_SIZE + 128];
		for (int pat = 0; pat < 6; ++pat) {
			fill_pattern(storage, sizeof storage, pat);
			for (size_t off = 0; off < 64; ++off)
				check_vec(storage, off);
		}
	}

	{
		/* Explicit boundary pairs around 'A'-'Z'. */
		static const unsigned char edges[] = {
			'@',
			'A',
			'B',
			'Y',
			'Z',
			'[',
			'`',
			'a',
			'b',
			'y',
			'z',
			'{',
			0x00,
			0x7F,
			0x80,
			0xFF
		};
		unsigned char storage[VEC_SIZE + 128];
		for (size_t i = 0; i < sizeof storage; ++i)
			storage[i] = edges[i % sizeof edges];
		for (size_t off = 0; off < 64; ++off)
			check_vec(storage, off);
	}
#endif

	return EXIT_SUCCESS;
}
