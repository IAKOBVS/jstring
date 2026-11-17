#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"

int
main(int argc, char **argv)
{
	START();
	assert(jstr_internal_io_isbinarysignature(jstr_literal("")) == -1);
	assert(jstr_internal_io_isbinarysignature(jstr_literal("ab")) == -1);
	{
		const unsigned char b[] = { 0x7F, 'E', 'L', 'F' };
		assert(jstr_internal_io_isbinarysignature((const char *)b, 4) == 1);
	}
	{
		const unsigned char b[] = { 0xEF, 0xBB, 0xBF };
		assert(jstr_internal_io_isbinarysignature((const char *)b, 3) == 0);
	}
	{
		const unsigned char b[] = { 0xEF, 0xBB, 0xBF, 'x' };
		assert(jstr_internal_io_isbinarysignature((const char *)b, 4) == 0);
	}
	assert(jstr_internal_io_isbinarysignature(jstr_literal("hello")) == -1);
	SUCCESS();
	return 0;
}
