#include <assert.h>
#include "jstr.h"

#include <string.h>
#include "struct.h"

int
main(void)
{
	/* Ordinary literal: size is sizeof(s) - 1 and the bytes match. */
	{
		jstr_literal_ty lit = jstr_literal_init("hello");
		assert(lit.size == 5);
		assert(lit.size == sizeof("hello") - 1);
		assert(memcmp(lit.data, "hello", 5) == 0);
		assert(lit.data[5] == '\0');
	}

	/* Empty string. */
	{
		jstr_literal_ty lit = jstr_literal_init("");
		assert(lit.size == 0);
		assert(lit.size == sizeof("") - 1);
		assert(lit.data != NULL);
		assert(lit.data[0] == '\0');
	}

	/* Single byte. */
	{
		jstr_literal_ty lit = jstr_literal_init("x");
		assert(lit.size == 1);
		assert(lit.data[0] == 'x');
		assert(lit.data[1] == '\0');
	}

	/* Embedded NUL is counted in the length and preserved in the data. */
	{
		jstr_literal_ty lit = jstr_literal_init("a\0b");
		assert(lit.size == 3);
		assert(lit.size == sizeof("a\0b") - 1);
		assert(lit.data[0] == 'a');
		assert(lit.data[1] == '\0');
		assert(lit.data[2] == 'b');
		assert(lit.data[3] == '\0');
	}

	/* Non-ASCII bytes (escaped to avoid encoding assumptions). */
	{
		jstr_literal_ty lit = jstr_literal_init("\x80\xfe\xff");
		assert(lit.size == 3);
		assert((unsigned char)lit.data[0] == 0x80);
		assert((unsigned char)lit.data[1] == 0xfe);
		assert((unsigned char)lit.data[2] == 0xff);
	}

	/* A longer literal: size tracks the literal, not strlen of anything. */
	{
		jstr_literal_ty lit =
			jstr_literal_init("the quick brown fox jumps over the lazy dog");
		assert(lit.size == 43);
		assert(lit.size == sizeof("the quick brown fox jumps over the lazy dog") - 1);
		assert(memcmp(lit.data, "the quick", 9) == 0);
	}

	return 0;
}
