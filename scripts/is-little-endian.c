#include <stdio.h>
int main(void){int n=1;putchar((*(unsigned char *)&n==1)+'0');return 0;}
