#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	const char *s = "x";
	JSTR_STATIC_ASSERT_IS_STR(s);
#endif
	return 0;
}
