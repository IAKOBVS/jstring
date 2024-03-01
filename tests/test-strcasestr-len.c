#include "test-array.h"
#include "test.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMMEM(jstr_strcasestr_len, simple_strcasestr_len, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
