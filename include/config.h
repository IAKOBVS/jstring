/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
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
#	define JSTR_CONFIG_H 1

/* Print error message and call exit() on errors. */
#	ifndef JSTR_PANIC
#		define JSTR_PANIC 0
#	endif

/* Behave as JSTR_PANIC would, activate debug asserts, and print debug messages. */
#	ifndef JSTR_DEBUG
#		define JSTR_DEBUG 0
#	endif

/* jstr_io_*() stdio functions and all functions which use them
 * will use the *_unlocked() versions when available. */
#	ifndef JSTR_USE_UNLOCKED_IO
#		define JSTR_USE_UNLOCKED_IO 0
#	endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that read. */
#	ifndef JSTR_USE_UNLOCKED_IO_READ
#		define JSTR_USE_UNLOCKED_IO_READ 0
#	endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that write. */
#	ifndef JSTR_USE_UNLOCKED_IO_WRITE
#		define JSTR_USE_UNLOCKED_IO_WRITE 0
#	endif

/* Minimum size of allocation of malloc(). */
#	ifndef JSTR_MIN_CAP
#		define JSTR_MIN_CAP ((sizeof(size_t) == 8) ? 24 : 12)
#	endif

/* Allocations will be aligned to this alignment. */
#	ifndef JSTR_MALLOC_ALIGNMENT
#		define JSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))
#	endif

#endif /* JSTR_CONFIG_H */
