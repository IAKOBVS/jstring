/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../src/jstr-regex.h"

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)
#define FILL(result, str) assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str))))
#define T_APPEND_NORET(func, ...)                 \
	do {                                      \
		TESTING(func);                    \
		func(__VA_ARGS__);                \
		T_ASSERT(func, result, expected); \
		(result).size = 0;                \
		*(result).data = '\0';            \
	} while (0)
#define T_RE(_string, _find, _rplc, _expected, _n)                                                           \
	do {                                                                                                 \
		const char *rplc, *find, *expected;                                                          \
		FILL(result, _string);                                                                       \
		find = _find;                                                                                \
		rplc = _rplc;                                                                                \
		expected = _expected;                                                                        \
		regex_t preg;\
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, _find, 0)));                                     \
		T_APPEND_NORET(jstr_re_rplcn_len, &preg, jstr_struct(&result), _rplc, strlen(_rplc), 0, _n); \
		jstr_re_free(&preg);                                                                         \
		(void)find;\
		(void)rplc;\
	} while (0)
int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 0);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 0);
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
