#include "test-array.h"
#include "test.h"

int
main(int argc, char **argv)
{
	START();
	
	T_MEMMEM(jstr_strcasestr_len, simple_strcasestr_len, test_array_memmem);
	T_STRSTR(jstr_strcasestr, simple_strcasestr, test_array_memmem);
	T_STRNSTR(jstr_strncasestr, simple_strncasestr, test_array_memmem);

	SUCCESS();
	return EXIT_SUCCESS;
}
