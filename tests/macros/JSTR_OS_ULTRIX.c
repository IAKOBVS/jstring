#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined ultrix || defined __ultrix || defined __ultrix__ || (defined unix && defined vax)
	assert(JSTR_OS_ULTRIX == 1);
#else
	assert(JSTR_OS_ULTRIX == 0);
#endif
	return 0;
}
