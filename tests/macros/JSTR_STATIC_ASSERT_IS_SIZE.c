#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
#if JSTR_HAVE_GENERIC
	JSTR_STATIC_ASSERT_IS_SIZE((size_t)1);
#endif
	return 0;
}
