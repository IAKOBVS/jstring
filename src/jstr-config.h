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

#ifndef JSTR_CONFIG_H
#define JSTR_CONFIG_H 1

/* Print error message and call exit() on errors. */
#ifndef JSTR_PANIC
#	define JSTR_PANIC 0
#endif

/* Behave as JSTR_PANIC would and print debug messages. */
#ifndef JSTR_DEBUG
#	define JSTR_DEBUG 0
#endif

/*
   Defining JSTR_USE_LGPL as 0 will exclude all LGPL code from being included.
*/
#ifndef JSTR_USE_LGPL
#	define JSTR_USE_LGPL 1
#endif

/*
   Always prefer standard functions over user functions.
   For example, use libc strcasestr() if available over
   our own strcasestr() in jstr_strcasestr().
*/
#ifndef JSTR_USE_STANDARD_ALWAYS
#	define JSTR_USE_STANDARD_ALWAYS 0
#endif

/*
   When removing a string (jstr_ty) from a string list (jstrlist_ty), don't immediately
   call free(). This may result in fewer allocations when strings are often added to
   and removed from the list. All elements are freed when jstrl_free() is called.
*/
#ifndef JSTRL_LAZY_FREE
#	define JSTRL_LAZY_FREE 0
#endif

#define JSTR_MIN_CAP          ((sizeof(size_t) == 8) ? 24 : 16)
#define JSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))

/* Allocate more than needed for the initial malloc(). */
#ifndef JSTR_ALLOC_MULTIPLIER
#	define JSTR_ALLOC_MULTIPLIER 2
#endif
#ifndef JARR_ALLOC_MULTIPLIER
#	define JARR_ALLOC_MULTIPLIER 1.5
#endif

/* Growth factor. */
#ifndef JSTR_GROWTH
#	define JSTR_GROWTH 1.5
#endif
#ifndef JSTRL_GROWTH
#	define JSTRL_GROWTH 1.5
#endif
#ifndef JARR_GROWTH
#	define JARR_GROWTH 1.5
#endif

/* File extensions interpreted as text files. */
#ifndef JSTRIO_FT_TEXT_ARRAY
#	define JSTRIO_FT_TEXT_ARRAY "C", "S", "c", "cc", "cs", "cpp", "h", "hh", "hpp", "html", "js", "json", "md", "pl", "pm", "py", "pyi", "rs", "s", "sh", "ts", "txt"
#endif
/* File extensions interpreted as binary files. */
#ifndef JSTRIO_FT_BINARY_ARRAY
#	define JSTRIO_FT_BINARY_ARRAY "a", "bin", "gz", "jpg", "jpeg", "mp4", "mp3", "mkv", "o", "pdf", "png", "pyc", "rar", "so", "wav", "zip"
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
