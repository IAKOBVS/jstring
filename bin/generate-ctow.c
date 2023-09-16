#include <stdio.h>
#include <stdlib.h>

void generate(unsigned int i,
	      const unsigned int j)
{
	for (; i <= j; ++i) {
		printf("case %u: return ", i);
		for (unsigned int k = i - 1; k >= 1; --k)
			printf("SH(%u) | ", k);
		printf("SH(0);\n");
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: ./generate-ctow: <min_bytes> <max_bytes>");
		return 1;
	}
	generate(strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10));
	return 0;
}
