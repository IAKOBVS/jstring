
#ifndef JSTR_IO_H
#	define JSTR_IO_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <dirent.h>
#	include <fcntl.h>
#	include <limits.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	if JSTR_HAVE_READV && JSTR_HAVE_WRITEV
#		include <sys/uio.h>
#	endif
JSTR_INTERNAL_END_DECLS

#	ifdef JSTR_IMPLEMENTATION
#		undef JSTR_IMPLEMENTATION
#		include "builder.h"
#		include "stdstring.h"
#		include "string.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

enum {
#	ifdef PATH_MAX
	JSTR_IO_PATH_MAX = PATH_MAX,
#	else
	JSTR_IO_PATH_MAX = 4096,
#	endif
#	define JSTR_IO_PATH_MAX JSTR_IO_PATH_MAX
#	ifdef NAME_MAX
	JSTR_IO_NAME_MAX = NAME_MAX,
#	else
	JSTR_IO_NAME_MAX = 255,
#	endif
#	define JSTR_IO_NAME_MAX JSTR_IO_NAME_MAX
	JSTR_IO_BINARY_CHECK_MAX = 64
};

/**
 * @brief jstr_internal_io_isbinarysignature
 *
 * Inspect the leading bytes of BUF for a known binary/text signature. Returns 1
 * for an ELF magic number, 0 for a UTF-8 BOM, and -1 when neither signature is
 * present (so callers fall back to heuristic detection).
 *
 * @param buf const char *buf Input buffer to inspect. May be NULL only when
 *            sz == 0; otherwise must point to at least sz readable bytes.
 * @param sz  size_t sz Number of bytes available in BUF.
 *
 * @return int 1 if ELF, 0 if UTF-8 BOM, -1 otherwise.
 *
 * @note Only the first 3-4 bytes are examined; no NUL termination is assumed
 *       or required.
 * @note When sz < 3 no bytes are dereferenced and -1 is returned.
 * @note Passing NULL with sz > 0 is undefined behavior.
 */
JSTR_FUNC
int
jstr_internal_io_isbinarysignature(const char *buf, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p = (const unsigned char *)buf;
	if (jstr_likely(sz >= 4)) {
		/* ELF */
		if (p[0] == 0x7F && p[1] == 'E' && p[2] == 'L' && p[3] == 'F')
			return 1;
check_utf:;
		/* UTF */
		if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
			return 0;
	} else if (sz == 3) {
		goto check_utf;
	}
	return -1;
}
#	else
;
#	endif

#	if JSTR_OS_WINDOWS || JSTR_OS_WINDOWSCE
#		define FORM_FEED_IS_BINARY 0
#	else
#		define FORM_FEED_IS_BINARY 1
#	endif

/**
 * @brief jstr_io_isbinary_maybe
 *
 * Heuristic binary detection over at most the first JSTR_IO_BINARY_CHECK_MAX
 * (64) bytes of BUF. First checks the ELF/UTF-8 BOM signature via
 * jstr_internal_io_isbinarysignature; if that is inconclusive, scans the
 * (capped) prefix for control characters outside tab/newline (and DEL). An
 * empty input is treated as text (returns 0).
 *
 * @param buf const char *buf Input buffer. May be NULL only when sz == 0.
 * @param sz  size_t sz Number of bytes available in BUF.
 *
 * @return int 1 if the prefix looks binary, 0 if it looks like text.
 *
 * @note Only the first min(sz, 64) bytes are examined; the rest of the buffer
 *       is ignored.
 * @note sz == 0 returns 0 without dereferencing BUF.
 * @note Passing NULL with sz > 0 is undefined behavior.
 */
JSTR_FUNC
int
jstr_io_isbinary_maybe(const char *buf, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const int ret = jstr_internal_io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	sz = JSTR_MIN(sz, JSTR_IO_BINARY_CHECK_MAX);
	const unsigned char *s = (unsigned char *)buf;
	for (; sz--; ++s)
		if ((*s < ' ' && jstr_unlikely(*s != '\t' && *s != '\n')) || jstr_unlikely(*s == 127))
			return 1;
	return 0;
}
#	else
;
#	endif

/**
 * @brief jstr_io_isbinary
 *
 * Determine whether BUF (which must be NUL-terminated and whose logical size
 * is sz) is binary. First checks the ELF/UTF-8 BOM signature; if inconclusive,
 * compares strlen(buf) to sz, treating an embedded NUL (strlen != sz) as
 * binary. Pure function.
 *
 * @param buf const char *buf NUL-terminated input buffer. Must not be NULL.
 * @param sz  size_t sz Declared length of BUF in bytes.
 *
 * @return int 1 if binary, 0 if text.
 *
 * @note BUF must be NUL-terminated; sz is only used for the signature check
 *       and the NUL-detection comparison.
 * @note If sz == 0, returns 0 (empty treated as text) provided BUF is "".
 * @note Passing NULL is undefined behavior.
 */
JSTR_FUNC_PURE
int
jstr_io_isbinary(const char *buf, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const int ret = jstr_internal_io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return strlen(buf) != sz;
}
#	else
;
#	endif

/**
 * @brief jstr_io_isbinary_atleast
 *
 * Like jstr_io_isbinary but only inspects the first min(n, sz) bytes for an
 * embedded NUL (via memchr) after the signature check. Pure function.
 *
 * @param buf const char *buf NUL-terminated input buffer. Must not be NULL.
 * @param sz  size_t sz Declared length of BUF in bytes.
 * @param n   size_t n Maximum number of bytes to scan for a NUL.
 *
 * @return int 1 if a NUL is found within the first min(n, sz) bytes (or the
 *             signature says binary), 0 otherwise.
 *
 * @note n == 0 skips the NUL scan (returns the signature result, or 0).
 * @note Does not require the scanned prefix to be NUL-terminated.
 * @note Passing NULL is undefined behavior.
 */
JSTR_FUNC_PURE
int
jstr_io_isbinary_atleast(const char *buf, size_t sz, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const int ret = jstr_internal_io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return memchr(buf, '\0', JSTR_MIN(n, sz)) != NULL;
}
#	else
;
#	endif

/**
 * @brief jstr_isbinary
 *
 * Scan the first min(n, sz) bytes of BUF for unprintable control characters
 * (bytes < ' ' except tab/newline, or DEL). No signature or NUL handling is
 * performed. Returns 0 when sz == 0.
 *
 * @param buf const char *buf Input buffer. May be NULL only when sz == 0 or
 *            n == 0.
 * @param sz  size_t sz Number of bytes available in BUF.
 * @param n   size_t n Number of bytes to inspect.
 *
 * @return int 1 if an unprintable byte is found, 0 otherwise.
 *
 * @note Only min(n, sz) bytes are examined.
 * @note sz == 0 returns 0 without dereferencing BUF.
 * @note Passing NULL with a non-zero effective scan length is undefined
 *       behavior.
 */
JSTR_FUNC
int
jstr_isbinary(const char *buf, size_t sz, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const unsigned char *s = (const unsigned char *)buf;
	for (n = JSTR_MIN(n, sz); n--; ++s)
		if ((*s < ' ' && jstr_unlikely(*s != '\t' && *s != '\n')) || jstr_unlikely(*s == 127))
			return 1;
	return 0;
}
#	else
;
#	endif

/**
 * @brief jstr_io_writefilefd_len
 *
 * Write exactly sz bytes from S to the file descriptor FD, appending a trailing
 * newline if S does not already end in '\n'. Handles partial writes (writev
 * when available, otherwise write). Returns success without writing when
 * sz == 0.
 *
 * @param s  const char *s Source buffer. Must not be NULL when sz > 0.
 * @param sz size_t sz Number of bytes to write.
 * @param fd int fd Writable file descriptor.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on write error.
 *
 * @note Does not close FD; the caller owns FD.
 * @note If sz == 0, nothing is written and JSTR_RET_SUCC is returned.
 * @note Embedded NUL bytes are written verbatim (binary-safe for the given
 *       length).
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_writefilefd_len(const char *s, size_t sz, int fd) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return JSTR_RET_SUCC;
#		if JSTR_HAVE_WRITEV
	struct iovec iov[] = {
		{ .iov_base = (void *)s,    .iov_len = sz                          },
		{ .iov_base = (void *)"\n", .iov_len = (s[sz - 1] != '\n') ? 1 : 0 }
	};
	const struct iovec *iop = iov;
	int iol = JSTR_ARRAY_COUNT(iov);
	ssize_t write_sz;
	size_t left = iov[0].iov_len + iov[1].iov_len;
	/* Handle partial writev */
	for (;;) {
		write_sz = writev(fd, iop, iol);
		/* Error */
		if (jstr_unlikely(write_sz < 0))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		left -= (size_t)write_sz;
		/* Update iov */
		if (jstr_likely(left == 0)) {
			break;
		} else if (left > 1) {
			iov[0].iov_base = (unsigned char *)iov[0].iov_base + write_sz;
			iov[0].iov_len -= (size_t)write_sz;
		} else { /* left == 1 */
			++iop;
			--iol;
		}
	}
	return JSTR_RET_SUCC;
#		else
	ssize_t write_sz;
	size_t left = sz;
	const char *src = s;
	/* Handle partial write */
	for (; left; left -= (size_t)write_sz, src += write_sz) {
		write_sz = write(fd, src, left);
		/* Error */
		if (jstr_unlikely(write_sz < 0))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (s[sz - 1] != '\n')
		do {
			write_sz = write(fd, "\n", 1);
			if (jstr_unlikely(write_sz < 0))
				JSTR_RETURN_ERR(JSTR_RET_ERR);
		} while (write_sz != 1);
	return JSTR_RET_SUCC;
#		endif
}
#	else
;
#	endif

/**
 * @brief jstr_io_writefile_len
 *
 * Open FNAME with O_WRONLY | oflag (and MODE when O_CREAT is implied by oflag)
 * and write S via jstr_io_writefilefd_len, then close the descriptor.
 *
 * @param s     const char *s Source buffer. Must not be NULL when sz > 0.
 * @param sz    size_t sz Number of bytes to write.
 * @param fname const char *fname Path of the file to open/write. Must not be
 *              NULL.
 * @param oflag int oflag Open flags (O_CREAT, O_APPEND, O_TRUNC, ...). O_WRONLY
 *              is always ORed in.
 * @param mode  int mode Permission bits used when creating the file; ignored
 *              otherwise.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/write/
 *         close failure.
 *
 * @note On failure the descriptor is closed before returning.
 * @note MODE is ignored unless O_CREAT is set in oflag.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_writefile_len(const char *R s, size_t sz, const char *R fname, int oflag, int mode) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const int fd = open(fname, oflag | O_WRONLY, mode);
	if (jstr_unlikely(fd < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_io_writefilefd_len(s, sz, fd))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_fwritefilefp_len
 *
 * Write sz bytes from S to the stream FP, appending a trailing newline if S
 * does not already end in '\n'. Returns success without writing when sz == 0.
 *
 * @param s  const char *s Source buffer. Must not be NULL when sz > 0.
 * @param sz size_t sz Number of bytes to write.
 * @param fp FILE *fp Destination stream. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR if fwrite/putc
 *         fails.
 *
 * @note Does not close or flush FP.
 * @note Embedded NUL bytes are written verbatim.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_fwritefilefp_len(const char *R s, size_t sz, FILE *fp) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return JSTR_RET_SUCC;
	if (jstr_unlikely(jstr_io_fwrite(s, 1, sz, fp) != sz))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (s[sz - 1] != '\n' && jstr_io_putc('\n', fp) == EOF)
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_fwritefile_len
 *
 * Open FNAME with fopen(MODES) and write S via jstr_io_fwritefilefp_len,
 * then fclose. Returns success/error accordingly.
 *
 * @param s     const char *s Source buffer. Must not be NULL when sz > 0.
 * @param sz    size_t sz Number of bytes to write.
 * @param fname const char *fname Path of the file to open/write. Must not be
 *              NULL.
 * @param modes const char *modes fopen mode string (e.g. "w", "a"). Must not
 *              be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/write/
 *         close failure.
 *
 * @note On failure the stream is closed before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_fwritefile_len(const char *R s, size_t sz, const char *R fname, const char *R modes) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
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
#	else
;
#	endif

/**
 * @brief jstr_io_readfilefd_len
 *
 * Read exactly file_size bytes from FD into *S, growing the buffer as needed
 * (reserving file_size + 1 bytes), NUL-terminate it, and set *sz =
 * file_size. Errors on read failure or premature EOF.
 *
 * @param s         char **s Pointer to destination buffer pointer. Must not be
 *                  NULL.
 * @param sz        size_t *sz Receives the number of bytes read. Must not be
 *                  NULL.
 * @param cap       size_t *cap Capacity of *S; updated on growth. Must not be
 *                  NULL.
 * @param fd        int fd Readable file descriptor.
 * @param file_size size_t file_size Exact number of bytes to read.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation/
 *         read/EOF error.
 *
 * @note When file_size == 0, allocates one byte, writes '\0', sets *sz = 0.
 * @note Does not close FD.
 * @note *S is reallocated as needed; existing contents are not preserved.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readfilefd_len(char *R *R s, size_t *R sz, size_t *R cap, int fd, size_t file_size) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char *dst = *s;
	size_t left = file_size;
	ssize_t read_sz;
	for (; left; dst += read_sz, left -= (size_t)read_sz) {
		read_sz = read(fd, dst, left);
		/* Error */
		if (jstr_unlikely(read_sz < 0))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		/* EOF */
		if (jstr_unlikely(read_sz == 0))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_readfilefd
 *
 * fstat FD to obtain st_size, then delegate to jstr_io_readfilefd_len. Fills
 * *st with the stat result.
 *
 * @param s   char **s Destination buffer pointer. Must not be NULL.
 * @param sz  size_t *sz Receives byte count. Must not be NULL.
 * @param cap size_t *cap Capacity; updated. Must not be NULL.
 * @param fd  int fd Readable file descriptor.
 * @param st  struct stat *st Receives fstat result. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on fstat/read
 *         failure.
 *
 * @note Does not close FD.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readfilefd(char *R *R s, size_t *R sz, size_t *R cap, int fd, struct stat *st) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(fstat(fd, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return jstr_io_readfilefd_len(s, sz, cap, fd, (size_t)st->st_size);
}
#	else
;
#	endif

/**
 * @brief jstr_io_freadfilefp_len
 *
 * Read exactly file_size bytes from stream FP into *S (reserving
 * file_size + 1), NUL-terminate, and set *sz = file_size.
 *
 * @param s         char **s Destination buffer pointer. Must not be NULL.
 * @param sz        size_t *sz Receives byte count. Must not be NULL.
 * @param cap       size_t *cap Capacity; updated. Must not be NULL.
 * @param fp        FILE *fp Source stream. Must not be NULL.
 * @param file_size size_t file_size Exact number of bytes to read.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation/
 *         short read.
 *
 * @note Does not close FP.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_freadfilefp_len(char *R *R s, size_t *R sz, size_t *R cap, FILE *fp, size_t file_size) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, file_size + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(file_size != (size_t)jstr_io_fread(*s, 1, file_size, fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*(*s + file_size) = '\0';
	*sz = file_size;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_freadfilefp
 *
 * Obtain the file size via fstat(fileno(fp)) (or stat(FNAME) when fileno is
 * unavailable), then delegate to jstr_io_freadfilefp_len.
 *
 * @param s    char **s Destination buffer pointer. Must not be NULL.
 * @param sz   size_t *sz Receives byte count. Must not be NULL.
 * @param cap  size_t *cap Capacity; updated. Must not be NULL.
 * @param fname const char *fname File path (used only when fileno unavailable).
 *              May be NULL only if fileno is available.
 * @param fp   FILE *fp Source stream. Must not be NULL.
 * @param st   struct stat *st Receives stat result. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on stat/read
 *         failure.
 *
 * @note Does not close FP.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_freadfilefp(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, FILE *fp, struct stat *st) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		if JSTR_HAVE_FILENO
	const int fd = jstr_io_fileno(fp);
	if (jstr_unlikely(fd < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(fstat(fd, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#		else
	if (jstr_unlikely(stat(fname, st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#		endif
	return jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size);
	(void)fname;
}
#	else
;
#	endif

/**
 * @brief jstr_io_freadfile_len
 *
 * fopen FNAME with MODES, read file_size bytes via jstr_io_freadfilefp_len,
 * then fclose.
 *
 * @param s         char **s Destination buffer pointer. Must not be NULL.
 * @param sz        size_t *sz Receives byte count. Must not be NULL.
 * @param cap       size_t *cap Capacity; updated. Must not be NULL.
 * @param fname     const char *fname File path. Must not be NULL.
 * @param modes     const char *modes fopen mode string. Must not be NULL.
 * @param file_size size_t file_size Exact number of bytes to read.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/read/
 *         close failure.
 *
 * @note On failure the stream is closed before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_freadfile_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, const char *R modes, const size_t file_size) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)file_size))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fclose(fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_freadfile
 *
 * fopen FNAME, stat it (via fileno or path), read st_size bytes, then fclose.
 *
 * @param s     char **s Destination buffer pointer. Must not be NULL.
 * @param sz    size_t *sz Receives byte count. Must not be NULL.
 * @param cap   size_t *cap Capacity; updated. Must not be NULL.
 * @param fname const char *fname File path. Must not be NULL.
 * @param modes const char *modes fopen mode string. Must not be NULL.
 * @param st    struct stat *st Receives stat result. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/stat/
 *         read/close failure.
 *
 * @note On failure the stream is closed before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_freadfile(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, const char *R modes, struct stat *st) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	FILE *fp = fopen(fname, modes);
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#		if JSTR_HAVE_FILENO
	int fd;
	fd = jstr_io_fileno(fp);
	if (jstr_unlikely(fd < 0)) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fstat(fd, st))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
#		else
	if (jstr_unlikely(stat(fname, st))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
#		endif
	if (jstr_chk(jstr_io_freadfilefp_len(s, sz, cap, fp, (size_t)st->st_size))) {
		fclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(fclose(fp)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_readfile_len
 *
 * Open FNAME with O_RDONLY | oflag and read exactly file_size bytes via
 * jstr_io_readfilefd_len, then close.
 *
 * @param s         char **s Destination buffer pointer. Must not be NULL.
 * @param sz        size_t *sz Receives byte count. Must not be NULL.
 * @param cap       size_t *cap Capacity; updated. Must not be NULL.
 * @param fname     const char *fname File path. Must not be NULL.
 * @param oflag     int oflag Additional open flags (O_CLOEXEC, ...).
 * @param file_size size_t file_size Exact number of bytes to read.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/read/
 *         close failure.
 *
 * @note On failure the descriptor is closed before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readfile_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, int oflag, size_t file_size) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_io_readfilefd_len(s, sz, cap, fd, file_size))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_readfile
 *
 * Open FNAME read-only, determine its size (via lseek SEEK_END, or fstat when
 * pread is unavailable), read all bytes with pread (or read), NUL-terminate,
 * and close. Returns the byte count in *sz.
 *
 * @param s     char **s Destination buffer pointer. Must not be NULL.
 * @param sz    size_t *sz Receives byte count. Must not be NULL.
 * @param cap   size_t *cap Capacity; updated. Must not be NULL.
 * @param fname const char *fname File path. Must not be NULL.
 * @param oflag int oflag Additional open flags.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on open/size/
 *         read/close failure.
 *
 * @note An empty file succeeds with *sz == 0.
 * @note On failure the descriptor is closed before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readfile(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, int oflag) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	int fd = open(fname, oflag | O_RDONLY);
	if (jstr_unlikely(fd < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#		if JSTR_HAVE_PREAD
	const off_t file_size = lseek(fd, 0, SEEK_END);
	if (jstr_unlikely(file_size < 0)) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_chk(jstr_reserve(s, sz, cap, (size_t)file_size + 1))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	char *dst = *s;
	size_t off = 0;
	size_t left = (size_t)file_size;
	ssize_t read_sz;
	for (; left; dst += read_sz, left -= (size_t)read_sz, off += (long unsigned int)read_sz) {
		read_sz = pread(fd, dst, left, (ssize_t)off);
		/* Error */
		if (jstr_unlikely(read_sz < 0)) {
			close(fd);
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		}
		/* EOF */
		if (jstr_unlikely(read_sz == 0)) {
			close(fd);
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		}
	}
	if (jstr_unlikely(close(fd) < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*(*s + file_size) = '\0';
	*sz = (size_t)file_size;
	return JSTR_RET_SUCC;
#		else
	struct stat st;
	if (jstr_unlikely(fstat(fd, &st))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_chk(jstr_io_readfilefd_len(s, sz, cap, fd, (size_t)st.st_size))) {
		close(fd);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(close(fd) < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
#		endif
}
#	else
;
#	endif

/**
 * @brief jstr_io_expandtildefirst_len_unsafe_p
 *
 * If S begins with '~', expand it to the value of $HOME in place, assuming
 * the buffer already has enough space for the expansion. Returns a pointer to
 * the terminating NUL. If S does not start with '~', returns S + sz without
 * modifying the buffer. Returns NULL if $HOME is unset.
 *
 * @param s  char *s Writable buffer containing a '~'-prefixed path. Must not
 *           be NULL.
 * @param sz size_t sz Length of the string in S (excluding NUL).
 *
 * @return char * Pointer to the terminating NUL on success, NULL on error
 *                (missing HOME).
 *
 * @note The caller must ensure S has room for strlen(HOME) + rest + 1 bytes.
 * @note No allocation is performed; overflow is undefined behavior.
 * @note Passing NULL is undefined behavior.
 */
JSTR_FUNC
char *
jstr_io_expandtildefirst_len_unsafe_p(char *s, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (*s != '~')
		return s + sz;
	const char *home = getenv("HOME");
	if (jstr_nullchk(home))
		JSTR_RETURN_ERR_P(NULL);
	const size_t len = strlen(home);
	const size_t remaining = strlen(s + 1) + 1;
	jstr_strmove_len(s + len, s + 1, remaining);
	memcpy(s, home, len);
	return s + len + remaining - 1;
}
#	else
;
#	endif

/**
 * @brief jstr_io_expandtildefirst
 *
 * Safe variant of jstr_io_expandtildefirst_len_unsafe_p: if *S starts with
 * '~', reserve capacity, move the remainder, and prepend $HOME, updating *sz.
 * No-op (success) when the first character is not '~'.
 *
 * @param s   char **s Destination buffer pointer. Must not be NULL.
 * @param sz  size_t *sz Current length; updated on expansion. Must not be
 *            NULL.
 * @param cap size_t *cap Capacity; updated on growth. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success or no-op, JSTR_RET_ERR if
 *         $HOME is unset or allocation fails.
 *
 * @note Does nothing when **s does not start with '~'.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_expandtildefirst(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (**s != '~')
		return JSTR_RET_SUCC;
	const char *home = getenv("HOME");
	if (jstr_nullchk(home))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	const size_t len = strlen(home);
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	jstr_strmove_len(*s + len, *s + 1, JSTR_DIFF(*s + *sz, *s + 1));
	memcpy(*s, home, len);
	*sz += len - 1;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

#	if JSTR_HAVE_POPEN

/**
 * @brief jstr_io_readsystem
 *
 * Run CMD via popen, read its entire stdout into *S, NUL-terminate, and set
 * *sz. Grows the buffer as needed.
 *
 * @param s   char **s Destination buffer pointer. Must not be NULL.
 * @param sz  size_t *sz Receives byte count. Must not be NULL.
 * @param cap size_t *cap Capacity; updated. Must not be NULL.
 * @param cmd const char *cmd Shell command to execute. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on popen/read/
 *         pclose failure or if the command produces no output.
 *
 * @note Available only when JSTR_HAVE_POPEN is defined.
 * @note On failure resources are released before returning.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readsystem(char *R *R s, size_t *R sz, size_t *R cap, const char *R cmd) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	enum { MINBUF = JSTR_PAGE_SIZE };
	FILE *fp = popen(cmd, "r");
	if (jstr_nullchk(fp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char buf[MINBUF];
	size_t read_sz = jstr_io_fread(buf, 1, sizeof(buf), fp);
	if (jstr_unlikely(read_sz == 0)) {
		pclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	const size_t initialsz = (size_t)read_sz == sizeof(buf) ? read_sz * JSTR_GROWTH : read_sz + 1;
	if (jstr_chk(jstr_reserve(s, sz, cap, initialsz))) {
		pclose(fp);
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	memcpy(*s, buf, read_sz);
	*sz = read_sz;
	if (jstr_unlikely(read_sz == MINBUF)) {
		size_t requested_sz;
		for (;;) {
			requested_sz = *cap - *sz;
			read_sz = jstr_io_fread(*s + *sz, 1, requested_sz, fp);
			if (jstr_unlikely(read_sz == (size_t)-1)) {
				pclose(fp);
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
			*sz += read_sz;
			if (read_sz < requested_sz) {
				/* EOF */
				if (feof(fp))
					break;
				if (jstr_unlikely(ferror(fp))) {
					clearerr(fp);
					pclose(fp);
					JSTR_RETURN_ERR(JSTR_RET_ERR);
				}
			}
			if (*sz == *cap)
				if (jstr_chk(jstr_reserveexactalways(s, sz, cap, (size_t)(*cap * JSTR_GROWTH)))) {
					pclose(fp);
					JSTR_RETURN_ERR(JSTR_RET_ERR);
				}
		}
	}
	*(*s + *sz) = '\0';
	if (jstr_unlikely(pclose(fp) < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

#	endif

/**
 * @brief jstr_io_readstdin
 *
 * Read all of standard input into *S, NUL-terminate, and set *sz. Grows the
 * buffer as needed.
 *
 * @param s   char **s Destination buffer pointer. Must not be NULL.
 * @param sz  size_t *sz Receives byte count. Must not be NULL.
 * @param cap size_t *cap Capacity; updated. Must not be NULL.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on read/
 *         allocation failure.
 *
 * @note Reads until EOF on stdin.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_readstdin(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	char buf[BUFSIZ];
	ssize_t read_sz = read(STDIN_FILENO, buf, sizeof(buf));
	if (jstr_unlikely(read_sz == (ssize_t)-1))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	const size_t initialsz = (size_t)(read_sz == sizeof(buf) ? read_sz * JSTR_GROWTH : read_sz + 1);
	if (jstr_chk(jstr_reserve(s, sz, cap, (size_t)initialsz)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	memcpy(*s, buf, (size_t)read_sz);
	*sz = (size_t)read_sz;
	if (jstr_unlikely(read_sz == BUFSIZ)) {
		ssize_t requested_sz;
		for (;;) {
			requested_sz = (ssize_t)(*cap - *sz);
			read_sz = read(STDIN_FILENO, *s + *sz, (size_t)requested_sz);
			if (jstr_unlikely(read_sz == (ssize_t)-1))
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			*sz += (size_t)read_sz;
			if (read_sz < requested_sz)
				break;
			if (*sz == *cap)
				if (jstr_chk(jstr_reserveexactalways(s, sz, cap, (size_t)(*cap * JSTR_GROWTH))))
					JSTR_RETURN_ERR(JSTR_RET_ERR);
		}
	}
	*(*s + *sz) = '\0';
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief jstr_io_appendpath_len_p
 *
 * Append '/' followed by fname (fname_len bytes) to PATH at offset sz,
 * NUL-terminate, and return a pointer to the terminating NUL. Assumes PATH has
 * enough space. PATH is not reallocated.
 *
 * @param path      char *path Writable destination buffer. Must not be NULL.
 * @param sz        size_t sz Offset in PATH where the separator is written.
 * @param fname     const char *fname Component to append. Must not be NULL.
 * @param fname_len size_t fname_len Length of FNAME in bytes.
 *
 * @return char * (non-NULL) Pointer to the terminating NUL of the result.
 *
 * @note The caller must ensure PATH has at least sz + 1 + fname_len + 1 bytes.
 * @note fname need not be NUL-terminated.
 */
JSTR_FUNC_RET_NONNULL
char *
jstr_io_appendpath_len_p(char *R path, size_t sz, const char *R fname, size_t fname_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	*(path + sz) = '/';
	jstr_strcpy_len(path + sz + 1, fname, fname_len);
	return path + sz + 1 + fname_len;
}
#	else
;
#	endif

/**
 * @brief jstr_io_appendpath_p
 *
 * Like jstr_io_appendpath_len_p but derives fname length with strlen.
 *
 * @param path  char *path Writable destination buffer. Must not be NULL.
 * @param sz    size_t sz Offset in PATH where the separator is written.
 * @param fname const char *fname NUL-terminated component to append. Must not
 *              be NULL.
 *
 * @return char * (non-NULL) Pointer to the terminating NUL of the result.
 *
 * @note The caller must ensure PATH has enough space.
 */
JSTR_FUNC_RET_NONNULL
char *
jstr_io_appendpath_p(char *R path, size_t sz, const char *R fname) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	*(path + sz) = '/';
	return jstr_stpcpy(path + sz + 1, fname);
}
#	else
;
#	endif

/**
 * @brief jstr_io_appendpath_len
 *
 * Reserve capacity and append '/' + FNAME (fname_len bytes) to *S, updating
 * *sz. NUL-terminates the result.
 *
 * @param s         char **s Destination buffer pointer. Must not be NULL.
 * @param sz        size_t *sz Current length; updated. Must not be NULL.
 * @param cap       size_t *cap Capacity; updated on growth. Must not be NULL.
 * @param fname     const char *fname Component to append. Must not be NULL.
 * @param fname_len size_t fname_len Length of FNAME in bytes.
 *
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation
 *         failure.
 *
 * @note fname need not be NUL-terminated.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_appendpath_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R fname, size_t fname_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + fname_len + 2)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_io_appendpath_len_p(*s, *sz, fname, fname_len), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

#	ifdef _DIRENT_HAVE_D_NAMLEN
#		ifndef _D_EXACT_NAMLEN
#			define JSTR_DIRENT_D_EXACT_NAMLEN(d) ((d)->d_namlen)
#		else
#			define JSTR_DIRENT_D_EXACT_NAMLEN(d) _D_EXACT_NAMLEN(d)
#		endif
#		ifndef _D_ALLOC_NAMLEN
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d) \
				(JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
#		else
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d) _D_ALLOC_NAMLEN(d)
#		endif
#	else
#		ifndef _D_EXACT_NAMLEN
#			define JSTR_DIRENT_D_EXACT_NAMLEN(d) (strlen((d)->d_name))
#		else
#			define JSTR_DIRENT_D_EXACT_NAMLEN(d) _D_EXACT_NAMLEN(d)
#		endif
#		ifndef _D_ALLOC_NAMLEN
#			define JSTR_DIRENT_D_ALLOC_NAMLEN(d) _D_ALLOC_NAMLEN(d)
#		else
#			ifdef _DIRENT_HAVE_D_RECLEN
#				define JSTR_DIRENT_D_ALLOC_NAMLEN(d) \
					(((char *)(d) + (d)->d_reclen) - &(d)->d_name[0])
#			else
#				define JSTR_DIRENT_D_ALLOC_NAMLEN(d) \
					(sizeof(d)->d_name > 1 ? sizeof(d)->d_name : JSTR_DIRENT_D_EXACT_NAMLEN(d) + 1)
#			endif
#		endif
#	endif

/* If JSTR_IO_ACTION_RETVAL is passed, use these as return values of FUNC. */
typedef enum jstr_io_ftw_actionretval_ty {
	JSTR_IO_FTW_RET_STOP = 0,
#	define JSTR_IO_FTW_RET_STOP JSTR_IO_FTW_RET_STOP
	JSTR_IO_FTW_RET_CONTINUE,
#	define JSTR_IO_FTW_RET_CONTINUE JSTR_IO_FTW_RET_CONTINUE
	JSTR_IO_FTW_RET_SKIP_SIBLINGS,
#	define JSTR_IO_FTW_RET_SKIP_SIBLINGS JSTR_IO_FTW_RET_SKIP_SIBLINGS
	JSTR_IO_FTW_RET_SKIP_SUBTREE
#	define JSTR_IO_FTW_RET_SKIP_SUBTREE JSTR_IO_FTW_RET_SKIP_SUBTREE
} jstr_io_ftw_actionretval_ty;

typedef enum jstr_io_ftw_flag_ty {
	JSTR_IO_FTW_MATCHPATH = (1),
#	define JSTR_IO_FTW_MATCHPATH JSTR_IO_FTW_MATCHPATH
	JSTR_IO_FTW_REG = (JSTR_IO_FTW_MATCHPATH << 1),
#	define JSTR_IO_FTW_REG JSTR_IO_FTW_REG
	JSTR_IO_FTW_DIR = (JSTR_IO_FTW_REG << 1),
#	define JSTR_IO_FTW_DIR JSTR_IO_FTW_DIR
	JSTR_IO_FTW_NOSUBDIR = (JSTR_IO_FTW_DIR << 1),
#	define JSTR_IO_FTW_NOSUBDIR JSTR_IO_FTW_NOSUBDIR
	JSTR_IO_FTW_NOSTAT = (JSTR_IO_FTW_NOSUBDIR << 1),
#	define JSTR_IO_FTW_NOSTAT JSTR_IO_FTW_NOSTAT
	JSTR_IO_FTW_STATREG = (JSTR_IO_FTW_NOSTAT << 1),
#	define JSTR_IO_FTW_STATREG JSTR_IO_FTW_STATREG
	JSTR_IO_FTW_NOHIDDEN = (JSTR_IO_FTW_STATREG << 1),
#	define JSTR_IO_FTW_NOHIDDEN JSTR_IO_FTW_NOHIDDEN
	JSTR_IO_FTW_ACTIONRETVAL = (JSTR_IO_FTW_NOHIDDEN << 1)
#	define JSTR_IO_FTW_ACTIONRETVAL JSTR_IO_FTW_ACTIONRETVAL
} jstr_io_ftw_flag_ty;

#	if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#		define USE_ATFILE 1
#	else
#		define USE_ATFILE 0
#	endif

#	define NONFATAL_ERR() \
		(jstr_likely(errno == EACCES) || jstr_likely(errno == ENOENT))

#	if JSTR_HAVE_DIRENT_D_NAMLEN
#		define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)                           \
			do {                                                                              \
				*(dirpath + dirpath_len) = '/';                                           \
				jstr_strcpy_len(dirpath + dirpath_len + 1, (ep)->d_name, (ep)->d_namlen); \
				newpath_len = dirpath_len + 1 + (ep)->d_namlen;                           \
			} while (0)
#	else
#		define FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)                                         \
			do {                                                                                            \
				*(dirpath + dirpath_len) = '/';                                                         \
				newpath_len = JSTR_DIFF(jstr_stpcpy(dirpath + dirpath_len + 1, (ep)->d_name), dirpath); \
			} while (0)
#	endif

#	if USE_ATFILE
#		define OPENDIR(fd, fname) fdopendir(fd)
#		define STAT_DO(st, fd, ep, dirpath, do_on_nonfatal_err)               \
			do {                                                           \
				if (jstr_unlikely(fstatat(fd, (ep)->d_name, st, 0))) { \
					if (NONFATAL_ERR()) {                          \
						do_on_nonfatal_err;                    \
					}                                              \
					closedir(dp);                                  \
					JSTR_RETURN_ERR(JSTR_RET_ERR);                 \
				}                                                      \
			} while (0)
#		define STAT_ALWAYS(st, fd, ep, dirpath) \
			STAT_DO(st, fd, ep, dirpath = JSTR_IO_FTW_STATE_NS; goto do_fn)
#		define OPENAT(dstfd, srcfd, file, oflag, do_on_err)                           \
			do {                                                                   \
				if (jstr_unlikely((dstfd = openat(srcfd, file, oflag)) < 0)) { \
					do_on_err;                                             \
				}                                                              \
			} while (0)
#		define OPEN(fd, file, oflag, do_on_err)                           \
			do {                                                       \
				if (jstr_unlikely((fd = open(file, oflag)) < 0)) { \
					do_on_err;                                 \
				}                                                  \
			} while (0)
#		define CLOSE(fd, do_on_err)                            \
			do {                                            \
				if (fd != -1) {                         \
					if (jstr_unlikely(close(fd))) { \
						do_on_err;              \
					}                               \
					fd = -1;                        \
				}                                       \
			} while (0)
#	else
#		define OPENDIR(fd, fname) opendir(fname)
#		define STAT_DO(st, fd, ep, dirpath, do_on_nonfatal_err) \
			do {                                             \
				if (jstr_unlikely(stat(dirpath, st))) {  \
					if (NONFATAL_ERR()) {            \
						do_on_nonfatal_err;      \
					}                                \
					closedir(dp);                    \
					JSTR_RETURN_ERR(JSTR_RET_ERR);   \
				}                                        \
			} while (0)
#		define STAT_ALWAYS(st, fd, ep, dirpath) \
			STAT_DO(st, fd, ep, dirpath = JSTR_IO_FTW_STATE_NS; goto do_fn)
#		define OPENAT(dstfd, srcfd, file, oflag, do_on_err) \
			do {                                         \
			} while (0)
#		define OPEN(fd, file, oflag, do_on_err) \
			do {                             \
			} while (0)
#		define CLOSE(fd, do_on_err) \
			do {                 \
			} while (0)
#	endif

#	if JSTR_HAVE_DIRENT_D_TYPE
#		define IS_DIR(ep, st)    ((ep)->d_type == DT_DIR)
#		define IS_REG(ep, st)    ((ep)->d_type == DT_REG)
#		define STAT_MODE(st, ep) ((void)((st)->st_mode = DTTOIF((ep)->d_type)))
#		define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) \
			FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#		define STAT(st, fd, ep, dirpath) STAT_ALWAYS(st, fd, ep, dirpath)
#		define STAT_OR_MODE(st, fd, ep, dirpath)          \
			do {                                       \
				if (FLAG(JSTR_IO_FTW_NOSTAT))      \
					STAT_MODE(st, ep);         \
				else                               \
					STAT(st, fd, ep, dirpath); \
			} while (0)
#	else
#		define IS_DIR(ep, st) S_ISDIR((st)->st_mode)
#		define IS_REG(ep, st) S_ISREG((st)->st_mode)
#		if USE_ATFILE
#			define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) \
				FILL_PATH_ALWAYS(newpath_len, dirpath, dirpath_len, ep)
#		else
#			define FILL_PATH(newpath_len, dirpath, dirpath_len, ep) \
				do {                                             \
				} while (0)
#		endif
#		define STAT(st, fd, ep, dirpath) \
			do {                      \
			} while (0)
#		define STAT_MODE(st, ep) \
			do {              \
			} while (0)
#		define STAT_OR_MODE(st, fd, ep, dirpath) \
			do {                              \
			} while (0)
#	endif

#	if JSTR_IO_PATH_MAX <= 65536
typedef uint16_t jstr_io_path_size_ty;
#	elif JSTR_IO_PATH_MAX <= 4294967296
typedef uint32_t jstr_io_path_size_ty;
#	else
typedef size_t jstr_io_path_size_ty;
#	endif

struct JSTR_IO_FTW {
	const char *dirpath;
	const struct stat *st;
	const struct dirent *ep;
	jstr_io_path_size_ty dirpath_len;
};

typedef int (*jstr_io_ftw_func_ty)(const struct JSTR_IO_FTW *ftw, const void *args);
typedef int (*jstr_io_ftw_func_match_ty)(const char *fname, jstr_io_path_size_ty fname_len, const void *args);

struct jstr_internal_io_ftw_data {
	jstr_io_ftw_func_ty func;
	const void *func_args;
	jstr_io_ftw_func_match_ty func_match;
	const void *func_match_args;
	struct JSTR_IO_FTW ftw;
	int ftw_flags;
	int action_stop;
};

#	define JSTR_IO_FTW_FUNC(func_name, ftw, func_args) \
		int func_name(const struct JSTR_IO_FTW *ftw, const void *func_args)

#	define JSTR_IO_FTW_FUNC_MATCH(func_name, filename, filename_len, args) \
		int func_name(const char *filename, jstr_io_path_size_ty filename_len, const void *args)

#	ifdef O_DIRECTORY
#		define JSTR_INTERNAL_IO_O_DIRECTORY O_DIRECTORY
#	else
#		define JSTR_INTERNAL_IO_O_DIRECTORY 0
#	endif

#	define FLAG(x) ((a)->ftw_flags & (x))

/**
 * @brief jstr_internal_io_ftw_len
 *
 * Internal recursive implementation of the file tree walk. Iterates directory
 * entries, applies flags/filter, invokes the callback, and recurses into
 * subdirectories. Not part of the public API.
 *
 * @param a            struct jstr_internal_io_ftw_data *a Walk state. Must not
 *                     be NULL.
 * @param dirpath_len  jstr_io_path_size_ty dirpath_len Length of the current
 *                     directory path.
 * @param dirfd        int dirfd Directory file descriptor (or -1).
 *
 * @return int JSTR_RET_SUCC on success, JSTR_RET_ERR on error, or a callback
 *             result depending on flags.
 *
 * @note Internal helper; callers should use jstr_io_ftw_len.
 */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
int
jstr_internal_io_ftw_len(struct jstr_internal_io_ftw_data *a, jstr_io_path_size_ty dirpath_len, int dirfd) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	(void)dirfd;
	DIR *dp = OPENDIR(dirfd, a->ftw.dirpath);
	if (jstr_nullchk(dp)) {
#	if USE_ATFILE
		close(dirfd);
#	endif
		if (NONFATAL_ERR()) {
			if (FLAG(JSTR_IO_FTW_REG))
				if (!FLAG(JSTR_IO_FTW_DIR))
					return JSTR_RET_SUCC;
			if (a->func_match && a->func_match(a->ftw.dirpath, a->ftw.dirpath_len, a->func_match_args))
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
		if (FLAG(JSTR_IO_FTW_NOHIDDEN)) {
			if (a->ftw.ep->d_name[0] == '.')
				goto next_entry;
			/* Ignore "." and "..". */
		} else {
			if (a->ftw.ep->d_name[0] == '.' && (a->ftw.ep->d_name[1] == '\0' || (a->ftw.ep->d_name[1] == '.' && a->ftw.ep->d_name[2] == '\0')))
				goto next_entry;
		}
#		if JSTR_HAVE_DIRENT_D_NAMLEN
		if (jstr_unlikely(dirpath_len + JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep) >= JSTR_IO_PATH_MAX)) {
			errno = ENAMETOOLONG;
			closedir(dp);
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		}
#		else
		if (jstr_unlikely(dirpath_len >= JSTR_IO_PATH_MAX - JSTR_IO_NAME_MAX) && jstr_unlikely(dirpath_len + JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep) >= JSTR_IO_PATH_MAX)) {
			errno = ENAMETOOLONG;
			closedir(dp);
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		}
#		endif
#		if !JSTR_HAVE_DIRENT_D_TYPE
		/* We must construct the full path for stat if we
		 * don't have *_at functions. */
		if (!USE_ATFILE)
			FILL_PATH_ALWAYS(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, a->ftw.ep);
		STAT_DO((struct stat *)a->ftw.st,
		        dirfd,
		        a->ftw.ep,
		        a->ftw.dirpath,
		        /* If stat fails. */
		        if (FLAG(JSTR_IO_FTW_DIR | JSTR_IO_FTW_REG)) goto next_entry;
		        else goto func;);
#		endif
		if (IS_REG(a->ftw.ep, a->ftw.st))
			goto reg;
		if (IS_DIR(a->ftw.ep, a->ftw.st))
			goto dir;
		/* If true, ignore other types of files. */
		if (FLAG(JSTR_IO_FTW_DIR | JSTR_IO_FTW_REG))
			goto next_entry;
		goto do_reg;
reg:
		if (FLAG(JSTR_IO_FTW_DIR))
			if (!(FLAG(JSTR_IO_FTW_REG)))
				goto next_entry;
do_reg:
		if (a->func_match) {
			if (FLAG(JSTR_IO_FTW_MATCHPATH)) {
				/* FILL_PATH will construct the full path if
				 * either *_at functions or d_type aren't
				 * available. */
				FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, a->ftw.ep);
				if (a->func_match(a->ftw.dirpath, a->ftw.dirpath_len, a->func_match_args))
					goto next_entry;
			} else {
				const size_t fname_len = JSTR_DIRENT_D_EXACT_NAMLEN(a->ftw.ep);
				if (a->func_match(a->ftw.ep->d_name, fname_len, a->func_match_args))
					goto next_entry;
				/* We haven't constructed the full path since we
				 * have *_at functions. */
				if (USE_ATFILE)
					a->ftw.dirpath_len = jstr_io_appendpath_len_p((char *)a->ftw.dirpath, dirpath_len, a->ftw.ep->d_name, fname_len) - a->ftw.dirpath;
			}
		} else {
			/* Construct the full path when needed. If we don't have
			 * *_at functions, we have already constructed it and
			 * this will be a no-op. */
			FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, a->ftw.ep);
		}
		if (FLAG(JSTR_IO_FTW_STATREG)) {
			/* We must stat. */
			if (IS_REG(a->ftw.ep, a->ftw.st))
				STAT((struct stat *)a->ftw.st, dirfd, a->ftw.ep, a->ftw.dirpath);
			/* Only the st_mode is guaranteed to be initialized.
			 * This is so that we can avoid the stat when d_type is
			 * available. */
			else
				STAT_MODE((struct stat *)a->ftw.st, a->ftw.ep);
		} else {
			/* If the NOSTAT flag is passed, only guarantee st_mode. */
			STAT_OR_MODE((struct stat *)a->ftw.st, dirfd, a->ftw.ep, a->ftw.dirpath);
		}
func:
		tmp = a->func(&a->ftw, a->func_args);
		/* ACTIONRETVAL according to glibc ftw. */
		if (FLAG(JSTR_IO_FTW_ACTIONRETVAL)) {
			if (tmp == JSTR_IO_FTW_RET_CONTINUE || tmp == JSTR_IO_FTW_RET_SKIP_SUBTREE) {
				goto next_entry;
			} else if (tmp == JSTR_IO_FTW_RET_SKIP_SIBLINGS) {
				break;
			} else /* tmp == RET_STOP (== JSTR_RET_SUCC == 0) */ {
				/* RET_STOP and JSTR_RET_SUCC are both 0, so the stop
				 * cannot travel through the return value. Record it in
				 * the shared walk data instead. */
				a->action_stop = 1;
				closedir(dp);
				return JSTR_RET_SUCC;
			}
		} else {
			if (jstr_chk(tmp)) {
				closedir(dp);
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
		}
		goto next_entry;
dir:
		/* Go to next entry if we don't traverse subdirectories and
		 * don't need to call FUNC on directories. */
		if (FLAG(JSTR_IO_FTW_NOSUBDIR) && FLAG(JSTR_IO_FTW_REG) && !FLAG(JSTR_IO_FTW_DIR))
			goto next_entry;
		FILL_PATH(a->ftw.dirpath_len, (char *)a->ftw.dirpath, dirpath_len, a->ftw.ep);
		if (FLAG(JSTR_IO_FTW_STATREG))
			STAT_MODE(a->ftw.st, a->ftw.ep);
		else
			STAT_OR_MODE(a->ftw.st, dirfd, a->ftw.ep, a->ftw.dirpath);
		/* Don't call FUNC on directory if we don't need to. */
		if (FLAG(JSTR_IO_FTW_REG) && !FLAG(JSTR_IO_FTW_DIR))
			goto skip_fn;
		tmp = a->func(&a->ftw, a->func_args);
		if (FLAG(JSTR_IO_FTW_ACTIONRETVAL)) {
			if (tmp == JSTR_IO_FTW_RET_CONTINUE) {
				/* Recurse into this directory. */
				goto skip_fn;
			} else if (tmp == JSTR_IO_FTW_RET_SKIP_SUBTREE) {
				/* Don't recurse, but keep processing siblings. */
				goto next_entry;
			} else if (tmp == JSTR_IO_FTW_RET_SKIP_SIBLINGS) {
				/* Don't recurse; abort sibling iteration. */
				break;
			} else /* tmp == RET_STOP (== JSTR_RET_SUCC == 0) */ {
				a->action_stop = 1;
				closedir(dp);
				return JSTR_RET_SUCC;
			}
		} else {
			if (jstr_chk(tmp)) {
				closedir(dp);
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
		}
skip_fn:
		/* Don't traverse subdirectories when we don't need to. */
		if (FLAG(JSTR_IO_FTW_NOSUBDIR))
			goto next_entry;
		/* If we have *_at functions, open d_name to get the fd.
		 * Otherwise, no-op. */
		int filefd = -1;
		(void)filefd;
		OPENAT(filefd, dirfd, a->ftw.ep->d_name, O_RDONLY | JSTR_INTERNAL_IO_O_DIRECTORY, goto next_entry);
		tmp = jstr_internal_io_ftw_len(a, a->ftw.dirpath_len, filefd);
		if (FLAG(JSTR_IO_FTW_ACTIONRETVAL)) {
			if (jstr_unlikely(a->action_stop)) {
				closedir(dp);
				return JSTR_RET_SUCC;
			}
		} else {
			if (jstr_chk(tmp)) {
				closedir(dp);
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
		}
next_entry:;
	}
	closedir(dp);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

#	undef OPENDIR
#	undef NONFATAL_ERR
#	undef IS_DIR
#	undef IS_REG
#	undef FILL_PATH
#	undef FILL_PATH_ALWAYS
#	undef STAT_DO
#	undef STAT
#	undef STAT_ALWAYS
#	undef STAT_MODE
#	undef STAT_OR_MODE
#	undef JSTR_INTERNAL_IO_O_DIRECTORY
#	undef FLAG

#	define FLAG(x) (jstr_io_ftw_flags & (x))

/**
 * @brief jstr_io_ftw_len
 *
 * Recursively walk the directory tree rooted at DIRPATH (length
 * dirpath_len), invoking FUNC for entries selected by the flags and optional
 * FUNC_MATCH filter. Supports regular-file/directory selection, stat control,
 * hidden-entry skipping, depth control, and action return values.
 *
 * @param dirpath        const char *dirpath Root path (need not be NUL-
 *                       terminated; length given). Must not be NULL and length
 *                       must be > 0 and < JSTR_IO_PATH_MAX.
 * @param dirpath_len    jstr_io_path_size_ty dirpath_len Length of DIRPATH.
 * @param func           jstr_io_ftw_func_ty func Callback invoked for matching
 *                       entries. Must not be NULL.
 * @param func_args      const void *func_args User data passed to FUNC.
 * @param jstr_io_ftw_flags int Flags controlling traversal (JSTR_IO_FTW_*).
 * @param func_match     jstr_io_ftw_func_match_ty func_match Optional name/path
 *                       filter returning non-zero to skip. May be NULL.
 * @param func_match_args const void *func_match_args User data for FUNC_MATCH.
 *
 * @return int JSTR_RET_SUCC on success, JSTR_RET_ERR on error, or the callback
 *             result / JSTR_RET_STOP as documented.
 *
 * @note Non-fatal errors (EACCES/ENOENT) on individual entries do not abort
 *       the whole walk.
 * @note DIRPATH trailing slashes are trimmed (except root "/").
 * @note Path length must be < JSTR_IO_PATH_MAX.
 */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((4))
int
jstr_io_ftw_len(const char *R dirpath, jstr_io_path_size_ty dirpath_len, jstr_io_ftw_func_ty func, const void *func_args, int jstr_io_ftw_flags, jstr_io_ftw_func_match_ty func_match, const void *func_match_args) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(dirpath_len == 0)) {
		errno = ENOENT;
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	if (jstr_unlikely(dirpath_len >= JSTR_IO_PATH_MAX)) {
		errno = ENAMETOOLONG;
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	/* Don't copy trailing // to FULPATH. */
	for (; dirpath_len != 1 && dirpath[dirpath_len - 1] == '/'; --dirpath_len) {}
	char fulpath[JSTR_IO_PATH_MAX];
	jstr_strcpy_len(fulpath, dirpath, dirpath_len);
	int dirfd = -1;
	(void)dirfd;
	OPEN(dirfd, fulpath, O_RDONLY, JSTR_RETURN_ERR(JSTR_RET_ERR));
	struct stat st;
	struct jstr_internal_io_ftw_data data;
	data.ftw.dirpath = fulpath;
	data.ftw.st = &st;
	data.ftw.ep = NULL;
	/* This will avoid things like //some/path if DIRPATH is /. */
	if (jstr_unlikely(dirpath_len == 1) && jstr_unlikely(*dirpath == '/')) {
		data.ftw.dirpath_len = 0;
		goto ftw;
	}
	data.ftw.dirpath_len = dirpath_len;
#		if USE_ATFILE
	if (jstr_unlikely(fstat(dirfd, &st))) {
		CLOSE(dirfd, );
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
#		else
	if (jstr_unlikely(stat(fulpath, &st)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
#		endif
	/* If DIRPATH is a directory, call FUNC on directory when needed and
	 * call ftw. */
	if (jstr_likely(S_ISDIR(data.ftw.st->st_mode))) {
ftw:;
		if (FLAG(JSTR_IO_FTW_REG))
			if (!FLAG(JSTR_IO_FTW_DIR))
				goto file;
		int tmp;
		tmp = func(&data.ftw, func_args);
		if (FLAG(JSTR_IO_FTW_ACTIONRETVAL)) {
			if (tmp == JSTR_IO_FTW_RET_STOP) {
				CLOSE(dirfd, );
				return (jstr_ret_ty)tmp;
			}
			/* SKIP_SUBTREE/SKIP_SIBLINGS at root mean continue
			 * (no parent/siblings to skip, recursion hasn't started) */
			if (jstr_unlikely(tmp != JSTR_IO_FTW_RET_CONTINUE &&
					 tmp != JSTR_IO_FTW_RET_SKIP_SUBTREE &&
					 tmp != JSTR_IO_FTW_RET_SKIP_SIBLINGS)) {
				CLOSE(dirfd, );
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
		} else {
			if (jstr_chk(tmp)) {
				CLOSE(dirfd, );
				JSTR_RETURN_ERR(JSTR_RET_ERR);
			}
		}
file:;
		data.func = func;
		data.func_args = func_args;
		data.func_match = func_match;
		data.ftw_flags = jstr_io_ftw_flags;
		data.func_match_args = func_match_args;
		data.action_stop = 0;
		tmp = jstr_internal_io_ftw_len(&data, dirpath_len, dirfd);
		return tmp;
	}
	/* DIRPATH is not a directory. */
	CLOSE(dirfd, JSTR_RETURN_ERR(JSTR_RET_ERR));
	if (FLAG(JSTR_IO_FTW_REG))
		if (jstr_unlikely(!S_ISREG(data.ftw.st->st_mode)))
			return JSTR_RET_SUCC;
	if (func_match) {
		if (FLAG(JSTR_IO_FTW_MATCHPATH)) {
			if (func_match(fulpath, dirpath_len, func_match_args))
				return JSTR_RET_SUCC;
		} else {
			dirpath = (char *)jstr_memrchr(fulpath, '/', dirpath_len);
			const char *end = (char *)dirpath + dirpath_len;
			if (dirpath) {
				if (*++dirpath && func_match(dirpath, end - dirpath, func_match_args))
					return JSTR_RET_SUCC;
			} else {
				if (func_match(fulpath, dirpath_len, func_match_args))
					return JSTR_RET_SUCC;
			}
		}
	}
	return (jstr_ret_ty)func(&data.ftw, func_args);
}
#	else
;
#	endif

#	undef OPEN
#	undef OPENAT
#	undef CLOSE
#	undef USE_ATFILE
#	undef FLAG

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_IO_H */
