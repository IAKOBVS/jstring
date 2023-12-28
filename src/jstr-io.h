/* SPDX-License-Identifier: MIT */
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
   SOFTWARE. */

#ifndef JSTRIO_H
#define JSTRIO_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-stdstring.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

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
} jstrio_ft_ty;

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
static jstrio_ft_ty
pjstrio_exttype(const char *ext)
JSTR_NOEXCEPT
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
pjstrio_extget_len(const char *fname,
                   size_t sz)
{
	const char *p = fname + sz - 1;
	for (; p > fname; --p) {
		if (*p == '.')
			return (char *)p + 1;
		if (*p == '/')
			break;
	}
	return NULL;
}

/* Return jstrio_ft_ty based on the FNAME extension. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstrio_ft_ty
jstrio_exttype(const char *R fname,
               size_t sz)
JSTR_NOEXCEPT
{
	fname = (char *)pjstrio_extget_len(fname, sz);
	return fname ? pjstrio_exttype(fname) : JSTRIO_FT_UNKNOWN;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static int
pjstrio_isbinarysignature(const char *R buf,
                          size_t sz)
{
	enum { ELFSZ = 4,
	       UTFSZ = 3 };
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

/* clang-format off */

#if JSTR_OS_WINDOWS || JSTR_OS_WINDOWSCE
#	define USE_FORM_FEED 0
#else
#	define USE_FORM_FEED 1
#endif

/* Do not pass a non-unsigned char. */
JSTR_ATTR_MAYBE_UNUSED
static const unsigned char pjstrio_binary_table[256] = {1,1,1,1,1,1,1,1,1,0,0,1,1,USE_FORM_FEED,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

#undef USE_FORM_FEED

/* clang-format on */

/* Check if the first JSTRIO_BINARY_CHECK_MAX bytes or fewer contain any unprintable char. */
JSTR_FUNC
static int
jstrio_isbinary_maybe(const char *R buf,
                      size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const int ret = pjstrio_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(sz, JSTRIO_BINARY_CHECK_MAX) + 1;
	const unsigned char *s = (unsigned char *)buf;
	for (; s < end; ++s)
		if (pjstrio_binary_table[*s])
			return 1;
	return 0;
}

/* Check the whole file for a NUL byte.
   File must be nul terminated. */
JSTR_FUNC_PURE
static int
jstrio_isbinary(const char *R buf,
                size_t sz)
JSTR_NOEXCEPT
{
	const int ret = pjstrio_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return strlen(buf) != sz;
}

/* Check MIN(N, SZ) bytes for any unprintable char. */
JSTR_FUNC
static int
jstr_isbinary(const char *R buf,
              size_t sz,
              size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(n, sz);
	const unsigned char *s = (unsigned char *)buf;
	for (; s < end; ++s)
		if (pjstrio_binary_table[*s])
			return 1;
	return 0;
}

JSTR_FUNC
static jstr_ret_ty
jstrio_writefilefd_len(const char *R s,
                       size_t sz,
                       int fd)
JSTR_NOEXCEPT
{
	if (jstr_unlikely((size_t)write(fd, s, sz) != sz))
		goto err;
	if (s[sz ? sz - 1 : 0] != '\n'
	    && jstr_unlikely(write(fd, "\n", 1) != 1))
		goto err;
	return JSTR_RET_SUCC;
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_writefile_len(const char *R s,
                     size_t sz,
                     const char *R fname,
                     int oflag)
JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_WRONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_chk(jstrio_writefilefd_len(s, sz, fd)))
		goto err_close;
	if (jstr_unlikely(close(fd)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	close(fd);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrio_fwritefilefp_len(const char *R s,
                        size_t sz,
                        FILE *fp)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(jstrio_fwrite(s, 1, sz, fp) != sz))
		goto err;
	if (s[sz ? sz - 1 : 0] != '\n'
	    && jstrio_putc('\n', fp) == EOF)
		goto err;
	return JSTR_RET_SUCC;
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrio_fwritefile_len(const char *R s,
                      size_t sz,
                      const char *R fname,
                      const char *R modes)
JSTR_NOEXCEPT
{
	FILE *R fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		goto err;
	if (jstr_unlikely(jstrio_fwritefilefp_len(s, sz, fp)))
		goto err_close;
	if (jstr_unlikely(fclose(fp)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	fclose(fp);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_readfilefd_len(char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      int fd,
                      size_t file_size)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size)))
		goto err;
	if (jstr_unlikely(file_size != (size_t)read(fd, *s, file_size)))
		goto err;
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_readfilefd(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  int fd,
                  struct stat *st)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(fstat(fd, st)))
		goto err;
	return jstrio_readfilefd_len(s, sz, cap, fd, (size_t)st->st_size);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_freadfilefp_len(char *R *R s,
                       size_t *R sz,
                       size_t *R cap,
                       FILE *fp,
                       size_t file_size)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size)))
		goto err;
	if (jstr_unlikely(file_size != (size_t)jstrio_fread(*s, 1, file_size, fp)))
		goto err;
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_freadfilefp(char *R *R s,
                   size_t *R sz,
                   size_t *R cap,
                   const char *R fname,
                   FILE *fp,
                   struct stat *st)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_FILENO
	const int fd = jstrio_fileno(fp);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_unlikely(fstat(fd, st)))
		goto err;
#else
	if (jstr_unlikely(stat(fname, st)))
		goto err;
#endif
	return jstrio_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
	(void)fname;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_freadfile_len(char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const char *R fname,
                     const char *R modes,
                     const size_t file_size)
JSTR_NOEXCEPT
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		goto err;
	if (jstr_chk(jstrio_freadfilefp_len(s, sz, cap, fp, (size_t)file_size)))
		goto err_close;
	if (jstr_unlikely(fclose(fp)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	fclose(fp);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_freadfile(char *R *R s,
                 size_t *R sz,
                 size_t *R cap,
                 const char *R fname,
                 const char *R modes,
                 struct stat *st)
JSTR_NOEXCEPT
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		goto err;
#if JSTR_HAVE_FILENO
	int fd;
	fd = jstrio_fileno(fp);
	if (jstr_unlikely(fd == -1))
		goto err_close;
	if (jstr_unlikely(fstat(fd, st)))
		goto err_close;
#else
	if (jstr_unlikely(stat(fname, st)))
		goto err_close;
#endif
	if (jstr_chk(jstrio_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size)))
		goto err_close;
	if (jstr_unlikely(fclose(fp)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	fclose(fp);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrio_readfile_len(char *R *R s,
                    size_t *R sz,
                    size_t *R cap,
                    const char *R fname,
                    int oflag,
                    size_t file_size)
JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_chk(jstrio_readfilefd_len(s, sz, cap, fd, file_size)))
		goto err_close;
	if (jstr_unlikely(close(fd)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	close(fd);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Return value:
   JSTR_RET_ERR on error;
   otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstrio_readfile(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R fname,
                int oflag,
                struct stat *R st)
JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_unlikely(fstat(fd, st)))
		goto err_close;
	if (jstr_chk(jstrio_readfilefd_len(s, sz, cap, fd, (size_t)st->st_size)))
		goto err_close;
	if (jstr_unlikely(close(fd)))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	close(fd);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Expand ~/some_dir to /home/username/some_dir.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error. */
JSTR_FUNC
static char *
jstrio_expandtildefirst_len_unsafe_p(char *R s,
                                     size_t sz)
JSTR_NOEXCEPT
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
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error. */
JSTR_FUNC
static jstr_ret_ty
jstrio_expandtildefirst(char *R *R s,
                        size_t *R sz,
                        size_t *R cap)
JSTR_NOEXCEPT
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
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S;
   NULL on error. */
JSTR_FUNC
static char *
jstrio_expandtilde_len_unsafe_p(char *R s,
                                size_t sz)
JSTR_NOEXCEPT
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
   Return value:
   JSTR_RET_ERR on error;
   otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstrio_expandtilde(char *R *R s,
                   size_t *R sz,
                   size_t *R cap)
JSTR_NOEXCEPT
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
		jstr_strmove_len(p + len, p + 1, JSTR_PTR_DIFF(*s + *sz, p + 1));
		memcpy(p, home, len);
		p += len;
		*sz += (len - 1);
	}
	return JSTR_RET_SUCC;
}

#if JSTR_HAVE_POPEN

JSTR_FUNC
static jstr_ret_ty
jstrio_readsystem(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  const char *R cmd)
JSTR_NOEXCEPT
{
	enum { MINBUF = JSTR_PAGE_SIZE };
	FILE *R fp = popen(cmd, "r");
	if (jstr_nullchk(fp))
		goto err;
	char buf[MINBUF];
	size_t readsz;
	readsz = fread(buf, 1, sizeof(buf), fp);
	if (jstr_unlikely(readsz == (size_t)-1))
		goto err_close;
	if (jstr_chk(jstr_reserve(s, sz, cap, readsz * 2)))
		goto err_close;
	memcpy(*s, buf, readsz);
	*sz = readsz;
	if (jstr_unlikely(readsz == MINBUF)) {
		size_t reqsz;
		for (;;) {
			reqsz = *cap - *sz;
			readsz = fread(*s + *sz, 1, reqsz, fp);
			if (jstr_unlikely(readsz == (size_t)-1))
				goto err_close;
			*sz += readsz;
			if (readsz < reqsz)
				break;
			if (*sz == *cap)
				if (jstr_chk(jstr_reserveexactalways(s, sz, cap, (size_t)(*cap * JSTR_GROWTH))))
					goto err_close;
		}
	}
	*(*s + *sz) = '\0';
	if (jstr_unlikely(pclose(fp) == -1))
		goto err;
	return JSTR_RET_SUCC;
err_close:
	pclose(fp);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#endif

JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstrio_appendpath_len_p(char *R path,
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
jstrio_appendpath_p(char *R path,
                    size_t sz,
                    const char *R fname)
{
	*(path + sz) = '/';
	return jstr_stpcpy(path + sz + 1, fname);
}

JSTR_FUNC
static jstr_ret_ty
jstrio_appendpath_len(char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      const char *R fname,
                      size_t fname_len)
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + fname_len)))
		return JSTR_RET_ERR;
	*sz = JSTR_PTR_DIFF(jstrio_appendpath_len_p(*s, *sz, fname, fname_len), *s);
	return JSTR_RET_SUCC;
}

#ifdef _DIRENT_HAVE_D_NAMLEN
#	ifndef _D_EXACT_NAMLEN
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) ((d)->d_namlen)
#	else
#		define JSTR_DIRENT_D_EXACT_NAMLEN(d) _D_EXACT_NAMLEN(d)
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		define JSTR_DIRENT_D_ALLOC_NAMLEN(d) (JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
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
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d) (((char *)(d) + (d)->d_reclen) - &(d)->d_name[0])
#		else
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d) (sizeof(d)->d_name > 1 ? sizeof(d)->d_name \
				                                                     : JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
#		endif
#	endif
#endif

/* If JSTRIO_ACTION_RETVAL is passed, use these as return values of FUNC(). */
typedef enum jstrio_ftw_actionretval_ty {
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
} jstrio_ftw_actionretval_ty;

typedef enum jstrio_ftw_flag_ty {
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
	/* Expand ~/somepath to $HOME/somepath if ~ is the first char. */
	JSTRIO_FTW_EXPTILDE = (JSTRIO_FTW_NOHIDDEN << 1),
#define JSTRIO_FTW_EXPTILDE JSTRIO_FTW_EXPTILDE
	/* Handle FUNC() return value according to jstrio_ftw_actionretval_ty. */
	JSTRIO_FTW_ACTIONRETVAL = (JSTRIO_FTW_EXPTILDE << 1)
#define JSTRIO_FTW_ACTIONRETVAL JSTRIO_FTW_ACTIONRETVAL
} jstrio_ftw_flag_ty;

#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#	define USE_ATFILE 1
#else
#	define USE_ATFILE 0
#endif

#define NONFATAL_ERR() (jstr_likely(errno == EACCES) || jstr_likely(errno == ENOENT))

#if JSTR_HAVE_DIRENT_D_NAMLEN
#	define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)                           \
		do {                                                                              \
			*(dirpath + dirpath_len) = '/';                                           \
			jstr_strcpy_len(dirpath + dirpath_len + 1, (ep)->d_name, (ep)->d_namlen); \
			newpath_len = dirpath_len + 1 + (ep)->d_namlen;                           \
		} while (0)
#else
#	define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)                                             \
		do {                                                                                                \
			*(dirpath + dirpath_len) = '/';                                                             \
			newpath_len = JSTR_PTR_DIFF(jstr_stpcpy(dirpath + dirpath_len + 1, (ep)->d_name), dirpath); \
		} while (0)
#endif

#if USE_ATFILE
#	define STAT_DO(st, ftw_state, fd, ep, dirpath, do_on_nonfatal_err)    \
		do {                                                           \
			if (jstr_unlikely(fstatat(fd, (ep)->d_name, st, 0))) { \
				if (NONFATAL_ERR()) {                          \
					do_on_nonfatal_err;                    \
				}                                              \
				goto err_closedir;                             \
			}                                                      \
		} while (0)
#	define OPENAT(dstfd, srcfd, file, oflag, do_on_err)                             \
		do {                                                                     \
			if (jstr_unlikely((dstfd = openat(srcfd, file, oflag)) == -1)) { \
				do_on_err;                                               \
			}                                                                \
		} while (0)
#	define OPEN(fd, file, oflag, do_on_err)                             \
		do {                                                         \
			if (jstr_unlikely((fd = open(file, oflag)) == -1)) { \
				do_on_err;                                   \
			}                                                    \
		} while (0)
#	define CLOSE(fd, do_on_err)                    \
		do {                                    \
			if (jstr_unlikely(close(fd))) { \
				do_on_err;              \
			}                               \
		} while (0)
#	define STAT_ALWAYS(st, ftw_state, fd, ep, dirpath) STAT_DO(st, ftw_state, fd, ep, dirpath, ftw_state = JSTRIO_FTW_STATE_NS; goto do_fn)
#	define OPENDIR(fd, fname)                          fdopendir(fd)
#else
#	define STAT_DO(st, ftw_state, fd, ep, dirpath, do_on_nonfatal_err) \
		do {                                                        \
			if (jstr_unlikely(stat(dirpath, st))) {             \
				if (NONFATAL_ERR()) {                       \
					do_on_nonfatal_err;                 \
				}                                           \
				goto err_closedir;                          \
			}                                                   \
		} while (0)
/* clang-format off */
#	define STAT_ALWAYS(st, ftw_state, fd, ep, dirpath) STAT_DO(st, ftw_state, fd, ep, dirpath, ftw_state = JSTRIO_FTW_STATE_NS; goto do_fn)
#	define OPENDIR(fd, fname) opendir(fname)
#	define OPENAT(dstfd, srcfd, file, oflag, do_on_err) do {} while (0)
#	define OPEN(fd, file, oflag, do_on_err) do {} while (0)
#	define CLOSE(fd, do_on_err) do {} while (0)
/* clang-format on */
#endif

#if JSTR_HAVE_DIRENT_D_TYPE
#	define IS_DIR(ep, st)                                   ((ep)->d_type == DT_DIR)
#	define IS_REG(ep, st)                                   ((ep)->d_type == DT_REG)
#	define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#	define STAT(st, ftw_state, fd, ep, dirpath)             STAT_ALWAYS(st, ftw_state, fd, ep, dirpath)
#	define STAT_OR_MODE(st, ftw_state, fd, ep, dirpath)          \
		do {                                                  \
			if (!(jflags & JSTRIO_FTW_NOSTAT))            \
				STAT(st, ftw_state, fd, ep, dirpath); \
		} while (0)
#else
#	define IS_DIR(ep, st) S_ISDIR((st)->st_mode)
#	define IS_REG(ep, st) S_ISREG((st)->st_mode)
#	if USE_ATFILE
#		define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#	else
/* clang-format off */
#		define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) do {} while (0)
#	endif
#	define STAT(st, ftw_state, fd, ep, dirpath) do {} while (0)
#	define STAT_OR_MODE(st, ftw_state, fd, ep, dirpath) do {} while (0)
/* clang-format on */
#endif

#if USE_ATFILE
#	define FD       fd
#	define FD_PARAM , int fd
#	define FD_ARG   , fd
#else
#	define FD
#	define FD_PARAM
#	define FD_ARG
#endif

typedef enum {
	JSTRIO_FTW_STATE_F,
#define JSTRIO_FTW_STATE_F JSTRIO_FTW_STATE_F
	JSTRIO_FTW_STATE_D,
#define JSTRIO_FTW_STATE_D JSTRIO_FTW_STATE_D
	JSTRIO_FTW_STATE_DNR,
#define JSTRIO_FTW_STATE_DNR JSTRIO_FTW_STATE_DNR
	JSTRIO_FTW_STATE_NS,
#define JSTRIO_FTW_STATE_NS JSTRIO_FTW_STATE_NS
	JSTRIO_FTW_STATE_SL,
#define JSTRIO_FTW_STATE_SL JSTRIO_FTW_STATE_SL
	JSTRIO_FTW_STATE_SLN,
#define JSTRIO_FTW_STATE_SLN JSTRIO_FTW_STATE_SLN
} jstrio_ftw_state_ty;

#if JSTRIO_PATH_MAX <= 65536
typedef uint16_t jstrio_path_size_ty;
#elif JSTRIO_PATH_MAX <= 4294967296
typedef uint32_t jstrio_path_size_ty;
#else
typedef size_t jstrio_path_size_ty;
#endif

struct JSTRIO_FTW {
	const char *dirpath;
	const struct stat *st;
	jstrio_path_size_ty dirpath_len;
	int ftw_state;
};

typedef int (*jstrio_ftw_func_ty)(const struct JSTRIO_FTW *ftw, const void *args);
typedef int (*jstrio_ftw_func_match_ty)(const char *fname, jstrio_path_size_ty fname_len, const void *args);

struct pjstrio_ftw_data {
	jstrio_ftw_func_ty func;
	const void *func_args;
	jstrio_ftw_func_match_ty func_match;
	const void *func_match_args;
	struct JSTRIO_FTW ftw;
	int ftw_flags;
};

#define JSTRIO_FTW_FUNC(func_name, ftw, func_args) \
	int func_name(const struct JSTRIO_FTW *ftw, const void *func_args)

#define JSTRIO_FTW_FUNC_MATCH(func_name, filename, filename_len, args) \
	int func_name(const char *filename, jstrio_path_size_ty filename_len, const void *args)

#ifdef O_DIRECTORY
#	define PJSTRIO_O_DIRECTORY O_DIRECTORY
#else
#	define PJSTRIO_O_DIRECTORY 0
#endif

JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
static int
pjstrio_ftw_len(struct pjstrio_ftw_data *a,
                jstrio_path_size_ty dirpath_len
                FD_PARAM)
JSTR_NOEXCEPT
{
	DIR *R const dp = OPENDIR(fd, a->ftw.dirpath);
	if (jstr_nullchk(dp)) {
		if (NONFATAL_ERR()) {
			if (a->ftw_flags & JSTRIO_FTW_REG)
				if (!(a->ftw_flags & JSTRIO_FTW_DIR))
					return JSTR_RET_SUCC;
			if (a->func_match && a->func_match(a->ftw.dirpath, a->ftw.dirpath_len, a->func_match_args))
				return JSTR_RET_SUCC;
			a->ftw.dirpath_len = dirpath_len;
			a->ftw.ftw_state = JSTRIO_FTW_STATE_DNR;
			a->func(&a->ftw, a->func_args);
			return JSTR_RET_SUCC;
		}
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	const struct dirent *R ep;
	int tmp;
	while ((ep = readdir(dp))) {
		if (a->ftw_flags & JSTRIO_FTW_NOHIDDEN) {
			/* Ignore hidden files. */
			if (ep->d_name[0] == '.')
				continue;
		} else {
			/* Ignore "." and "..". */
			if (ep->d_name[0] == '.'
			    && (ep->d_name[1] == '\0' || (ep->d_name[1] == '.' && ep->d_name[2] == '\0')))
				continue;
		}
		/* Stop processing if DIRPATH is longer than PATH_MAX. */
		if (JSTR_HAVE_DIRENT_D_NAMLEN) {
			if (jstr_unlikely(dirpath_len + JSTR_DIRENT_D_EXACT_NAMLEN(ep) >= JSTRIO_PATH_MAX)) {
				errno = ENAMETOOLONG;
				goto err_closedir;
			}
		} else {
			if (jstr_unlikely(dirpath_len >= JSTRIO_PATH_MAX - JSTRIO_NAME_MAX)
			    && jstr_unlikely(dirpath_len + strlen(ep->d_name) >= JSTRIO_PATH_MAX)) {
				errno = ENAMETOOLONG;
				goto err_closedir;
			}
		}
		/* We must [f]stat() to get the type. If !USE_ATFILE, we must construct the full path for stat().
		   FILL_PATH() will construct the full path if USE_ATFILE or HAVE_DIRENT_D_TYPE.
		*/
		if (!JSTR_HAVE_DIRENT_D_TYPE) {
			if (!USE_ATFILE)
				FILL_PATH_ALWAYS(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, ep);
			STAT_DO((struct stat *)a->ftw.st,
			        a->ftw.ftw_state,
			        fd,
			        ep,
			        a->ftw.dirpath,
			        if (a->ftw_flags & (JSTRIO_FTW_DIR | JSTRIO_FTW_REG)) {
				        goto CONT;
			        } else {
				        a->ftw.ftw_state = JSTRIO_FTW_STATE_NS;
				        goto func;
			        });
		}
		if (IS_REG(ep, a->ftw.st)) {
			a->ftw.ftw_state = JSTRIO_FTW_STATE_F;
			goto reg;
		}
		if (IS_DIR(ep, a->ftw.st)) {
			a->ftw.ftw_state = JSTRIO_FTW_STATE_D;
			goto dir;
		}
		/* If true, ignore other types of files. */
		if (a->ftw_flags & (JSTRIO_FTW_DIR | JSTRIO_FTW_REG))
			continue;
		goto do_reg;
reg:
		if (a->ftw_flags & JSTRIO_FTW_DIR)
			if (!(a->ftw_flags & JSTRIO_FTW_REG))
				continue;
do_reg:
		if (a->func_match) {
			if (a->ftw_flags & JSTRIO_FTW_MATCHPATH) {
				FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, ep);
				if (a->func_match(a->ftw.dirpath, a->ftw.dirpath_len, a->func_match_args))
					continue;
			} else {
				const size_t fname_len = JSTR_DIRENT_D_EXACT_NAMLEN(ep);
				if (a->func_match(ep->d_name, fname_len, a->func_match_args))
					continue;
				if (USE_ATFILE)
					a->ftw.dirpath_len = jstrio_appendpath_len_p((char *)a->ftw.dirpath, dirpath_len, ep->d_name, fname_len) - a->ftw.dirpath;
			}
		} else {
			FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, ep);
		}
		if (a->ftw_flags & JSTRIO_FTW_STATREG) {
			if (IS_REG(ep, a->ftw.st))
				STAT(a->ftw.st, a->ftw.ftw_state, fd, ep, a->ftw.dirpath);
		} else {
			STAT_OR_MODE(a->ftw.st, a->ftw.ftw_state, fd, ep, a->ftw.dirpath);
		}
func:
		tmp = a->func(&a->ftw, a->func_args);
		if (a->ftw_flags & JSTRIO_FTW_ACTIONRETVAL) {
			if (tmp == JSTRIO_FTW_RET_CONTINUE
			    || tmp == JSTRIO_FTW_RET_SKIP_SUBTREE)
				continue;
			else if (tmp == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* RET_STOP */
				goto ret_stop;
		} else {
			if (jstr_chk(tmp))
				goto err_closedir;
		}
		continue;
dir:
		if (a->ftw_flags & JSTRIO_FTW_NOSUBDIR)
			if (a->ftw_flags & JSTRIO_FTW_REG)
				if (!(a->ftw_flags & JSTRIO_FTW_DIR))
					continue;
		FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, ep);
		if (!(a->ftw_flags & JSTRIO_FTW_STATREG))
			STAT_OR_MODE(a->ftw.st, a->ftw.ftw_state, fd, ep, a->ftw.dirpath);
		if (a->ftw_flags & JSTRIO_FTW_REG)
			if (!(a->ftw_flags & JSTRIO_FTW_DIR))
				goto skip_fn;
		tmp = a->func(&a->ftw, a->func_args);
		if (a->ftw_flags & JSTRIO_FTW_ACTIONRETVAL) {
			if (tmp == JSTRIO_FTW_RET_CONTINUE)
				continue;
			else if (tmp == JSTRIO_FTW_RET_SKIP_SUBTREE
			         || tmp == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* RET_STOP */
				goto ret_stop;
		} else {
			if (jstr_chk(tmp))
				goto err_closedir;
		}
skip_fn:
		if (a->ftw_flags & JSTRIO_FTW_NOSUBDIR)
			continue;
		OPENAT(tmp, fd, ep->d_name, O_RDONLY | O_NONBLOCK | PJSTRIO_O_DIRECTORY, goto CONT);
		tmp = pjstrio_ftw_len(a, a->ftw.dirpath_len FD_ARG);
		CLOSE(FD, goto err_closedir);
		if (a->ftw_flags & JSTRIO_FTW_ACTIONRETVAL) {
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
#undef STAT_OR_MODE
#undef FD
#undef FD_PARAM
#undef PJSTRIO_O_DIRECTORY

/* Call FUNC() on files found recursively where FUNC_MATCH() returns 0.
   If FUNC_MATCH() is NULL, it behaves as if it matches.
   If FUNC() returns JSTR_RET_ERR, stop processing.
   Return value:
   JSTR_RET_ERR on error;
   JSTR_RET_SUCC on success or non-fatal errors (EACCES or ENOENT) encountered on some entries;
   JSTR_RET_STOP if FUNC() returns RET_STOP and JSTRIO_FTW_ACTIONRETVAL is used.
   or the return value of FUNC() if DIRPATH is not a directory and FUNC() is executed.
   If a non-fatal error is encountered, continue processing other entries. */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((4))
static int
jstrio_ftw_len(const char *R dirpath,
               jstrio_path_size_ty dirpath_len,
               jstrio_ftw_func_ty func,
               const void *func_args,
               int jstrio_ftw_flags,
               jstrio_ftw_func_match_ty func_match,
               const void *func_match_args)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(dirpath_len == 0)) {
		errno = ENOENT;
		goto err;
	}
	if (jstr_unlikely(dirpath_len >= JSTRIO_PATH_MAX)) {
		errno = ENAMETOOLONG;
		goto err;
	}
	for (; dirpath_len != 1 && dirpath[dirpath_len - 1] == '/'; --dirpath_len) {}
	char fulpath[JSTRIO_PATH_MAX];
	if (jstrio_ftw_flags & JSTRIO_FTW_EXPTILDE) {
		if (*dirpath == '~') {
			const char *R home = getenv("HOME");
			if (jstr_nullchk(home))
				goto err;
			const jstrio_path_size_ty homelen = JSTR_PTR_DIFF(jstr_stpcpy(fulpath, home), fulpath);
			memcpy(fulpath + homelen, dirpath + 1, dirpath_len);
			dirpath_len += homelen - 1;
		}
	} else {
		jstr_strcpy_len(fulpath, dirpath, dirpath_len);
	}
#if USE_ATFILE
	int fd;
#endif
	OPEN(fd, fulpath, O_RDONLY | O_NONBLOCK, goto err);
	struct stat st;
	struct pjstrio_ftw_data data;
	data.ftw.dirpath = fulpath;
	data.ftw.st = &st;
	/* This will avoid things like //some/path if DIRPATH is /. */
	if (jstr_unlikely(dirpath_len == 1)
	    && jstr_unlikely(*dirpath == '/')) {
		data.ftw.dirpath_len = 0;
		goto ftw;
	}
	data.ftw.dirpath_len = dirpath_len;
#if USE_ATFILE
	if (jstr_unlikely(fstat(fd, &st)))
#else
	if (jstr_unlikely(stat(fulpath, &st)))
#endif
	{
		data.ftw.ftw_state = JSTRIO_FTW_STATE_NS;
		goto func;
	}
	if (jstr_likely(S_ISDIR(data.ftw.st->st_mode))) {
ftw:;
		if (jstrio_ftw_flags & JSTRIO_FTW_REG)
			if (!(jstrio_ftw_flags & JSTRIO_FTW_DIR))
				goto CONT;
		int tmp;
		data.ftw.ftw_state = JSTRIO_FTW_STATE_D;
		tmp = func(&data.ftw, func_args);
		if (jstrio_ftw_flags & JSTRIO_FTW_ACTIONRETVAL) {
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
		data.ftw_flags = jstrio_ftw_flags;
		data.func_match_args = func_match_args;
		tmp = pjstrio_ftw_len(&data, dirpath_len FD_ARG);
		CLOSE(fd, goto err);
		return tmp;
	}
func:
	CLOSE(fd, goto err);
	if (jstrio_ftw_flags & JSTRIO_FTW_REG)
		if (jstr_unlikely(!S_ISREG(data.ftw.st->st_mode)))
			return JSTR_RET_SUCC;
	data.ftw.ftw_state = JSTRIO_FTW_STATE_F;
	if (func_match) {
		if (jstrio_ftw_flags & JSTRIO_FTW_MATCHPATH) {
func_match_path:
			if (func_match(fulpath, dirpath_len, func_match_args))
				return JSTR_RET_SUCC;
		} else {
			dirpath = (char *)jstr_memrchr(fulpath, '/', dirpath_len);
			const char *const end = (char *)dirpath + dirpath_len;
			if (dirpath) {
				if (*++dirpath && func_match(dirpath, end - dirpath, func_match_args))
					return JSTR_RET_SUCC;
			} else {
				goto func_match_path;
			}
		}
	}
	return (jstr_ret_ty)func(&data.ftw, func_args);
err_close:
	CLOSE(fd, );
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#undef FD_ARG
#undef OPEN
#undef OPENAT
#undef CLOSE
#undef USE_ATFILE

PJSTR_END_DECLS

#undef R

#endif /* JSTRIO_H */
