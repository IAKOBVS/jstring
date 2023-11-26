/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef JSTR_FEATURES_H
#define JSTR_FEATURES_H 1

/* This is to enable the ISO C11 extension.  */
#if (defined _ISOC11_SOURCE || defined _ISOC2X_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L))
#	define JSTR_USE_ISOC11 1
#endif

/* This is to enable the ISO C99 extension.  */
#if (defined _ISOC99_SOURCE || defined _ISOC11_SOURCE \
     || defined _ISOC2X_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#	define JSTR_USE_ISOC99 1
#endif

/* This is to enable the ISO C90 Amendment 1:1995 extension.  */
#if (defined _ISOC99_SOURCE || defined _ISOC11_SOURCE \
     || defined _ISOC2X_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199409L))
#	define JSTR_USE_ISOC95 1
#endif

#ifdef __cplusplus
/* This is to enable compatibility for ISO C++17.  */
#	if __cplusplus >= 201703L
#		define JSTR_USE_ISOC11 1
#	endif
/* This is to enable compatibility for ISO C++11.
   Check the temporary macro for now, too.  */
#	if __cplusplus >= 201103L || defined __GXX_EXPERIMENTAL_CXX0X__
#		define JSTR_USE_ISOCXX11 1
#		define JSTR_USE_ISOC99   1
#	endif
#endif

/* If none of the ANSI/POSIX macros are defined, or if _DEFAULT_SOURCE
   is defined, use POSIX.1-2008 (or another version depending on
   _XOPEN_SOURCE).  */
#ifdef _DEFAULT_SOURCE
#	if !defined _POSIX_SOURCE && !defined _POSIX_C_SOURCE
#		define JSTR_USE_POSIX_IMPLICITLY 1
#	endif
#endif

#if (defined _POSIX_SOURCE \
     || (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 1) \
     || defined _XOPEN_SOURCE)
#	define JSTR_USE_POSIX 1
#endif

#if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 2 || defined _XOPEN_SOURCE
#	define JSTR_USE_POSIX2 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 199309L
#	define JSTR_USE_POSIX199309 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 199506L
#	define JSTR_USE_POSIX199506 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 200112L
#	define JSTR_USE_XOPEN2K 1
#	undef JSTR_USE_ISOC95
#	define JSTR_USE_ISOC95 1
#	undef JSTR_USE_ISOC99
#	define JSTR_USE_ISOC99 1
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_USE_XOPEN 1
#	if (_XOPEN_SOURCE - 0) >= 500
#		define JSTR_USE_XOPEN_EXTENDED 1
#		define JSTR_USE_UNIX98         1
#		if (_XOPEN_SOURCE - 0) >= 600
#			if (_XOPEN_SOURCE - 0) >= 700
#				define JSTR_USE_XOPEN2K8    1
#				define JSTR_USE_XOPEN2K8XSI 1
#			endif
#			define JSTR_USE_XOPEN2K    1
#			define JSTR_USE_XOPEN2KXSI 1
#			undef JSTR_USE_ISOC95
#			define JSTR_USE_ISOC95 1
#			undef JSTR_USE_ISOC99
#			define JSTR_USE_ISOC99 1
#		endif
#	else
#		ifdef _XOPEN_SOURCE_EXTENDED
#			define JSTR_USE_XOPEN_EXTENDED 1
#		endif
#	endif
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#	define JSTR_USE_FILE_OFFSET64 1
#endif

#if defined _DEFAULT_SOURCE
#	define JSTR_USE_MISC 1
#endif

#ifdef _ATFILE_SOURCE
#	define JSTR_USE_ATFILE 1
#endif

#ifdef _DYNAMIC_STACK_SIZE_SOURCE
#	define JSTR_USE_DYNAMIC_STACK_SIZE 1
#endif

#ifdef _GNU_SOURCE
#	define JSTR_USE_GNU 1
#endif

#endif /* JSTR_FEATURES_H */
