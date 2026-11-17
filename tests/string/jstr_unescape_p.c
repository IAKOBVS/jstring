/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static size_t
ref_unescape(char *dst, const char *src)
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	for (;; ++d) {
		if (*s != '\\') {
			if (!*s)
				break;
			*d = *s++;
		} else {
			switch (s[1]) {
			case 0:
				goto out;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7': {
				unsigned char v = 0;
				++s;
				for (int n = 3; n-- && *s >= '0' && *s <= '7'; ++s)
					v = (unsigned char)((v << 3) + (*s - '0'));
				*d = v;
				if (v == 0)
					goto out;
				goto cont;
			}
			case 'b': *d = '\b'; break;
			case 'f': *d = '\f'; break;
			case 'n': *d = '\n'; break;
			case 'r': *d = '\r'; break;
			case 't': *d = '\t'; break;
			case 'v': *d = '\v'; break;
			default: *d = s[1]; break;
			}
			s += 2;
		}
cont:;
	}
out:
	*d = '\0';
	return (size_t)(d - (unsigned char *)dst);
}

static void
chk(const char *src)
{
	char buf[1200], want[1200];
	const size_t wl = ref_unescape(want, src);
	strcpy(buf, src);
	char *p = jstr_unescape_p(buf);
	RCHECK(p == buf + wl);
	RCHECK(!memcmp(buf, want, wl + 1));
}

int
main(void)
{
	char src[1300], buf[1300], want[1300];

	chk("");
	chk("abc");
	chk("a\\nb");
	chk("\\b\\f\\n\\r\\t\\v");
	chk("\\\\");
	chk("\\\"");
	chk("\\q");
	chk("a\\q\\x");
	chk("abc\\");
	chk("\\101");
	chk("\\1012");
	chk("\\1234");
	chk("\\7");
	chk("\\12");
	chk("\\377");
	chk("\\777");
	chk("\\0");
	chk("\\000");
	chk("a\\0b");
	chk("\\400");
	chk("\\8");
	chk("\\x41");
	chk("\\061");
	chk("\\\\n");
	chk("\\999");
	{
		char b[8] = "abc";
		char *p = jstr_unescape_p(b);
		RCHECK(p == b + 3);
		RCHECK(!strcmp(b, "abc"));
	}
	{
		char b[8] = "a\\0b";
		char *p = jstr_unescape_p(b);
		RCHECK(p == b + 1);
		RCHECK(b[0] == 'a' && b[1] == '\0');
	}

	rigor_srand(0x0e5cb0u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		static const char alpha[] = "\\01234567bfnrtvqxABC ";
		for (size_t i = 0; i < len; ++i)
			src[i] = alpha[rigor_randn(sizeof alpha - 1)];
		src[len] = 0;
		const size_t wl = ref_unescape(want, src);
		memcpy(buf, src, len + 1);
		memset(buf + len + 1, 0x7f, 3);
		char *p = jstr_unescape_p(buf);
		RCHECK(p == buf + wl);
		RCHECK(!memcmp(buf, want, wl + 1));
	}

	return 0;
}
