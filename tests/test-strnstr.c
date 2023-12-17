#include "test.h"
#include "test-array.h"
int
main(int argc, char **argv)
{
	START();
	T_STRNSTR(jstr_strnstr, simple_strnstr, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
