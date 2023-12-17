#include "test.h"
#include "test-array.h"

int
main(int argc, char **argv)
{
	START();

	T_MEMMEM(jstr_memmem, simple_memmem, test_array_memmem);

	T_STRNSTR(jstr_strnstr, simple_strnstr, test_array_memmem);
	T_MEMMEM(jstr_strrstr_len, simple_strrstr_len, test_array_memmem);

	T_STRCPY(jstr_stpcpy, simple_stpcpy, test_array_memmem);

	T_STRCPY(jstr_revcpy_p, simple_revcpy_p, test_array_memmem);

	SUCCESS();
	return EXIT_SUCCESS;
}
