
#ifndef JSTR_BUILDER_H
#	define JSTR_BUILDER_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <errno.h>
#	include <limits.h>
#	include <stdarg.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
#	include <wchar.h>
JSTR_INTERNAL_END_DECLS

#	ifdef JSTR_IMPLEMENTATION
#		undef JSTR_IMPLEMENTATION
#		include "config.h"
#		include "ctype.h"
#		include "macros.h"
#		include "stdstring.h"
#		include "string.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

/**
 * \brief Alias for JSTR_RESTRICT.
 */
#	define R JSTR_RESTRICT

/**
 * \brief Growth factor used by reallocating reserve functions.
 */
#	define JSTR_GROWTH 2

/**
 * \brief Zero initializer for jstr_ty.
 */
#	define JSTR_INIT { 0 }

/**
 * \brief Iterate over each byte of a jstr_ty's data.
 */
#	define jstr_foreach(j, p) \
		for (char *p = ((j)->data), *_jstr_internal_foreach_end_##p = ((j)->data) + ((j)->size); p < _jstr_internal_foreach_end_##p; ++p)
/**
 * \brief Iterate over indices of a jstr_ty's data.
 */
#	define jstr_foreachi(j, i) \
		for (size_t i = 0, _jstr_internal_foreachi_end_##i = ((j)->size); i < _jstr_internal_foreachi_end_##i; ++i)
/**
 * \brief Compute the index of curr within j's data.
 */
#	define jstr_index(j, curr) JSTR_DIFF(curr, (j)->data)
/**
 * \brief Internal unchecked pointer to j's i-th byte.
 */
#	define jstr_internal_at(j, i) ((j)->data + (i))
/**
 * \brief Pointer to the first byte of j's data.
 */
#	define jstr_start(j) ((j)->data)
/**
 * \brief Pointer one past the last byte of j's data.
 */
#	define jstr_end(j) ((j)->data + (j)->size)
/**
 * \brief Bounds-checked (in debug) pointer to j's i-th byte.
 */
#	if JSTR_DEBUG
#		define jstr_at(j, i) \
			((i) <= (j)->size ? ((j)->data + (i)) : (assert((i) <= (j)->size), (j)->data))
#	else
#		define jstr_at(j, i) ((j)->data + (i))
#	endif

JSTR_INTERNAL_BEGIN_DECLS

/**
 * \brief Reset a raw character buffer to empty.
 *
 * \details Sets the first byte of \p s to '\0' when \p s is non-NULL and sets
 *          \p *sz to 0. Does not free memory or modify capacity.
 *
 * \param[in,out] s Pointer to buffer data; may be NULL, in which case only
 *                  the size is cleared.
 * \param[out] sz Non-NULL pointer to the current size; set to 0.
 * \pre sz != NULL.
 * \note Capacity is unchanged; caller retains allocated storage.
 * \warning If sz is NULL the behaviour is undefined.
 */
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE void
jstr_empty(char *R s, size_t *R sz) JSTR_NOEXCEPT
{
	if (jstr_likely(s != NULL))
		*s = '\0';
	*sz = 0;
}

/**
 * \brief Print diagnostic information for a jstr_ty to stderr.
 *
 * \details Prints size, capacity, size (as length) and data bytes. Uses the
 *          size field rather than strlen, so embedded NULs are reported.
 *
 * \param[in] j Non-NULL pointer to the string object.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if any fprintf/fwrite to stderr fails.
 * \pre j != NULL.
 * \note Writes to stderr.
 * \warning Not thread-safe with respect to stderr usage.
 */
JSTR_FUNC_VOID
jstr_ret_ty
jstr_debug(const jstr_ty *j) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	int ret;
	ret = fprintf(stderr, "size       :%zu.\n"
	                      "capacity     :%zu.\n",
	              j->size,
	              j->capacity);
	if (jstr_unlikely(ret < 0)) {
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	const char *data;
	size_t size;
	if (jstr_likely(j->data != NULL)) {
		data = j->data;
		/* The struct's size field is authoritative; strlen here would
		 * rescan the whole buffer (and lie about embedded NULs). */
		size = j->size;
	} else {
		data = "(null)";
		size = 0;
	}
	ret = fprintf(stderr, "strlen.  :%zu.\n", size);
	if (jstr_unlikely(ret < 0)) {
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	ret = fprintf(stderr, "data puts:%s.\n", data);
	if (jstr_unlikely(ret < 0)) {
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	ret = fputs("data     :", stderr);
	if (jstr_unlikely(ret < 0)) {
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	jstr_io_fwrite(j->data, 1, j->size, stderr);
	if (jstr_unlikely(ferror(stderr)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	ret = fputs(".\n", stderr);
	if (jstr_unlikely(ret < 0)) {
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Free a raw buffer and reset pointers and sizes.
 *
 * \details Calls free(*s), then sets *s to NULL, *cap to 0 and *sz to 0.
 *          free(NULL) is safe, so a NULL buffer is accepted.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer; cleared on return.
 * \param[in,out] sz Non-NULL pointer to size; set to 0.
 * \param[in,out] cap Non-NULL pointer to capacity; set to 0.
 * \pre s, sz and cap are non-NULL.
 * \note Does not free the jstr_ty struct itself.
 */
JSTR_FUNC_VOID
void
jstr_free(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	free(*s);
	*s = NULL;
	*cap = 0;
	*sz = 0;
}
#	else
;
#	endif

/**
 * \brief Free a jstr_ty's buffer and reset the struct.
 *
 * \details Delegates to jstr_free() on j->data, j->size and j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \pre j != NULL.
 */
JSTR_FUNC_VOID
void
jstr_free_j(jstr_ty *j) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_free(&j->data, &j->size, &j->capacity);
}
#	else
;
#	endif

/**
 * \brief Grow capacity to at least new_cap, always reallocating or growing.
 *
 * \details Starts from max(*cap, JSTR_MIN_CAP), multiplies by JSTR_GROWTH
 *          until >= new_cap, then reallocates. Updates *cap and *s on success;
 *          *sz is unchanged.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer; updated on success.
 * \param[in,out] sz Non-NULL pointer to size; ignored and unchanged.
 * \param[in,out] cap Non-NULL pointer to capacity; updated on success.
 * \param[in] new_cap Requested capacity including the NUL terminator.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if realloc fails (buffer and capacity unchanged).
 * \pre s, sz and cap are non-NULL.
 * \note new_cap == 0 still reallocates to at least JSTR_MIN_CAP.
 * \warning If realloc fails, *s and *cap are left unchanged.
 */
JSTR_FUNC
jstr_ret_ty
jstr_reservealways(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	size_t old_cap = JSTR_MAX(*cap, JSTR_MIN_CAP);
	while (old_cap < new_cap) {
		if (jstr_unlikely(old_cap > SIZE_MAX / JSTR_GROWTH))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		old_cap *= JSTR_GROWTH;
	}
	char *tmp = (char *)realloc(*s, old_cap);
	if (jstr_nullchk(tmp))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*cap = old_cap;
	*s = tmp;
	return JSTR_RET_SUCC;
	(void)sz;
}
#	else
;
#	endif

/**
 * \brief Reserve capacity if new_cap exceeds current capacity.
 *
 * \details Returns success without action when new_cap <= *cap; otherwise
 *          delegates to jstr_reservealways().
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] new_cap Requested capacity including the NUL terminator.
 * \return JSTR_RET_SUCC on success (no-op if new_cap <= *cap); JSTR_RET_ERR if realloc fails.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_reserve(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (new_cap <= *cap)
		return JSTR_RET_SUCC;
	return jstr_reservealways(s, sz, cap, new_cap);
}
#	else
;
#	endif

/**
 * \brief Reallocate to exactly new_cap when new_cap is non-zero.
 *
 * \details If new_cap != 0, reallocates *s to that size and updates *cap and
 *          *s only after the allocation succeeds. If new_cap == 0, skips
 *          realloc and returns success.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size; ignored.
 * \param[in,out] cap Non-NULL pointer to capacity; set to new_cap when non-zero.
 * \param[in] new_cap Exact capacity including the NUL terminator.
 * \return JSTR_RET_SUCC on success (no-op if new_cap == 0); JSTR_RET_ERR if realloc fails when new_cap != 0.
 * \pre s, sz and cap are non-NULL.
 * \note new_cap == 0 leaves capacity unchanged.
 * \warning On allocation failure *s and *cap are left unchanged.
 */
JSTR_FUNC
jstr_ret_ty
jstr_reserveexactalways(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(new_cap != 0)) {
		char *tmp = (char *)realloc(*s, new_cap);
		if (jstr_nullchk(tmp))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		*cap = new_cap;
		*s = tmp;
	}
	return JSTR_RET_SUCC;
	(void)sz;
}
#	else
;
#	endif

/**
 * \brief Reserve exact capacity if new_cap exceeds current capacity.
 *
 * \details Returns success without action when new_cap < *cap; otherwise
 *          delegates to jstr_reserveexactalways().
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] new_cap Requested exact capacity including the NUL terminator.
 * \return JSTR_RET_SUCC on success (no-op if new_cap < *cap); JSTR_RET_ERR if realloc fails.
 * \pre s, sz and cap are non-NULL.
 * \note The no-op condition uses strict less-than, so an equal new_cap
 *       still reallocates.
 */
JSTR_FUNC
jstr_ret_ty
jstr_reserveexact(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (new_cap < *cap)
		return JSTR_RET_SUCC;
	return jstr_reserveexactalways(s, sz, cap, new_cap);
}
#	else
;
#	endif

/**
 * \brief Shrink capacity to size + 1 (exact fit for current size).
 *
 * \details Calls jstr_reserveexactalways() with *sz + 1.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if realloc fails.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_shrink_to_fit(char *R *R s, size_t *R sz, size_t *R cap) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_reserveexactalways(s, sz, cap, *sz + 1);
}
#	else
;
#	endif

/**
 * \brief Write a jstr_ty's bytes to stdout without trailing newline.
 *
 * \details Writes j->size bytes from j->data to stdout and returns success
 *          only when all bytes are written.
 *
 * \param[in] j Non-NULL pointer to the string object.
 * \return JSTR_RET_SUCC if all j->size bytes are written; JSTR_RET_ERR otherwise.
 * \pre j != NULL.
 */
JSTR_FUNC
static JSTR_ATTR_INLINE
jstr_ret_ty
jstr_io_print(const jstr_ty *j) JSTR_NOEXCEPT
{
	return jstr_io_fwrite(j->data, 1, j->size, stdout) == j->size ? JSTR_RET_SUCC : JSTR_RET_ERR;
}

/**
 * \brief Write a jstr_ty's bytes to stdout followed by a newline.
 *
 * \details Writes j->size bytes then a '\n'; returns error if either write
 *          fails.
 *
 * \param[in] j Non-NULL pointer to the string object.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if writing data or the trailing newline fails.
 * \pre j != NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_io_println(const jstr_ty *j) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(jstr_io_fwrite(j->data, 1, j->size, stdout) != j->size))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_unlikely(jstr_io_putchar('\n') == EOF))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Internal variadic concatenation helper.
 *
 * \details Reserves *sz + arg_len + 1 bytes, then appends each NUL-terminated
 *          argument from the va_list using stpcpy, updating *sz by arg_len.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size; increased by arg_len.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] ap va_list of const char * arguments terminated by NULL.
 * \param[in] arg_len Total length of all arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if allocation fails.
 * \pre s, sz and cap are non-NULL.
 * \note Internal helper; not intended for direct use.
 */
JSTR_FUNC
jstr_ret_ty
jstr_internal_cat(char *R *R s, size_t *R sz, size_t *R cap, va_list ap, size_t arg_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	char *p;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + arg_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	p = *s + *sz;
	*sz += arg_len;
	const char *arg;
	for (; (arg = va_arg(ap, const char *)); p = jstr_stpcpy(p, arg)) {}
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Concatenate an array of (ptr, size) chunks into a buffer.
 *
 * \details Computes total size; returns success without changes when total is
 *          0. Otherwise reserves *sz + total + 1 bytes, copies each chunk
 *          with memcpy, NUL-terminates and updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] args Array of jstr_cat_arg_ty chunks; may be NULL when nargs is 0.
 * \param[in] nargs Number of chunks.
 * \return JSTR_RET_SUCC on success (no changes if total == 0); JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 * \note args entries provide ptr and size; ptr may be NULL only with size 0.
 */
JSTR_FUNC_MAY_NULL
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_NONNULL((3))
static JSTR_ATTR_INLINE
jstr_ret_ty
jstr_cat(char *R *R s, size_t *R sz, size_t *R cap, const jstr_cat_arg_ty *args, size_t nargs) JSTR_NOEXCEPT
{
	size_t total = 0;
	for (size_t i = 0; i < nargs; i++)
		total += args[i].size;
	if (jstr_unlikely(total == 0))
		return JSTR_RET_SUCC;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + total + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char *p = *s + *sz;
	*sz += total;
	for (size_t i = 0; i < nargs; i++) {
		memcpy(p, args[i].ptr, args[i].size);
		p += args[i].size;
	}
	*p = '\0';
	return JSTR_RET_SUCC;
}

/**
 * \brief Concatenate chunks into a jstr_ty.
 *
 * \details Delegates to jstr_cat() using j->data, j->size and j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] args Array of jstr_cat_arg_ty chunks.
 * \param[in] nargs Number of chunks.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre j != NULL.
 */
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
static JSTR_ATTR_INLINE
jstr_ret_ty
jstr_cat_j(jstr_ty *j, const jstr_cat_arg_ty *args, size_t nargs) JSTR_NOEXCEPT
{
	return jstr_cat(&j->data, &j->size, &j->capacity, args, nargs);
}

/**
 * \brief Append src (length src_len) to s at offset sz without bounds checks.
 *
 * \details Copies src_len bytes to s + sz and NUL-terminates, returning a
 *          pointer to the resulting '\0'.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size (offset at which to append).
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to copy.
 * \return Pointer to the terminating NUL in s (borrows s; no ownership transfer; caller ensures capacity).
 * \pre s has at least sz + src_len + 1 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
char *
jstr_append_len_unsafe_p(char *R s, size_t sz, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_stpcpy_len(s + sz, src, src_len);
}
#	else
;
#	endif

/**
 * \brief Append a NUL-terminated src to s at offset sz without bounds checks.
 *
 * \details Equivalent to strcpy(s + sz, src); assumes sufficient capacity.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size (offset at which to append).
 * \param[in] src NUL-terminated source string.
 * \pre s has enough space for src.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE void
jstr_append_unsafe(char *R s, size_t sz, const char *R src) JSTR_NOEXCEPT
{
	strcpy(s + sz, src);
}

/**
 * \brief Append a NUL-terminated src to s at offset sz without bounds checks.
 *
 * \details Equivalent to stpcpy(s + sz, src), returning a pointer to the
 *          resulting '\0'.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size (offset at which to append).
 * \param[in] src NUL-terminated source string.
 * \return Pointer to the terminating NUL in s (borrows s; no ownership transfer; caller ensures capacity).
 * \pre s has enough space for src.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC
char *
jstr_append_unsafe_p(char *R s, size_t sz, const char *R src) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_stpcpy(s + sz, src);
}
#	else
;
#	endif

/**
 * \brief Append src (length src_len) to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + src_len + 1 bytes, copies src_len bytes,
 *          NUL-terminates and updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to copy.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_append_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_append_len_unsafe_p(*s, *sz, src, src_len), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Fill existing buffer contents up to the first NUL with character c.
 *
 * \details Calls memset(s, c, strlen(s)); does not change length.
 *
 * \param[in,out] s NUL-terminated buffer.
 * \param[in] c Fill character (converted to unsigned char).
 * \pre s is non-NULL and NUL-terminated.
 * \note Does not resize or reallocate.
 */
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE void
jstr_strset(char *s, int c) JSTR_NOEXCEPT
{
	memset(s, c, strlen(s));
}

/**
 * \brief Fill s with n copies of c and NUL-terminate without bounds checks.
 *
 * \details Writes n bytes of c followed by '\0', returning s + n.
 *
 * \param[out] s Buffer with at least n + 1 writable bytes.
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return Pointer to the terminating NUL in s (borrows s; no ownership transfer; caller ensures capacity).
 * \pre s has at least n + 1 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC
char *
jstr_assignnchr_unsafe_p(char *s, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	memset(s, c, n);
	*(s + n) = '\0';
	return s + n;
}
#	else
;
#	endif

/**
 * \brief Assign n copies of c to a managed buffer, truncating or growing.
 *
 * \details If n > *sz, grows capacity to at least n + 1. Fills n bytes with
 *          c, NUL-terminates and sets *sz = n. If n <= *sz, overwrites the
 *          first n bytes and shrinks the logical size to n.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if growth allocation fails (n > *sz).
 * \pre s, sz and cap are non-NULL.
 * \note n == 0 empties the buffer.
 */
JSTR_FUNC
jstr_ret_ty
jstr_assignnchr(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(n > *sz)) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, n + 1)))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
	}
	memset(*s, c, n);
	*(*s + n) = '\0';
	*sz = n;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Append n copies of c to s at offset sz without bounds checks.
 *
 * \details Fills s + sz with n bytes of c and returns s + sz + n. Does not
 *          write a trailing NUL terminator.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size (offset at which to append).
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return Pointer past the last filled byte (borrows s; no NUL written; caller ensures capacity).
 * \pre s has at least sz + n writable bytes.
 * \warning No bounds checking and no NUL termination.
 */
JSTR_FUNC
static JSTR_ATTR_INLINE char *
jstr_pushbackn_len_unsafe_p(char *s, size_t sz, int c, size_t n) JSTR_NOEXCEPT
{
	return (char *)memset(s + sz, c, n) + n;
}

/**
 * \brief Append n copies of c to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + n + 1 bytes, fills n copies of c at the end and
 *          updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_pushbackn(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + n + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_pushbackn_len_unsafe_p(*s, *sz, c, n), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Prepend n copies of c to s without bounds checks.
 *
 * \details Shifts the existing sz bytes right by n, fills the first n bytes
 *          with c and returns s + sz + n.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size.
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return Pointer past the original content (borrows s; caller ensures capacity).
 * \pre s has at least sz + n + 1 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC
char *
jstr_pushfrontn_len_unsafe_p(char *s, size_t sz, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_strmove_len(s + n, s, sz);
	return (char *)jstr_mempset(s, c, n) + sz;
}
#	else
;
#	endif

/**
 * \brief Prepend n copies of c to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + n + 1 bytes, shifts content and fills n copies at
 *          the front, updating *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] c Fill character.
 * \param[in] n Number of copies.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_pushfrontn(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + n + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_pushfrontn_len_unsafe_p(*s, *sz, c, n), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Prepend src (length src_len) to s at the front without bounds checks.
 *
 * \details Shifts existing content right by src_len when sz != 0; if sz is 0
 *          writes a NUL at s[src_len]. Copies src to the front and returns
 *          s + sz + src_len.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size.
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to prepend.
 * \return Pointer past the resulting content (borrows s; caller ensures capacity).
 * \pre s has at least sz + src_len + 1 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
char *
jstr_prepend_len_unsafe_p(char *R s, size_t sz, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(sz != 0))
		jstr_strmove_len(s + src_len, s, sz);
	else
		*(s + src_len) = '\0';
	memcpy(s, src, src_len);
	return s + sz + src_len;
}
#	else
;
#	endif

/**
 * \brief Prepend src (length src_len) to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + src_len + 1 bytes, prepends src and updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to prepend.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_prepend_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_prepend_len_unsafe_p(*s, *sz, src, src_len), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Assign src (length src_len) to s without bounds checks.
 *
 * \details Copies src_len bytes to s and NUL-terminates, returning a pointer
 *          to the resulting '\0'.
 *
 * \param[out] s Buffer with at least src_len + 1 writable bytes.
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to copy.
 * \return Pointer to the terminating NUL in s (borrows s; no ownership transfer; caller ensures capacity).
 * \pre s has at least src_len + 1 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC
char *
jstr_assign_len_unsafe_p(char *R s, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_stpcpy_len(s, src, src_len);
}
#	else
;
#	endif

/**
 * \brief Assign src (length src_len) to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + src_len + 1 bytes, copies src_len bytes,
 *          NUL-terminates and sets *sz = src_len.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size; set to src_len.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] src Source bytes; must point to at least src_len bytes.
 * \param[in] src_len Number of bytes to copy.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_assign_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Append a single character to s at offset sz without bounds checks.
 *
 * \details Writes c at s[sz], NUL-terminates at s[sz + 1] and returns
 *          s + sz + 1.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size (offset at which to append).
 * \param[in] c Character to append.
 * \return Pointer to the terminating NUL in s (borrows s; no ownership transfer; caller ensures capacity).
 * \pre s has at least sz + 2 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC
char *
jstr_pushback_unsafe_p(char *s, size_t sz, char c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	s += sz;
	*s = c;
	*++s = '\0';
	return s;
}
#	else
;
#	endif

/**
 * \brief Append a single character to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + 2 bytes, appends c, NUL-terminates and updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] c Character to append.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_pushback(char *R *R s, size_t *R sz, size_t *R cap, char c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + 1 + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_pushback_unsafe_p(*s, *sz, c), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Prepend a single character to s without bounds checks.
 *
 * \details Shifts sz bytes right by one, writes c at s[0] and returns
 *          s + sz + 1.
 *
 * \param[in,out] s Buffer with sufficient spare capacity.
 * \param[in] sz Current size.
 * \param[in] c Character to prepend.
 * \return Pointer past the resulting content (borrows s; caller ensures capacity).
 * \pre s has at least sz + 2 writable bytes.
 * \warning No bounds checking; caller must ensure capacity.
 */
JSTR_FUNC_VOID
char *
jstr_pushfront_unsafe_p(char *s, size_t sz, char c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_strmove_len(s + 1, s, sz);
	*s = c;
	return s + sz + 1;
}
#	else
;
#	endif

/**
 * \brief Prepend a single character to a managed buffer, growing as needed.
 *
 * \details Reserves *sz + 2 bytes, prepends c and updates *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] c Character to prepend.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR on allocation failure.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_FUNC
jstr_ret_ty
jstr_pushfront(char *R *R s, size_t *R sz, size_t *R cap, char c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + 1 + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_pushfront_unsafe_p(*s, *sz, c), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Remove the last character of s.
 *
 * \details If sz != 0, sets s[sz - 1] to '\0' and returns s + sz - 1.
 *          If sz == 0, returns s unchanged.
 *
 * \param[in,out] s Buffer to modify.
 * \param[in] sz Current size.
 * \return Pointer to the new terminator, or s when sz is 0 (borrows s; size not updated).
 * \note Does not update the caller's size variable.
 */
JSTR_FUNC_VOID
char *
jstr_popback_p(char *s, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(sz != 0)) {
		*(s + sz - 1) = '\0';
		return s + sz - 1;
	}
	return s;
}
#	else
;
#	endif

/**
 * \brief Remove the first character of s.
 *
 * \details If sz != 0, moves sz bytes left by one via memmove and returns
 *          s + sz - 1. If sz == 0, returns s unchanged.
 *
 * \param[in,out] s Buffer to modify.
 * \param[in] sz Current size.
 * \return Pointer to the new terminator, or s when sz is 0 (borrows s; size not updated).
 * \note Does not update the caller's size variable.
 */
JSTR_FUNC_VOID
char *
jstr_popfront_p(char *s, size_t sz) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(sz != 0)) {
		memmove(s, s + 1, sz);
		return s + sz - 1;
	}
	return s;
}
#	else
;
#	endif

#	if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) \
	|| (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 200112L)
#		define JSTR_HAVE_SNPRINTF_STRLEN 1
#	else
#		define JSTR_HAVE_SNPRINTF_STRLEN 0
#	endif

/**
 * \brief Return the maximum buffer size needed for a printf format.
 *
 * \details Computes an upper bound for vsprintf output including the NUL
 *          terminator. Uses vsnprintf(NULL, 0, ...) when available; otherwise
 *          a conservative estimator. Sets errno and returns -1 on invalid
 *          format or overflow.
 *
 * \param[in] ap va_list of format arguments.
 * \param[in] fmt Printf-style format string.
 * \return Required size including NUL terminator, or -1 on error (invalid format sets errno to EINVAL; overflow sets errno to EOVERFLOW).
 * \note The returned length is a conservative upper bound.
 */
JSTR_FUNC
JSTR_ATTR_FORMAT(printf, 2, 0)
int
jstr_vsprintf_maxlen(va_list ap, const char *fmt) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		if JSTR_HAVE_SNPRINTF_STRLEN && !JSTR_TEST
	const int ret = vsnprintf(NULL, 0, fmt, ap);
	if (jstr_likely(ret > 0) || ret == 0)
		return ret + 1;
	JSTR_RETURN_ERR(-1);
#		else
#			define JSTR_INTERNAL_COUNTDIGITS(lflag, base)       \
				if (lflag == L_INT)                          \
					arg_len += INT / base;               \
				else {                                       \
					if (lflag == L_LONG)                 \
						arg_len += LONG / base;      \
					else if (lflag == L_LONG_LONG)       \
						arg_len += LONG_LONG / base; \
					else                                 \
						goto einval;                 \
					lflag = L_INT;                       \
				}
#			define JSTR_INTERNAL_GETMAXDIGITS(length)                                 \
				do {                                                               \
					if (base == B_DEC)                                         \
						arg_len                                            \
						+= (is_thousep) ? DEC_##length : 2 * DEC_##length; \
					else if (base == B_HEX)                                    \
						arg_len                                            \
						+= (is_thousep) ? HEX_##length : 2 * DEC_##length; \
					else                                                       \
						arg_len                                            \
						+= (is_thousep) ? OCT_##length : 2 * DEC_##length; \
				} while (0)
	typedef enum {
		B_DEC = 10,
		B_OCT = 8,
		B_HEX = 16,
	} base_ty; /* base */
	enum {
		L_INT = 0,
		L_LONG,
		L_LONG_LONG,
	}; /* lflag */
	enum {
		WINT = CHAR_BIT * sizeof(wint_t),
		INT = CHAR_BIT * sizeof(int),
		LONG = CHAR_BIT * sizeof(long),
		LONG_LONG = CHAR_BIT * sizeof(long long),
		SIZE = CHAR_BIT * sizeof(size_t),
		UINTPTR = CHAR_BIT * sizeof(uintptr_t),
		FLOAT = 1 + CHAR_BIT * sizeof(float),
		DOUBLE = 1 + CHAR_BIT * sizeof(double),
		DOUBLE_LONG = 1 + CHAR_BIT * sizeof(long double),
		DEC_WINT = 1 + WINT / 10,
		DEC_INT = 1 + INT / 10,
		DEC_LONG = 1 + LONG / 10,
		DEC_LONG_LONG = 1 + LONG_LONG / 10,
		DEC_SIZE = 1 + SIZE / 10,
		DEC_UINTPTR = 1 + UINTPTR / 10,
		OCT_WINT = 1 + WINT / 8,
		OCT_INT = 1 + INT / 8,
		OCT_LONG = 1 + LONG / 8,
		OCT_LONG_LONG = 1 + LONG_LONG / 8,
		OCT_SIZE = 1 + SIZE / 8,
		OCT_UINTPTR = 1 + UINTPTR / 8,
		HEX_WINT = 1 + WINT / 16,
		HEX_INT = 1 + INT / 16,
		HEX_LONG = 1 + LONG / 16,
		HEX_LONG_LONG = 1 + LONG_LONG / 16,
		HEX_SIZE = 1 + SIZE / 16,
		HEX_UINTPTR = 1 + UINTPTR / 16,
	}; /* sizeof */
	/* Use size_t because we are calculating the length for the
	 * buffer conservatively and it may be much larger than the
	 * actual length. */
	const char *arg;
	size_t arg_len = 0;
	unsigned int errno_len = 0;
	int lflag = L_INT;
	int is_pad = 0;
	int is_thousep = 0;
	unsigned int pad_len;
	base_ty base;
	for (;; ++fmt) {
		if (*fmt == '%') {
cont_switch:
			switch (*++fmt) {
			case 's':
				arg = va_arg(ap, const char *);
				arg_len += jstr_likely(arg != NULL) ? strlen(arg) : sizeof("(null)") - 1;
				break;
			case 'c':
				if (jstr_likely(lflag == L_INT)) {
					++arg_len;
				} else if (lflag == L_LONG) {
					arg_len += INT;
					lflag = L_INT;
				} else {
					goto einval;
				}
				goto get_arg;
				/* int */
			case 'u':
			case 'd':
			case 'i': base = B_DEC; goto check_integer;
			case 'x':
			case 'X': base = B_HEX; goto check_integer;
			case 'o':
				base = B_OCT;
				goto check_integer;
check_integer:
				if (lflag == L_INT) {
					JSTR_INTERNAL_GETMAXDIGITS(INT);
				} else {
					if (lflag == L_LONG)
						JSTR_INTERNAL_GETMAXDIGITS(LONG);
					else if (lflag == L_LONG_LONG)
						JSTR_INTERNAL_GETMAXDIGITS(LONG_LONG);
					else
						goto einval;
					lflag = L_INT;
				}
				goto get_arg;
				/* ptr */
			case 'p':
				/* max + 2 to make room for the 0x at
				 * the start of the number. */
				arg_len += jstr_likely(va_arg(ap, const void *) != NULL) ? (HEX_UINTPTR + sizeof("0x") - 1) : sizeof("(nil)") - 1;
				break;
				/* chars written */
			case 'n':
				arg_len += INT;
				break;
				/* double */
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				if (lflag == L_INT) {
					arg_len += (is_thousep) ? DOUBLE * 2 : DOUBLE;
				} else if (lflag == L_LONG) {
					arg_len += (is_thousep) ? DOUBLE_LONG * 2 : DOUBLE_LONG;
					lflag = L_INT;
				} else {
					goto einval;
				}
				goto get_arg;
			case '%': arg_len += 2; break;
			case 'm':
				if (errno_len == 0)
					arg_len += strlen(strerror(errno));
				else
					arg_len += errno_len;
				break;
			/* padding */
			case '*':
				/* Get length of padding from argument. */
				arg_len += (size_t)va_arg(ap, int);
				goto cont_switch;
			case '-': is_pad = 1; pad_len = 1;
			/* flags */
			case '+':
			case '#':
			case '0':
			case ' ': goto cont_switch;
			case '\'': is_thousep = 1; goto cont_switch;
			/* precision */
			case '.':
				is_pad = 1;
				pad_len = 1;
				goto cont_switch;
			/* length modifier */
			case 'l': ++lflag; goto cont_switch;
			case 't': lflag = UINTPTR; goto cont_switch;
			case 'z': lflag = SIZE; goto cont_switch;
			case 'h':
			case 'j': goto cont_switch;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (is_pad) {
					const unsigned char *p = (const unsigned char *)fmt;
					if (!jstr_isdigit(*p)) {
						pad_len = 0;
						goto cont_switch;
					}
					pad_len = *p++ - '0';
					for (; jstr_isdigit(*p); ++p)
						pad_len = (pad_len * 10)
						          + (*p - '0');
					fmt = (char *)p - 1;
					if (jstr_unlikely(pad_len > INT_MAX)) {
						errno = EOVERFLOW;
						JSTR_RETURN_ERR(-1);
					}
					arg_len += pad_len;
				}
				goto cont_switch;
			/* case '\0': */
			default:
einval:
				errno = EINVAL;
				JSTR_RETURN_ERR(-1);
get_arg:
				va_arg(ap, void *);
				break;
			}
			lflag = L_INT;
			is_pad = 0;
			is_thousep = 0;
		} else {
			++arg_len;
			if (jstr_unlikely(*fmt == '\0'))
				break;
		}
	}
	++arg_len;
	if (jstr_unlikely(arg_len > INT_MAX))
		arg_len = INT_MAX;
	return arg_len;
#		endif
#		undef JSTR_INTERNAL_COUNTDIGITS
#		undef JSTR_INTERNAL_GETMAXDIGITS
}
#	else
;
#	endif

#	if JSTR_HAVE_SNPRINTF_STRLEN

/**
 * \brief Format into a managed buffer, replacing its contents.
 *
 * \details Computes required length, reserves exactly, formats with vsprintf
 *          and sets *sz. Supports a subset of conversions; invalid format
 *          sets errno to EINVAL and returns an error.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[out] sz Non-NULL pointer to size; set on success.
 * \param[out] cap Non-NULL pointer to capacity; updated on success.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre s, sz and cap are non-NULL; fmt is non-NULL.
 * \warning Caller must ensure format validity; unsupported conversions fail.
 */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int arg_len = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserveexact(s, sz, cap, (size_t)arg_len)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	arg_len = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = (size_t)arg_len;
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

/**
 * \brief Format into a jstr_ty, replacing its contents.
 *
 * \details Delegates to jstr_asprintf() using j->data, j->size and
 *          j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre j != NULL.
 */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf_j(jstr_ty *R j, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserveexact(&j->data, &j->size, &j->capacity, (size_t)ret)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	j->size = (size_t)ret;
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

/**
 * \brief Append formatted output to a managed buffer.
 *
 * \details Computes required length, reserves space after *sz, formats with
 *          vsprintf at the end and increases *sz.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre s, sz and cap are non-NULL.
 */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf_append(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + (size_t)ret)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	ret = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz += (size_t)ret;
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

/**
 * \brief Append formatted output to a jstr_ty.
 *
 * \details Delegates to jstr_asprintf_append() using j->data, j->size and
 *          j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre j != NULL.
 */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf_append_j(jstr_ty *R j, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserve(&j->data, &j->size, &j->capacity, j->size + (size_t)ret)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	ret = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	j->size += (size_t)ret;
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

/**
 * \brief Format into a managed buffer starting at start_idx.
 *
 * \details Computes required length, reserves start_idx + length bytes,
 *          formats at *s + start_idx and sets *sz = length + start_idx.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[in,out] sz Non-NULL pointer to size; set on success.
 * \param[in,out] cap Non-NULL pointer to capacity.
 * \param[in] start_idx Offset at which to begin writing.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre s, sz and cap are non-NULL.
 * \note Bytes before start_idx are not preserved.
 */
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserve(s, sz, cap, start_idx + (size_t)ret)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
	(void)cap;
}
#		else
;
#		endif

/**
 * \brief Format into a jstr_ty starting at start_idx.
 *
 * \details Delegates to jstr_asprintf_from() using j->data, j->size and
 *          j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] start_idx Offset at which to begin writing.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if format length computation, allocation, or vsprintf fails.
 * \pre j != NULL.
 */
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
jstr_ret_ty
jstr_asprintf_from_j(jstr_ty *R j, size_t start_idx, const char *R fmt, ...) JSTR_NOEXCEPT
#		ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserve(&j->data, &j->size, &j->capacity, start_idx + (size_t)ret)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	va_start(ap, fmt);
	ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	j->size = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
}
#		else
;
#		endif

#	endif

/**
 * \brief Format into an already-allocated buffer, replacing its contents.
 *
 * \details Calls vsprintf(*s, fmt, ap) without growing; sets *sz to the
 *          number of characters written.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[out] sz Non-NULL pointer to size; set on success.
 * \param[in,out] cap Non-NULL pointer to capacity; ignored.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if vsprintf fails (caller must ensure sufficient buffer capacity).
 * \pre Buffer capacity is sufficient; overflow is undefined.
 * \warning Caller must ensure the buffer has enough space.
 */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
jstr_ret_ty
jstr_sprintf(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = (size_t)ret;
	return JSTR_RET_SUCC;
	(void)cap;
}
#	else
;
#	endif

/**
 * \brief Format into an already-allocated jstr_ty, replacing its contents.
 *
 * \details Delegates to jstr_sprintf() using j->data, j->size and
 *          j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if vsprintf fails (caller must ensure sufficient buffer capacity).
 * \pre Buffer capacity is sufficient; overflow is undefined.
 */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
jstr_ret_ty
jstr_sprintf_j(jstr_ty *R j, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	j->size = (size_t)ret;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * \brief Format into an already-allocated buffer starting at start_idx.
 *
 * \details Calls vsprintf(*s + start_idx, fmt, ap) without growing; sets
 *          *sz = return value + start_idx.
 *
 * \param[in,out] s Non-NULL pointer to buffer pointer.
 * \param[out] sz Non-NULL pointer to size; set on success.
 * \param[in,out] cap Non-NULL pointer to capacity; ignored.
 * \param[in] start_idx Offset at which to begin writing.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if vsprintf fails (caller must ensure sufficient buffer capacity).
 * \pre Buffer capacity is sufficient; overflow is undefined.
 */
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
jstr_ret_ty
jstr_sprintf_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
	(void)cap;
}
#	else
;
#	endif

/**
 * \brief Format into an already-allocated jstr_ty starting at start_idx.
 *
 * \details Delegates to jstr_sprintf_from() using j->data, j->size and
 *          j->capacity.
 *
 * \param[in,out] j Non-NULL pointer to the string object.
 * \param[in] start_idx Offset at which to begin writing.
 * \param[in] fmt Printf-style format string.
 * \param[in] ... Format arguments.
 * \return JSTR_RET_SUCC on success; JSTR_RET_ERR if vsprintf fails (caller must ensure sufficient buffer capacity).
 * \pre Buffer capacity is sufficient; overflow is undefined.
 */
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
jstr_ret_ty
jstr_sprintf_from_j(jstr_ty *R j, size_t start_idx, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	j->size = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_BUILDER_H */
