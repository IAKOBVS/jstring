#include <stdio.h>
int main()
{
	int n = 1;
	if (*(char *)&n == 1)
		putchar('1');
	else
		putchar('0');
	return 0;
}
