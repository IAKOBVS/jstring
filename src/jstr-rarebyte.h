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

/* Do not pass a non-unsigned char. */
JSTR_MAYBE_UNUSED
static const unsigned char jstr_rarebyte_table[256] = {
	0, /* 0 */
	0, /* 1 */
	0, /* 2 */
	0, /* 3 */
	0, /* 4 */
	0, /* 5 */
	0, /* 6 */
	0, /* 7 */
	0, /* 8 */
	0, /* 9 */
	0, /* 10 */
	0, /* 11 */
	0, /* 12 */
	0, /* 13 */
	0, /* 14 */
	0, /* 15 */
	0, /* 16 */
	0, /* 17 */
	0, /* 18 */
	0, /* 19 */
	0, /* 20 */
	0, /* 21 */
	0, /* 22 */
	0, /* 23 */
	0, /* 24 */
	0, /* 25 */
	0, /* 26 */
	0, /* 27 */
	0, /* 28 */
	0, /* 29 */
	0, /* 30 */
	0, /* 31 */
	0, /* ' ' */
	1, /* '!' */
	1, /* '"' */
	1, /* '#' */
	1, /* '$' */
	1, /* '%' */
	1, /* '&' */
	1, /* '\'' */
	1, /* '(' */
	1, /* ')' */
	1, /* '*' */
	1, /* '+' */
	1, /* '', */
	1, /* '-' */
	1, /* '.' */
	1, /* '/' */
	1, /* '0' */
	1, /* '1' */
	1, /* '2' */
	1, /* '3' */
	1, /* '4' */
	1, /* '5' */
	1, /* '6' */
	1, /* '7' */
	1, /* '8' */
	1, /* '9' */
	1, /* ':' */
	1, /* ';' */
	1, /* '<' */
	1, /* '=' */
	1, /* '>' */
	1, /* '?' */
	1, /* '@' */
	1, /* 'A' */
	1, /* 'B' */
	1, /* 'C' */
	1, /* 'D' */
	1, /* 'E' */
	1, /* 'F' */
	1, /* 'G' */
	1, /* 'H' */
	1, /* 'I' */
	1, /* 'J' */
	1, /* 'K' */
	1, /* 'L' */
	1, /* 'M' */
	1, /* 'N' */
	1, /* 'O' */
	1, /* 'P' */
	1, /* 'Q' */
	1, /* 'R' */
	1, /* 'S' */
	1, /* 'T' */
	1, /* 'U' */
	1, /* 'V' */
	1, /* 'W' */
	1, /* 'X' */
	1, /* 'Y' */
	1, /* 'Z' */
	1, /* '[' */
	1, /* '\\' */
	1, /* ']' */
	1, /* '^' */
	1, /* '_' */
	1, /* '`' */
	0, /* 'a' */
	0, /* 'b' */
	0, /* 'c' */
	0, /* 'd' */
	0, /* 'e' */
	0, /* 'f' */
	0, /* 'g' */
	0, /* 'h' */
	0, /* 'i' */
	1, /* 'j' */
	0, /* 'k' */
	0, /* 'l' */
	0, /* 'm' */
	0, /* 'n' */
	0, /* 'o' */
	0, /* 'p' */
	1, /* 'q' */
	0, /* 'r' */
	0, /* 's' */
	0, /* 't' */
	0, /* 'u' */
	0, /* 'v' */
	0, /* 'w' */
	1, /* 'x' */
	0, /* 'y' */
	1, /* 'z' */
	1, /* '{' */
	1, /* '|' */
	1, /* '}' */
	1, /* '~' */
	0, /* 127 */
	1, /* 128 */
	1, /* 129 */
	1, /* 130 */
	1, /* 131 */
	1, /* 132 */
	1, /* 133 */
	1, /* 134 */
	1, /* 135 */
	1, /* 136 */
	1, /* 137 */
	1, /* 138 */
	1, /* 139 */
	1, /* 140 */
	1, /* 141 */
	1, /* 142 */
	1, /* 143 */
	1, /* 144 */
	1, /* 145 */
	1, /* 146 */
	1, /* 147 */
	1, /* 148 */
	1, /* 149 */
	1, /* 150 */
	1, /* 151 */
	1, /* 152 */
	1, /* 153 */
	1, /* 154 */
	1, /* 155 */
	1, /* 156 */
	1, /* 157 */
	1, /* 158 */
	1, /* 159 */
	1, /* 160 */
	1, /* 161 */
	1, /* 162 */
	1, /* 163 */
	1, /* 164 */
	1, /* 165 */
	1, /* 166 */
	1, /* 167 */
	1, /* 168 */
	1, /* 169 */
	1, /* 170 */
	1, /* 171 */
	1, /* 172 */
	1, /* 173 */
	1, /* 174 */
	1, /* 175 */
	1, /* 176 */
	1, /* 177 */
	1, /* 178 */
	1, /* 179 */
	1, /* 180 */
	1, /* 181 */
	1, /* 182 */
	1, /* 183 */
	1, /* 184 */
	1, /* 185 */
	1, /* 186 */
	1, /* 187 */
	1, /* 188 */
	1, /* 189 */
	1, /* 190 */
	1, /* 191 */
	1, /* 192 */
	1, /* 193 */
	1, /* 194 */
	1, /* 195 */
	1, /* 196 */
	1, /* 197 */
	1, /* 198 */
	1, /* 199 */
	1, /* 200 */
	1, /* 201 */
	1, /* 202 */
	1, /* 203 */
	1, /* 204 */
	1, /* 205 */
	1, /* 206 */
	1, /* 207 */
	1, /* 208 */
	1, /* 209 */
	1, /* 210 */
	1, /* 211 */
	1, /* 212 */
	1, /* 213 */
	1, /* 214 */
	1, /* 215 */
	1, /* 216 */
	1, /* 217 */
	1, /* 218 */
	1, /* 219 */
	1, /* 220 */
	1, /* 221 */
	1, /* 222 */
	1, /* 223 */
	1, /* 224 */
	1, /* 225 */
	1, /* 226 */
	1, /* 227 */
	1, /* 228 */
	1, /* 229 */
	1, /* 230 */
	1, /* 231 */
	1, /* 232 */
	1, /* 233 */
	1, /* 234 */
	1, /* 235 */
	1, /* 236 */
	1, /* 237 */
	1, /* 238 */
	1, /* 239 */
	1, /* 240 */
	1, /* 241 */
	1, /* 242 */
	1, /* 243 */
	1, /* 244 */
	1, /* 245 */
	1, /* 246 */
	1, /* 247 */
	1, /* 248 */
	1, /* 249 */
	1, /* 250 */
	1, /* 251 */
	1, /* 252 */
	1, /* 253 */
	1, /* 254 */
	1 /* 255 */
};

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_rarebyteget_len(const void *ne,
                     size_t n)
{
	int c;
	const unsigned char *p = (const unsigned char *)ne;
	for (; n-- && (c = *p); ++p)
		if (jstr_rarebyte_table[c])
			return (unsigned char *)p;
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_rarebyteget(const char *n)
{
	const unsigned char *p = (const unsigned char *)n;
	int c;
	for (; (c = *p); ++p)
		if (jstr_rarebyte_table[c])
			return (char *)p;
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_rarebytegetcase(const char *n)
{
	const unsigned char *p = (const unsigned char *)n;
	int c;
	for (; (c = *p); ++p)
		if (jstr_ctype_table[c] & (JSTR_ISDIGIT | JSTR_ISPUNCT))
			return (char *)p;
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_rarebytegetcase_len(const void *ne,
                         size_t n)
{
	const unsigned char *p = (const unsigned char *)ne;
	int c;
	for (; n-- && (c = *p); ++p)
		if (jstr_ctype_table[c] & (JSTR_ISDIGIT | JSTR_ISPUNCT))
			return (char *)p;
	return NULL;
}

PJSTR_END_DECLS

#endif /* JSTR_RAREBYTE_H */
