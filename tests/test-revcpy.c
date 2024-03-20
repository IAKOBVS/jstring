#include "test.h"
#include "test-array.h"
int
main(int argc, char **argv)
{
	START();
	T_STRCPY(jstr_revcpy_p, simple_revcpy_p, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
