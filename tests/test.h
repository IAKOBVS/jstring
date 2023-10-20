#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#include "../src/jstr.h"
#include <assert.h>

#define PRINT(fmt, ...) \
	fprintf(stderr, fmt, __VA_ARGS__)

#define SUCCESS() \
	PRINT("%s succeeded.\n", argv[0])

#endif /* JSTR_TEST_H */
