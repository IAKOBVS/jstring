#include "test.h"

#define T(string, expected_result)                               \
	do {                                                     \
		char s[1024];                                    \
		memcpy(s, string, strlen(string) + 1);           \
		assert(jstr_thousep_p(s, ',') == s + strlen(s)); \
		assert(!strcmp(s, expected_result));             \
	} while (0)

int
main()
{
	T("1", "1");
	T("12", "12");
	T("123", "123");
	T("1234", "1,234");
	T("12345", "12,345");
	T("123456", "123,456");
	T("123456", "1,234,567");
	return 0;
}
