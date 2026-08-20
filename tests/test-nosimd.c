#define _POSIX_C_SOURCE 200809L
#include "jstr.h"
#include "test.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* Helper: allocate a buffer at a known alignment offset. */
static char *
alloc_buf(size_t extra)
{
	char *buf = (char *)malloc(64 + extra);
	assert(buf != NULL);
	memset(buf, 0, 64 + extra);
	return buf;
}

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	/* Test jstr_memrchr (exercises jstr_internal_memrchr_musl). */
	{
		const char *s = "hello world!";
		void *p = jstr_memrchr(s, 'o', 12);
		assert(p == s + 7);
		p = jstr_memrchr(s, 'x', 12);
		assert(p == NULL);
		p = jstr_memrchr(s, '!', 12);
		assert(p == s + 11);
		/* unaligned buffers */
		char *buf = alloc_buf(1);
		memcpy(buf + 1, "abcdefghijklmnopqrstuvwxyz", 26);
		p = jstr_memrchr(buf + 1, 'z', 26);
		assert(p == buf + 1 + 25);
		p = jstr_memrchr(buf + 1, 'a', 26);
		assert(p == buf + 1 + 0);
		/* '0' not in the alphabet string */
		p = jstr_memrchr(buf + 1, '0', 25);
		assert(p == NULL);
		free(buf);
		/* single element */
		p = jstr_memrchr("a", 'a', 1);
		assert(p == (void *)"a");
		p = jstr_memrchr("a", 'b', 1);
		assert(p == NULL);
		/* n == 0 */
		p = jstr_memrchr("hello", 'h', 0);
		assert(p == NULL);
	}

	/* Test jstr_strcasechrnul (exercises jstr_internal_strcasechrnul_musl). */
	{
		const char *s = "ABCdef";
		char *p = jstr_strcasechrnul(s, 'd');
		assert(p == s + 3);
		p = jstr_strcasechrnul(s, 'z');
		assert(*p == '\0');
		/* unaligned */
		char *buf = alloc_buf(1);
		memcpy(buf + 1, "aBcDeF", 6);
		p = jstr_strcasechrnul(buf + 1, 'd');
		assert(p == buf + 1 + 3);
		free(buf);
		/* first char matches at start */
		p = jstr_strcasechrnul("Hello", 'h');
		assert(p == (void *)"Hello");
		/* non-alpha char */
		p = jstr_strcasechrnul("hello world", ' ');
		assert(p == (void *)"hello world" + 5);
	}

	/* Test jstr_strcasechr (also uses strcasechrnul). */
	{
		const char *p = jstr_strcasechr("ABCdef", 'd');
		assert(p == (void *)"ABCdef" + 3);
		p = jstr_strcasechr("ABCdef", 'z');
		assert(p == NULL);
	}

	/* Test jstr_memcasechr (exercises jstr_internal_memcasechr_musl). */
	{
		const char *s = "ABCdef";
		void *p = jstr_memcasechr(s, 'd', 6);
		assert(p == s + 3);
		p = jstr_memcasechr(s, 'z', 6);
		assert(p == NULL);
		/* unaligned, short n */
		char *buf = alloc_buf(1);
		memcpy(buf + 1, "aBc", 3);
		p = jstr_memcasechr(buf + 1, 'b', 3);
		assert(p == buf + 1 + 1);
		free(buf);
		/* n < sizeof(size_t) */
		p = jstr_memcasechr("A", 'a', 1);
		assert(p == (void *)"A");
	}

	/* Test jstr_strnchr (exercises jstr_internal_strnchr_musl). */
	{
		const char *s = "hello world";
		char *p = jstr_strnchr(s, 'w', 11);
		assert(p == s + 6);
		p = jstr_strnchr(s, 'z', 11);
		assert(p == NULL);
		p = jstr_strnchr(s, 'o', 5);
		assert(p == s + 4);
		p = jstr_strnchr(s, 'o', 4);
		assert(p == NULL);
		/* unaligned */
		char *buf = alloc_buf(1);
		memcpy(buf + 1, "xxyzxx", 6);
		/* "xxyzxx" -> 'z' is at index 3 */
		p = jstr_strnchr(buf + 1, 'z', 6);
		assert(p == buf + 1 + 3);
		free(buf);
	}

	/* Test jstr_strncasechr (exercises jstr_internal_strncasechr_musl). */
	{
		const char *s = "Hello World";
		char *p = jstr_strncasechr(s, 'w', 11);
		assert(p == s + 6);
		p = jstr_strncasechr(s, 'z', 11);
		assert(p == NULL);
		/* unaligned */
		char *buf = alloc_buf(1);
		memcpy(buf + 1, "aBcDe", 5);
		p = jstr_strncasechr(buf + 1, 'd', 5);
		assert(p == buf + 1 + 3);
		free(buf);
		/* non-alpha */
		p = jstr_strncasechr("abc def", ' ', 7);
		assert(p == (void *)"abc def" + 3);
	}

	/* Test jstr_strcasestr_len. */
	{
		const char *p = jstr_strcasestr_len("hello", 5, "", 0);
		assert(p == (const char *)"hello");
		p = jstr_strcasestr_len("Hello World", 11, "world", 5);
		assert(p != NULL);
		p = jstr_strcasestr_len("Hello World", 11, "xyz", 3);
		assert(p == NULL);
		p = jstr_strcasestr_len("aBcDe", 5, "cd", 2);
		assert(p != NULL);
	}

	/* Test jstr_memrmem. */
	{
		const char *hs = "ababa";
		void *p = jstr_memrmem(hs, 5, "ba", 2);
		assert(p != NULL);
		p = jstr_memrmem(hs, 5, "aba", 3);
		assert(p != NULL);
		const char *hs2 = "xxabcdefghixx";
		p = jstr_memrmem(hs2, 13, "abcdefghi", 9);
		assert(p != NULL);
		assert(memcmp(p, "abcdefghi", 9) == 0);
		const char *hs3 = "xxabcdefghxx";
		p = jstr_memrmem(hs3, 12, "abcdefgh", 8);
		assert(p != NULL);
		assert(memcmp(p, "abcdefgh", 8) == 0);
		p = jstr_memrmem("hello", 5, "x", 1);
		assert(p == NULL);
		/* ne_len == 0 */
		p = jstr_memrmem("hello", 5, "", 0);
		assert(p == (void *)"hello" + 5);
	}

	/* Test jstr_memmem. */
	{
		const char *hs = "abcde";
		void *p = jstr_memmem(hs, 5, "cd", 2);
		assert(p == hs + 2);
		p = jstr_memmem(hs, 5, "cde", 3);
		assert(p == hs + 2);
		p = jstr_memmem(hs, 5, "bcde", 4);
		assert(p == hs + 1);
		p = jstr_memmem(hs, 5, "xyz", 3);
		assert(p == NULL);
		p = jstr_memmem("ab\0cd", 5, "cd", 2);
		assert(p == (const char *)"ab\0cd" + 3);
		/* ne_len == 0 */
		p = jstr_memmem("hello", 5, "", 0);
		assert(p == (void *)"hello");
		/* ne_len == 1 */
		p = jstr_memmem("hello", 5, "e", 1);
		assert(p == (void *)"hello" + 1);
	}

	/* Test jstr_strstr_len. */
	{
		const char *p = jstr_strstr_len("hello world", 11, "world", 5);
		assert(p != NULL);
		assert(strcmp(p, "world") == 0);
		p = jstr_strstr_len("hello world", 11, "xyz", 3);
		assert(p == NULL);
		/* ne_len == 0 */
		p = jstr_strstr_len("hello", 5, "", 0);
		assert(p == (void *)"hello");
	}

	/* Test jstr_strrstr_len. */
	{
		const char *p = jstr_strrstr_len("ababa", 5, "aba", 3);
		assert(p != NULL);
		p = jstr_strrstr_len("hello", 5, "x", 1);
		assert(p == NULL);
	}

	/* Test jstr_strrcspn_len (memrcspn_musl).
	 * Returns (rightmost_match_index + 1), or 0 if none found. */
	{
		/* "hello world": rightmost ' ' is at index 5 → 6 */
		size_t n = jstr_strrcspn_len("hello world", " ", 11);
		assert(n == 6);
		/* "hello": no 'xyz' → 0 */
		n = jstr_strrcspn_len("hello", "xyz", 5);
		assert(n == 0);
		/* "hello": rightmost 'l' at index 3 → 4 */
		n = jstr_strrcspn_len("hello", "l", 5);
		assert(n == 4);
		/* empty reject returns n */
		n = jstr_strrcspn_len("hello", "", 5);
		assert(n == 5);
	}

	/* Test jstr_strrspn_len (memrspn_musl).
	 * Returns (rightmost_nonmatch_index + 1), or n if all match. */
	{
		/* "hello   ": all 3 trailing spaces match → rightmost non-match is 'o' at index 4 → 5 */
		size_t n = jstr_strrspn_len("hello   ", " ", 8);
		assert(n == 5);
		/* "hello": 'x' doesn't match → all 5 don't match → 5 */
		n = jstr_strrspn_len("hello", "x", 5);
		assert(n == 5);
		/* "ababa": rightmost non-'a' is 'b' at index 3 → 4 */
		n = jstr_strrspn_len("ababa", "a", 5);
		assert(n == 4);
	}

	/* Test jstr_strrcspn (no-len wrapper). */
	{
		size_t n = jstr_strrcspn("hello world", " ");
		assert(n == 6);
	}

	/* Test jstr_strrspn (no-len wrapper). */
	{
		size_t n = jstr_strrspn("hello   ", " ");
		assert(n == 5);
	}

	SUCCESS();
	return EXIT_SUCCESS;
}
