#include "test-array.h"
#include "test.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMMEM(jstr_memmem, simple_memmem, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
