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

#ifndef JSTR_IO_H
#define JSTR_IO_H 1

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
JSTR__END_DECLS

#include "jstr-builder.h"
#include "jstr-stdstring.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

typedef enum {
	/* Unknown file type. */
	JSTRIO_FT_UNKNOWN = 0,
#define JSTRIO_FT_UNKNOWN JSTRIO_FT_UNKNOWN
	/* Text file type. */
	JSTRIO_FT_TEXT,
#define JSTRIO_FT_TEXT JSTRIO_FT_TEXT
	/* Binary file type. */
	JSTRIO_FT_BINARY
#define JSTRIO_FT_BINARY JSTRIO_FT_BINARY
} jstr_io_ft_ty;

enum {
#ifdef PATH_MAX
	JSTRIO_PATH_MAX = PATH_MAX,
#else
	JSTRIO_PATH_MAX = 4096,
#endif
#define JSTRIO_PATH_MAX JSTRIO_PATH_MAX
#ifdef NAME_MAX
	JSTRIO_NAME_MAX = NAME_MAX,
#else
	JSTRIO_NAME_MAX = 255,
#endif
#define JSTRIO_NAME_MAX JSTRIO_NAME_MAX
	JSTRIO_BINARY_CHECK_MAX = 64
};

JSTR_FUNC_PURE
static jstr_io_ft_ty
jstr__io_exttype(const char *ext) JSTR_NOEXCEPT
{
	static const char *text[] = { JSTRIO_FT_TEXT_ARRAY };
	static const char *binary[] = { JSTRIO_FT_BINARY_ARRAY };
	unsigned int i;
	for (i = 0; i < JSTR_ARRAY_COUNT(text); ++i)
		if (!jstr_strcmpeq_loop(ext, text[i]))
			return JSTRIO_FT_TEXT;
	for (i = 0; i < JSTR_ARRAY_COUNT(binary); ++i)
		if (!jstr_strcmpeq_loop(ext, binary[i]))
			return JSTRIO_FT_BINARY;
	return JSTRIO_FT_UNKNOWN;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr__io_extget_len(const char *fname, size_t sz)
{
	const char *p = fname + sz - 1;
	for (; sz--; --p) {
		if (*p == '.')
			return (char *)p + 1;
		if (*p == '/')
			break;
	}
	return NULL;
}

/* Return jstr_io_ft_ty based on the FNAME extension. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstr_io_ft_ty
jstr_io_exttype(const char *R fname, size_t sz) JSTR_NOEXCEPT
{
	fname = (char *)jstr__io_extget_len(fname, sz);
	return fname ? jstr__io_exttype(fname) : JSTRIO_FT_UNKNOWN;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static int
jstr__io_isbinarysignature(const char *R buf, size_t sz)
{
	enum { ELFSZ = 4, UTFSZ = 3 };
	const unsigned char *p = (const unsigned char *)buf;
	if (jstr_likely(sz >= ELFSZ)) {
#if JSTR_HAVE_UNALIGNED_ACCESS && JSTR_HAVE_BUILTIN_MEMCMP
		static unsigned char elf[] = { 0x7, 'E', 'L', 'F' };
		JSTR_STATIC_ASSERT(sizeof(elf) == ELFSZ, "");
		if (!memcmp(p, elf, 4))
			return 0;
#else
		if (p[0] == 0x7 && p[1] == 'E' && p[2] == 'L' && p[3] == 'F')
			return 0;
#endif
check_utf:;
#if JSTR_HAVE_UNALIGNED_ACCESS && JSTR_HAVE_BUILTIN_MEMCMP
		static unsigned char utf[] = { 0xEF, 0xBB }; /* 0xBF */
		JSTR_STATIC_ASSERT(sizeof(utf) + 1 == UTFSZ, "");
		if (!memcmp(p, utf, 2) && p[2] == 0xBF)
			return 1;
#else
		if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
			return 1;
#endif
	} else if (jstr_likely(sz == UTFSZ)) {
		goto check_utf;
	}
	return -1;
}

#if JSTR_OS_WINDOWS || JSTR_OS_WINDOWSCE
#	define USE_FORM_FEED 0
#else
#	define USE_FORM_FEED 1
#endif

/* clang-format off */

/* Do not pass a non-unsigned char. */
JSTR_ATTR_MAYBE_UNUSED
static const unsigned char jstr__io_binary_table[256] = {1,1,1,1,1,1,1,1,1,0,0,1,1,USE_FORM_FEED,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

/* clang-format on */

#undef USE_FORM_FEED

/* Check if the first JSTRIO_BINARY_CHECK_MAX bytes or fewer contain any
 * unprintable char. */
JSTR_FUNC
static int
jstr_io_isbinary_maybe(const char *R buf, size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const int ret = jstr__io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	sz = JSTR_MIN(sz, JSTRIO_BINARY_CHECK_MAX);
	const unsigned char *s = (unsigned char *)buf;
	for (; sz--; ++s)
		if (jstr__io_binary_table[*s])
			return 1;
	return 0;
}

/* Check the whole file for a NUL byte.
 * File must be nul terminated. */
JSTR_FUNC_PURE
static int
jstr_io_isbinary(const char *R buf, size_t sz) JSTR_NOEXCEPT
{
	const int ret = jstr__io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return strlen(buf) != sz;
}

/* Check MIN(N, SZ) bytes for any unprintable char. */
JSTR_FUNC
static int
jstr_isbinary(const char *R buf, size_t sz, size_t n) JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const unsigned char *const end
	= (const unsigned char *)buf + JSTR_MIN(n, sz);
	const unsigned char *s = (unsigned char *)buf;
	for (; s < end; ++s)
		if (jstr__io_binary_table[*s])
			return 1;
	return 0;
}

JSTR_FUNC
static jstr_ret_ty
jstr_io_writefilefd_len(const char *R s, size_t sz, int fd) JSTR_NOEXCEPT
{
	if (jstr_unlikely((size_t)write(fd, s, sz) != sz))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (s[sz ? sz - 1 : 0] != '\n'
	    && jstr_unlikely(write(fd, "\n", 1) != 1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_writefile_len(const char *R s, size_t sz, const char *R fname, int oflag)
JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_WRONLY);
	if (jstr_unlikely(fd == -1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_io_writefilefd_len(s, sz, fd))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
static jstr_ret_ty
jstr_io_fwritefilefp_len(const char *R s, size_t sz, FILE *fp) JSTR_NOEXCEPT
{
	if (jstr_unlikely(jstr_io_fwrite(s, 1, sz, fp) != sz))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (s[sz ? sz - 1 : 0] != '\n' && jstr_io_putc('\n', fp) == EOF)
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
static jstr_ret_ty
jstr_io_fwritefile_len(const char *R s,
                      size_t sz,
                      const char *R fname,
                      const char *R modes) JSTR_NOEXCEPT
{
	FILE *R fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(jstr_io_fwritefilefp_len(s, sz, fp))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fclose(fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_readfilefd_len(char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      int fd,
                      size_t file_size) JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(file_size != (size_t)read(fd, *s, file_size)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_readfilefd(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  int fd,
                  struct stat *st) JSTR_NOEXCEPT
{
	if (jstr_unlikely(fstat(fd, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return jstr_io_readfilefd_len(s, sz, cap, fd, (size_t)st->st_size);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_freadfilefp_len(char *R *R s,
                       size_t *R sz,
                       size_t *R cap,
                       FILE *fp,
                       size_t file_size) JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(file_size
	                  != (size_t)jstr_io_fread(*s, 1, file_size, fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_freadfilefp(char *R *R s,
                   size_t *R sz,
                   size_t *R cap,
                   const char *R fname,
                   FILE *fp,
                   struct stat *st) JSTR_NOEXCEPT
{
#if JSTR_HAVE_FILENO
	const int fd = jstr_io_fileno(fp);
	if (jstr_unlikely(fd == -1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(fstat(fd, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#else
	if (jstr_unlikely(stat(fname, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#endif
	return jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size);
	(void)fname;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_freadfile_len(char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const char *R fname,
                     const char *R modes,
                     const size_t file_size) JSTR_NOEXCEPT
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(
	    jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)file_size))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fclose(fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_freadfile(char *R *R s,
                 size_t *R sz,
                 size_t *R cap,
                 const char *R fname,
                 const char *R modes,
                 struct stat *st) JSTR_NOEXCEPT
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#if JSTR_HAVE_FILENO
	int fd;
	fd = jstr_io_fileno(fp);
	if (jstr_unlikely(fd == -1)) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fstat(fd, st))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
#else
	if (jstr_unlikely(stat(fname, st))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
#endif
	if (jstr_chk(
	    jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fclose(fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

JSTR_FUNC
static jstr_ret_ty
jstr_io_readfile_len(char *R *R s,
                    size_t *R sz,
                    size_t *R cap,
                    const char *R fname,
                    int oflag,
                    size_t file_size) JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd == -1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_io_readfilefd_len(s, sz, cap, fd, file_size))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

/* Return value:
 * JSTR_RET_ERR on error;
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_io_readfile(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R fname,
                int oflag,
                struct stat *R st) JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd == -1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(fstat(fd, st))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_chk(
	    jstr_io_readfilefd_len(s, sz, cap, fd, (size_t)st->st_size))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

/* Expand ~/some_dir to /home/username/some_dir.
 * Assume that S has enough space.
 * Return value:
 * ptr to '\0' in S.
 * NULL on error. */
JSTR_FUNC
static char *
jstr_io_expandtildefirst_len_unsafe_p(char *R s, size_t sz) JSTR_NOEXCEPT
{
	if (*s != '~')
		return s + sz;
	const char *R home = getenv("HOME");
	if (jstr_nullchk(home))
		return NULL;
	const size_t len = strlen(home);
	jstr_strmove_len(s + len, s + 1, JSTR_PTR_DIFF(s + sz, s + 1));
	memcpy(s, home, len);
	return s + sz + len - 1;
}

/* Expand ~/some_dir to /home/username/some_dir.
 * Assume that S has enough space.
 * Return value:
 * ptr to '\0' in S.
 * NULL on error. */
JSTR_FUNC
static jstr_ret_ty
jstr_io_expandtildefirst(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
{
	if (**s != '~')
		return JSTR_RET_SUCC;
	const char *R home = getenv("HOME");
	if (jstr_nullchk(home))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	const size_t len = strlen(home);
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + len)))
		return JSTR_RET_ERR;
	jstr_strmove_len(*s + len, *s + 1, JSTR_PTR_DIFF(*s + *sz, *s + 1));
	memcpy(*s, home, len);
	*sz += len;
	return JSTR_RET_SUCC;
}

/* Expand every ~ to /home/username.
 * Assume that S has enough space.
 * Return value:
 * ptr to '\0' in S;
 * NULL on error. */
JSTR_FUNC
static char *
jstr_io_expandtilde_len_unsafe_p(char *R s, size_t sz) JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_nullchk(home))
		return NULL;
	const size_t len = strlen(home);
	char *p = s;
	while ((p = (char *)memchr(p, '~', JSTR_PTR_DIFF(s + sz, p)))) {
		jstr_strmove_len(p + len, p + 1, JSTR_PTR_DIFF(s + sz, p + 1));
		memcpy(p, home, len);
		p += len;
		sz += (len - 1);
	}
	return s + sz;
}

/* Expand every ~ to /home/username.
 * Return value:
 * JSTR_RET_ERR on error;
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_io_expandtilde(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_nullchk(home))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	const size_t len = strlen(home);
	const char *tmp;
	char *p = *s;
	while ((p = (char *)memchr(p, '~', JSTR_PTR_DIFF(*s + *sz, p)))) {
		if (jstr_unlikely(*sz + len >= *cap)) {
			tmp = *s;
			if (jstr_chk(jstr_reservealways(s, sz, cap, *sz + len)))
				return JSTR_RET_ERR;
			p = *s + (p - tmp);
		}
		jstr_strmove_len(
		p + len, p + 1, JSTR_PTR_DIFF(*s + *sz, p + 1));
		memcpy(p, home, len);
		p += len;
		*sz += (len - 1);
	}
	return JSTR_RET_SUCC;
}

#if JSTR_HAVE_POPEN

JSTR_FUNC
static jstr_ret_ty
jstr_io_readsystem(char *R *R s, size_t *R sz, size_t *R cap, const char *R cmd)
JSTR_NOEXCEPT
{
	enum { MINBUF = JSTR_PAGE_SIZE };
	FILE *R fp = popen(cmd, "r");
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char buf[MINBUF];
	size_t readsz;
	readsz = fread(buf, 1, sizeof(buf), fp);
	if (jstr_unlikely(readsz == (size_t)-1)) {
		pclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_chk(jstr_reserve(s, sz, cap, readsz * 2))) {
		pclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	memcpy(*s, buf, readsz);
	*sz = readsz;
	if (jstr_unlikely(readsz == MINBUF)) {
		size_t reqsz;
		for (;;) {
			reqsz = *cap - *sz;
			readsz = fread(*s + *sz, 1, reqsz, fp);
			if (jstr_unlikely(readsz == (size_t)-1)) {
				pclose(fp);
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
			*sz += readsz;
			if (readsz < reqsz)
				break;
			if (*sz == *cap)
				if (jstr_chk(jstr_reserveexactalways(
				    s,
				    sz,
				    cap,
				    (size_t)(*cap * JSTR_GROWTH)))) {
					pclose(fp);
					JSTR_RETURN_ERR(JSTR_RET_ERR);
				}
		}
	}
	*(*s + *sz) = '\0';
	if (jstr_unlikely(pclose(fp) == -1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}

#endif

JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_io_appendpath_len_p(char *R path,
                        size_t sz,
                        const char *R fname,
                        size_t fname_len)
{
	*path = '/';
	jstr_strcpy_len(path + sz + 1, fname, fname_len);
	return path + sz + 1 + fname_len;
}

JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_io_appendpath_p(char *R path, size_t sz, const char *R fname)
{
	*(path + sz) = '/';
	return jstr_stpcpy(path + sz + 1, fname);
}

JSTR_FUNC
static jstr_ret_ty
jstr_io_appendpath_len(char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      const char *R fname,
                      size_t fname_len)
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + fname_len)))
		return JSTR_RET_ERR;
	*sz
	= JSTR_PTR_DIFF(jstr_io_appendpath_len_p(*s, *sz, fname, fname_len), *s);
	return JSTR_RET_SUCC;
}

#ifdef _DIRENT_HAVE_D_NAMLEN
#	ifndef _D_EXACT_NAMLEN
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) ((d)->d_namlen)
#	else
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) _D_EXACT_NAMLEN(d)
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		define JSTR_DIRENT_D_ALLOC_NAMLEN(d)                          \
			(JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
#	else
#		define JSTR_DIRENT_D_ALLOC_NAMLEN(d) _D_ALLOC_NAMLEN(d)
#	endif
#else
#	ifndef _D_EXACT_NAMLEN
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) (strlen((d)->d_name))
#	else
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) _D_EXACT_NAMLEN(d)
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		define JSTR_DIRENT_D_ALLOC_NAMLEN(c) _D_ALLOC_NAMLEN(d)
#	else
#		ifdef _DIRENT_HAVE_D_RECLEN
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d)                  \
				(((char *)(d) + (d)->d_reclen)                 \
				 - &(d)->d_name[0])
#		else
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d)                  \
				(sizeof(d)->d_name > 1                         \
				 ? sizeof(d)->d_name                           \
				 : JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
#		endif
#	endif
#endif

/* If JSTRIO_ACTION_RETVAL is passed, use these as return values of FUNC(). */
typedef enum jstr_io_ftw_actionretval_ty {
	JSTRIO_FTW_RET_STOP = 0,
#define JSTRIO_FTW_RET_STOP JSTRIO_FTW_RET_STOP
	JSTRIO_FTW_RET_CONTINUE,
#define JSTRIO_FTW_RET_CONTINUE JSTRIO_FTW_RET_CONTINUE
	/* Skip sibling entries. */
	JSTRIO_FTW_RET_SKIP_SIBLINGS,
#define JSTRIO_FTW_RET_SKIP_SIBLINGS JSTRIO_FTW_RET_SKIP_SIBLINGS
	/* Skip subdirectories. */
	JSTRIO_FTW_RET_SKIP_SUBTREE
#define JSTRIO_FTW_RET_SKIP_SUBTREE JSTRIO_FTW_RET_SKIP_SUBTREE
} jstr_io_ftw_actionretval_ty;

typedef enum jstr_io_ftw_flag_ty {
	/* Match glob with FULPATH instead of d_name. */
	JSTRIO_FTW_MATCHPATH = (1),
#define JSTRIO_FTW_MATCHPATH JSTRIO_FTW_MATCHPATH
	/* Call FUNC() on regular files. */
	JSTRIO_FTW_REG = (JSTRIO_FTW_MATCHPATH << 1),
#define JSTRIO_FTW_REG JSTRIO_FTW_REG
	/* Call FUNC() on directories. */
	JSTRIO_FTW_DIR = (JSTRIO_FTW_REG << 1),
#define JSTRIO_FTW_DIR JSTRIO_FTW_DIR
	/* Do not traverse subdirectories. */
	JSTRIO_FTW_NOSUBDIR = (JSTRIO_FTW_DIR << 1),
#define JSTRIO_FTW_NOSUBDIR JSTRIO_FTW_NOSUBDIR
	/* Do not call stat. Only st.st_mode is defined. */
	JSTRIO_FTW_NOSTAT = (JSTRIO_FTW_NOSUBDIR << 1),
#define JSTRIO_FTW_NOSTAT JSTRIO_FTW_NOSTAT
	/* Only call stat on regular files. */
	JSTRIO_FTW_STATREG = (JSTRIO_FTW_NOSTAT << 1),
#define JSTRIO_FTW_STATREG JSTRIO_FTW_STATREG
	/* Ignore hidden entries. */
	JSTRIO_FTW_NOHIDDEN = (JSTRIO_FTW_STATREG << 1),
#define JSTRIO_FTW_NOHIDDEN JSTRIO_FTW_NOHIDDEN
	/* Handle FUNC() return value according to jstr_io_ftw_actionretval_ty.
	 */
	JSTRIO_FTW_ACTIONRETVAL = (JSTRIO_FTW_NOHIDDEN << 1)
#define JSTRIO_FTW_ACTIONRETVAL JSTRIO_FTW_ACTIONRETVAL
} jstr_io_ftw_flag_ty;

#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#	define USE_ATFILE 1
#else
#	define USE_ATFILE 0
#endif

#define NONFATAL_ERR()                                                         \
	(jstr_likely(errno == EACCES) || jstr_likely(errno == ENOENT))

#if JSTR_HAVE_DIRENT_D_NAMLEN
#	define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)        \
		do {                                                           \
			*(dirpath + dirpath_len) = '/';                        \
			jstr_strcpy_len(dirpath + dirpath_len + 1,             \
			                (ep)->d_name,                          \
			                (ep)->d_namlen);                       \
			newpath_len = dirpath_len + 1 + (ep)->d_namlen;        \
		} while (0)
#else
#	define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)        \
		do {                                                           \
			*(dirpath + dirpath_len) = '/';                        \
			newpath_len = JSTR_PTR_DIFF(                           \
			jstr_stpcpy(dirpath + dirpath_len + 1, (ep)->d_name),  \
			dirpath);                                              \
		} while (0)
#endif

#if USE_ATFILE
#	define STAT_DO(st, fd, ep, dirpath, do_on_nonfatal_err)               \
		do {                                                           \
			if (jstr_unlikely(fstatat(fd, (ep)->d_name, st, 0))) { \
				if (NONFATAL_ERR()) {                          \
					do_on_nonfatal_err;                    \
				}                                              \
				goto err_closedir;                             \
			}                                                      \
		} while (0)
#	define OPENAT(dstfd, srcfd, file, oflag, do_on_err)                   \
		do {                                                           \
			if (jstr_unlikely((dstfd = openat(srcfd, file, oflag)) \
			                  == -1)) {                            \
				do_on_err;                                     \
			}                                                      \
		} while (0)
#	define OPEN(fd, file, oflag, do_on_err)                               \
		do {                                                           \
			if (jstr_unlikely((fd = open(file, oflag)) == -1)) {   \
				do_on_err;                                     \
			}                                                      \
		} while (0)
#	define CLOSE(fd, do_on_err)                                           \
		do {                                                           \
			if (jstr_unlikely(close(fd))) {                        \
				do_on_err;                                     \
			}                                                      \
		} while (0)
#	define STAT_ALWAYS(st, fd, ep, dirpath)                               \
		STAT_DO(st, fd, ep, dirpath = JSTRIO_FTW_STATE_NS; goto do_fn)
#	define OPENDIR(fd, fname) fdopendir(fd)
#else
#	define STAT_DO(st, fd, ep, dirpath, do_on_nonfatal_err)               \
		do {                                                           \
			if (jstr_unlikely(stat(dirpath, st))) {                \
				if (NONFATAL_ERR()) {                          \
					do_on_nonfatal_err;                    \
				}                                              \
				goto err_closedir;                             \
			}                                                      \
		} while (0)
#	define STAT_ALWAYS(st, fd, ep, dirpath)                               \
		STAT_DO(st, fd, ep, dirpath = JSTRIO_FTW_STATE_NS; goto do_fn)
#	define OPENDIR(fd, fname) opendir(fname)
#	define OPENAT(dstfd, srcfd, file, oflag, do_on_err)                   \
		do {                                                           \
		} while (0)
#	define OPEN(fd, file, oflag, do_on_err)                               \
		do {                                                           \
		} while (0)
#	define CLOSE(fd, do_on_err)                                           \
		do {                                                           \
		} while (0)
#endif

#if JSTR_HAVE_DIRENT_D_TYPE
#	define IS_DIR(ep, st)    ((ep)->d_type == DT_DIR)
#	define IS_REG(ep, st)    ((ep)->d_type == DT_REG)
#	define STAT_MODE(st, ep) ((void)((st)->st_mode = DTTOIF((ep)->d_type)))
#	define FILL_PATH(newpath_len, dirpath, dirpath_len, ep)               \
		FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#	define STAT(st, fd, ep, dirpath) STAT_ALWAYS(st, fd, ep, dirpath)
#	define STAT_OR_MODE(st, fd, ep, dirpath)                              \
		do {                                                           \
			if (FLAG(JSTRIO_FTW_NOSTAT))                           \
				STAT_MODE(st, ep);                             \
			else                                                   \
				STAT(st, fd, ep, dirpath);                     \
		} while (0)
#else
#	define IS_DIR(ep, st) S_ISDIR((st)->st_mode)
#	define IS_REG(ep, st) S_ISREG((st)->st_mode)
#	if USE_ATFILE
#		define FILL_PATH(newpath_len, dirpath, dirpath_len, ep)       \
			FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#	else
#		define FILL_PATH(newpath_len, dirpath, dirpath_len, ep)       \
			do {                                                   \
			} while (0)
#	endif
#	define STAT(st, fd, ep, dirpath)                                      \
		do {                                                           \
		} while (0)
#	define STAT_MODE(st, ep)                                              \
		do {                                                           \
		} while (0)
#	define STAT_OR_MODE(st, fd, ep, dirpath)                              \
		do {                                                           \
		} while (0)
#endif

#if USE_ATFILE
#	define FD         fd
#	define FD_PARAM   , int fd
#	define FD_ARG     , fd
#	define FD_DECLARE int fd
#else
#	define FD
#	define FD_PARAM
#	define FD_ARG
#	define FD_DECLARE                                                     \
		do {                                                           \
		} while (0)
#endif

#if JSTRIO_PATH_MAX <= 65536
typedef uint16_t jstr_io_path_size_ty;
#elif JSTRIO_PATH_MAX <= 4294967296
typedef uint32_t jstr_io_path_size_ty;
#else
typedef size_t jstr_io_path_size_ty;
#endif

struct JSTRIO_FTW {
	const char *dirpath;
	const struct stat *st;
	const struct dirent *ep;
	jstr_io_path_size_ty dirpath_len;
};

typedef int (*jstr_io_ftw_func_ty)(const struct JSTRIO_FTW *ftw,
                                  const void *args);
typedef int (*jstr_io_ftw_func_match_ty)(const char *fname,
                                        jstr_io_path_size_ty fname_len,
                                        const void *args);

struct jstr__io_ftw_data {
	jstr_io_ftw_func_ty func;
	const void *func_args;
	jstr_io_ftw_func_match_ty func_match;
	const void *func_match_args;
	struct JSTRIO_FTW ftw;
	int ftw_flags;
};

#define JSTRIO_FTW_FUNC(func_name, ftw, func_args)                             \
	int func_name(const struct JSTRIO_FTW *ftw, const void *func_args)

#define JSTRIO_FTW_FUNC_MATCH(func_name, filename, filename_len, args)         \
	int func_name(const char *filename,                                    \
	              jstr_io_path_size_ty filename_len,                        \
	              const void *args)

#ifdef O_DIRECTORY
#	define JSTRIO__O_DIRECTORY O_DIRECTORY
#else
#	define JSTRIO__O_DIRECTORY 0
#endif

#define FLAG(x) ((a)->ftw_flags & (x))

/* We try to avoid stat when d_type is available, and avoid constructing the
 * full path when we can use *_at functions. */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
static int
jstr__io_ftw_len(struct jstr__io_ftw_data *a,
                jstr_io_path_size_ty dirpath_len FD_PARAM) JSTR_NOEXCEPT
{
	DIR *R const dp = OPENDIR(fd, a->ftw.dirpath);
	if (jstr_nullchk(dp)) {
		if (NONFATAL_ERR()) {
			if (FLAG(JSTRIO_FTW_REG))
				if (!FLAG(JSTRIO_FTW_DIR))
					return JSTR_RET_SUCC;
			if (a->func_match
			    && a->func_match(a->ftw.dirpath,
			                     a->ftw.dirpath_len,
			                     a->func_match_args))
				return JSTR_RET_SUCC;
			a->ftw.dirpath_len = dirpath_len;
			a->func(&a->ftw, a->func_args);
			return JSTR_RET_SUCC;
		}
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	int tmp;
	while ((a->ftw.ep = readdir(dp))) {
		/* Ignore hidden files. */
		if (FLAG(JSTRIO_FTW_NOHIDDEN)) {
			if (a->ftw.ep->d_name[0] == '.')
				continue;
			/* Ignore "." and "..". */
		} else {
			if (a->ftw.ep->d_name[0] == '.'
			    && (a->ftw.ep->d_name[1] == '\0'
			        || (a->ftw.ep->d_name[1] == '.'
			            && a->ftw.ep->d_name[2] == '\0')))
				continue;
		}
		/* Stop processing if DIRPATH is longer than PATH_MAX.
		   If we don't have d_namlen, try to estimate the length. */
		if (JSTR_HAVE_DIRENT_D_NAMLEN) {
			if (jstr_unlikely(
			    dirpath_len + JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep)
			    >= JSTRIO_PATH_MAX)) {
				errno = ENAMETOOLONG;
				goto err_closedir;
			}
		} else {
			if (jstr_unlikely(dirpath_len
			                  >= JSTRIO_PATH_MAX - JSTRIO_NAME_MAX)
			    && jstr_unlikely(
			    dirpath_len + JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep)
			    >= JSTRIO_PATH_MAX)) {
				errno = ENAMETOOLONG;
				goto err_closedir;
			}
		}
		/* We must stat to get the type if we don't have d_type. */
		if (!JSTR_HAVE_DIRENT_D_TYPE) {
			/* We must construct the full path for stat if we
			 * don't have *_at functions. */
			if (!USE_ATFILE)
				FILL_PATH_ALWAYS(a->ftw.dirpath_len,
				                 (char *)a->ftw.dirpath,
				                 dirpath_len,
				                 a->ftw.ep);
			STAT_DO(
			(struct stat *)a->ftw.st,
			fd,
			a->ftw.ep,
			a->ftw.dirpath,
			/* If stat fails. */
			if (FLAG(JSTRIO_FTW_DIR | JSTRIO_FTW_REG)) goto CONT;
			else goto func;);
		}
		if (IS_REG(a->ftw.ep, a->ftw.st))
			goto reg;
		if (IS_DIR(a->ftw.ep, a->ftw.st))
			goto dir;
		/* If true, ignore other types of files. */
		if (FLAG(JSTRIO_FTW_DIR | JSTRIO_FTW_REG))
			continue;
		goto do_reg;
reg:
		if (FLAG(JSTRIO_FTW_DIR))
			if (!(FLAG(JSTRIO_FTW_REG)))
				continue;
do_reg:
		if (a->func_match) {
			if (FLAG(JSTRIO_FTW_MATCHPATH)) {
				/* FILL_PATH() will construct the full path if
				 * either *_at functions or d_type aren't
				 * available. */
				FILL_PATH(a->ftw.dirpath_len,
				          (char *)a->ftw.dirpath,
				          dirpath_len,
				          a->ftw.ep);
				if (a->func_match(a->ftw.dirpath,
				                  a->ftw.dirpath_len,
				                  a->func_match_args))
					continue;
			} else {
				const size_t fname_len
				= JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep);
				if (a->func_match(a->ftw.ep->d_name,
				                  fname_len,
				                  a->func_match_args))
					continue;
				/* We haven't constructed the full path since we
				 * have *_at functions. */
				if (USE_ATFILE)
					a->ftw.dirpath_len
					= jstr_io_appendpath_len_p(
					  (char *)a->ftw.dirpath,
					  dirpath_len,
					  a->ftw.ep->d_name,
					  fname_len)
					  - a->ftw.dirpath;
			}
		} else {
			/* Construct the full path when needed. If we don't have
			 * *_at functions, we have already constructed it and
			 * this will be a no-op. */
			FILL_PATH(a->ftw.dirpath_len,
			          (char *)a->ftw.dirpath,
			          dirpath_len,
			          a->ftw.ep);
		}
		if (FLAG(JSTRIO_FTW_STATREG)) {
			/* We must stat. */
			if (IS_REG(a->ftw.ep, a->ftw.st))
				STAT((struct stat *)a->ftw.st,
				     fd,
				     a->ftw.ep,
				     a->ftw.dirpath);
			/* Only the st_mode is guaranteed to be initialized.
			 * This is so that we can avoid the stat when d_type is
			 * available. */
			else
				STAT_MODE((struct stat *)a->ftw.st, a->ftw.ep);
		} else {
			/* If the NOSTAT flag is passed, only guarantee st_mode.
			 */
			STAT_OR_MODE((struct stat *)a->ftw.st,
			             fd,
			             a->ftw.ep,
			             a->ftw.dirpath);
		}
func:
		tmp = a->func(&a->ftw, a->func_args);
		/* ACTIONRETVAL according to glibc ftw. */
		if (FLAG(JSTRIO_FTW_ACTIONRETVAL)) {
			if (tmp == JSTRIO_FTW_RET_CONTINUE
			    || tmp == JSTRIO_FTW_RET_SKIP_SUBTREE)
				continue;
			else if (tmp == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* tmp == RET_STOP */
				goto ret_stop;
		} else {
			if (jstr_chk(tmp))
				goto err_closedir;
		}
		continue;
dir:
		/* Go to next entry if we don't traverse subdirectories and
		 * don't need to call FUNC on directories. */
		if (FLAG(JSTRIO_FTW_NOSUBDIR) && FLAG(JSTRIO_FTW_REG)
		    && !FLAG(JSTRIO_FTW_DIR))
			continue;
		FILL_PATH(a->ftw.dirpath_len,
		          (char *)a->ftw.dirpath,
		          dirpath_len,
		          a->ftw.ep);
		if (FLAG(JSTRIO_FTW_STATREG))
			STAT_MODE(a->ftw.st, a->ftw.ep);
		else
			STAT_OR_MODE(a->ftw.st, fd, a->ftw.ep, a->ftw.dirpath);
		/* Don't call FUNC on directory if we don't need to. */
		if (FLAG(JSTRIO_FTW_REG) && !FLAG(JSTRIO_FTW_DIR))
			goto skip_fn;
		tmp = a->func(&a->ftw, a->func_args);
		if (FLAG(JSTRIO_FTW_ACTIONRETVAL)) {
			if (tmp == JSTRIO_FTW_RET_CONTINUE)
				continue;
			else if (tmp == JSTRIO_FTW_RET_SKIP_SUBTREE
			         || tmp == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* tmp == RET_STOP */
				goto ret_stop;
		} else {
			if (jstr_chk(tmp))
				goto err_closedir;
		}
skip_fn:
		/* Don't traverse subdirectories when we don't need to. */
		if (FLAG(JSTRIO_FTW_NOSUBDIR))
			continue;
		/* If we have *_at functions, open d_name to get the fd.
		 * Otherwise, no-op. */
		OPENAT(tmp,
		       fd,
		       a->ftw.ep->d_name,
		       O_RDONLY | O_NONBLOCK | JSTRIO__O_DIRECTORY,
		       goto CONT);
		tmp = jstr__io_ftw_len(a, a->ftw.dirpath_len FD_ARG);
		/* Close when we have *_at functions. */
		CLOSE(FD, goto err_closedir);
		if (FLAG(JSTRIO_FTW_ACTIONRETVAL)) {
			if (jstr_unlikely(tmp == JSTRIO_FTW_RET_STOP))
				goto ret_stop;
		} else {
			if (jstr_chk(tmp))
				goto err_closedir;
		}
CONT:;
	}
	closedir(dp);
	return JSTR_RET_SUCC;
ret_stop:
	closedir(dp);
	return JSTRIO_FTW_RET_STOP;
err_closedir:
	closedir(dp);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#undef OPENDIR
#undef NONFATAL_ERR
#undef IS_DIR
#undef IS_REG
#undef FILL_PATH
#undef FILL_PATH_ALWAYS
#undef STAT_DO
#undef STAT
#undef STAT_ALWAYS
#undef STAT_MODE
#undef STAT_OR_MODE
#undef FD
#undef FD_PARAM
#undef JSTRIO__O_DIRECTORY
#undef FLAG

#define FLAG(x) (jstr_io_ftw_flags & (x))

/* Call FUNC() on files found recursively where FUNC_MATCH() returns 0.
 * If FUNC_MATCH() is NULL, it behaves as if it matches.
 * If FUNC() returns JSTR_RET_ERR, stop processing.
 * Return value:
 * JSTR_RET_ERR on error;
 * JSTR_RET_SUCC on success or non-fatal errors (EACCES or ENOENT) encountered
 * on some entries; JSTR_RET_STOP if FUNC() returns RET_STOP and
 * JSTRIO_FTW_ACTIONRETVAL is used. or the return value of FUNC() if DIRPATH is
 * not a directory and FUNC() is executed. If a non-fatal error is encountered,
 * continue processing other entries. */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((4))
static int
jstr_io_ftw_len(const char *R dirpath,
               jstr_io_path_size_ty dirpath_len,
               jstr_io_ftw_func_ty func,
               const void *func_args,
               int jstr_io_ftw_flags,
               jstr_io_ftw_func_match_ty func_match,
               const void *func_match_args) JSTR_NOEXCEPT
{
	if (jstr_unlikely(dirpath_len == 0)) {
		errno = ENOENT;
		goto err;
	}
	if (jstr_unlikely(dirpath_len >= JSTRIO_PATH_MAX)) {
		errno = ENAMETOOLONG;
		goto err;
	}
	/* Don't copy trailing // to FULPATH. */
	for (; dirpath_len != 1 && dirpath[dirpath_len - 1] == '/';
	     --dirpath_len) {}
	char fulpath[JSTRIO_PATH_MAX];
	jstr_strcpy_len(fulpath, dirpath, dirpath_len);
	FD_DECLARE;
	OPEN(fd, fulpath, O_RDONLY | O_NONBLOCK, goto err);
	struct stat st;
	struct jstr__io_ftw_data data;
	data.ftw.dirpath = fulpath;
	data.ftw.st = &st;
	/* This will avoid things like //some/path if DIRPATH is /. */
	if (jstr_unlikely(dirpath_len == 1) && jstr_unlikely(*dirpath == '/')) {
		data.ftw.dirpath_len = 0;
		goto ftw;
	}
	data.ftw.dirpath_len = dirpath_len;
#if USE_ATFILE
	if (jstr_unlikely(fstat(fd, &st)))
		goto func;
#else
	if (jstr_unlikely(stat(fulpath, &st)))
		goto func;
#endif
	/* If DIRPATH is a directory, call FUNC on directory when needed and
	 * call ftw. */
	if (jstr_likely(S_ISDIR(data.ftw.st->st_mode))) {
ftw:;
		if (FLAG(JSTRIO_FTW_REG))
			if (!FLAG(JSTRIO_FTW_DIR))
				goto CONT;
		int tmp;
		tmp = func(&data.ftw, func_args);
		if (FLAG(JSTRIO_FTW_ACTIONRETVAL)) {
			if (jstr_unlikely(tmp != JSTRIO_FTW_RET_CONTINUE))
				goto err_close;
		} else {
			if (jstr_chk(tmp))
				goto err_close;
		}
CONT:;
		data.func = func;
		data.func_args = func_args;
		data.func_match = func_match;
		data.ftw_flags = jstr_io_ftw_flags;
		data.func_match_args = func_match_args;
		tmp = jstr__io_ftw_len(&data, dirpath_len FD_ARG);
		CLOSE(fd, goto err);
		return tmp;
	}
func:
	/* DIRPATH is not a directory. */
	CLOSE(fd, goto err);
	if (FLAG(JSTRIO_FTW_REG))
		if (jstr_unlikely(!S_ISREG(data.ftw.st->st_mode)))
			return JSTR_RET_SUCC;
	if (func_match) {
		if (FLAG(JSTRIO_FTW_MATCHPATH)) {
			if (func_match(fulpath, dirpath_len, func_match_args))
				return JSTR_RET_SUCC;
		} else {
			dirpath
			= (char *)jstr_memrchr(fulpath, '/', dirpath_len);
			const char *const end = (char *)dirpath + dirpath_len;
			if (dirpath) {
				if (*++dirpath
				    && func_match(
				    dirpath, end - dirpath, func_match_args))
					return JSTR_RET_SUCC;
			} else {
				if (func_match(
				    fulpath, dirpath_len, func_match_args))
					return JSTR_RET_SUCC;
			}
		}
	}
	return (jstr_ret_ty)func(&data.ftw, func_args);
err_close:
	CLOSE(fd, );
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#undef OPEN
#undef OPENAT
#undef CLOSE
#undef USE_ATFILE
#undef FD_DECLARE
#undef FD_ARG
#undef FLAG

JSTR__END_DECLS

#undef R

#endif /* JSTR_IO_H */
