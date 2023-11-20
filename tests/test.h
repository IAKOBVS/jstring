#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#define __STRICT_ANSI__

#include "../src/jstr.h"
#include <assert.h>

#define PRINT(fmt, ...) \
	printf(fmt, __VA_ARGS__)

#define PRINTERR(fmt, ...) \
	fprintf(stdout, fmt, __VA_ARGS__)

#define SUCCESS() \
	PRINT("%s succeeded.\n", argv[0])

#define ASSERT(expr, result, expected)            \
	do {                                      \
		if (jstr_unlikely(!(expr))) {     \
			PRINTERR("result:%s\n"    \
				 "expected:%s\n", \
				 result,          \
				 expected);       \
			assert(expr);             \
		}                                 \
	} while (0)

#define TESTING(func) PRINT("Testing %s.\n", JSTR_STRING(func) "()")

#endif /* JSTR_TEST_H */
