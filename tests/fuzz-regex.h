/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Shared declarations for the regex fuzz tests.
 * The fuzz_regex_patterns() implementation and the replacement tables live
 * in fuzz-regex.c, which the test harness compiles and links into each
 * fuzz-regex test binary. Each including .c file defines FUZZ_PATTERNS /
 * FUZZ_NPATTERNS before using them. */

#ifndef FUZZ_REGEX_H
#define FUZZ_REGEX_H

#include "fuzz-shared.h"
#include "../include/regex.h"
#include <stdio.h>

enum { BFUZZ = 256 };

struct regex_pattern {
	const char *pat;
	int cflags;
	int has_capture;
};

extern const char *const replacements[];
extern const size_t nrepl;
extern const char *const breplacements[];
extern const size_t nbrepl;

/* Fuzz every regex operation against PATTERNS (npatterns entries), for
 * `iter` iterations of random haystacks. Implemented in fuzz-regex.c. */
void fuzz_regex_patterns(size_t iter,
                         const struct regex_pattern *patterns,
                         size_t npatterns);

#endif /* FUZZ_REGEX_H */
