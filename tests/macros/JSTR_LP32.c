#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_LP32 == 0) || (JSTR_LP32 == 1));
	assert((JSTR_LP64 + JSTR_LP32 + JSTR_LP_UNKNOWN) == 1);
	return 0;
}
