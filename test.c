#define _GNU_SOURCE
#include "jstr.h"

int main()
{
	jstr_t s;
	jstr_alloc_append(&s.data, &s.size, &s.cap, "wha1234567890123456789--_________________-1234567890123456789");
	jstr_replaceall(&s.data, &s.size, &s.cap, "1234567890123456789", "");
	printf("len:%zu\n", strlen(s.data));
	printf("sz:%zu\n", s.size);
	printf("cap:%zu\n", s.cap);
	puts(s.data);
	free(s.data);
	return 0;
}
