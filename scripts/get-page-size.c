#include <unistd.h>
#include <stdio.h>
int main()
{
	return printf("%ld\n", sysconf(_SC_PAGE_SIZE));
}
