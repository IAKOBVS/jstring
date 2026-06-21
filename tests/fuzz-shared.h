/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * Shared helpers for fuzz test files.
 */

#ifndef FUZZ_SHARED_H
#define FUZZ_SHARED_H

#include <string.h>
#include <stdlib.h>

#include "../include/macros.h"

enum { FSTR = 256, FMEM = 128, FITER = 5000 };

enum gen_mode {
	GEN_ASCII = 0,   /* printable ASCII (0x20-0x7E) */
	GEN_MEM,          /* any non-NUL byte */
	GEN_REPEAT,       /* single printable char repeated */
	GEN_BINARY        /* any byte (including NUL) */
};

/* Fill buf with up to max_len random bytes, return <= max_len. */
JSTR_ATTR_MAYBE_UNUSED
static size_t
gen_str(char *buf, size_t max_len, enum gen_mode mode)
{
	size_t len = (size_t)rand() % (max_len + 1);
	if (mode == GEN_REPEAT) {
		memset(buf, (char)('!' + (rand() % 94)), len);
	} else {
		for (size_t i = 0; i < len; ++i) {
			if (mode == GEN_ASCII)
				buf[i] = (char)(' ' + (rand() % 95));
			else if (mode == GEN_MEM)
				buf[i] = (char)(1 + (rand() % 254));
			else /* GEN_BINARY */
				buf[i] = (char)(rand() & 0xFF);
		}
	}
	buf[len] = '\0';
	return len;
}

/* Simple forward memmem (no embedded-NUL hazards). */
JSTR_ATTR_MAYBE_UNUSED
static void *
fuzz_memmem(const void *hs, size_t hl, const void *ne, size_t nl)
{
	if (!nl)
		return (char *)hs;
	if (nl > hl)
		return NULL;
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	for (const unsigned char *p = h; p <= h + hl - nl; ++p)
		if (*p == *n && memcmp(p, n, nl) == 0)
			return (void *)p;
	return NULL;
}

/* Simple reverse memmem.  Empty needle -> end of haystack. */
JSTR_ATTR_MAYBE_UNUSED
static void *
fuzz_memrmem(const void *hs, size_t hl, const void *ne, size_t nl)
{
	if (!nl)
		return (char *)hs + hl;
	if (nl > hl)
		return NULL;
	const unsigned char *h = (const unsigned char *)hs;
	for (size_t i = hl - nl; i != (size_t)-1; --i)
		if (memcmp(h + i, ne, nl) == 0)
			return (void *)(h + i);
	return NULL;
}

#endif /* FUZZ_SHARED_H */
