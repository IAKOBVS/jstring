/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_RAREBYTE_H
#define JSTR_RAREBYTE_H

#include "jstr-ctype.h"
#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stddef.h>
PJSTR_END_DECLS

PJSTR_BEGIN_DECLS

/* clang-format off */

/* Do not pass a non-unsigned char.
 * Lower is rarer. */
JSTR_ATTR_MAYBE_UNUSED
static const unsigned char jstr_rarebyte_table[256]={0,1,13,56,59,60,61,62,63,232,248,2,158,4,5,6,7,8,9,10,14,20,26,29,37,46,52,53,54,55,57,58,255,172,242,193,162,174,178,182,218,219,212,180,249,197,221,210,253,231,230,224,225,226,227,223,222,220,176,213,184,229,188,164,159,209,181,203,189,216,196,192,185,205,161,168,215,187,211,194,195,165,206,204,214,198,173,179,175,183,167,202,239,201,160,241,163,246,233,238,240,254,237,208,234,250,169,186,236,217,245,243,228,170,247,244,251,235,199,200,252,207,177,191,171,190,166,3,140,134,124,126,86,128,95,117,114,93,81,87,132,96,112,97,103,82,139,89,98,88,119,74,156,115,104,75,120,106,76,155,90,122,107,125,152,145,136,137,101,116,102,108,99,141,77,78,118,79,109,100,150,73,94,72,121,151,113,135,110,105,83,91,11,12,64,149,146,111,65,69,66,15,16,17,18,19,130,92,144,123,21,22,23,24,131,133,127,142,25,70,129,27,28,67,153,84,143,138,147,157,148,68,71,30,31,32,33,34,35,36,154,38,39,40,41,42,80,43,44,45,47,48,85,49,50,51};

/* Do not pass a non-unsigned char.
 * Lower is rarer.
 * Ignore alpha chars. */
JSTR_ATTR_MAYBE_UNUSED
static const unsigned char jstr_rarebyte_table_case[256]={0,1,13,56,59,60,61,62,63,232,248,2,158,4,5,6,7,8,9,10,14,20,26,29,37,46,52,53,54,55,57,58,255,172,242,193,162,174,178,182,218,219,212,180,249,197,221,210,253,231,230,224,225,226,227,223,222,220,176,213,184,229,188,164,159,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,202,239,201,160,241,163,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,191,171,190,166,3,140,134,124,126,86,128,95,117,114,93,81,87,132,96,112,97,103,82,139,89,98,88,119,74,156,115,104,75,120,106,76,155,90,122,107,125,152,145,136,137,101,116,102,108,99,141,77,78,118,79,109,100,150,73,94,72,121,151,113,135,110,105,83,91,11,12,64,149,146,111,65,69,66,15,16,17,18,19,130,92,144,123,21,22,23,24,131,133,127,142,25,70,129,27,28,67,153,84,143,138,147,157,148,68,71,30,31,32,33,34,35,36,154,38,39,40,41,42,80,43,44,45,47,48,85,49,50,51};

/* clang-format on */

JSTR_FUNC_PURE
static void *
jstr_rarebytefind_len(const void *ne, size_t n)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table[*rare];
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table[*p];
		if (c < c_rare) {
			rare = p;
			c_rare = c;
		}
	}
	return (void *)rare;
}

JSTR_FUNC_PURE
static char *
jstr_rarebytefind(const char *ne)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table[*rare];
	int c;
	for (;; ++p) {
		c = jstr_rarebyte_table[*p];
		if (c < c_rare) {
			if (jstr_unlikely(c == 0))
				break;
			rare = p;
			c_rare = c;
		}
	}
	return (char *)rare;
}

JSTR_FUNC_PURE
static char *
jstr_rarebytefindcase(const char *ne)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table_case[*rare];
	int c;
	for (;; ++p) {
		c = jstr_rarebyte_table_case[*p];
		if (c < c_rare) {
			if (jstr_unlikely(c == 0))
				break;
			rare = p;
			c_rare = c;
		}
	}
	return jstr_isalpha(*rare) ? NULL : (char *)rare;
}

JSTR_FUNC_PURE
static void *
jstr_rarebytefindcase_len(const void *ne, size_t n)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table_case[*rare];
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table_case[*p];
		if (c < c_rare) {
			rare = p;
			c_rare = c;
		}
	}
	return jstr_isalpha(*rare) ? NULL : (char *)rare;
}

JSTR_FUNC_PURE
static void *
jstr_rarebytefindprefernonalpha(const void *ne)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *rare_backup = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table_case[*rare];
	int c;
	for (;; ++p) {
		c = jstr_rarebyte_table[*p];
		if (c < c_rare) {
			if (jstr_isalpha(*p)) {
				rare_backup = p;
			} else if (jstr_unlikely(c == 0)) {
				break;
			} else {
				rare = p;
				c_rare = c;
			}
		}
	}
	return (void *)(jstr_isalpha(*rare) ? rare_backup : rare);
}

JSTR_FUNC_PURE
static void *
jstr_rarebytefindprefernonalpha_len(const void *ne, size_t n)
{
	const unsigned char *rare = (const unsigned char *)ne;
	const unsigned char *rare_backup = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c_rare = jstr_rarebyte_table_case[*rare];
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table[*p];
		if (c < c_rare) {
			if (jstr_isalpha(*p)) {
				rare_backup = p;
			} else {
				rare = p;
				c_rare = c;
			}
		}
	}
	return (void *)(jstr_isalpha(*rare) ? rare_backup : rare);
}

PJSTR_END_DECLS

#endif /* JSTR_RAREBYTE_H */
