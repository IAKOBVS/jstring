/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#include <stdio.h>
#include <stdlib.h>

void
generate(unsigned int i,
         const unsigned int j)
{
	for (; i <= j; ++i) {
		printf("case %u: return ", i);
		for (unsigned int k = i - 1; k >= 1; --k)
			printf("SH(%u) | ", k);
		printf("SH(0);\n");
	}
}

int
main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s: <min_bytes> <max_bytes>", argv[0]);
		return 1;
	}
	generate(strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10));
	return 0;
}
