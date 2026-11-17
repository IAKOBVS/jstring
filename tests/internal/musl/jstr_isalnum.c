#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../test.h"
#include "internal/musl/ctype.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	for (int c = 0; c < 256; c++) {
		assert((jstr_isalnum(c) != 0) == (isalnum((unsigned char)c) != 0));
	}
	assert(jstr_isalnum('Z'));
	assert(jstr_isalnum('0'));
	assert(!jstr_isalnum('.'));

	SUCCESS();
	return EXIT_SUCCESS;
}
