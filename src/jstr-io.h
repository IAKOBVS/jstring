/* See LICENSE file for copyright and license details. */

#ifndef JSTRIO_H
#define JSTRIO_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-io-table.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

typedef enum {
	JSTRIO_FT_UNKNOWN = 0,
	JSTRIO_FT_TEXT,
	JSTRIO_FT_BINARY
} jstrio_ext_ty;

enum {
#ifdef PATH_MAX
	JSTRIO_PATH_MAX = PATH_MAX,
#else
	JSTRIO_PATH_MAX = 4096,
#endif
#ifdef NAME_MAX
	JSTRIO_NAME_MAX = NAME_MAX,
#else
	JSTRIO_NAME_MAX = 255,
#endif
	JSTRIO_BINARY_CHECK_MAX = 64
};

JSTR_FUNC_PURE
JSTR_INLINE
static int
pjstrio_strcmpeq(const char *s1,
                 const char *s2)
{
	while ((*s1++ == *s2++))
		;
	return *s2;
}

JSTR_FUNC_PURE
JSTR_NOINLINE
static jstrio_ext_ty
pjstrio_exttype(const char *ext)
JSTR_NOEXCEPT
{
	enum { TEXT = 0,
	       BINARY };
	static const char *text[] = { PJSTRIO_EXT_ARRAY_FT_TEXT_NULL };
	static const char *binary[] = { PJSTRIO_EXT_ARRAY_FT_BINARY_NULL };
	int i;
	for (i = 0; i < (int)JSTR_ARRAY_SIZE(text); ++i)
		if (!pjstrio_strcmpeq(ext, text[i]))
			return JSTRIO_FT_TEXT;
	for (i = 0; i < (int)JSTR_ARRAY_SIZE(binary); ++i)
		if (!pjstrio_strcmpeq(ext, binary[i]))
			return JSTRIO_FT_BINARY;
	return JSTRIO_FT_UNKNOWN;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstrio_extget_len(const char *fname,
                   const size_t sz)
{
	const char *p = fname + sz - 1;
	for (; p > fname && *p != '.' && *p != '/'; --p)
		;
	return (*p == '.' && p > fname) ? (char *)p : NULL;
}

/*
   Return jstrio_ext_ty based on the FNAME extension;
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstrio_ext_ty
jstrio_exttype(const char *R fname,
               const size_t sz)
JSTR_NOEXCEPT
{
	fname = (char *)pjstrio_extget_len(fname, sz);
	return fname ? pjstrio_exttype(fname + 1) : JSTRIO_FT_UNKNOWN;
}

/*
   Check MIN(N, SZ) bytes for any unprintable char.
*/
JSTR_FUNC
static int
jstr_isbinary(const char *R buf,
              const size_t n,
              const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return JSTR_ERR;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(n, sz);
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (pjstrio_reject_table[*s++])
			return JSTR_SUCC;
	return JSTR_ERR;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstrio_isbinarysignature(const char *R buf,
                          const size_t sz)
{
#define ELF "\x7f\ELF"
#define UTF "\xEF\xBB\xBF"
	if (jstr_likely(sz >= sizeof(ELF) - 1)) {
		if (!pjstrio_strcmpeq(buf, ELF))
			return JSTR_SUCC;
check_utf:;
		if (!pjstrio_strcmpeq(buf, UTF))
			return JSTR_ERR;
	} else if (jstr_likely(sz == sizeof(UTF) - 1)) {
		goto check_utf;
	}
	return -1;
#undef ELF
#undef UTF
}

/*
   Check if the first JSTRIO_BINARY_CHECK_MAX bytes or fewer contain any unprintable char.
*/
JSTR_FUNC
static int
jstrio_isbinary_maybe(const char *R buf,
                      const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return JSTR_ERR;
	const int ret = pjstrio_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(sz, JSTRIO_BINARY_CHECK_MAX) + 1;
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (pjstrio_reject_table[*s++])
			return JSTR_SUCC;
	return JSTR_ERR;
}

/*
   Check the whole file for a NUL byte.
   File must be nul terminated.
*/
JSTR_FUNC_PURE
static int
jstrio_isbinary(const char *R buf,
                const size_t sz)
JSTR_NOEXCEPT
{
	const int ret = pjstrio_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return strlen(buf) != sz;
}

JSTR_FUNC
static int
jstrio_writefile(const char *R s,
                 const size_t sz,
                 const char *R fname,
                 const int oflag)
JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_WRONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_unlikely((size_t)write(fd, s, sz) != sz))
		goto err_close;
	close(fd);
	return JSTR_SUCC;
err_close:
	close(fd);
err:
	return JSTR_ERR;
}

JSTR_FUNC
static int
jstrio_fwritefile(const char *R s,
                  const size_t sz,
                  const char *R fname,
                  const char *R modes)
JSTR_NOEXCEPT
{
	FILE *R fp = fopen(fname, modes);
	if (jstr_unlikely(fp == NULL))
		goto err;
	if (jstr_unlikely(!jstrio_fwrite(s, sz, fp)))
		goto err_close;
	fclose(fp);
	return JSTR_SUCC;
err_close:
	fclose(fp);
err:
	return JSTR_ERR;
}

#if JSTR_HAVE_POPEN

JSTR_FUNC
static int
jstrio_readsystem(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  const char *R cmd)
JSTR_NOEXCEPT
{
	enum { MINBUF = JSTR_PAGE_SIZE };
	FILE *R fp = popen(cmd, "r");
	if (jstr_unlikely(fp == NULL))
		goto err;
	char buf[MINBUF];
	size_t readsz;
	readsz = fread(buf, 1, sizeof(buf), fp);
	if (jstr_unlikely(readsz == (size_t)-1))
		goto err_close;
	PJSTR_RESERVE(s, sz, cap, readsz * 2, goto err_close);
	memcpy(*s, buf, readsz);
	*sz = readsz;
	if (jstr_unlikely(readsz == MINBUF)) {
		size_t reqsz;
		for (;;) {
			reqsz = *cap - *sz;
			readsz = fread(*s + *sz, 1, reqsz, fp);
			if (jstr_unlikely(readsz == (size_t)-1))
				goto err_close_free;
			*sz += readsz;
			if (readsz < reqsz)
				break;
			if (*sz == *cap)
				PJSTR_RESERVEEXACTALWAYS(s, sz, cap, (*cap * PJSTR_GROWTH), goto err_close)
		}
	}
	*(*s + *sz) = '\0';
	pclose(fp);
	return JSTR_SUCC;
err_close_free:
	jstr_free(s, sz, cap);
err_close:
	pclose(fp);
err:
	return JSTR_ERR;
}

#endif

JSTR_FUNC
JSTR_INLINE
static int
pjstrio_readfile_len(char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const int fd,
                     const size_t file_size)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, file_size, goto err)
	if (jstr_unlikely(file_size != (size_t)read(fd, *s, file_size)))
		goto err_free;
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_SUCC;
err_free:
	jstr_free(s, sz, cap);
err:
	return JSTR_ERR;
}

JSTR_FUNC
static int
jstrio_readfile_len(char *R *R s,
                    size_t *R sz,
                    size_t *R cap,
                    const char *R fname,
                    const size_t file_size)
JSTR_NOEXCEPT
{
	const int fd = open(fname, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_chk(pjstrio_readfile_len(s, sz, cap, fd, file_size)))
		goto err_close;
	close(fd);
	return JSTR_SUCC;
err_close:
	close(fd);
err:
	return JSTR_ERR;
}

/*
   Return value:
   JSTR_ERR on error;
   otherwise JSTR_SUCC.
*/
JSTR_FUNC
static int
jstrio_readfile(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R fname,
                struct stat *R st)
JSTR_NOEXCEPT
{
	const int fd = open(fname, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (jstr_unlikely(fstat(fd, st)))
		goto err_close;
	return jstrio_readfile_len(s, sz, cap, fname, st->st_size);
err_close:
	close(fd);
err:
	return JSTR_ERR;
}

/*
   Expand ~/some_dir to /home/username/some_dir.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error.
*/
JSTR_FUNC
static char *
jstrio_expandtildefirst_len_unsafe_p(char *R s,
                                     const size_t sz)
JSTR_NOEXCEPT
{
	if (*s != '~')
		return s + sz;
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	jstr_strmove_len(s + len, s + 1, (s + sz) - (s + 1));
	memcpy(s, home, len);
	return s + sz + len - 1;
}

/*
   Expand ~/some_dir to /home/username/some_dir.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error.
*/
JSTR_FUNC
static int
jstrio_expandtildefirst_p(char *R *R s,
                          size_t *R sz,
                          size_t *R cap)
JSTR_NOEXCEPT
{
	if (**s != '~')
		return JSTR_SUCC;
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return JSTR_ERR;
	const size_t len = strlen(home);
	PJSTR_RESERVE(s, sz, cap, *sz + len, return JSTR_ERR);
	jstr_strmove_len(*s + len, *s + 1, (*s + *sz) - (*s + 1));
	memcpy(*s, home, len);
	*sz += len;
	return JSTR_SUCC;
}

/*
   Expand every ~ to /home/username.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S;
   NULL on error.
*/
JSTR_FUNC
static char *
jstrio_expandtilde_len_unsafe_p(char *R s,
                                size_t sz)
JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	char *p = s;
	while ((p = (char *)memchr(p, '~', (s + sz) - p))) {
		jstr_strmove_len(p + len, p + 1, (s + sz) - (p + 1));
		memcpy(p, home, len);
		p += len;
		sz += (len - 1);
	}
	return s + sz;
}

/*
   Expand every ~ to /home/username.
   Return value:
   JSTR_ERR on error;
   otherwise JSTR_SUCC.
*/
JSTR_FUNC
static int
jstrio_expandtilde(char *R *R s,
                   size_t *R sz,
                   size_t *R cap)
JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return JSTR_ERR;
	const size_t len = strlen(home);
	const char *tmp;
	char *p = *s;
	while ((p = (char *)memchr(p, '~', (*s + *sz) - p))) {
		if (jstr_unlikely(*sz + len >= *cap)) {
			tmp = *s;
			PJSTR_RESERVEALWAYS(s, sz, cap, *sz + len, return JSTR_ERR)
			p = *s + (p - tmp);
		}
		jstr_strmove_len(p + len, p + 1, (*s + *sz) - (p + 1));
		memcpy(p, home, len);
		p += len;
		*sz += (len - 1);
	}
	return JSTR_SUCC;
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstrio_appendpath_len_p(char *R path,
                        const size_t sz,
                        const char *R fname,
                        const size_t fname_len)
{
	*path = '/';
	jstr_strcpy_len(path + sz + 1, fname, fname_len);
	return path + sz + 1 + fname_len;
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstrio_appendpath_p(char *R path,
                    const size_t sz,
                    const char *R fname)
{
	*(path + sz) = '/';
	return jstr_stpcpy(path + sz + 1, fname);
}

JSTR_FUNC
static int
jstrio_appendpath_len(char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      const char *R fname,
                      const size_t fname_len)
{
	PJSTR_RESERVE(s, sz, cap, *sz + fname_len, return JSTR_ERR);
	*sz = jstrio_appendpath_len_p(*s, *sz, fname, fname_len) - *s;
	return JSTR_SUCC;
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
pjstrio_appendpath_p(char *R path_end,
                     const char *R fname)
JSTR_NOEXCEPT
{
	*path_end = '/';
	return jstr_stpcpy(path_end + 1, fname);
}

JSTR_INLINE
JSTR_FUNC_VOID
static void
pjstrio_appendpath_len(char *R path_end,
                       const char *R fname,
                       const size_t fname_len)
JSTR_NOEXCEPT
{
	*path_end = '/';
	jstr_strcpy_len(path_end + 1, fname, fname_len);
}

#ifdef _DIRENT_HAVE_D_NAMLEN
#	ifndef _D_EXACT_NAMLEN
#		define _D_EXACT_NAMLEN(d) ((d)->d_namlen)
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		define _D_ALLOC_NAMLEN(d) (_D_EXACT_NAMLEN(d) + 1)
#	endif
#else
#	ifndef _D_EXACT_NAMLEN
#		define _D_EXACT_NAMLEN(d) (strlen((d)->d_name))
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		ifdef _DIRENT_HAVE_D_RECLEN
#			define _D_ALLOC_NAMLEN(d) (((char *)(d) + (d)->d_reclen) - &(d)->d_name[0])
#		else
#			define _D_ALLOC_NAMLEN(d) (sizeof(d)->d_name > 1 ? sizeof(d)->d_name \
				                                          : _D_EXACT_NAMLEN(d) + 1)
#		endif
#	endif
#endif

/* If JSTRIO_ACTION_RETVAL is passed, use these as return values of FN(). */
typedef enum jstrio_ftw_actionretval_ty {
	JSTRIO_FTW_RET_STOP = 0,
	JSTRIO_FTW_RET_CONTINUE,
	/* Skip sibling entries. */
	JSTRIO_FTW_RET_SKIP_SIBLINGS,
	/* Skip subdirectories. */
	JSTRIO_FTW_RET_SKIP_SUBTREE
} jstrio_ftw_actionretval_ty;

typedef enum jstrio_ftw_flag_ty {
	/* Match glob with FULPATH instead of d_name. */
	JSTRIO_FTW_MATCHPATH = (1),
	/* Call FN() on regular files. */
	JSTRIO_FTW_REG = (JSTRIO_FTW_MATCHPATH << 1),
	/* Call FN() on directories. */
	JSTRIO_FTW_DIR = (JSTRIO_FTW_REG << 1),
	/* Do not traverse subdirectories. */
	JSTRIO_FTW_NOSUBDIR = (JSTRIO_FTW_DIR << 1),
	/* Do not call stat. Only st.st_mode is defined. */
	JSTRIO_FTW_NOSTAT = (JSTRIO_FTW_NOSUBDIR << 1),
	/* Only call stat on regular files. */
	JSTRIO_FTW_STATREG = (JSTRIO_FTW_NOSTAT << 1),
	/* Ignore hidden entries. */
	JSTRIO_FTW_NOHIDDEN = (JSTRIO_FTW_STATREG << 1),
	/* Expand ~/somepath to $HOME/somepath if ~ is the first char. */
	JSTRIO_FTW_EXPTILDE = (JSTRIO_FTW_NOHIDDEN << 1),
	/* Handle FN() return value according to jstrio_ftw_actionretval_ty. */
	JSTRIO_FTW_ACTIONRETVAL = (JSTRIO_FTW_EXPTILDE << 1)
} jstrio_ftw_flag_ty;

#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#	define USE_ATFILE 1
#else
#	define USE_ATFILE 0
#endif

#define NONFATAL_ERR() jstr_likely(errno == EACCES || errno == ENOENT)

#if JSTR_HAVE_DIRENT_D_NAMLEN
#	define FILL_PATHALWAYS()                                                                \
		do {                                                                             \
			pjstrio_appendpath_len(dirpath + dirpath_len, ep->d_name, ep->d_namlen); \
			path_len = dirpath_len + 1 + ep->d_namlen;                               \
		} while (0)
#else
#	define FILL_PATHALWAYS() ((void)(path_len = pjstrio_appendpath_p(dirpath + dirpath_len, ep->d_name) - dirpath))
#endif

#if USE_ATFILE
#	define STATALWAYS(st)                                               \
		do {                                                         \
			if (jstr_unlikely(fstatat(fd, ep->d_name, st, 0))) { \
				if (NONFATAL_ERR())                          \
					continue;                            \
				return JSTR_ERR;                             \
			}                                                    \
		} while (0)
#else
#	define STATALWAYS(st)                                \
		do {                                          \
			if (jstr_unlikely(stat(dirpath, st))) \
				if (NONFATAL_ERR())           \
					continue;             \
			return JSTR_ERR;                      \
		} while (0)
#endif

#if JSTR_HAVE_DIRENT_D_TYPE
#	define ISDIR()       (ep->d_type == DT_DIR)
#	define ISREG()       (ep->d_type == DT_REG)
#	define FILL_PATH()   FILL_PATHALWAYS()
#	define STAT(st)      STATALWAYS(st)
#	define STAT_MODE(st) ((void)((st).st_mode = DTTOIF(ep->d_type)))
#	define STAT_OR_MODE(st)                        \
		do {                                    \
			if (jflags & JSTRIO_FTW_NOSTAT) \
				STAT_MODE(st);          \
			else                            \
				STAT(st);               \
		} while (0)
#else
#	define ISDIR() (S_ISDIR(st->st_mode))
#	define ISREG() (S_ISREG(st->st_mode))
#	if USE_ATFILE
#		define FILL_PATH() FILL_PATHALWAYS()
#	else
#		define FILL_PATH() \
			do {        \
			} while (0)
#	endif
#	define STAT(st) \
		do {     \
		} while (0)
#	define STAT_MODE(st) \
		do {          \
		} while (0)
#	define STAT_OR_MODE(st) \
		do {             \
		} while (0)
#endif

#if USE_ATFILE
#	define FD_PARAM , int fd
#	define FD_ARG   , fd
#else
#	define FD_PARAM
#	define FD_ARG
#endif

typedef int (*jstrio_ftw_func_ty)(const char *dirpath,
                                  size_t dirpath_len,
                                  const struct stat *st);

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
JSTR_NONNULL(7)
static int
pjstrio_ftw_len(char *R dirpath,
                const size_t dirpath_len,
                int (*fn)(const char *, size_t, const struct stat *),
                const int jflags,
                const char *R fn_glob,
                const int fn_flags,
                struct stat *R st
                FD_PARAM)
JSTR_NOEXCEPT
{
	DIR *R const dp =
#if USE_ATFILE
	fdopendir(fd);
#else
	opendir(dirpath);
#endif
	if (jstr_unlikely(dp == NULL))
		return NONFATAL_ERR() ? JSTR_SUCC : JSTR_ERR;
	size_t path_len = 0;
	const struct dirent *R ep;
	int ret;
	while ((ep = readdir(dp)) != NULL) {
		if (jflags & JSTRIO_FTW_NOHIDDEN) {
			/* Ignore hidden files. */
			if (ep->d_name[0] == '.')
				continue;
		} else {
			/* Ignore "." and "..". */
			if (ep->d_name[0] == '.'
			    && (ep->d_name[1] == '\0' || (ep->d_name[1] == '.' && ep->d_name[2] == '\0')))
				continue;
		}
		/* Exit if DIRPATH is longer than PATH_MAX. */
		if (
#if JSTR_HAVE_DIRENT_D_NAMLEN
		jstr_unlikely(dirpath_len + ep->d_namlen >= JSTRIO_PATH_MAX)
#else
		jstr_unlikely(dirpath_len >= JSTRIO_PATH_MAX - JSTRIO_NAME_MAX)
		&& jstr_unlikely(dirpath_len + strlen(ep->d_name) >= JSTRIO_PATH_MAX)
#endif
		) {
			errno = ENAMETOOLONG;
			goto err_closedir;
		}
#if !JSTR_HAVE_DIRENT_D_TYPE
#	if !USE_ATFILE
		FILL_PATHALWAYS();
#	endif /* ATFILE */
		STATALWAYS(st);
#endif /* D_TYPE */
		if (ISREG())
			goto do_reg;
		if (ISDIR())
			goto do_dir;
		/* If true, ignore other types of files. */
		if (jflags & (JSTRIO_FTW_DIR | JSTRIO_FTW_REG))
			continue;
do_reg:
		if (jflags & JSTRIO_FTW_DIR)
			if (!(jflags & JSTRIO_FTW_REG))
				continue;
		if (fn_glob != NULL) {
			if (jflags & JSTRIO_FTW_MATCHPATH) {
				FILL_PATH();
				if (fnmatch(fn_glob, dirpath, fn_flags))
					continue;
			} else {
				if (fnmatch(fn_glob, ep->d_name, fn_flags))
					continue;
				FILL_PATH();
			}
		} else {
			FILL_PATH();
		}
		if (jflags & JSTRIO_FTW_STATREG) {
			if (ISREG())
				STAT(st);
			else
				STAT_MODE(st);
		} else {
			STAT_OR_MODE(st);
		}
		ret = fn(dirpath, path_len, st);
		if (jflags & JSTRIO_FTW_ACTIONRETVAL) {
			if (ret == JSTRIO_FTW_RET_CONTINUE
			    || ret == JSTRIO_FTW_RET_SKIP_SUBTREE)
				continue;
			else if (ret == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* RET_STOP */
				goto err_closedir;
		} else {
			if (jstr_chk(ret))
				goto err_closedir;
		}
		continue;
do_dir:
		if (jflags & JSTRIO_FTW_NOSUBDIR)
			if (jflags & JSTRIO_FTW_REG)
				if (!(jflags & JSTRIO_FTW_DIR))
					continue;
		FILL_PATH();
		if (jflags & JSTRIO_FTW_STATREG)
			STAT_MODE(st);
		else
			STAT_OR_MODE(st);
		if (jflags & JSTRIO_FTW_REG)
			if (!(jflags & JSTRIO_FTW_DIR))
				goto CONT;
		ret = fn(dirpath, dirpath_len, st);
		if (jflags & JSTRIO_FTW_ACTIONRETVAL) {
			if (ret == JSTRIO_FTW_RET_CONTINUE)
				continue;
			else if (ret == JSTRIO_FTW_RET_SKIP_SUBTREE
			         || ret == JSTRIO_FTW_RET_SKIP_SIBLINGS)
				goto CONT;
			else /* RET_STOP */
				goto err_closedir;
		} else {
			if (jstr_chk(ret))
				goto err_closedir;
		}
CONT:
		if (jflags & JSTRIO_FTW_NOSUBDIR)
			continue;
#if USE_ATFILE
		fd = openat(fd, ep->d_name, O_RDONLY);
		if (jstr_unlikely(fd == -1))
			continue;
		ret = pjstrio_ftw_len(dirpath, path_len, fn, jflags, fn_glob, fn_flags, st, fd);
		close(fd);
		if (jstr_chk(ret))
			goto err_closedir;
#else
		if (jstr_chk(pjstrio_ftw_len(dirpath, path_len, fn, jflags, fn_glob, fn_flags, st)))
			goto err_closedir;
#endif
	}
	closedir(dp);
	return JSTR_SUCC;
err_closedir:
	closedir(dp);
	return JSTR_ERR;
}

#undef ISDIR
#undef ISREG
#undef FILL_PATH
#undef FILL_PATHALWAYS
#undef STAT_OR_MODE
#undef STAT_MODE
#undef STAT
#undef NONFATAL_ERR
#undef STATALWAYS
#undef FD_PARAM

/*
   Call FN() on files found recursively that matches GLOB.
   If FN() returns JSTR_ERR, stop processing.
   Return value:
   JSTR_ERR on error;
   JSTR_SUCC on success or non-fatal errors (EACCES or ENOENT) encountered on some entries;
   or the return value of FN() if DIRPATH is not a directory and FN() is executed.
   If a non-fatal error is encountered, continue processing other entries.
*/
JSTR_FUNC_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
static int
jstrio_ftw_len(const char *R dirpath,
               size_t dirpath_len,
               int (*fn)(const char *, size_t, const struct stat *),
               const int jstrio_ftw_flag,
               const char *R fn_glob,
               const int fn_flags)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(dirpath_len == 0)) {
		errno = ENOENT;
		return JSTR_ERR;
	}
	if (jstr_unlikely(dirpath_len >= JSTRIO_PATH_MAX)) {
		errno = ENAMETOOLONG;
		return JSTR_ERR;
	}
	for (; dirpath_len != 1 && dirpath[dirpath_len - 1] == '/'; --dirpath_len)
		;
	char fulpath[JSTRIO_PATH_MAX];
	if (jstrio_ftw_flag & JSTRIO_FTW_EXPTILDE) {
		if (*dirpath == '~') {
			const char *R home = getenv("HOME");
			if (jstr_unlikely(home == NULL))
				return JSTR_ERR;
			const size_t homelen = jstr_stpcpy(fulpath, home) - fulpath;
			memcpy(fulpath + homelen, dirpath + 1, dirpath_len);
			dirpath_len += homelen - 1;
		}
	} else {
		jstr_strcpy_len(fulpath, dirpath, dirpath_len);
	}
#if USE_ATFILE
	const int fd = open(fulpath, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		return JSTR_ERR;
#endif
	struct stat st;
	/* This avoids things like //usr/cache. */
	if (jstr_unlikely(dirpath_len == 1)
	    && jstr_unlikely(*dirpath == '/')) {
		dirpath_len = 0;
		goto ftw;
	}
#if USE_ATFILE
	if (jstr_unlikely(fstat(fd, &st)))
		goto err_close;
#else
	if (jstr_unlikely(stat(fulpath, &st)))
		return JSTR_ERR;
#endif
	if (jstr_likely(S_ISDIR(st.st_mode))) {
ftw:
		if (jstrio_ftw_flag & JSTRIO_FTW_REG)
			if (!(jstrio_ftw_flag & JSTRIO_FTW_DIR))
				goto CONT;
		int ret;
		ret = fn(dirpath, dirpath_len, &st);
		if (jstrio_ftw_flag & JSTRIO_FTW_ACTIONRETVAL) {
			if (jstr_unlikely(ret != JSTRIO_FTW_RET_CONTINUE))
				goto err_close;
		} else {
			if (jstr_chk(ret))
				goto err_close;
		}
CONT:
		pjstrio_ftw_len(fulpath, dirpath_len, fn, jstrio_ftw_flag, fn_glob, fn_flags, &st FD_ARG);
#if USE_ATFILE
		close(fd);
#endif
		return JSTR_SUCC;
	}
#if USE_ATFILE
	close(fd);
#endif
	if (jstrio_ftw_flag & JSTRIO_FTW_REG)
		if (jstr_unlikely(!S_ISREG(st.st_mode)))
			return JSTR_SUCC;
	if (fn_glob != NULL)
		if (fnmatch(fn_glob, fulpath, fn_flags))
			return JSTR_SUCC;
	return fn(fulpath, dirpath_len, &st);
err_close:
#if USE_ATFILE
	close(fd);
#endif
	return JSTR_ERR;
}

#undef USE_ATFILE
#undef FD_ARG

PJSTR_END_DECLS

#undef R

#endif /* JSTRIO_H */
