#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_TYPEOF && JSTR_HAVE_GENERIC
	JSTR_STATIC_ASSERT_TYPECHECK(1, 2);
#endif
	return 0;
}
