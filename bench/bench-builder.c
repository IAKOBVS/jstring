/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/builder.h"
#include "../include/replace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NLOOPS  50
#define NSTRS   1000

static const char *strs[NSTRS];
static size_t strs_len[NSTRS];
static char strs_buf[NSTRS][32];

static size_t
b_jstr_cat(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		for (size_t j = 0; j < NSTRS; j++) {
			jstr_cat_arg_ty args[] = {{strs[j], strs_len[j]}};
			jstr_cat(&s, &sz, &cap, args, 1);
		}
		cs += sz;
		free(s);
	}
	return cs;
}

static size_t
b_jstr_cat_batch(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		jstr_cat_arg_ty args[NSTRS];
		for (size_t j = 0; j < NSTRS; j++)
			args[j] = (jstr_cat_arg_ty){strs[j], strs_len[j]};
		jstr_cat(&s, &sz, &cap, args, NSTRS);
		cs += sz;
		free(s);
	}
	return cs;
}

static size_t
b_jstr_cat_j(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		jstr_ty j = JSTR_INIT;
		for (size_t k = 0; k < NSTRS; k++) {
			jstr_cat_arg_ty args[] = {{strs[k], strs_len[k]}};
			jstr_cat_j(&j, args, 1);
		}
		cs += j.size;
		free(j.data);
	}
	return cs;
}

static size_t
b_jstr_cat_j_batch(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		jstr_ty j = JSTR_INIT;
		jstr_cat_arg_ty args[NSTRS];
		for (size_t k = 0; k < NSTRS; k++)
			args[k] = (jstr_cat_arg_ty){strs[k], strs_len[k]};
		jstr_cat_j(&j, args, NSTRS);
		cs += j.size;
		free(j.data);
	}
	return cs;
}

static size_t
b_jstr_append_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		for (size_t j = 0; j < NSTRS; j++)
			jstr_append_len(&s, &sz, &cap, strs[j], strs_len[j]);
		cs += sz;
		free(s);
	}
	return cs;
}

static size_t
b_simple_realloc_strcat(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		for (size_t j = 0; j < NSTRS; j++) {
			size_t len = strs_len[j];
			if (sz + len + 1 > cap) {
				cap = (sz + len + 1) * 2;
				s = realloc(s, cap);
			}
			memcpy(s + sz, strs[j], len + 1);
			sz += len;
		}
		cs += sz;
		free(s);
	}
	return cs;
}

static size_t
b_jstr_insert_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		jstr_append_len(&s, &sz, &cap, "start", 5);
		for (size_t j = 0; j < NSTRS; j++)
			jstr_insert_len(&s, &sz, &cap, 0, strs[j], strs_len[j]);
		cs += sz;
		free(s);
	}
	return cs;
}

static size_t
b_jstr_reserve_append(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < NLOOPS; i++) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		size_t total = 0;
		for (size_t j = 0; j < NSTRS; j++)
			total += strs_len[j];
		jstr_reserve(&s, &sz, &cap, total + 1);
		size_t off = 0;
		for (size_t j = 0; j < NSTRS; j++) {
			jstr_append_len_unsafe_p(s, off, strs[j], strs_len[j]);
			off += strs_len[j];
		}
		sz = total;
		cs += sz;
		free(s);
	}
	return cs;
}

int
main()
{
	srand(42);
	for (int i = 0; i < NSTRS; i++) {
		int len = 4 + rand() % 28;
		for (int j = 0; j < len; j++)
			strs_buf[i][j] = 'a' + rand() % 26;
		strs_buf[i][len] = '\0';
		strs[i] = strs_buf[i];
		strs_len[i] = (size_t)len;
	}

	RUN(b_jstr_cat, 0);
	RUN(b_jstr_cat_batch, 0);
	RUN(b_jstr_cat_j, 0);
	RUN(b_jstr_cat_j_batch, 0);
	RUN(b_jstr_append_len, 0);
	RUN(b_simple_realloc_strcat, 0);
	RUN(b_jstr_insert_len, 0);
	RUN(b_jstr_reserve_append, 0);

	return 0;
}
