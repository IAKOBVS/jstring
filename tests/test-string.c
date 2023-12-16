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

#define T_HS(test, i) ((test)[i].hs)
#define T_NE(test, i) ((test)[i].ne)
#define T_S1(test, i) ((test)[i].s2)
#define T_S2(test, i) ((test)[i].s2)

#define T_FOREACH_NE(needle, needle_len) for (const char *np = needle; needle_len != (size_t)-1; ++np, --needle_len)

#define T_CPY(fn, simple_fn, test_array)                                                      \
	do {                                                                                  \
		TESTING(fn);                                                                  \
		T_FOREACHI(test_array, i)                                                     \
		{                                                                             \
			t_init();                                                             \
			t.src = T_HS(test_array, i);                                          \
			t.src_len = 0;                                                        \
			T_FOREACH_NE(t.src, t.src_len)                                        \
			{                                                                     \
				t.expected_len = JSTR_PTR_DIFF(fn(buf_r, t.src), buf_r);      \
				t.result_len = JSTR_PTR_DIFF(simple_fn(buf_e, t.src), buf_e); \
				T_DEBUG(fn, !strcmp(buf_e, buf_r));                           \
				T_DEBUG(fn, t.result_len == t.expected_len);                  \
				if (JSTR_TEST_SLOW == 0)                                      \
					break;                                                \
			}                                                                     \
		}                                                                             \
	} while (0)

#define T(fn, simple_fn, test_array)                                          \
	do {                                                                  \
		TESTING(fn);                                                  \
		T_FOREACHI(test_array, i)                                     \
		{                                                             \
			t_init();                                             \
			t.hs = T_HS(test_array, i);                           \
			t.ne = T_NE(test_array, i);                           \
			T_DEBUG(fn, fn(t.hs, t.ne) == simple_fn(t.hs, t.ne)); \
		}                                                             \
	} while (0)

#define T_LEN(fn, simple_fn, test_array)                                                 \
	do {                                                                             \
		TESTING(fn);                                                             \
		T_FOREACHI(test_array, i)                                                \
		{                                                                        \
			t_init();                                                        \
			t.hs = T_HS(test_array, i);                                      \
			t.ne = T_NE(test_array, i);                                      \
			t.hs_len = strlen(t.hs);                                         \
			t.ne_len = strlen(t.ne);                                         \
			T_FOREACH_NE(t.ne, t.ne_len)                                     \
			{                                                                \
				t.result = fn(t.hs, t.hs_len, t.ne, t.ne_len);           \
				t.expected = simple_fn(t.hs, t.hs_len, t.ne, t.ne_len);  \
				T_DEBUG(fn, t.result == t.expected);                     \
				if (i < t.hs_len) {                                      \
					t.result = simple_fn(t.hs, i, t.ne, t.ne_len);   \
					t.expected = fn(t.hs, i, t.ne, t.ne_len);        \
					T_DEBUG(fn, t.result == t.expected);             \
				}                                                        \
				if (i < t.ne_len) {                                      \
					t.result = fn(t.hs, t.hs_len, t.ne, i);          \
					t.expected = simple_fn(t.hs, t.hs_len, t.ne, i); \
					T_DEBUG(fn, t.result == t.expected);             \
				}                                                        \
				if (JSTR_TEST_SLOW == 0)                                 \
					break;                                           \
			}                                                                \
		}                                                                        \
	} while (0)

#define T_N(fn, simple_fn, test_array)                                   \
	do {                                                             \
		TESTING(fn);                                             \
		T_FOREACHI(test_array, i)                                \
		{                                                        \
			t_init();                                        \
			t.n = strlen(test_array[i].hs);                  \
			t.hs = T_HS(test_array, i);                      \
			t.ne = T_NE(test_array, i);                      \
			t.ne_len = strlen(t.ne);                         \
			T_FOREACH_NE(t.ne, t.ne_len)                     \
			{                                                \
				t.result = fn(t.hs, t.ne, t.n);          \
				t.expected = simple_fn(t.hs, t.ne, t.n); \
				T_DEBUG(fn, t.result == t.expected);     \
				if (JSTR_TEST_SLOW == 0)                 \
					break;                           \
			}                                                \
		}                                                        \
	} while (0)

#define T_CMP_LEN(fn, simple_fn, test_array)                                     \
	do {                                                                     \
		TESTING(fn);                                                     \
		T_FOREACHI(test_array, i)                                        \
		{                                                                \
			t_init();                                                \
			t.s1 = T_S1(test_array, i);                              \
			t.s2 = T_S2(test_array, i);                              \
			t.s1_len = strlen(t.s1);                                 \
			t.s2_len = strlen(t.s2);                                 \
			t.n = JSTR_MIN(t.s1_len, JSTR_MIN(i, t.s2_len));         \
			t.result_n = (size_t)fn(t.s1, t.s2, t.n);                \
			t.expected_n = (size_t)simple_fn(t.s1, t.s2, t.n);       \
			T_DEBUG(fn, t.result_n == t.expected_n);                 \
			if (i < t.n) {                                           \
				t.result_n = (size_t)fn(t.s1, t.s2, i);          \
				t.expected_n = (size_t)simple_fn(t.s1, t.s2, i); \
				T_DEBUG(fn, t.result_n == t.expected_n);         \
			}                                                        \
		}                                                                \
	} while (0)

#define T_CMP(fn, simple_fn, test_array)                              \
	do {                                                          \
		TESTING(fn);                                          \
		T_FOREACHI(test_array, i)                             \
		{                                                     \
			t_init();                                     \
			t.s1 = T_S1(test_array, i);                   \
			t.s2 = T_S2(test_array, i);                   \
			t.result_n = (size_t)fn(t.s1, t.s2);          \
			t.expected_n = (size_t)simple_fn(t.s1, t.s2); \
			T_DEBUG(fn, t.expected_n == t.result_n);      \
		}                                                     \
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

	T(jstr_strcasestr, simple_strcasestr, test_array_memmem);
	T_LEN(jstr_strcasestr_len, simple_strcasestr_len, test_array_memmem);

	T_N(jstr_strnstr, simple_strnstr, test_array_memmem);
	T_N(jstr_strncasestr, simple_strncasestr, test_array_memmem);
	T_LEN(jstr_memmem, simple_memmem, test_array_memmem);
	T_CPY(jstr_stpcpy, simple_stpcpy, test_array_memmem);

	T_LEN(jstr_strrstr_len, simple_strrstr_len, test_array_memmem);

	T_CMP_LEN(!jstr_memcmpeq_loop, !memcmp, test_array_memcmp);
	T_CMP_LEN(jstr_strncasecmp, simple_strncasecmp, test_array_memcmp);
	T_CMP_LEN(!jstr_strcasecmpeq_len, !simple_strncasecmp, test_array_memcmp);
	T_CMP_LEN(!jstr_strcasecmpeq_len_loop, !simple_strncasecmp, test_array_memcmp);
	T_CMP(jstr_strcasecmp, simple_strcasecmp, test_array_memcmp);
	T_CMP(!jstr_strcasecmpeq, !simple_strcasecmp, test_array_memcmp);
	T_CMP(!jstr_strcasecmpeq_loop, !simple_strcasecmp, test_array_memcmp);

	T_CPY(jstr_revcpy_p, simple_revcpy_p, test_array_memmem);

	SUCCESS();
	return EXIT_SUCCESS;
}
