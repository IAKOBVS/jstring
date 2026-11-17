#include <assert.h>
#include "jstr.h"

int
main(void)
{
	unsigned char a[256];
	unsigned char b[10];
	JSTR_MEMSET_ARRAY(a, 0xff);
	JSTR_MEMSET_ARRAY(b, 0xff);
	JSTR_BZERO_ARRAY(a);
	JSTR_BZERO_ARRAY(b);
	for (size_t i = 0; i < sizeof a; ++i)
		assert(a[i] == 0);
	for (size_t i = 0; i < sizeof b; ++i)
		assert(b[i] == 0);
	return 0;
}
