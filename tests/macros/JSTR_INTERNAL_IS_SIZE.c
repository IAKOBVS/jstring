#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
#if JSTR_HAVE_GENERIC
	assert(JSTR_INTERNAL_IS_SIZE((size_t)1));
	assert(!JSTR_INTERNAL_IS_SIZE((char)1));
#endif
	return 0;
}
