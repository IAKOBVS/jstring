/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#ifndef JSTR_RAREBYTE_H
#define JSTR_RAREBYTE_H

#include "jstr-ctype-table.h"
#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stddef.h>
PJSTR_END_DECLS

PJSTR_BEGIN_DECLS

/*
   Do not pass a non-unsigned char.
   Lower is rarer.
*/
JSTR_MAYBE_UNUSED
static const unsigned char jstr_rarebyte_table[256] = {
	0, /* 0 */
	1, /* 1 */
	13, /* 2 */
	56, /* 3 */
	59, /* 4 */
	60, /* 5 */
	61, /* 6 */
	62, /* 7 */
	63, /* 8 */
	232, /* 9 */
	248, /* 10 */
	2, /* 11 */
	158, /* 12 */
	4, /* 13 */
	5, /* 14 */
	6, /* 15 */
	7, /* 16 */
	8, /* 17 */
	9, /* 18 */
	10, /* 19 */
	14, /* 20 */
	20, /* 21 */
	26, /* 22 */
	29, /* 23 */
	37, /* 24 */
	46, /* 25 */
	52, /* 26 */
	53, /* 27 */
	54, /* 28 */
	55, /* 29 */
	57, /* 30 */
	58, /* 31 */
	255, /* 32 */
	172, /* 33 */
	242, /* 34 */
	193, /* 35 */
	162, /* 36 */
	174, /* 37 */
	178, /* 38 */
	182, /* 39 */
	218, /* 40 */
	219, /* 41 */
	212, /* 42 */
	180, /* 43 */
	249, /* 44 */
	197, /* 45 */
	221, /* 46 */
	210, /* 47 */
	253, /* 48 */
	231, /* 49 */
	230, /* 50 */
	224, /* 51 */
	225, /* 52 */
	226, /* 53 */
	227, /* 54 */
	223, /* 55 */
	222, /* 56 */
	220, /* 57 */
	176, /* 58 */
	213, /* 59 */
	184, /* 60 */
	229, /* 61 */
	188, /* 62 */
	164, /* 63 */
	159, /* 64 */
	209, /* 65 */
	181, /* 66 */
	203, /* 67 */
	189, /* 68 */
	216, /* 69 */
	196, /* 70 */
	192, /* 71 */
	185, /* 72 */
	205, /* 73 */
	161, /* 74 */
	168, /* 75 */
	215, /* 76 */
	187, /* 77 */
	211, /* 78 */
	194, /* 79 */
	195, /* 80 */
	165, /* 81 */
	206, /* 82 */
	204, /* 83 */
	214, /* 84 */
	198, /* 85 */
	173, /* 86 */
	179, /* 87 */
	175, /* 88 */
	183, /* 89 */
	167, /* 90 */
	202, /* 91 */
	239, /* 92 */
	201, /* 93 */
	160, /* 94 */
	241, /* 95 */
	163, /* 96 */
	246, /* 97 */
	233, /* 98 */
	238, /* 99 */
	240, /* 100 */
	254, /* 101 */
	237, /* 102 */
	208, /* 103 */
	234, /* 104 */
	250, /* 105 */
	169, /* 106 */
	186, /* 107 */
	236, /* 108 */
	217, /* 109 */
	245, /* 110 */
	243, /* 111 */
	228, /* 112 */
	170, /* 113 */
	247, /* 114 */
	244, /* 115 */
	251, /* 116 */
	235, /* 117 */
	199, /* 118 */
	200, /* 119 */
	252, /* 120 */
	207, /* 121 */
	177, /* 122 */
	191, /* 123 */
	171, /* 124 */
	190, /* 125 */
	166, /* 126 */
	3, /* 127 */
	140, /* 128 */
	134, /* 129 */
	124, /* 130 */
	126, /* 131 */
	86, /* 132 */
	128, /* 133 */
	95, /* 134 */
	117, /* 135 */
	114, /* 136 */
	93, /* 137 */
	81, /* 138 */
	87, /* 139 */
	132, /* 140 */
	96, /* 141 */
	112, /* 142 */
	97, /* 143 */
	103, /* 144 */
	82, /* 145 */
	139, /* 146 */
	89, /* 147 */
	98, /* 148 */
	88, /* 149 */
	119, /* 150 */
	74, /* 151 */
	156, /* 152 */
	115, /* 153 */
	104, /* 154 */
	75, /* 155 */
	120, /* 156 */
	106, /* 157 */
	76, /* 158 */
	155, /* 159 */
	90, /* 160 */
	122, /* 161 */
	107, /* 162 */
	125, /* 163 */
	152, /* 164 */
	145, /* 165 */
	136, /* 166 */
	137, /* 167 */
	101, /* 168 */
	116, /* 169 */
	102, /* 170 */
	108, /* 171 */
	99, /* 172 */
	141, /* 173 */
	77, /* 174 */
	78, /* 175 */
	118, /* 176 */
	79, /* 177 */
	109, /* 178 */
	100, /* 179 */
	150, /* 180 */
	73, /* 181 */
	94, /* 182 */
	72, /* 183 */
	121, /* 184 */
	151, /* 185 */
	113, /* 186 */
	135, /* 187 */
	110, /* 188 */
	105, /* 189 */
	83, /* 190 */
	91, /* 191 */
	11, /* 192 */
	12, /* 193 */
	64, /* 194 */
	149, /* 195 */
	146, /* 196 */
	111, /* 197 */
	65, /* 198 */
	69, /* 199 */
	66, /* 200 */
	15, /* 201 */
	16, /* 202 */
	17, /* 203 */
	18, /* 204 */
	19, /* 205 */
	130, /* 206 */
	92, /* 207 */
	144, /* 208 */
	123, /* 209 */
	21, /* 210 */
	22, /* 211 */
	23, /* 212 */
	24, /* 213 */
	131, /* 214 */
	133, /* 215 */
	127, /* 216 */
	142, /* 217 */
	25, /* 218 */
	70, /* 219 */
	129, /* 220 */
	27, /* 221 */
	28, /* 222 */
	67, /* 223 */
	153, /* 224 */
	84, /* 225 */
	143, /* 226 */
	138, /* 227 */
	147, /* 228 */
	157, /* 229 */
	148, /* 230 */
	68, /* 231 */
	71, /* 232 */
	30, /* 233 */
	31, /* 234 */
	32, /* 235 */
	33, /* 236 */
	34, /* 237 */
	35, /* 238 */
	36, /* 239 */
	154, /* 240 */
	38, /* 241 */
	39, /* 242 */
	40, /* 243 */
	41, /* 244 */
	42, /* 245 */
	80, /* 246 */
	43, /* 247 */
	44, /* 248 */
	45, /* 249 */
	47, /* 250 */
	48, /* 251 */
	85, /* 252 */
	49, /* 253 */
	50, /* 254 */
	51 /* 255 */
};

/*
   Do not pass a non-unsigned char.
   Lower is rarer.
   Ignore alpha chars.
*/
JSTR_MAYBE_UNUSED
static const unsigned char jstr_rarebyte_table_case[256] = {
	0, /* 0 */
	1, /* 1 */
	13, /* 2 */
	56, /* 3 */
	59, /* 4 */
	60, /* 5 */
	61, /* 6 */
	62, /* 7 */
	63, /* 8 */
	232, /* 9 */
	248, /* 10 */
	2, /* 11 */
	158, /* 12 */
	4, /* 13 */
	5, /* 14 */
	6, /* 15 */
	7, /* 16 */
	8, /* 17 */
	9, /* 18 */
	10, /* 19 */
	14, /* 20 */
	20, /* 21 */
	26, /* 22 */
	29, /* 23 */
	37, /* 24 */
	46, /* 25 */
	52, /* 26 */
	53, /* 27 */
	54, /* 28 */
	55, /* 29 */
	57, /* 30 */
	58, /* 31 */
	255, /* 32 */
	172, /* 33 */
	242, /* 34 */
	193, /* 35 */
	162, /* 36 */
	174, /* 37 */
	178, /* 38 */
	182, /* 39 */
	218, /* 40 */
	219, /* 41 */
	212, /* 42 */
	180, /* 43 */
	249, /* 44 */
	197, /* 45 */
	221, /* 46 */
	210, /* 47 */
	253, /* 48 */
	231, /* 49 */
	230, /* 50 */
	224, /* 51 */
	225, /* 52 */
	226, /* 53 */
	227, /* 54 */
	223, /* 55 */
	222, /* 56 */
	220, /* 57 */
	176, /* 58 */
	213, /* 59 */
	184, /* 60 */
	229, /* 61 */
	188, /* 62 */
	164, /* 63 */
	159, /* 64 */
	255, /* 65 */
	255, /* 66 */
	255, /* 67 */
	255, /* 68 */
	255, /* 69 */
	255, /* 70 */
	255, /* 71 */
	255, /* 72 */
	255, /* 73 */
	255, /* 74 */
	255, /* 75 */
	255, /* 76 */
	255, /* 77 */
	255, /* 78 */
	255, /* 79 */
	255, /* 80 */
	255, /* 81 */
	255, /* 82 */
	255, /* 83 */
	255, /* 84 */
	255, /* 85 */
	255, /* 86 */
	255, /* 87 */
	255, /* 88 */
	255, /* 89 */
	255, /* 90 */
	202, /* 91 */
	239, /* 92 */
	201, /* 93 */
	160, /* 94 */
	241, /* 95 */
	163, /* 96 */
	255, /* 97 */
	255, /* 98 */
	255, /* 99 */
	255, /* 100 */
	255, /* 101 */
	255, /* 102 */
	255, /* 103 */
	255, /* 104 */
	255, /* 105 */
	255, /* 106 */
	255, /* 107 */
	255, /* 108 */
	255, /* 109 */
	255, /* 110 */
	255, /* 111 */
	255, /* 112 */
	255, /* 113 */
	255, /* 114 */
	255, /* 115 */
	255, /* 116 */
	255, /* 117 */
	255, /* 118 */
	255, /* 119 */
	255, /* 120 */
	255, /* 121 */
	255, /* 122 */
	191, /* 123 */
	171, /* 124 */
	190, /* 125 */
	166, /* 126 */
	3, /* 127 */
	140, /* 128 */
	134, /* 129 */
	124, /* 130 */
	126, /* 131 */
	86, /* 132 */
	128, /* 133 */
	95, /* 134 */
	117, /* 135 */
	114, /* 136 */
	93, /* 137 */
	81, /* 138 */
	87, /* 139 */
	132, /* 140 */
	96, /* 141 */
	112, /* 142 */
	97, /* 143 */
	103, /* 144 */
	82, /* 145 */
	139, /* 146 */
	89, /* 147 */
	98, /* 148 */
	88, /* 149 */
	119, /* 150 */
	74, /* 151 */
	156, /* 152 */
	115, /* 153 */
	104, /* 154 */
	75, /* 155 */
	120, /* 156 */
	106, /* 157 */
	76, /* 158 */
	155, /* 159 */
	90, /* 160 */
	122, /* 161 */
	107, /* 162 */
	125, /* 163 */
	152, /* 164 */
	145, /* 165 */
	136, /* 166 */
	137, /* 167 */
	101, /* 168 */
	116, /* 169 */
	102, /* 170 */
	108, /* 171 */
	99, /* 172 */
	141, /* 173 */
	77, /* 174 */
	78, /* 175 */
	118, /* 176 */
	79, /* 177 */
	109, /* 178 */
	100, /* 179 */
	150, /* 180 */
	73, /* 181 */
	94, /* 182 */
	72, /* 183 */
	121, /* 184 */
	151, /* 185 */
	113, /* 186 */
	135, /* 187 */
	110, /* 188 */
	105, /* 189 */
	83, /* 190 */
	91, /* 191 */
	11, /* 192 */
	12, /* 193 */
	64, /* 194 */
	149, /* 195 */
	146, /* 196 */
	111, /* 197 */
	65, /* 198 */
	69, /* 199 */
	66, /* 200 */
	15, /* 201 */
	16, /* 202 */
	17, /* 203 */
	18, /* 204 */
	19, /* 205 */
	130, /* 206 */
	92, /* 207 */
	144, /* 208 */
	123, /* 209 */
	21, /* 210 */
	22, /* 211 */
	23, /* 212 */
	24, /* 213 */
	131, /* 214 */
	133, /* 215 */
	127, /* 216 */
	142, /* 217 */
	25, /* 218 */
	70, /* 219 */
	129, /* 220 */
	27, /* 221 */
	28, /* 222 */
	67, /* 223 */
	153, /* 224 */
	84, /* 225 */
	143, /* 226 */
	138, /* 227 */
	147, /* 228 */
	157, /* 229 */
	148, /* 230 */
	68, /* 231 */
	71, /* 232 */
	30, /* 233 */
	31, /* 234 */
	32, /* 235 */
	33, /* 236 */
	34, /* 237 */
	35, /* 238 */
	36, /* 239 */
	154, /* 240 */
	38, /* 241 */
	39, /* 242 */
	40, /* 243 */
	41, /* 244 */
	42, /* 245 */
	80, /* 246 */
	43, /* 247 */
	44, /* 248 */
	45, /* 249 */
	47, /* 250 */
	48, /* 251 */
	85, /* 252 */
	49, /* 253 */
	50, /* 254 */
	51 /* 255 */
};

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_rarebytefind_len(const void *ne,
                      size_t n)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table[*p];
		if (c < *save)
			save = p;
	}
	return save != &i ? (char *)save : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_rarebytefind(const char *ne)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; (c = jstr_rarebyte_table[*p]); ++p)
		if (c < *save)
			save = p;
	return save != &i ? (char *)save : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_rarebytefindcase(const char *ne)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; (c = jstr_rarebyte_table_case[*p]); ++p)
		if (c < *save)
			save = p;
	return save != &i ? (char *)save : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_rarebytefindcase_len(const void *ne,
                          size_t n)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table_case[*p];
		if (c < *save)
			save = p;
	}
	return save != &i ? (void *)save : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_rarebytefindeither(const void *ne)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *save_backup = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; (c = jstr_rarebyte_table_case[*p]); ++p)
		if (c < *save) {
			if (jstr_ctype_table[c] & JSTR_ISALPHA)
				save_backup = p;
			else
				save = p;
		}
	return (void *)(save != &i ? save : save_backup);
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_rarebytefindeither_len(const void *ne,
                            size_t n)
{
	const unsigned char i = (unsigned char)-1;
	const unsigned char *save = &i;
	const unsigned char *save_backup = (const unsigned char *)ne;
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; n--; ++p) {
		c = jstr_rarebyte_table_case[*p];
		if (c < *save) {
			if (jstr_ctype_table[c] & JSTR_ISALPHA)
				save_backup = p;
			else
				save = p;
		}
	}
	return (void *)(save != &i ? save : save_backup);
}

PJSTR_END_DECLS

#endif /* JSTR_RAREBYTE_H */
