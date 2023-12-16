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

PJSTR_BEGIN_DECLS

JSTR_ATTR_MAYBE_UNUSED
static int TOLOWER(int c)
{
	c = (char)c;
	return (unsigned char)(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c);
}

JSTR_ATTR_MAYBE_UNUSED
static int TOUPPER(int c)
{
	c = (char)c;
	return (unsigned char)(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
}

JSTR_ATTR_MAYBE_UNUSED
JSTR_ATTR_INLINE
static char *clean_func(const char *func)
{
	for (; !isalpha(*func); ++func);
	return (char *)func;
}

PJSTR_END_DECLS

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
		PRINTERR("%s:", #fmt); \
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
	PS(t.hs);
	PZ(t.hs_len);
	PS(t.ne);
	PZ(t.ne_len);
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
t_init()
{
	t = (test_ty) { 0 };
}

#define T_DEBUG(func, expr)           \
	do {                          \
		if (!(expr)) {        \
			PS(#func);    \
			t_debug();    \
			assert(expr); \
		}                     \
	} while (0)

#endif /* JSTR_TEST_H */
