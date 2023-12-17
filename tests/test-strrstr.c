#include "test.h"
#include "test-array.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMMEM(jstr_strrstr_len, simple_strrstr_len, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
