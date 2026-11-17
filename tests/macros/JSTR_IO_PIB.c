#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_PIB == (size_t)1125899906842624);
	return 0;
}
