#include <assert.h>
#include "jstr.h"

#include <ctype.h>
#include <string.h>

static int
classify(int c)
{
	switch (c) {
	JSTR_BLANK_CASE
		return 1;
	default:
		return 0;
	}
}

int
main(void)
{
	for (int i = 0; i < 256; ++i) {
		unsigned char ch = (unsigned char)i;
		assert((classify(i) != 0) == (ch == ' ' || ch == '\t'));
	}
	return 0;
}
