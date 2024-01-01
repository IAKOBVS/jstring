#include "test.h"
#include "../src/jstr-itoa.h"

char buf[1024];

void
T(const char *string, const char *expected_result)
{
	memcpy(buf, string, strlen(string) + 1);
	ASSERT_RESULT(jstr_thousep_p, jstr_thousep_p(buf, ',') == buf + strlen(buf), buf, expected_result);
	ASSERT_RESULT(jstr_thousep_p, !strcmp(buf, expected_result), buf, expected_result);
	ASSERT_RESULT(jstr_thousepcpy_p, jstr_thousepcpy_p(buf, string, ',') == buf + strlen(buf), buf, expected_result);
	ASSERT_RESULT(jstr_thousepcpy_p, !strcmp(buf, expected_result), buf, expected_result);
}

int
main(int argc, char **argv)
{
	START();
	TESTING(jstr_thousep_p);
	TESTING(jstr_thousepcpy_p);
	T("1", "1");
	T("12", "12");
	T("123", "123");
	T("1234", "1,234");
	T("12345", "12,345");
	T("123456", "123,456");
	T("1234567", "1,234,567");
	T("12345678", "12,345,678");
	T("123456789", "123,456,789");
	SUCCESS();
	return EXIT_SUCCESS;
}
