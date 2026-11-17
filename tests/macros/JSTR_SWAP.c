#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int a = 1, b = 2;
	JSTR_SWAP(int, a, b);
	assert(a == 2 && b == 1);
	const char *s = "x", *t = "y";
	JSTR_SWAP(const char *, s, t);
	assert(strcmp(s, "y") == 0 && strcmp(t, "x") == 0);
	return 0;
}
