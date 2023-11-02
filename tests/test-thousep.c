#include "test.h"

#define T(string, expected_result)                                          \
	do {                                                                \
		char s[1024];                                               \
		memcpy(s, string, strlen(string) + 1);                      \
		assert(jstr_thousep_p(s, ',') == s + strlen(s));            \
		assert(!strcmp(s, expected_result));                        \
		assert(jstr_thousepcpy_p(s, string, ',') == s + strlen(s)); \
		assert(!strcmp(s, expected_result));                        \
	} while (0)

int
main(int argc, char **argv)
{
	puts("Testing jstr_thousep_p().");
	puts("Testing jstr_thousepcpy_p().");
	T("1", "1");
	T("12", "12");
	T("123", "123");
	T("1234", "1,234");
	T("12345", "12,345");
	T("123456", "123,456");
	T("1234567", "1,234,567");
	SUCCESS();
	return 0;
}
