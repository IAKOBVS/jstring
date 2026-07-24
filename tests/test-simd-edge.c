#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "jstr.h"

int
main(void)
{
	/* Test jstr_strncasechr: not found across >2 VEC chunks.
	 * VEC_SIZE is 16 (SSE2) on this system.
	 * This exercises simd.h line 188 (return NULL). */
	{
		char buf[200];
		memset(buf, 'a', 199);
		buf[199] = '\0';
		/* search for 'z' with n=100, which spans at least 6 vectors */
		const char *p = jstr_strncasechr(buf, 'z', 100);
		assert(p == NULL);
	}

	/* Test jstr_memmem: page-crossing needle.
	 * This exercises simd.h line 585 (memcpy safe fallback). */
	{
		long page_size = sysconf(_SC_PAGE_SIZE);
		char *buf = mmap(NULL, (size_t)page_size + 64,
		                 PROT_READ | PROT_WRITE,
		                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		assert(buf != MAP_FAILED);

		/* Place needle near end of first page so it crosses boundary.
		 * With page_size=4096, VEC_SIZE=16: offset 4088 means
		 * a VEC load from ne reads into bytes 4088-4103,
		 * crossing into the second page. */
		const char *ne = buf + page_size - 8;
		memcpy((char *)ne, "hello", 5);
		size_t ne_len = 5;

		const char *hs = "a quick brown fox jumps over the lazy dog";
		size_t hs_len = strlen(hs);

		void *r = jstr_memmem(hs, hs_len, ne, ne_len);
		assert(r == NULL);

		munmap(buf, (size_t)page_size + 64);
	}

	return 0;
}
