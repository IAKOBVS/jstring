/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../src/jstr-regex.h"

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)
#define FILL(result, str)                                                                     \
	do {                                                                                  \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
	} while (0)
#define T_APPEND_NORET(func, ...)                 \
	do {                                      \
		TESTING(func);                    \
		func(__VA_ARGS__);                \
		T_ASSERT(func, result, expected); \
		(result).size = 0;                \
		*(result).data = '\0';            \
	} while (0)
#define T_RE_BREF(_string, _find, _rplc, _expected, _nmatch, _n)                                                              \
	do {                                                                                                                  \
		const char *expected;                                                                                         \
		FILL(result, _string);                                                                                        \
		expected = _expected;                                                                                         \
		regex_t preg;                                                                                                 \
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, _find, 0)));                                                      \
		T_APPEND_NORET(jstr_re_rplcn_backref_len, &preg, jstr_struct(&result), _rplc, strlen(_rplc), 0, _nmatch, _n); \
		jstr_re_free(&preg);                                                                                          \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;

	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, (size_t)-1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, 4);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 1, 3);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 1, 2);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1, 1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 1, 0);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, (size_t)-1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, 4);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 1, 3);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 1, 2);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1, 1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 1, 0);

	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, 4);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_world", 2, 3);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hello_world", 2, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_world_hello_world", 2, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hello_world_hello_world", 2, 0);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_hello_world", 3, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "hello_world_hello_world", 3, 0);

	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_Pworldworld", 2, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_Pworldworld", 2, 4);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_world", 2, 3);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_hello_world", 2, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_world_hello_world", 2, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "hello_world_hello_world", 2, 0);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_Phelloworld", 3, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_Phelloworld", 3, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_hello_world", 3, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "hello_world_hello_world", 3, 0);

	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_worldworldQ", 2, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_worldworldQ", 2, 4);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_world", 2, 3);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hello_world", 2, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_world_hello_world", 2, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hello_world_hello_world", 2, 0);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_helloworldQ", 3, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_helloworldQ", 3, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_hello_world", 3, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "hello_world_hello_world", 3, 0);

	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world\\\\worldQ", 2, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world\\\\worldQ", 2, 4);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world", 2, 3);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello_world", 2, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world_hello_world", 2, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello_world_hello_world", 2, 0);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello\\\\worldQ", 3, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello\\\\worldQ", 3, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello_world", 3, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello_world_hello_world", 3, 0);

	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
