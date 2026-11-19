#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	setenv("HOME", "/home/tester", 1);
	/* not tilde */
	{
		char buf[64];
		strcpy(buf, "foo");
		char *p = jstr_io_expandtildefirst_len_unsafe_p(buf, 3);
		assert(p == buf + 3);
		assert(strcmp(buf, "foo") == 0);
	}
	/* tilde */
	{
		char buf[128];
		strcpy(buf, "~/x");
		char *p = jstr_io_expandtildefirst_len_unsafe_p(buf, 3);
		assert(p != NULL);
		assert(strcmp(buf, "/home/tester/x") == 0);
		assert(p == buf + strlen(buf));
	}
	/* missing HOME */
	{
		unsetenv("HOME");
		char buf[64];
		strcpy(buf, "~");
		char *p = jstr_io_expandtildefirst_len_unsafe_p(buf, 1);
		assert(p == NULL);
		setenv("HOME", "/home/tester", 1);
	}
	return 0;
}
