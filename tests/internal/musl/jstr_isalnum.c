#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../test.h"
#include "internal/musl/ctype.h"

int
main(void)
{

	for (int c = 0; c < 256; c++) {
		assert((jstr_isalnum(c) != 0) == (isalnum((unsigned char)c) != 0));
	}
	assert(jstr_isalnum('Z'));
	assert(jstr_isalnum('0'));
	assert(!jstr_isalnum('.'));

	return EXIT_SUCCESS;
}
