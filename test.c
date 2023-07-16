#define _GNU_SOURCE
#include "jstd.h"

int main()
{
	jstd_declare_string(s, ssz, scap);
	jstd_alloc_append(&s, &ssz, &scap, "wha1234567890123456789--_________________-1234567890123456789");
	jstd_replaceall(&s, &ssz, &scap, "1234567890123456789", "");
	printf("len:%zu\n", strlen(s));
	printf("sz:%zu\n", ssz);
	printf("cap:%zu\n", scap);
	puts(s);
	free(s);
	return 0;
}
