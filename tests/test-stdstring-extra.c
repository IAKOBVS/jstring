#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <string.h>
#include "stdstring.h"

int main(void) {
	/* jstr_atou */ { unsigned int n = jstr_atou("42"); assert(n == 42); }
	/* jstr_atoul */ { unsigned long n = jstr_atoul("42"); assert(n == 42); }
	/* jstr_atoull */ { unsigned long long n = jstr_atoull("42"); assert(n == 42); }
	/* jstr_atoi */ { int n = jstr_atoi("-42"); assert(n == -42); }
	/* jstr_atol */ { long n = jstr_atol("-42"); assert(n == -42); }
	/* jstr_atoll */ { long long n = jstr_atoll("-42"); assert(n == -42); }
	/* jstr_atod */ { double n = jstr_atod("3.14"); assert(n > 3.13 && n < 3.15); }
	/* jstr_atof */ { float n = jstr_atof("2.71"); assert(n > 2.70f && n < 2.72f); }
	return 0;
}
