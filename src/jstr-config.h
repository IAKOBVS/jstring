/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_CONFIG_H
#define JSTR_CONFIG_H 1

/* Print error message and call exit() on errors. */
#ifndef JSTR_PANIC
#	define JSTR_PANIC 0
#endif

/* Behave as JSTR_PANIC would, activate debug asserts, and print debug messages.
 */
#ifndef JSTR_DEBUG
#	define JSTR_DEBUG 0
#endif

/* jstrio_*() stdio functions and all functions which use them
 * will use the *_unlocked() versions when available. */
#ifndef JSTR_USE_UNLOCKED_IO
#	define JSTR_USE_UNLOCKED_IO 0
#endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that read. */
#ifndef JSTR_USE_UNLOCKED_IO_READ
#	define JSTR_USE_UNLOCKED_IO_READ 0
#endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that write. */
#ifndef JSTR_USE_UNLOCKED_IO_WRITE
#	define JSTR_USE_UNLOCKED_IO_WRITE 0
#endif

/* When removing a string (jstr_ty) from a string list (jstrlist_ty), don't
 * immediately call free(). This may result in fewer allocations when strings
 * are often added to and removed from the list. All elements are freed when
 * jstrl_free() is called. */
#ifndef JSTRL_LAZY_FREE
#	define JSTRL_LAZY_FREE 1
#endif

/* Minimum size of allocation of malloc(). */
#ifndef JSTR_MIN_CAP
#	define JSTR_MIN_CAP ((sizeof(size_t) == 8) ? 24 : 12)
#endif

/* Allocations will be aligned to this alignment. */
#ifndef JSTR_MALLOC_ALIGNMENT
#	define JSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))
#endif

/* Allocate more than needed for the initial malloc(). */
#ifndef JSTR_ALLOC_MULTIPLIER
#	define JSTR_ALLOC_MULTIPLIER 2
#endif
#ifndef JARR_ALLOC_MULTIPLIER
#	define JARR_ALLOC_MULTIPLIER 1.5
#endif

/* Growth factor of jstr_ty. */
#ifndef JSTR_GROWTH
#	define JSTR_GROWTH 1.5
#endif

/* Growth factor of jstrlist_ty. */
#ifndef JSTRL_GROWTH
#	define JSTRL_GROWTH 1.5
#endif

/* Growth factor of jarr_ty. */
#ifndef JARR_GROWTH
#	define JARR_GROWTH 1.5
#endif

/* Filename extensions interpreted as text. */
#ifndef JSTRIO_FT_TEXT_ARRAY
#	define JSTRIO_FT_TEXT_ARRAY                                           \
		"C", "S", "c", "cc", "cs", "cpp", "h", "hh", "hpp", "html",    \
		"js", "json", "md", "pl", "pm", "py", "pyi", "rs", "s", "sh",  \
		"ts", "txt"
#endif

/* Filename extensions interpreted as binary. */
#ifndef JSTRIO_FT_BINARY_ARRAY
#	define JSTRIO_FT_BINARY_ARRAY                                         \
		"a", "bin", "gz", "jpg", "jpeg", "mp4", "mp3", "mkv", "o",     \
		"pdf", "png", "pyc", "rar", "so", "wav", "zip"
#endif

#if !defined JSTR_ENDIAN_LITTLE && !defined JSTR_ENDIAN_BIG
/* This is defined by ./check-little-endian. */
#	define JSTR_ENDIAN_LITTLE 1
#endif
#if JSTR_ENDIAN_LITTLE
#	undef JSTR_ENDIAN_LITTLE
#	define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    0
#else
#	undef JSTR_ENDIAN_LITTLE
#	define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    1
#endif

#endif /* JSTR_CONFIG_H */
