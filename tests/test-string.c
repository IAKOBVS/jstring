#include "test.h"
#include "test-array.h"
#include "../src/jstr-string.h"

static char *
simple_strrstr_len(const char *h,
                   size_t hl,
                   const char *n,
                   size_t nl)
{
	if (nl == 0)
		return (char *)h + hl;
	if (hl < nl)
		return NULL;
	const char *p = h + hl - nl;
	hl -= (nl - 1);
	for (; hl--; --p)
		if (*p == *n && !memcmp(p, n, nl))
			return (char *)p;
	return NULL;
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strrstr(const char *h,
               const char *n)
{
#if JSTR_HAVE_STRSTR && !JSTR_USE_SIMPLE
	return strstr(h, n);
#else
	return simple_strrstr_len(h, strlen(h), n, strlen(n));
#endif
}

static char *
simple_memmem(const void *hs,
              const size_t hl,
              const void *ne,
              const size_t nl)
{
#if JSTR_HAVE_MEMMEM && !JSTR_USE_SIMPLE
	return memmem(hs, hl, ne, nl);
#else
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const unsigned char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (*h == *n && !memcmp(h, n, nl))
			return (char *)h;
	return NULL;
#endif
}

static char *
simple_strnstr(const char *hs,
               const char *ne,
               size_t n)
{
	return (char *)simple_memmem(hs, jstr_strnlen(hs, n), ne, strlen(ne));
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strstr(const char *h,
              const char *n)
{
#if JSTR_HAVE_STRSTR && !JSTR_USE_SIMPLE
	return strstr(h, n);
#else
	return simple_memmem(h, strlen(h), n, strlen(n));
#endif
}

static int
simple_strcasecmp(const char *s1,
                  const char *s2)
{
#if JSTR_HAVE_STRCASECMP && !JSTR_USE_SIMPLE
	return strcasecmp(s1, s2);
#else
	int ret;
	for (; (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1; ++s1, ++s2)
		;
	return ret;
#endif
}

static int
simple_strncasecmp(const char *s1,
                   const char *s2,
                   size_t n)
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_USE_SIMPLE
	return strncasecmp(s1, s2, n);
#else
	if (n == 0)
		return 0;
	int ret;
	for (; n && (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1; --n, ++s1, ++s2)
		;
	if (n == 0)
		return 0;
	return ret;
#endif
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_strcasestr_len(const char *h,
                      const size_t hl,
                      const char *n,
                      const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl < nl)
		return NULL;
	const char *end = h + hl - nl;
	for (; h <= end; ++h)
		if (!simple_strncasecmp(h, n, nl))
			return (char *)h;
	return NULL;
}

static char *
simple_strcasestr(const char *h,
                  const char *n)
{
#if JSTR_HAVE_STRCASESTR && !JSTR_USE_SIMPLE
	return strcasestr(h, n);
#else
	return simple_strcasestr_len(h, strlen(h), n, strlen(n));
#endif
}

static char *
simple_strncasestr(const char *hs,
                   const char *ne,
                   size_t n)
{
	return (char *)simple_strcasestr_len(hs, jstr_strnlen(hs, n), ne, strlen(ne));
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_stpcpy(char *d,
              const char *s)
{
#if JSTR_HAVE_STPCPY && !JSTR_USE_SIMPLE
	return stpcpy(d, s);
#else
	const size_t len = strlen(s);
	*((char *)memcpy(d, s, len) + len) = '\0';
	return d + len;
#endif
}

static void
T_DEBUG(const char *func, const char *hs, const char *ne, size_t hs_len, size_t ne_len, size_t n, const char *result, const char *expected)
{
	if (jstr_unlikely(result != expected)) {
		size_t hl = hs_len, nl = ne_len;
		if (hl == 0)
			hl = strlen(hs);
		if (nl == 0)
			nl = strlen(ne);
		PRINTERR("hsn:\n");
		PRINTERR("hs:\n");
		fwrite(hs, 1, hl, stderr);
		fputc('\n', stderr);
		PRINTERR("ne:\n");
		fwrite(ne, 1, nl, stderr);
		fputc('\n', stderr);
		fwrite(hs, 1, JSTR_MIN(n, hl), stderr);
		PRINTERR("\n");
		PRINTERR("hs_len:\n%zu\n", hl);
		PRINTERR("nl:\n%zu\n", nl);
		PRINTERR("n:\n%zu\n", (size_t)n);
		PRINTERR("expected:\n%s\n", EMPTY((char *)expected));
		PRINTERR("expected_len:\n%zu\n", strlen(EMPTY((char *)expected)));
		PRINTERR("result:\n%s\n", EMPTY((char *)result));
		PRINTERR("result_len:\n%zu\n", strlen(EMPTY((char *)result)));
		PRINTERR("%s\n", func);
		assert(result == expected);
	}
}

#define T_HS(test, i) ((test)[i].hs)
#define T_NE(test, i) ((test)[i].ne)
#define T_S1(test, i) ((test)[i].s2)
#define T_S2(test, i) ((test)[i].s2)

#define T_FOREACH_NE(needle, needle_len) for (const char *np = needle; needle_len != (size_t)-1; ++np, --needle_len)

#define T_CPY(fn, simple_fn, test_array)                                                  \
	do {                                                                              \
		TESTING(fn);                                                              \
		T_FOREACHI(test_array, i)                                                 \
		{                                                                         \
			size_t expected_len, result_len;                                  \
			const char *src;                                                  \
			src = T_HS(test_array, i);                                        \
			size_t src_len = 0;                                               \
			T_FOREACH_NE(src, src_len)                                        \
			{                                                                 \
				expected_len = JSTR_PTR_DIFF(fn(buf_r, src), buf_r);      \
				result_len = JSTR_PTR_DIFF(simple_fn(buf_e, src), buf_e); \
				if (strcmp(buf_e, buf_r) || result_len != expected_len) { \
					PRINTERR("expected_len:%zu\n"                     \
					         "%s\n",                                  \
					         expected_len,                            \
					         buf_e);                                  \
					PRINTERR("result_len:%zu\n"                       \
					         "%s\n",                                  \
					         result_len,                              \
					         buf_r);                                  \
					assert(!strcmp(buf_e, buf_r));                    \
					assert(result_len == expected_len);               \
				}                                                         \
				src = T_NE(test_array, i);                                \
				if (JSTR_TEST_SLOW == 0)                                  \
					break;                                            \
			}                                                                 \
		}                                                                         \
	} while (0)

#define T(fn, simple_fn, test_array)                                                  \
	do {                                                                          \
		TESTING(fn);                                                          \
		T_FOREACHI(test_array, i)                                             \
		{                                                                     \
			const char *hs = T_HS(test_array, i);                         \
			const char *ne = T_NE(test_array, i);                         \
			T_DEBUG(#fn, hs, ne, 0, 0, 0, fn(hs, ne), simple_fn(hs, ne)); \
		}                                                                     \
	} while (0)

#define T_LEN(fn, simple_fn, test_array)                                                                                                   \
	do {                                                                                                                               \
		TESTING(fn);                                                                                                               \
		T_FOREACHI(test_array, i)                                                                                                  \
		{                                                                                                                          \
			const char *hs = T_HS(test_array, i);                                                                              \
			const char *ne = T_NE(test_array, i);                                                                              \
			const size_t hs_len = strlen(hs);                                                                                  \
			size_t ne_len = strlen(ne);                                                                                        \
			T_FOREACH_NE(ne, ne_len)                                                                                           \
			{                                                                                                                  \
				T_DEBUG(#fn, hs, ne, hs_len, ne_len, 0, fn(hs, hs_len, ne, ne_len), simple_fn(hs, hs_len, ne, ne_len));    \
				if (i < hs_len)                                                                                            \
					T_DEBUG(#fn, hs, ne, i, ne_len, 0, fn(hs, hs_len, ne, ne_len), simple_fn(hs, hs_len, ne, ne_len)); \
				if (i < ne_len)                                                                                            \
					T_DEBUG(#fn, hs, ne, hs_len, i, 0, fn(hs, hs_len, ne, ne_len), simple_fn(hs, hs_len, ne, ne_len)); \
				if (JSTR_TEST_SLOW == 0)                                                                                   \
					break;                                                                                             \
			}                                                                                                                  \
		}                                                                                                                          \
	} while (0)

#define T_N(fn, simple_fn, test_array)                                                                                \
	do {                                                                                                          \
		TESTING(fn);                                                                                          \
		T_FOREACHI(test_array, i)                                                                             \
		{                                                                                                     \
			size_t n = strlen(test_array[i].hs);                                                          \
			const char *hs = T_HS(test_array, i);                                                         \
			const char *ne = T_NE(test_array, i);                                                         \
			const size_t hs_len = strlen(hs);                                                             \
			size_t ne_len = strlen(ne);                                                                   \
			T_FOREACH_NE(ne, ne_len)                                                                      \
			{                                                                                             \
				T_DEBUG(#fn, hs, np, hs_len, ne_len, n, fn(hs, np, n), simple_fn(hs, np, n));         \
				if (i < n)                                                                            \
					T_DEBUG(#fn, hs, np, hs_len, ne_len, i, fn(hs, np, n), simple_fn(hs, np, n)); \
				if (i < hs_len)                                                                       \
					T_DEBUG(#fn, hs, np, i, ne_len, n, fn(hs, np, n), simple_fn(hs, np, n));      \
				if (i < ne_len)                                                                       \
					T_DEBUG(#fn, hs, np, hs_len, i, n, fn(hs, np, n), simple_fn(hs, np, n));      \
				if (JSTR_TEST_SLOW == 0)                                                              \
					break;                                                                        \
			}                                                                                             \
		}                                                                                                     \
	} while (0)

#define T_CMP_LEN(fn, simple_fn, test_array)                                                                              \
	do {                                                                                                              \
		TESTING(fn);                                                                                              \
		T_FOREACHI(test_array, i)                                                                                 \
		{                                                                                                         \
			const char *s1 = T_S1(test_array, i);                                                             \
			const char *s2 = T_S2(test_array, i);                                                             \
			const size_t s1_len = strlen(s1);                                                                 \
			const size_t s2_len = strlen(s2);                                                                 \
			size_t n = JSTR_MIN(s1_len, JSTR_MIN(i, s2_len));                                                 \
			T_DEBUG(#fn, s1, s2, s1_len, s2_len, n, s1 + !fn(s1, s2, n), s1 + !simple_fn(s1, s2, n));         \
			if (i < n)                                                                                        \
				T_DEBUG(#fn, s1, s2, s1_len, s2_len, i, s1 + !fn(s1, s2, n), s1 + !simple_fn(s1, s2, n)); \
		}                                                                                                         \
	} while (0)

#define T_CMP(fn, simple_fn, test_array)                                                        \
	do {                                                                                    \
		TESTING(fn);                                                                    \
		T_FOREACHI(test_array, i)                                                       \
		{                                                                               \
			const char *s1 = T_S1(test_array, i);                                   \
			const char *s2 = T_S2(test_array, i);                                   \
			T_DEBUG(#fn, s1, s2, 0, 0, 0, s1 + fn(s1, s2), s1 + simple_fn(s1, s2)); \
		}                                                                               \
	} while (0)

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_revcpy_p(char *dst,
                const char *src)
{
	size_t src_len = strlen(src);
	src += src_len - 1;
	for (; src_len--; *dst++ = *src--) {}
	*dst = '\0';
	return dst;
}

static char buf_r[4096];
static char buf_e[4096];

int
main(int argc, char **argv)
{
	START();

/* 	T_CMP_LEN(!jstr_memcmpeq_loop, !memcmp, test_array_memcmp); */
/* 	T_CMP_LEN(jstr_strncasecmp, simple_strncasecmp, test_array_memcmp); */
/* 	T_CMP_LEN(!jstr_strcasecmpeq_len, !simple_strncasecmp, test_array_memcmp); */
/* 	T_CMP_LEN(!jstr_strcasecmpeq_len_loop, !simple_strncasecmp, test_array_memcmp); */
/* 	T_CMP(jstr_strcasecmp, simple_strcasecmp, test_array_memcmp); */
/* 	T_CMP(!jstr_strcasecmpeq, !simple_strcasecmp, test_array_memcmp); */
/* 	T_CMP(!jstr_strcasecmpeq_loop, !simple_strcasecmp, test_array_memcmp); */

/* 	T_CPY(jstr_revcpy_p, simple_revcpy_p, test_array_memmem); */

/* 	T(jstr_strcasestr, simple_strcasestr, test_array_memmem); */
/* 	T_N(jstr_strnstr, simple_strnstr, test_array_memmem); */

/* 	T_N(jstr_strncasestr, simple_strncasestr, test_array_memmem); */

	T_LEN(jstr_strcasestr_len, simple_strcasestr_len, test_array_memmem);

/* 	T_LEN(jstr_strrstr_len, simple_strrstr_len, test_array_memmem); */

/* 	T_LEN(jstr_memmem, simple_memmem, test_array_memmem); */
/* 	T_CPY(jstr_stpcpy, simple_stpcpy, test_array_memmem); */

#ifdef __AVX2__
	T_CPY(pjstr_stpcpy_avx2, simple_stpcpy, test_array_memmem);
	T_LEN(pjstr_memmem_avx2, simple_memmem, test_array_memmem);
	T_LEN(pjstr_strcasestr_len_avx2, simple_strcasestr_len, test_array_memmem);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
