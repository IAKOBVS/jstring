#include "jstd.h"

int main()
{
	jstd_declare_string(s, ssz, scap);
	jstd_alloc_append(&s, &ssz, &scap, "hwa");
	return 0;
}
