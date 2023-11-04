#include "test.h"

#define T(func, string, expected)                      \
	do {                                           \
		char s[1024] = string;                 \
		char *p = func(s);                     \
		assert(strlen(s) == strlen(expected)); \
		assert(p - s == strlen(expected));     \
		assert(!strcmp(s, expected));          \
	} while (0)

int
main()
{
	T(jstr_toCamelCaseP, "", "");
	T(jstr_toCamelCaseP, "hello", "hello");
	T(jstr_toCamelCaseP, "hello_world", "helloWorld");
	T(jstr_to_snake_case_p, "", "");
	T(jstr_to_snake_case_p, "hello", "hello");
	T(jstr_to_snake_case_p, "helloWorld", "hello_world");
	return 0;
}
