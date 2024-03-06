#include "test.h"
#include "test-array.h"
#include "../src/jstr-string.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMCMP(!jstr_memcmpeq_loop, !memcmp, test_array_memcmp);
	T_MEMCMP(jstr_strncasecmp, simple_strncasecmp, test_array_memcmp);
	T_MEMCMP(!jstr_strcasecmpeq_len, !simple_strncasecmp, test_array_memcmp);
	T_MEMCMP(!jstr_strcasecmpeq_len_loop, !simple_strncasecmp, test_array_memcmp);
	T_STRCMP(jstr_strcasecmp, simple_strcasecmp, test_array_memcmp);
	T_STRCMP(!jstr_strcasecmpeq, !simple_strcasecmp, test_array_memcmp);
	T_STRCMP(!jstr_strcasecmpeq_loop, !simple_strcasecmp, test_array_memcmp);
	SUCCESS();
	return EXIT_SUCCESS;
}
