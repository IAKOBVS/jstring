#define __STRICT_ANSI__

#include "../src/jstr.h"
#include <assert.h>

#define PRINT(fmt, ...) \
	fprintf(stderr, fmt, __VA_ARGS__)

#define SUCCESS() \
	PRINT("%s succeeded.\n", argv[0])

#define T_STRSTR(fn, simple_fn, ...)                               \
	do {                                                       \
		assert(fn(__VA_ARGS__) == simple_fn(__VA_ARGS__)); \
	} while (0)

#define T_FN(TEST_FN, fn, simple_fn)                         \
	do {                                                 \
		PRINT("Testing %s.\n", JSTR_STRINGIFY(fn));  \
		TEST_FN(fn, simple_fn, "yxxxyxxxxy", "xxx"); \
		TEST_FN(fn, simple_fn, "yxxxyxxy", "xxx");   \
		TEST_FN(fn, simple_fn, "xxx", "xxx");        \
		TEST_FN(fn, simple_fn, "xxx", "x");          \
		TEST_FN(fn, simple_fn, "xxx", "yyy");        \
		TEST_FN(fn, simple_fn, "x", "xxx");          \
		TEST_FN(fn, simple_fn, "xxx", "");           \
		TEST_FN(fn, simple_fn, "", "xxx");           \
		TEST_FN(fn, simple_fn, "", "");              \
	} while (0)

#define GET_LEN(x) x, strlen(x)

#define T_FN_MEM(TEST_FN, fn, simple_fn)                                       \
	do {                                                                   \
		PRINT("Testing %s.\n", JSTR_STRINGIFY(fn));                    \
		TEST_FN(fn, simple_fn, GET_LEN("yxxxyxxxxy"), GET_LEN("xxx")); \
		TEST_FN(fn, simple_fn, GET_LEN("yxxxyxxy"), GET_LEN("xxx"));   \
		TEST_FN(fn, simple_fn, GET_LEN("xxx"), GET_LEN("xxx"));        \
		TEST_FN(fn, simple_fn, GET_LEN("xxx"), GET_LEN("x"));          \
		TEST_FN(fn, simple_fn, GET_LEN("xxx"), GET_LEN("yyy"));        \
		TEST_FN(fn, simple_fn, GET_LEN("x"), GET_LEN("xxx"));          \
		TEST_FN(fn, simple_fn, GET_LEN("xxx"), GET_LEN(""));           \
		TEST_FN(fn, simple_fn, GET_LEN(""), GET_LEN("xxx"));           \
		TEST_FN(fn, simple_fn, GET_LEN(""), GET_LEN(""));              \
	} while (0)

#define TOLOWER(c) (unsigned char)(((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A' + 'a') : (c))
#define TOUPPER(c) (unsigned char)(((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 'A') : (c))

char *
simple_strrstr(const char *h, const char *n)
{
	const size_t hl = strlen(h);
	const size_t nl = strlen(n);
	if (hl < nl)
		return NULL;
	const char *p = h + hl - nl;
	for (; p >= h; --p)
		if (!memcmp(p, n, nl))
			return (char *)p;
	return NULL;
}

char *
simple_memmem(const char *h,
	      const size_t hl,
	      const char *n,
	      const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (!memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strstr(const char *h,
	      const char *n)
{
	return simple_memmem(h, strlen(h), n, strlen(n));
}

int
simple_strncasecmp(const char *s1,
		   const char *s2,
		   size_t n)
{
	if (n == 0)
		return 0;
	int ret;
	for (; (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1 && --n; ++s1, ++s2)
		;
	return ret;
}

char *
simple_strcasestr_len(const char *h,
		      const size_t hl,
		      const char *n,
		      const size_t nl)
{
	if (hl < nl)
		return NULL;
	const char *end = h + hl - nl;
	for (; h <= end; ++h)
		if (!simple_strncasecmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strcasestr(const char *h,
		  const char *n)
{
	return simple_strcasestr_len(h, strlen(h), n, strlen(n));
}

char *
jstr_strrstr(const char *h, const char *n)
{
	return jstr_strrstr_len(h, strlen(h), n, strlen(n));
}

int
main(int argc, char **argv)
{
	T_FN(T_STRSTR, jstr_strrstr, simple_strrstr);
	T_FN(T_STRSTR, jstr_strcasestr, simple_strcasestr);
	T_FN_MEM(T_STRSTR, jstr_memmem, simple_memmem);
	T_FN_MEM(T_STRSTR, jstr_strcasestr_len, simple_strcasestr_len);
	SUCCESS();
	return EXIT_SUCCESS;
}
