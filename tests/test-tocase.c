#include "test.h"

#define T(func, string, expected)                                   \
	do {                                                        \
		char s[1024] = string;                              \
		JSTR_STATIC_ASSERT(sizeof(s) > 2 * strlen(string)          \
		            && sizeof(s) > 2 * strlen(expected),    \
		            "");                                    \
		const char *const p = func(s);                      \
		ASSERT(strlen(s) == strlen(expected), s, expected); \
		ASSERT(p - s == strlen(expected), s, expected);     \
		ASSERT(!strcmp(s, expected), s, expected);          \
	} while (0)

#define T_CPY(func, string, expected)                                   \
	do {                                                            \
		char dst[1024];                                         \
		JSTR_STATIC_ASSERT(sizeof(dst) > 2 * strlen(string)            \
		            && sizeof(dst) > 2 * strlen(expected),      \
		            "");                                        \
		const char *const p = func(dst, string);                \
		ASSERT(strlen(dst) == strlen(expected), dst, expected); \
		ASSERT(p - dst == strlen(expected), dst, expected);     \
		ASSERT(!strcmp(dst, expected), dst, expected);          \
		ASSERT(!strcmp(dst, expected), dst, expected);          \
	} while (0)

int
main(int argc, char **argv)
{
	TESTING(jstr_toCamelCaseP);
	T(jstr_toCamelCaseP, "", "");
	T(jstr_toCamelCaseP, "hello", "hello");
	T(jstr_toCamelCaseP, "hello_world", "helloWorld");
	T(jstr_toCamelCaseP, "_hello", "_hello");
	T(jstr_toCamelCaseP, "_hello_world", "_helloWorld");

	TESTING(jstr_tocamelCaseCpyP);
	T_CPY(jstr_toCamelCaseCpyP, "", "");
	T_CPY(jstr_toCamelCaseCpyP, "hello", "hello");
	T_CPY(jstr_toCamelCaseCpyP, "hello_world", "helloWorld");
	T_CPY(jstr_toCamelCaseCpyP, "__hello", "__hello");
	T_CPY(jstr_toCamelCaseCpyP, "__hello_world", "__helloWorld");

	TESTING(jstr_to_snake_case_p);
	T(jstr_to_snake_case_p, "", "");
	T(jstr_to_snake_case_p, "hello", "hello");
	T(jstr_to_snake_case_p, "helloWorld", "hello_world");
	T(jstr_to_snake_case_p, "__hello", "__hello");
	T(jstr_to_snake_case_p, "__helloWorld", "__hello_world");

	TESTING(jstr_to_snake_case_cpy_p);
	T_CPY(jstr_to_snake_case_cpy_p, "", "");
	T_CPY(jstr_to_snake_case_cpy_p, "hello", "hello");
	T_CPY(jstr_to_snake_case_cpy_p, "helloWorld", "hello_world");
	T_CPY(jstr_to_snake_case_cpy_p, "__hello", "__hello");
	T_CPY(jstr_to_snake_case_cpy_p, "__helloWorld", "__hello_world");

	SUCCESS();
	return 0;
}
