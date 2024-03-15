#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#define JSTR_PANIC 1
#define JSTR_TEST  1
#define JSTR_DEBUG 1
/* Force use of slower simple functions. */
#ifndef JSTR_USE_SIMPLE
#	define JSTR_USE_SIMPLE 0
#endif
/* Do more tests. */
#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

#define JSTR_BUILT

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

#include "../src/jstr-macros.h"
#include "simple.h"

#define SUCCESS()                                                   \
	do {                                                        \
		(void)argc;                                         \
		(void)argv;                                         \
		PRINT("%s ", __FILE__);                             \
		for (; *argv; ++argv)                               \
			if (!strncmp(*argv, "-D", 2))               \
				PRINT("%s ", *argv + strlen("-D")); \
		PRINT("succeeded.\n");                              \
	} while (0)

/* clang-format off */
#define EMPTY(p)          (sizeof(p) == sizeof(const char *) ? ((const char *)p) == (const char *)NULL ? "(null)" : (p) : (p))
#define PRINT(...)    printf(__VA_ARGS__)
#define PRINTERR(...) fprintf(stderr, __VA_ARGS__)
#define START() do{}while(0)

JSTR__BEGIN_DECLS

JSTR_ATTR_MAYBE_UNUSED
JSTR_ATTR_INLINE
static char *clean_func(const char *func)
{
	for (; !isalpha(*func); ++func);
	return (char *)func;
}

JSTR__END_DECLS

#if VERBOSE
#	define TESTING(func) PRINTERR("Testing %s().\n", clean_func(#func))
#else
#	define TESTING(func)
#endif
/* clang-format on */

#define ASSERT_RESULT(func, expr, result, expected)                      \
	do {                                                             \
		if (jstr_unlikely(!(expr))) {                            \
			PRINTERR("Assertion failure: %s().\nResult:%s\n" \
			         "Expected:%s\n",                        \
			         #func,                                  \
			         EMPTY(result),                          \
			         EMPTY(expected));                       \
			assert(expr);                                    \
		}                                                        \
	} while (0)

#define ASSERT_ERRFUNC(func, expr)                                     \
	do {                                                           \
		if (jstr_unlikely(!(expr))) {                          \
			PRINTERR("Assertion failure: %s().\n", #func); \
			assert(expr);                                  \
		}                                                      \
	} while (0)

#define P(fmt, x)                      \
	do {                           \
		PRINTERR("%s:", #x);   \
		PRINTERR(fmt "\n", x); \
	} while (0)

#define PS(x) P("%s", x)
#define PZ(x) P("%zu", x)

typedef struct test_ty {
	const char *hs;
	size_t hs_len;
	const char *ne;
	size_t ne_len;
	const char *s1;
	size_t s1_len;
	const char *s2;
	size_t s2_len;
	const char *result;
	size_t result_len;
	const char *expected;
	size_t expected_len;
	size_t n;
	size_t result_n;
	size_t expected_n;
	const char *src;
	size_t src_len;
	const char *dst;
	size_t dst_len;
} test_ty;
static test_ty t;

JSTR_ATTR_MAYBE_UNUSED
static void
t_debug(void)
{
	if (!t.hs_len && t.hs)
		t.hs_len = strlen(t.hs);
	if (!t.ne_len && t.ne)
		t.ne_len = strlen(t.ne);
	if (!t.result_len && t.result)
		t.result_len = strlen(t.result);
	if (!t.expected_len && t.expected)
		t.expected_len = strlen(t.expected);
	PS(t.hs);
	PZ(t.hs_len);
	PS(t.ne);
	PZ(t.ne_len);
	PZ(t.result_n);
	PZ(t.expected_n);
	PS(t.result);
	PZ(t.result_len);
	PS(t.expected);
	PZ(t.expected_len);
	PS(t.s1);
	PZ(t.s1_len);
	PS(t.s2);
	PZ(t.s2_len);
}

JSTR_ATTR_MAYBE_UNUSED
static void
t_init(void)
{
	memset(&t, 0, sizeof(t));
}

#define T_DEBUG(func, expr)           \
	do {                          \
		if (!(expr)) {        \
			PS(#func);    \
			t_debug();    \
			assert(expr); \
		}                     \
	} while (0)

#define T_HS(test, i) ((test)[i].hs)
#define T_NE(test, i) ((test)[i].ne)
#define T_S1(test, i) ((test)[i].s2)
#define T_S2(test, i) ((test)[i].s2)

#define T_FOREACH_NE(needle, needle_len) for (; needle_len != (size_t)-1; ++needle, --needle_len)

#define T_STRCPY(fn, simple_fn, test_array)                                                   \
	do {                                                                                  \
		char buf_r[4096];                                                             \
		char buf_e[4096];                                                             \
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

#define T_STRSTR(fn, simple_fn, test_array)                  \
	do {                                                 \
		TESTING(fn);                                 \
		T_FOREACHI(test_array, i)                    \
		{                                            \
			t_init();                            \
			t.hs = T_HS(test_array, i);          \
			t.ne = T_NE(test_array, i);          \
			t.result = fn(t.hs, t.ne);           \
			t.expected = simple_fn(t.hs, t.ne);  \
			T_DEBUG(fn, t.result == t.expected); \
		}                                            \
	} while (0)

#define T_MEMMEM(fn, simple_fn, test_array)                                              \
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

#define T_STRNSTR(fn, simple_fn, test_array)                             \
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

#define T_MEMCMP(fn, simple_fn, test_array)                                      \
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

#define T_STRCMP(fn, simple_fn, test_array)                           \
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

#endif /* JSTR_TEST_H */
