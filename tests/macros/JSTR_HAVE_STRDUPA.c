#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRDUPA == 0) || (JSTR_HAVE_STRDUPA == 1));
	return 0;
}
