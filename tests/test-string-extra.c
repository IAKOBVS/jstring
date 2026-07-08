/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "test-array.h"

#define _POSIX_C_SOURCE 200809L

int
main(int argc, char **argv)
{
	START();
	/* jstr_memrchrnul */
	{
		const char s[] = "abcde";
		assert(jstr_memrchrnul(s, 'c', 5) == s + 2);
		assert(jstr_memrchrnul(s, 'z', 5) == s + 5);
	}
	/* jstr_memchrnul */
	{
		const char s[] = "abcde";
		assert(jstr_memchrnul(s, 'c', 5) == s + 2);
		assert(jstr_memchrnul(s, 'z', 5) == s + 5);
	}
	/* jstr_stpcat */
	{
		char buf[32] = "hello";
		char *end = jstr_stpcat(buf, " world");
		assert(end == buf + 11);
		assert(!strcmp(buf, "hello world"));
	}
	/* jstr_basename_len */
	{
		const char s[] = "/usr/local/bin";
		char *p = jstr_basename_len(s, 14);
		assert(!strcmp(p, "bin"));
		p = jstr_basename_len("foo", 3);
		assert(!strcmp(p, "foo"));
		p = jstr_basename_len("", 0);
		assert(!strcmp(p, ""));
	}
	/* jstr_strstrnul */
	{
		const char hs[] = "hello world";
		const char *p = jstr_strstrnul(hs, "wo");
		assert(!strcmp(p, "world"));
		p = jstr_strstrnul(hs, "xyz");
		assert(p == hs + 11);
		p = jstr_strstrnul(hs, "h");
		assert(p == hs);
		p = jstr_strstrnul(hs, "d");
		assert(!strcmp(p, "d"));
	}
	/* jstr_cmpeq */
	{
		assert(jstr_cmpeq("abc", 3, "abc", 3) == 0);
		assert(jstr_cmpeq("abc", 3, "abd", 3) != 0);
		assert(jstr_cmpeq("abc", 3, "ab", 2) == 1);
	}
	/* jstr_cmpcaseeq */
	{
		assert(jstr_cmpcaseeq("AbC", 3, "aBc", 3) == 0);
		assert(jstr_cmpcaseeq("AbC", 3, "abc", 2) == 1);
	}
	/* jstr_endscase */
	{
		assert(jstr_endscase("hello world", "WORLD"));
		assert(!jstr_endscase("hello world", "worldx"));
	}
	/* jstr_startscase */
	{
		assert(jstr_startscase("Hello World", "hello"));
		assert(!jstr_startscase("Hello World", "world"));
		assert(jstr_startscase("Hello", ""));
	}
	/* jstr_startscasenul */
	{
		size_t n = jstr_startscasenul("Hello World", "hello");
		assert(n == 5);
		n = jstr_startscasenul("Hello World", "world");
		assert(n == 0);
		n = jstr_startscasenul("", "");
		assert(n == 0);
	}
	/* jstr_strnstr_len */
	{
		const char *p = jstr_strnstr_len("hello world", 11, "wo", 2, 8);
		assert(p != NULL && !strcmp(p, "world"));
		p = jstr_strnstr_len("hello world", 11, "wo", 2, 4);
		assert(p == NULL);
	}
	/* jstr_memnmem */
	{
		const char *p = (const char *)jstr_memnmem("hello world", 11, "wo", 2, 8);
		assert(p != NULL && !strcmp(p, "world"));
		p = (const char *)jstr_memnmem("hello world", 11, "wo", 2, 4);
		assert(p == NULL);
	}
	/* jstr_memmemnul */
	{
		const char *p = (const char *)jstr_memmemnul("hello world", 11, "wo", 2);
		assert(!strcmp(p, "world"));
		(void)jstr_memmemnul("hello world", 11, "xyz", 3);
	}
	/* jstr_strstrnul_len */
	{
		const char *p = jstr_strstrnul_len("hello world", 11, "wo", 2);
		assert(!strcmp(p, "world"));
		(void)jstr_strstrnul_len("hello world", 11, "xyz", 3);
	}
	/* jstr_strcasestr_len_comp / exec */
	{
		jstr_twoway_ty tw;
		jstr_strcasestr_len_comp(&tw, "abc", 3);
		const char *p = jstr_strcasestr_len_exec(&tw, "xxABCxx", 7, "abc", 3);
		assert(p != NULL && !strncasecmp(p, "ABC", 3));
	}
	/* jstr_strncasestr_len */
	{
		const char *p = jstr_strncasestr_len("hello WORLD", 11, "world", 5, 11);
		assert(p != NULL && !strncasecmp(p, "WORLD", 5));
		p = jstr_strncasestr_len("hello WORLD", 11, "world", 5, 6);
		assert(p == NULL);
	}
	/* jstr_strcasestr_comp / exec */
	{
		jstr_twoway_ty tw;
		jstr_strcasestr_comp(&tw, "abc");
		const char *p = jstr_strcasestr_exec(&tw, "xxABCxx", "abc");
		assert(p != NULL && !strncasecmp(p, "ABC", 3));
	}
	{
		jstr_twoway_ty tw;
		jstr_strcasestr_comp(&tw, "a");
		const char *p = jstr_strcasestr_exec(&tw, "bcAde", "a");
		assert(p != NULL && *p == 'A');
	}
	{
		jstr_twoway_ty tw;
		jstr_strcasestr_comp(&tw, "");
		const char *p = jstr_strcasestr_exec(&tw, "hello", "");
		assert(p != NULL);
	}
	{
		jstr_twoway_ty tw;
		jstr_strcasestr_comp(&tw, "ab");
		const char *p = jstr_strcasestr_exec(&tw, "xABy", "ab");
		assert(p != NULL && !strncasecmp(p, "AB", 2));
	}
	/* jstr_strstr_exec (SIMD > THRES path) */
	{
		/* The SIMD > THRES path requires ne_len > sizeof(jstr_vvec_ty)
		   (typically > 32 on x86-64). So use a long needle. */
		char long_ne[64];
		memset(long_ne, 'a', 32);
		long_ne[32] = 'b';
		long_ne[33] = '\0';
		char long_hs[128];
		memset(long_hs, 'x', 64);
		memcpy(long_hs + 64, long_ne, 33);
		long_hs[97] = '\0';
		jstr_twoway_ty tw;
		jstr_strstr_comp(&tw, long_ne);
		const char *p = jstr_strstr_exec(&tw, long_hs, long_ne);
		assert(p != NULL && p == long_hs + 64);
	}
	/* jstr_strrcspn */
	{
		/* returns count of non-reject chars from end */
		size_t n = jstr_strrcspn("/usr/local", "/");
		assert(n == 5);
		n = jstr_strrcspn("hello", "/");
		assert(n == 0);
	}
	/* jstr_strrspn */
	{
		/* returns number of chars NOT matched from end */
		size_t n = jstr_strrspn("hello", "hel");
		assert(n == 5);
		n = jstr_strrspn("ohell", "hel");
		assert(n == 1);
	}
	/* jstr_strrpbrk_len */
	{
		const char *p = jstr_strrpbrk_len("/usr/local", "/", 10);
		assert(p != NULL);
		(void)jstr_strrpbrk_len("hello", "/", 5);
	}
	/* jstr_strrpbrk */
	{
		const char *p = jstr_strrpbrk("hello world", " ow");
		assert(p != NULL);
	}
	/* jstr_memspn */
	{
		size_t n = jstr_memspn("hello", "hel", 5);
		assert(n == 4);
		n = jstr_memspn("hello", "xyz", 5);
		assert(n == 0);
	}
	/* jstr_memcspn */
	{
		size_t n = jstr_memcspn("hello", "ol", 5);
		assert(n == 2);
		n = jstr_memcspn("hello", "xyz", 5);
		assert(n == 5);
	}
	/* jstr_mempbrk */
	{
		const char *s = "hello";
		const char *p = (const char *)jstr_mempbrk(s, "ol", 5);
		assert(p != NULL && *p == 'l');
		p = (const char *)jstr_mempbrk(s, "xyz", 5);
		assert(p == NULL);
	}
	/* jstr_strchrnulinv */
	{
		const char *p = jstr_strchrnulinv("aaaabc", 'a');
		assert(*p == 'b');
		p = jstr_strchrnulinv("aaaa", 'a');
		assert(*p == '\0');
		p = jstr_strchrnulinv("abc", '\0');
		assert(p == (const char *)"abc");
	}
	/* jstr_strchrinv */
	{
		const char *p = jstr_strchrinv("aaaabc", 'a');
		assert(*p == 'b');
		p = jstr_strchrinv("aaaa", 'a');
		assert(p != NULL && *p == '\0');
	}
	/* jstr_memchrnulinv */
	{
		const char *p = (const char *)jstr_memchrnulinv("aaab", 'a', 4);
		assert(*p == 'b');
		p = (const char *)jstr_memchrnulinv("aaa", 'a', 3);
		assert(p == (const char *)"aaa" + 3);
	}
	/* jstr_memchrinv */
	{
		const char *p = (const char *)jstr_memchrinv("aaab", 'a', 4);
		assert(*p == 'b');
		p = (const char *)jstr_memchrinv("aaa", 'a', 3);
		assert(p == NULL);
	}
	/* jstr_memrchrinv */
	{
		const char *p = (const char *)jstr_memrchrinv("aaab", 'b', 4);
		assert(p != NULL && *p == 'b');
		p = (const char *)jstr_memrchrinv("aaaa", 'b', 4);
		assert(p == NULL);
	}
	/* jstr_strrchrinv */
	{
		const char *p = (const char *)jstr_strrchrinv("hello", 'o');
		assert(p != NULL && *p == 'o');
		p = (const char *)jstr_strrchrinv("hello", 'x');
		assert(p == NULL);
	}
	/* jstr_linestart */
	{
		const char s[] = "abc\ndef\nghi";
		const char *p = jstr_linestart(s + 4, s + 7);
		assert(p == s + 4);
		p = jstr_linestart(s, s + 3);
		assert(p == s);
	}
	/* jstr_linenext_len */
	{
		const char s[] = "abc\ndef\nghi";
		const char *p = jstr_linenext_len(s, s + 11);
		assert(p == s + 4);
		p = jstr_linenext_len(s, s + 3);
		assert(p == NULL);
	}
	/* jstr_linenext */
	{
		const char *p = jstr_linenext("abc\ndef");
		assert(p != NULL && *p == 'd');
		p = jstr_linenext("abc");
		assert(p == NULL);
		p = jstr_linenext("abc\n");
		assert(p == NULL);
	}
	/* jstr_linenextnul_len */
	{
		const char s[] = "abc\ndef";
		const char *p = jstr_linenextnul_len(s, s + 7);
		assert(p == s + 4);
		p = jstr_linenextnul_len(s, s + 3);
		assert(p == s + 3);
	}
	/* jstr_linenextnul */
	{
		const char *p = jstr_linenextnul("abc\ndef");
		assert(p != NULL && *p == 'd');
		p = jstr_linenextnul("abc");
		assert(p != NULL && *p == '\0');
		p = jstr_linenextnul("abc\n");
		assert(p != NULL && *p == '\n');
	}
	/* jstr_trimstart (void version) */
	{
		char buf[] = "   hello";
		jstr_trimstart(buf);
		assert(!strcmp(buf, "hello"));
	}
	{
		char buf[] = "";
		jstr_trimstart(buf);
		assert(!strcmp(buf, ""));
	}
	/* jstr_repeat_len_unsafe_p edge cases */
	{
		char buf[32] = "AB";
		char *end = jstr_repeat_len_unsafe_p(buf, 2, 1);
		assert(end == buf + 2);
		assert(!strcmp(buf, "AB"));
	}
	{
		char buf[32] = "AB";
		char *end = jstr_repeat_len_unsafe_p(buf, 2, 3);
		assert(end == buf + 6);
		assert(!strcmp(buf, "ABABAB"));
	}
	{
		char buf[32] = "A";
		char *end = jstr_repeat_len_unsafe_p(buf, 1, 3);
		assert(end == buf + 3);
		assert(!strcmp(buf, "AAA"));
	}
	/* jstr_repeatcpy_len_p */
	{
		char buf[32];
		char *end = jstr_repeatcpy_len_p(buf, "ab", 2, 3);
		assert(end == buf + 6);
		assert(!strcmp(buf, "ababab"));
	}
	{
		char buf[32];
		char *end = jstr_repeatcpy_len_p(buf, "x", 1, 5);
		assert(end == buf + 5);
		assert(!strcmp(buf, "xxxxx"));
	}
	/* jstr_strtok_ne_len */
	{
		const char *save = "a,b,c,d";
		const char *end = save + 7;
		const char *tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "a", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "b", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "c", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "d", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok == NULL);
	}
	/* jstr_strtok_ne */
	{
		const char *save = "a,b,c,d";
		const char *tok = jstr_strtok_ne(&save, ",");
		assert(tok != NULL && !strncmp(tok, "a", 1));
		tok = jstr_strtok_ne(&save, ",");
		assert(tok != NULL && !strncmp(tok, "b", 1));
		tok = jstr_strtok_ne(&save, ",");
		assert(tok != NULL && !strncmp(tok, "c", 1));
		tok = jstr_strtok_ne(&save, ",");
		assert(tok != NULL && !strncmp(tok, "d", 1));
		tok = jstr_strtok_ne(&save, ",");
		assert(tok == NULL);
	}
	/* jstr_strtok */
	{
		const char *save = "a,b,c,d";
		const char *tok = jstr_strtok(&save, ",");
		assert(tok != NULL && !strncmp(tok, "a", 1));
		tok = jstr_strtok(&save, ",");
		assert(tok != NULL && !strncmp(tok, "b", 1));
		tok = jstr_strtok(&save, ",");
		assert(tok != NULL && !strncmp(tok, "c", 1));
		tok = jstr_strtok(&save, ",");
		assert(tok != NULL && !strncmp(tok, "d", 1));
		tok = jstr_strtok(&save, ",");
		assert(tok == NULL);
	}
	/* jstr_thousepcpy_len_p edge cases */
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "", 0, ',');
		assert(end == buf);
		assert(*buf == '\0');
	}
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "123", 3, ',');
		assert(end == buf + 3);
		assert(!strcmp(buf, "123"));
	}
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "1234", 4, ',');
		assert(!strcmp(buf, "1,234"));
		assert(end == buf + 5);
	}
	/* jstr_trimend_len_p sz==0 path */
	{
		char buf[] = "";
		char *end = jstr_trimend_len_p(buf, 0);
		assert(end == buf);
	}
	/* jstr_trimstart_len_p sz==0 path */
	{
		char buf[] = "";
		char *end = jstr_trimstart_len_p(buf, 0);
		assert(end == buf);
	}
	/* jstr_trimstart_p *s=='\0' path */
	{
		char buf[] = "";
		char *end = jstr_trimstart_p(buf);
		assert(end == buf);
	}
	/* jstr_count_len_exec find_len==0 path */
	{
		jstr_twoway_ty tw;
		jstr_memmem_comp(&tw, "x", 1);
		size_t n = jstr_count_len_exec(&tw, "hello", 5, "", 0);
		assert(n == 0);
	}
	/* jstr_count find[0]=='\0' path */
	{
		size_t n = jstr_count("hello", "");
		assert(n == 0);
	}
	/* jstr_count *s=='\0' path */
	{
		size_t n = jstr_count("", "abc");
		assert(n == 0);
	}
	/* jstr_strcasestr_len ne_len==0 path */
	{
		const char *p = jstr_strcasestr_len("hello", 5, "", 0);
		assert(p == (const char *)"hello");
	}
	/* jstr_strnlen basic sanity */
	{
		assert(jstr_strnlen("hello world", 5) == 5);
		assert(jstr_strnlen("hello world", 50) == 11);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}
