#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char buf[128];
	strcpy(buf, "abcABC");
	assert(jstr_internal_strcasechrnul_musl(buf, 'd') == buf + strlen(buf));
	assert(jstr_internal_strcasechrnul_musl(buf, 'b') == buf + 1);
	assert(jstr_internal_strcasechrnul_musl(buf, 'A') == buf);

	SUCCESS();
	return EXIT_SUCCESS;
}
