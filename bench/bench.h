/* SPDX-License-Identifier: MIT */
/* This file is part of the jstring library, and is based on musl's
 * libc-bench
 *
 * libc-bench is licensed under the following standard MIT license:
 *
 * Copyright © 2011 Rich Felker
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef BENCH_H
#define BENCH_H

#define _GNU_SOURCE

#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define JSTR_BUILT 1
#include "../include/macros.h"

static JSTR_ATTR_MAYBE_UNUSED void
print_stats(struct timespec tv0)
{
	FILE *f;
	char buf[256];
	struct timespec tv;
	int maj, min, in_heap = 0;
	unsigned long l;
	size_t vm_size = 0, vm_rss = 0, vm_priv_dirty = 0;
	clock_gettime(CLOCK_REALTIME, &tv);
	tv.tv_sec -= tv0.tv_sec;
	if ((tv.tv_nsec -= tv0.tv_nsec) < 0) {
		tv.tv_nsec += 1000000000;
		tv.tv_sec--;
	}
	f = fopen("/proc/self/smaps", "rb");
	if (f)
		while (fgets(buf, sizeof buf, f)) {
			if (sscanf(buf, "%*lx-%*lx %*s %*lx %x:%x %*lu %*s", &maj, &min) == 2)
				in_heap = (!maj && !min && !strstr(buf, "---p") && (strstr(buf, "[heap]") || !strchr(buf, '[')));
			if (in_heap) {
				if (sscanf(buf, "Size: %lu", &l) == 1)
					vm_size += l;
				else if (sscanf(buf, "Rss: %lu", &l) == 1)
					vm_rss += l;
				else if (sscanf(buf, "Private_Dirty: %lu", &l) == 1)
					vm_priv_dirty += l;
			}
		}
	if (f)
		fclose(f);
#if 0
	printf("  time: %ld.%.9ld, virt: %zu, res: %zu, dirty: %zu\n\n",
	       (long)tv.tv_sec,
	       (long)tv.tv_nsec,
	       vm_size,
	       vm_rss,
	       vm_priv_dirty);
#else
	printf("%ld.%.9ld\n", (long)tv.tv_sec, (long)tv.tv_nsec);
#endif
}

static JSTR_ATTR_MAYBE_UNUSED int
run_bench(const char *label, size_t (*bench)(void *), void *params)
{
	struct timespec tv0;
	pid_t p = fork();
	assert(p != -1);
	if (p) {
		int status;
		wait(&status);
		return status;
	}
	printf("%s", label);
	clock_gettime(CLOCK_REALTIME, &tv0);
	bench(params);
	print_stats(tv0);
	exit(0);
}

static JSTR_ATTR_MAYBE_UNUSED void *
xmalloc(size_t n)
{
	void *p = malloc(n);
	if (jstr_unlikely(p == NULL))
		jstr_errdie("");
	return p;
}

#define malloc xmalloc

#define RUN(a, b)                 \
	extern size_t(a)(void *); \
	run_bench(#a " " #b " ", (a), (void *)(b))

#endif /* BENCH_H */
