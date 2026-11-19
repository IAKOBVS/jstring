#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../test.h"
#include "internal/musl/ctype.h"

int
main(void)
{

	for (int c = 0; c < 256; c++) {
		assert(jstr_toupper(c) == toupper((unsigned char)c));
	}
	assert(jstr_toupper('a') == 'A');
	assert(jstr_toupper('z') == 'Z');
	assert(jstr_toupper('A') == 'A');
	assert(jstr_toupper('0') == '0');

	return EXIT_SUCCESS;
}
