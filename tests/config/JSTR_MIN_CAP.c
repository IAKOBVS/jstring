#include <assert.h>
#include "jstr.h"

#include "config.h"

int
main(void)
{
	assert(JSTR_MIN_CAP == (sizeof(size_t) == 8 ? 24u : 12u));
	assert(JSTR_MIN_CAP > 0);
	return 0;
}
