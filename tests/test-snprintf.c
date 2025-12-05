/* Modified version of functional/snprintf.c taken from musl's libc-test
 *
 * libc-test is licensed under the following standard MIT license:
 *
 * Copyright © 2005-2013 libc-test AUTHORS
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef _XOPEN_SOURCE
#	define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>
#include "test.h"
#include "../src/jstr-builder.h"

/* clang-format off */

static const struct {
	const char *fmt;
	int i;
	const char *expect;
} int_tests[] = {
	/* width, precision, alignment */
	{ "%04d", 12, "0012" },
	{ "%.3d", 12, "012" },
	{ "%3d", 12, " 12" },
	{ "%-3d", 12, "12 " },
	{ "%+3d", 12, "+12" },
	{ "%+-5d", 12, "+12  " },
	{ "%+- 5d", 12, "+12  " },
	{ "%- 5d", 12, " 12  " },
	{ "% d", 12, " 12" },
	{ "%0-5d", 12, "12   " },
	{ "%-05d", 12, "12   " },

	/* ...explicit precision of 0 shall be no characters except for alt-octal. */
	{ "%.0d", 0, "" },
	{ "%.0o", 0, "" },
	{ "%#.0d", 0, "" },
	{ "%#.0o", 0, "0" },
	{ "%#.0x", 0, "" },

	/* ...but it still has to honor width and flags. */
	{ "%2.0u", 0, "  " },
	{ "%02.0u", 0, "  " },
	{ "%2.0d", 0, "  " },
	{ "%02.0d", 0, "  " },
	{ "% .0d", 0, " " },
	{ "%+.0d", 0, "+" },

	/* hex: test alt form and case */
	{ "%x", 63, "3f" },
	{ "%#x", 63, "0x3f" },
	{ "%X", 63, "3F" },

	/* octal: test alt form */
	{ "%o", 15, "17" },
	{ "%#o", 15, "017" },

	/* octal: corner cases */
	{ "%#o", 0, "0" },
	{ "%#.0o", 0, "0" },
	{ "%#.1o", 0, "0" },
	{ "%#o", 1, "01" },
	{ "%#.0o", 1, "01" },
	{ "%#.1o", 1, "01" },
	{ "%#04o", 1, "0001" },
	{ "%#04.0o", 1, "  01" },
	{ "%#04.1o", 1, "  01" },
	{ "%04o", 1, "0001" },
	{ "%04.0o", 1, "   1" },
	{ "%04.1o", 1, "   1" },

	{ NULL, 0.0, NULL }
};

static const struct {
	const char *fmt;
	double f;
	const char *expect;
} fp_tests[] = {
	/* basic form, handling of exponent/precision for 0 */
	{ "%a", 0.0, "0x0p+0" },
	{ "%e", 0.0, "0.000000e+00" },
	{ "%f", 0.0, "0.000000" },
	{ "%g", 0.0, "0" },
	{ "%#g", 0.0, "0.00000" },
	{ "%la", 0.0, "0x0p+0" },
	{ "%le", 0.0, "0.000000e+00" },
	{ "%lf", 0.0, "0.000000" },
	{ "%lg", 0.0, "0" },
	{ "%#lg", 0.0, "0.00000" },

	/* rounding */
	{ "%f", 1.1, "1.100000" },
	{ "%f", 1.2, "1.200000" },
	{ "%f", 1.3, "1.300000" },
	{ "%f", 1.4, "1.400000" },
	{ "%f", 1.5, "1.500000" },
	{ "%.4f", 1.06125, "1.0613" }, /* input is not representible exactly as double */
	{ "%.4f", 1.03125, "1.0312" }, /* 0x1.08p0 */
	{ "%.2f", 1.375, "1.38" },
	{ "%.1f", 1.375, "1.4" },
	{ "%.1lf", 1.375, "1.4" },
	{ "%.15f", 1.1, "1.100000000000000" },
	{ "%.16f", 1.1, "1.1000000000000001" },
	{ "%.17f", 1.1, "1.10000000000000009" },
	{ "%.2e", 1500001.0, "1.50e+06" },
	{ "%.2e", 1505000.0, "1.50e+06" },
	{ "%.2e", 1505000.00000095367431640625, "1.51e+06" },
	{ "%.2e", 1505001.0, "1.51e+06" },
	{ "%.2e", 1506000.0, "1.51e+06" },
	
	/* correctness in DBL_DIG places */
	{ "%.15g", 1.23456789012345, "1.23456789012345" },

	/* correct choice of notation for %g */
	{ "%g", 0.0001, "0.0001" },
	{ "%g", 0.00001, "1e-05" },
	{ "%g", 123456, "123456" },
	{ "%g", 1234567, "1.23457e+06" },
	{ "%.7g", 1234567, "1234567" },
	{ "%.7g", 12345678, "1.234568e+07" },
	{ "%.8g", 0.1, "0.1" },
	{ "%.9g", 0.1, "0.1" },
	{ "%.10g", 0.1, "0.1" },
	{ "%.11g", 0.1, "0.1" },

	/* pi in double precision, printed to a few extra places */
	{ "%.15f", M_PI, "3.141592653589793" },
	{ "%.18f", M_PI, "3.141592653589793116" },

	/* exact conversion of large integers */
	{ "%.0f", 340282366920938463463374607431768211456.0,
	         "340282366920938463463374607431768211456" },

	{ NULL, 0.0, NULL }
};

/* clang-format on */

#define T(func, simple_func, fmt, ...)                                                     \
	do {                                                                               \
		int result, expected;                                                      \
		result = func(0, 0, fmt, __VA_ARGS__);                                     \
		if (jstr_unlikely(result == -1)) {                                         \
			PRINTERR("fmt:%s\n", fmt);                                         \
			jstr_err("");                                                      \
			assert(result != -1);                                              \
		}                                                                          \
		expected = simple_func(0, 0, fmt, __VA_ARGS__);                            \
		if (jstr_unlikely(expected == -1)) {                                       \
			PRINTERR("%s\n", strerror(expected));                              \
			assert(expected != -1);                                            \
		}                                                                          \
		assert(-1 != sprintf(result_num, "%d", result));                           \
		assert(-1 != sprintf(expected_num, "%d", expected));                       \
		if (jstr_unlikely(result < expected)) {                                    \
			PRINTERR("fmt:%s\n", fmt);                                         \
			ASSERT_RESULT(func, result >= expected, result_num, expected_num); \
		}                                                                          \
	} while (0)

static int
jstr_sprintfstrlen(char *s, size_t n, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	return ret;
	(void)s;
	(void)n;
}

int
main(int argc, char **argv)
{
	START();
	TESTING(jstr_vsprintfstrlen);
	char result_num[sizeof(size_t) * 8 + 1];
	char expected_num[sizeof(size_t) * 8 + 1];
	T(jstr_sprintfstrlen, snprintf, "%d", 123456);
	T(jstr_sprintfstrlen, snprintf, "%.4s", "hello");
	T(jstr_sprintfstrlen, snprintf, "%.0s", "goodbye");
	/* Perform ascii arithmetic to test printing tiny doubles */
	T(jstr_sprintfstrlen, snprintf, "%.1022f", 0x1p-1021);
	for (int i = 0; int_tests[i].fmt; i++)
		T(jstr_sprintfstrlen, snprintf, int_tests[i].fmt, int_tests[i].i);
	for (int i = 0; fp_tests[i].fmt; i++)
		T(jstr_sprintfstrlen, snprintf, fp_tests[i].fmt, fp_tests[i].f);
	T(jstr_sprintfstrlen, snprintf, "%.4a", 1.0);
	SUCCESS();
	return EXIT_SUCCESS;
	(void)argc;
}
