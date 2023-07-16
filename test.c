#define _GNU_SOURCE
#include "jstd.h"

int main()
{
	jstd_declare_string(s, ssz, scap);
	jstd_alloc_append(&s, &ssz, &scap, "wha1234567890123456789--_________________-1234567890123456789");
	jstd_replaceall(&s, &ssz, &scap, "1234567890123456789", "");
	puts(s);
	free(s);
	return 0;
}
