#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __hiuxmpp
	assert(JSTR_OS_HIUXMPP == 1);
#else
	assert(JSTR_OS_HIUXMPP == 0);
#endif
	return 0;
}
