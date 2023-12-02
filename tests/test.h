#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#define JSTR_PANIC 1
#define JSTR_TEST  1
#define JSTR_DEBUG 1

#define JSTR_BUILT
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "../jstr/jstr-macros.h"

/* clang-format off */
#define EMPTY(p)          (sizeof(p) == sizeof(const char *) ? ((const char *)p) == (const char *)NULL ? "(null)" : (p) : (p))
#define PRINT(...)    printf(__VA_ARGS__)
#define PRINTERR(...) fprintf(stderr, __VA_ARGS__)
#define START() do{}while(0)
#define SUCCESS()     PRINT("%s succeeded.\n", strstr(argv[0], "test-"))

PJSTR_BEGIN_DECLS
JSTR_ATTR_MAYBE_UNUSED
JSTR_ATTR_INLINE
char *clean_func(const char *func)
{
	for (; !isalpha(*func); ++func);
	return (char *)func;
}
PJSTR_END_DECLS

#if VERBOSE
#	define TESTING(func) PRINT("Testing %s().\n", clean_func(#func))
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
			         result,                                 \
			         expected);                              \
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

#endif /* JSTR_TEST_H */
