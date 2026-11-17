#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	char c = 'x';
	JSTR_STATIC_ASSERT_IS_CHAR(c);
#endif
	return 0;
}
