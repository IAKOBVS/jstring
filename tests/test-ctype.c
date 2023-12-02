#include "test.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../src/jstr-ctype.h"

#define T(func)                       \
	for (int i = 0; i < 128; ++i) \
	ASSERT_ERRFUNC(#func, !jstr_##func(i) == !func(i))
#define T_STR(func, str, result)                                                                                             \
	do {                                                                                                                 \
		size_t expected_len, result_len;                                                                             \
		JSTR_STATIC_ASSERT(sizeof(buffer_expected) > strlen(str), "");                                               \
		*(char *)memcpy(buffer_expected, str, strlen(str)) = '\0';                                                   \
		*(char *)memcpy(buffer_result, str, strlen(str)) = '\0';                                                     \
		(void)jstr_##func(buffer_result);                                                                            \
		(void)simple_##func(buffer_expected);                                                                        \
		expected_len = strlen(buffer_expected);                                                                      \
		result_len = strlen(buffer_result);                                                                          \
		ASSERT_RESULT(#func, !strcmp(buffer_result, buffer_expected), buffer_result, buffer_expected);               \
		ASSERT_RESULT(#func, result_len == expected_len, buffer_result, buffer_expected);                            \
		ASSERT_RESULT(#func, !memcmp(buffer_result, buffer_expected, result_len), buffer_expected, buffer_expected); \
	} while (0)
#define T_TO(str, result)                         \
	do {                                      \
		T_STR(tolowerstr_p, str, result); \
		T_STR(toupperstr_p, str, result); \
	} while (0)

static char buffer_result[256];
static char buffer_expected[256];

static char *
simple_toupperstr_p(char *s)
{
	for (; *s; ++s)
		*s = toupper(*s);
	return s;
}

static char *
simple_tolowerstr_p(char *s)
{
	for (; *s; ++s)
		*s = tolower(*s);
	return s;
}

int
main(int argc, char **argv)
{
	START();
	T(isalpha);
	T(islower);
	T(isupper);
	T(isspace);
	T(isblank);
	T(isdigit);
	T(isxdigit);
	T(ispunct);
	T(isgraph);
	T(isprint);
	T(isprint);
	T(iscntrl);
	T(tolower);
	T(toupper);
	T_TO("", "");
	T_TO(";", ";");
	T_STR(toupperstr_p, "a", "A");
	T_STR(toupperstr_p, "aA", "AA");
	T_STR(tolowerstr_p, "A", "a");
	T_STR(tolowerstr_p, "aA", "aa");
	SUCCESS();
	return EXIT_SUCCESS;
	(void)argc;
}
