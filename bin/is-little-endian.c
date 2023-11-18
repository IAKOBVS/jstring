#include <stdio.h>
int main(void)
{
	const int n = 1;
	if (*(unsigned char *)&n == 1)
		putchar('1');
	else
		putchar('0');
	return 0;
}
