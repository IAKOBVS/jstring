#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../test.h"
#include "internal/musl/ctype.h"

int
main(void)
{

	for (int c = 0; c < 256; c++) {
		assert(jstr_tolower(c) == tolower((unsigned char)c));
	}
	assert(jstr_tolower('A') == 'a');
	assert(jstr_tolower('Z') == 'z');
	assert(jstr_tolower('a') == 'a');
	assert(jstr_tolower('0') == '0');

	return EXIT_SUCCESS;
}
