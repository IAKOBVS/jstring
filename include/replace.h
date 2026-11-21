
#ifndef JSTR_REPLACE_H
#	define JSTR_REPLACE_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#	ifdef JSTR_IMPLEMENTATION
#		undef JSTR_IMPLEMENTATION
#		include "builder.h"
#		include "ctype.h"
#		include "macros.h"
#		include "stdstring.h"
#		include "string.h"
#		include "internal/inplace.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

/**
 * @brief Insert SRC into DST[AT].
 * @details Return value: ptr to '\0' in S. Assume that S have enough space for SRC.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 * @warning Caller must ensure the destination buffer has sufficient capacity;
 *         no allocation or bounds checking is performed.
 */
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
JSTR_FUNC_VOID
void
jstr_insert_unsafe(char *R s, size_t sz, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	memmove(s + at + src_len, s + at, sz - at + 1);
	memcpy(s + at, src, src_len);
}
#	else
;
#	endif

/**
 * @brief Insert SRC into DST[AT].
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_insert_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	jstr_insert_unsafe(*s, *sz, at, src, src_len);
	*sz += src_len;
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Replace RPLC in S with FIND.
 * @details Return value: ptr to RPLC in S + RPLCLEN. NULL on error.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @param[in] find_len number of bytes in find
 * @return char * Pointer to the replacement within *s on success, NULL on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
char *
jstr_rplcat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R rplc, size_t rplc_len, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + rplc_len - find_len + 1)))
		JSTR_RETURN_ERR_P(NULL);
	if (jstr_likely(rplc_len != find_len))
		jstr_strmove_len(*s + at + rplc_len, *s + at + find_len, *sz - (at + find_len));
	*sz += rplc_len - find_len;
	return (char *)jstr_mempcpy(*s + at, rplc, rplc_len);
}
#	else
;
#	endif

/**
 * @brief Insert SRC after C in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] c character to match/replace/remove
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const char *p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s) + 1, src, src_len);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Insert SRC after all C in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] c character to match/replace/remove
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterallchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(src_len == 0 || *sz == 0))
		return JSTR_RET_SUCC;
	/* Pass 1: count matches so the buffer can be grown exactly once. */
	size_t cnt = 0;
	for (const char *p = *s; (p = (const char *)memchr(p, c, (size_t)JSTR_DIFF(*s + *sz, p))) != NULL; ++cnt, ++p) {}
	if (jstr_unlikely(cnt == 0))
		return JSTR_RET_SUCC;
	const size_t new_sz = *sz + cnt * src_len;
	if (jstr_chk(jstr_reservealways(s, sz, cap, new_sz + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char *R S = *s;
	const size_t old_sz = *sz;
	/* Pass 2: rebuild back-to-front. Reads always happen below writes,
	 * so every byte of the original data is consumed before overwritten. */
	size_t rd = old_sz;
	size_t wr = new_sz;
	while (rd != 0) {
		const char *m = (const char *)jstr_memrchr(S, c, rd);
		if (m == NULL)
			break;
		const size_t pos = (size_t)JSTR_DIFF(m, S);
		const size_t seg = rd - (pos + 1);
		if (seg != 0)
			memmove(S + wr - seg, m + 1, seg);
		wr -= seg;
		memcpy(S + wr - src_len, src, src_len);
		wr -= src_len;
		S
			[--wr] = *m;
		rd = pos;
	}
	*sz = new_sz;
	S
		[new_sz] = '\0';
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Insert SRC after end of NE in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_insertafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_insertafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s) + find_len, src, src_len);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Insert SRC after all end of NE in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_insertafterallchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	size_t cnt = 0;
	for (const char *p = *s;
	     (p = (const char *)jstr_memmem_exec(&t, p, (size_t)JSTR_DIFF(*s + *sz, p), find, find_len)) != NULL;
	     ++cnt)
		p += find_len;
	if (jstr_unlikely(cnt == 0))
		return JSTR_RET_SUCC;
	const size_t new_sz = *sz + cnt * src_len;
	if (jstr_chk(jstr_reservealways(s, sz, cap, new_sz + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	char *R S = *s;
	const size_t old_sz = *sz;
	/* Record match offsets (one malloc), then rebuild back-to-front:
	 * reads always happen below writes. */
	size_t *const offs = (size_t *)malloc(cnt * sizeof(size_t));
	if (jstr_nullchk(offs))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	{
		size_t k = 0;
		const char *p = S;
		while ((p = (const char *)jstr_memmem_exec(&t, p, (size_t)JSTR_DIFF(S + old_sz, p), find, find_len)) != NULL) {
			offs[k++] = (size_t)JSTR_DIFF(p, S);
			p += find_len;
		}
	}
	size_t rd = old_sz;
	size_t wr = new_sz;
	for (size_t i = cnt; i-- > 0;) {
		const size_t pos = offs[i];
		const size_t seg = rd - (pos + find_len);
		if (seg != 0)
			memmove(S + wr - seg, S + pos + find_len, seg);
		wr -= seg;
		memcpy(S + wr - src_len, src, src_len);
		wr -= src_len;
		memmove(S + wr - find_len, S + pos, find_len);
		wr -= find_len;
		rd = pos;
	}
	free(offs);
	*sz = new_sz;
	S
		[new_sz] = '\0';
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Replace first group of REJECT in S with RPLC.
 * @details Return value: Characters replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] reject NUL-terminated set of rejected characters
 * @param[in] rplc replacement bytes
 * @return size_t Number of characters replaced (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rplcspn(char *R s, const char *R reject, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(*s) && (*(s += strcspn(s, reject)))) {
		const size_t changed = strspn(s, reject);
		memset(s, rplc, changed);
		return changed;
	}
	return 0;
}
#	else
;
#	endif

/**
 * @brief Replace all REJECT in S with RPLC from START_IDX.
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] reject NUL-terminated set of rejected characters
 * @param[in] rplc replacement bytes
 * @return size_t Number of characters replaced (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rplcallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	char *p = s + start_idx;
	size_t changed = 0;
	size_t i;
	while (*p && (*(p += strcspn(p, reject)))) {
		i = strspn(p, reject);
		changed += i;
		p = jstr_stpset_len(p, rplc, i);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}
#	else
;
#	endif

/**
 * @brief Replace all REJECT in S with RPLC.
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] reject NUL-terminated set of rejected characters
 * @param[in] rplc replacement bytes
 * @return size_t Number of characters replaced (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rplcallspn(char *R s, size_t *R sz, const char *R reject, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcallspn_from(s, sz, 0, reject, rplc);
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC from START_IDX.
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, (size_t)JSTR_DIFF(s + *sz, p) - changed);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmspn_from(s, sz, 0, reject);
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC from START_IDX.
 * @details Return value: Characters replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmspn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, JSTR_DIFF(s + *sz, p) - changed);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC
 * @details Return value: Characters replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmspn_len(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmspn_len_from(s, sz, 0, reject);
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC from START_IDX.
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, s, sz, NULL, start_idx, (size_t)-1, JSTR_INPLACE_ZERO_NOOP);
	if (jstr_unlikely(*it.src_e == '\0') || !(*(it.src_e += strcspn(it.src_e, reject))))
		return 0;
	const char *m = it.src_e;
	size_t find_len = strspn(m, reject);
	it.changed = find_len;
	for (;;) {
		jstr_internal_inplace_stage(&it, m, find_len);
		jstr_internal_inplace_commit(&it, NULL, 0);
		if (!*it.src_e || !*(it.src_e += strcspn(it.src_e, reject)))
			break;
		m = it.src_e;
		find_len = strspn(m, reject);
		it.changed += find_len;
	}
	return jstr_internal_inplace_finalize(&it);
}
#	else
;
#	endif

/**
 * @brief Remove all REJECT in S with RPLC.
 * @details Return value: Characters replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmallspn_from(s, sz, 0, reject);
}
#	else
;
#	endif

/**
 * @brief Remove N Cs in S from START_IDX.
 * @details Return value: Number of Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] c character to match/replace/remove
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	const char *end = s + *sz;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, s, sz, NULL, start_idx, n, JSTR_INPLACE_ZERO_NOOP);
	if (jstr_unlikely(n == 0) || jstr_unlikely(*it.src_e == '\0') || !(it.src_e = (char *)memchr(it.src_e, c, JSTR_DIFF(end, it.src_e))))
		return 0;
	const char *m = it.src_e;
	for (;;) {
		jstr_internal_inplace_stage(&it, m, 1);
		jstr_internal_inplace_commit(&it, NULL, 0);
		--it.n;
		++it.changed;
		if (!it.n || !(it.src_e = (char *)memchr(it.src_e, c, JSTR_DIFF(end, it.src_e))))
			break;
		m = it.src_e;
	}
	return jstr_internal_inplace_finalize(&it);
}
#	else
;
#	endif

/**
 * @brief Remove N Cs in S.
 * @details Return value: Number of Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_len(char *R s, size_t *R sz, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, n);
}
#	else
;
#	endif

/**
 * @brief Remove all C in S from START_IDX.
 * @details Return value: ptr to '\0' in S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_len_from(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Remove all C in S.
 * @details Return value: ptr to '\0' in S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_len(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len(s, sz, c, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Remove N C in S from START_IDX.
 * @details Return value: Cs replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] c character to match/replace/remove
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, s, sz, NULL, start_idx, n, JSTR_INPLACE_ZERO_NOOP);
	if (jstr_unlikely(n == 0) || jstr_unlikely(*it.src_e == '\0') || !*(it.src_e = jstr_strchrnul((char *)it.src_e, c)))
		return 0;
	const char *m = it.src_e;
	for (;;) {
		jstr_internal_inplace_stage(&it, m, 1);
		jstr_internal_inplace_commit(&it, NULL, 0);
		--it.n;
		++it.changed;
		if (!it.n || !*(it.src_e = jstr_strchrnul((char *)it.src_e, c)))
			break;
		m = it.src_e;
	}
	return jstr_internal_inplace_finalize(&it);
}
#	else
;
#	endif

/**
 * @brief Remove all C in S.
 * @details Return value: Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmnchr(char *R s, size_t *R sz, int c, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, 0, c, n);
}
#	else
;
#	endif

/**
 * @brief Remove all C in S.
 * @details Return value: Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, start_idx, c, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Remove all C in S.
 * @details Return value: Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmallchr(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmallchr_from(s, sz, 0, c);
}
#	else
;
#	endif

/**
 * @brief Remove first C in S from START_IDX.
 * @details Return value: Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, start_idx, c, 1);
}
#	else
;
#	endif

/**
 * @brief Remove first C in S.
 * @details Return value: Cs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmchr_len(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, 1);
}
#	else
;
#	endif

/**
 * @brief Remove first C in S from START_IDX.
 * @details Return value: Cs replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmchr_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, start_idx, c, 1);
}
#	else
;
#	endif

/**
 * @brief Remove first C in S.
 * @details Return value: Cs replaced. SZ is only used to save the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] c character to match/replace/remove
 * @return size_t Number of occurrences removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmchr(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, 0, c, 1);
}
#	else
;
#	endif

/**
 * @brief Remove chars in REJECT in S.
 * @details Return value: chars replaced. SZ is only used to store the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_stripspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, s, sz, NULL, start_idx, (size_t)-1, JSTR_INPLACE_ZERO_NOOP);
	if (jstr_unlikely(*it.src_e == '\0') || !*(it.src_e += strcspn(it.src_e, reject)))
		return 0;
	const char *m = it.src_e;
	for (;;) {
		jstr_internal_inplace_stage(&it, m, 1);
		jstr_internal_inplace_commit(&it, NULL, 0);
		++it.changed;
		if (!*(it.src_e += strcspn(it.src_e, reject)))
			break;
		m = it.src_e;
	}
	if (it.changed)
		jstr_internal_inplace_finalize(&it);
	return it.changed;
}
#	else
;
#	endif

/**
 * @brief Remove chars in REJECT in S.
 * @details Return value: chars replaced. SZ is only used to store the length of S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] reject NUL-terminated set of rejected characters
 * @return size_t Number of characters removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_stripspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_stripspn_from(s, sz, 0, reject);
}
#	else
;
#	endif

/**
 * @brief Remove first HS in S.
 * @details Return value: ptr to '\0' in S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] find_len number of bytes in find
 * @return char * Pointer to the terminating NUL of the result (non-NULL).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_RET_NONNULL
char *
jstr_rmat_len_p(char *s, size_t sz, size_t at, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_strmove_len(s + at, s + at + find_len, JSTR_DIFF(s + sz, s + at + find_len));
	return s + sz - find_len;
}
#	else
;
#	endif

/**
 * @brief Remove first HS in S from START_IDX.
 * @details Return value: ptr to '\0' in S. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rm_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *p = (char *)jstr_memmem_exec(t, s + start_idx, *sz - start_idx, find, find_len);
	if (p == NULL)
		return 0;
	*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, s), find_len), s);
	return 1;
}
#	else
;
#	endif

/**
 * @brief Remove first HS in S.
 * @details Return value: ptr to '\0' in S. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rm_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rm_len_from_exec(t, s, sz, 0, find, find_len);
}
#	else
;
#	endif

/**
 * @brief Remove first HS in S from START_IDX.
 * @details Return value: ptr to '\0' in S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rm_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rm_len_from_exec(&t, s, sz, start_idx, find, find_len);
}
#	else
;
#	endif

/**
 * @brief Remove first HS in S.
 * @details Return value: ptr to '\0' in S.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rm_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rm_len_from(s, sz, 0, find, find_len);
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in REPLACE.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
void
jstr_rplcallchr(char *s, char find, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	for (; (s = strchr(s, find)); *s++ = rplc) {}
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in REPLACE.
 * @details Return value: number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences replaced (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rplcnchr_len(char *s, size_t sz, char find, char rplc, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const char *end = s + sz;
	size_t changed = 0;
	for (; n-- && (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc, ++changed) {}
	return changed;
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in REPLACE.
 * @details Return value: number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of occurrences replaced (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rplcnchr(char *s, char find, char rplc, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	size_t changed = 0;
	for (; n-- && (s = strchr(s, find)); *s++ = rplc, ++changed) {}
	return changed;
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in REPLACE.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
void
jstr_rplcallchr_len(char *s, size_t sz, char find, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const char *end = s + sz;
	for (; (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc) {}
}
#	else
;
#	endif

/**
 * @brief Replace last SEARCH in S with REPLACE.
 * @details Return value: number of number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rmlast_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	const char *p = jstr_strrstr_len(s + start_idx, *sz - start_idx, find, find_len);
	if (p) {
		*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, s), find_len), s);
		return 1;
	}
	return 0;
}
#	else
;
#	endif

/**
 * @brief Replace last SEARCH in S with REPLACE.
 * @details Return value: number of number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return int 1 if a match was removed, 0 if not found.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rmlast_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmlast_len_from(s, sz, 0, find, find_len);
}
#	else
;
#	endif

/**
 * @brief Replace last SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rplclast_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const char *p = jstr_strrstr_len(*s, *sz, find, find_len);
	if (p) {
		if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
			JSTR_RETURN_ERR(-1);
		return 1;
	}
	return 0;
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE from START_IDX.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	if (find_len == 1)
		return jstr_rmnchr_len_from(s, sz, start_idx, *find, n);
	if (jstr_unlikely(find_len == 0))
		return 0;
	const char *end = s + *sz;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, s, sz, NULL, start_idx, n, JSTR_INPLACE_ZERO_NOOP);
	if (jstr_unlikely(n == 0) || !(it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len)))
		return 0;
	const char *m = it.src_e;
	for (;;) {
		jstr_internal_inplace_stage(&it, m, find_len);
		jstr_internal_inplace_commit(&it, NULL, 0);
		--it.n;
		++it.changed;
		if (!it.n || !(it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len)))
			break;
		m = it.src_e;
	}
	return jstr_internal_inplace_finalize(&it);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_from_exec(t, s, sz, 0, find, find_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in S.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmall_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_exec(t, s, sz, find, find_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE from START_IDX.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rmn_len_from_exec(&t, s, sz, start_idx, find, find_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmn_len(char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_from(s, sz, 0, find, find_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in S.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @return size_t Number of matches removed (0 if none).
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
size_t
jstr_rmall_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len(s, sz, find, find_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
int
jstr_rplc_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *p = (char *)jstr_memmem_exec(t, *s + start_idx, *sz - start_idx, find, find_len);
	if (p == NULL)
		return 0;
	if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
		return -1;
	return 1;
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
int
jstr_rplc_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplc_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE from S + START_IDX.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp. The current implementation is O(n).
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcn_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	if (n == 1)
		return (size_t)jstr_rplc_len_from_exec(t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len);
	if (jstr_unlikely(n == 0))
		return 0;
	if (jstr_unlikely(rplc_len == 0))
		return jstr_rmn_len_from_exec(t, *s, sz, start_idx, find, find_len, n);
	if (find_len == 1) {
		if (rplc_len == 1)
			return jstr_rplcnchr_len(*s + start_idx, *sz - start_idx, *find, *rplc, n);
	} else if (jstr_unlikely(find_len == 0)) {
		return 0;
	}
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, *s, sz, cap, start_idx, n, JSTR_INPLACE_ZERO_NOOP);
	const char *end = it.end;
	if (rplc_len <= find_len) {
		/* Explicit state: SCAN -> COMMIT for each match, no allocation. */
		it.state = JSTR_INPLACE_SCAN;
		it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len);
		if (!it.src_e)
			return 0;
		const char *m = it.src_e;
		for (;;) {
			jstr_internal_inplace_stage(&it, m, find_len);
			jstr_internal_inplace_commit(&it, rplc, rplc_len);
			--it.n;
			++it.changed;
			if (!it.n || !(it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len)))
				break;
			m = it.src_e;
		}
		if (rplc_len != find_len)
			jstr_internal_inplace_finalize(&it);
		return it.changed;
	}
	/* Explicit GROW state: count pass, reserve/rebase, rebuild pass. */
	char *first = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len);
	if (jstr_nullchk(first))
		return 0;
	it.src_e = first;
	const char *last;
	do {
		++it.changed;
		last = it.src_e;
		it.src_e += find_len;
	} while (--it.n && (it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(end, it.src_e), find, find_len)));
	if (!it.changed)
		return 0;
	if (it.changed == 1) {
		if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(first, *s), rplc, rplc_len, find_len)))
			JSTR_RETURN_ERR_ZU((size_t)-1);
		return 1;
	}
	last += find_len;
	const size_t new_size = *sz + it.changed * (rplc_len - find_len);
	const size_t first_len = *sz - JSTR_DIFF(first, *s);
	const uintptr_t s_old = (uintptr_t)*s;
	it.state = JSTR_INPLACE_GROW;
	if (jstr_chk(jstr_reserve(s, sz, cap, new_size + first_len + 1)))
		JSTR_RETURN_ERR_ZU((size_t)-1);
	it.dst = *s + JSTR_DIFF(first, s_old);
	it.src = *s + new_size;
	memmove((void *)it.src, it.dst, first_len);
	last = it.src + (last - first);
	end = it.src + (end - first);
	first = (char *)it.src;
	it.end = end;
	it.n = it.changed;
	it.src_e = first;
	it.state = JSTR_INPLACE_SCAN;
	for (;;) {
		jstr_internal_inplace_stage(&it, it.src_e, find_len);
		jstr_internal_inplace_commit_ov(&it, rplc, rplc_len);
		--it.n;
		if (!it.n || !(it.src_e = (char *)jstr_memmem_exec(t, it.src_e, JSTR_DIFF(last, it.src_e), find, find_len)))
			break;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(it.dst, it.src, JSTR_DIFF(end, it.src)), *s);
	return it.changed;
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcn_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcall_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[in] t precompiled two-way matcher (from jstr_memmem_comp)
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcall_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced. T must be precompiled with jstr_memmem_comp.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
int
jstr_rplc_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplc_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len);
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
int
jstr_rplc_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplc_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE from S + START_IDX.
 * @details Return -1 on malloc error.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcn_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplcn_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcn_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}
#	else
;
#	endif

/**
 * @brief Replace all SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] start_idx starting byte offset in *s
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcall_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace N SEARCH in S with REPLACE.
 * @details Return -1 on malloc error. Otherwise, number of FINDs replaced.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] rplc replacement bytes
 * @param[in] rplc_len number of bytes in rplc
 * @return size_t Number of matches replaced (0 if none), or (size_t)-1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
size_t
jstr_rplcall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in REPLACE.
 * @details Return value: number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] sz in/out: current length of *s in bytes (updated on success)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rplcchr_len(char *s, size_t sz, char find, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	s = (char *)memchr(s, find, sz);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}
#	else
;
#	endif

/**
 * @brief Replace first SEARCH in REPLACE.
 * @details Return value: number of FINDs replaced.
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] find needle bytes to search for
 * @param[in] rplc replacement bytes
 * @return int 1 if replaced, 0 if no match, -1 on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC_VOID
int
jstr_rplcchr(char *s, char find, char rplc) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	s = strchr(s, find);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}
#	else
;
#	endif

/**
 * @brief Place SRC into DST[AT].
 * @details Assume that S have enough space for SRC. Return value:
 * @param[in,out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 * @warning Caller must ensure the destination buffer has sufficient capacity;
 *         no allocation or bounds checking is performed.
 */
JSTR_FUNC_VOID
void
jstr_place_len_unsafe(char *R s, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	memcpy(s + at, src, src_len);
}
#	else
;
#	endif

/**
 * @brief Place SRC into DST[AT].
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] at byte offset within *s where operation occurs
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_place_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	size_t old_sz = *sz;
	if (at + src_len > *sz) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, at + src_len + 1)))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		*sz = at + src_len;
		*(*s + *sz) = '\0';
		if (at > old_sz)
			memset(*s + old_sz, 0, at - old_sz);
	}
	jstr_place_len_unsafe(*s, at, src, src_len);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Place SRC after C in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] c character to match/replace/remove
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_placeafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const char *p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s) + 1, src, src_len);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Place SRC after end of NE in DST.
 * @details Return JSTR_RET_ERR on malloc error. Otherwise, JSTR_RET_SUCC.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] find needle bytes to search for
 * @param[in] find_len number of bytes in find
 * @param[in] src source bytes to insert/place
 * @param[in] src_len number of bytes in src
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_placeafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_placeafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s) + find_len, src, src_len);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

/**
 * @brief Return value:
 * @details JSTR_RET_ERR on error. JSTR_RET_SUCC otherwise.
 * @param[out] s target string buffer (mutable; NUL-terminated unless _mem variant)
 * @param[in,out] sz in/out: current length of *s in bytes (updated on success)
 * @param[in,out] cap in/out: allocated capacity of *s (updated on realloc)
 * @param[in] n maximum number of occurrences (SIZE_MAX/-1 means all)
 * @return jstr_ret_ty JSTR_RET_SUCC on success, JSTR_RET_ERR on allocation failure.
 * @note Pointer arguments must be non-NULL where dereferenced; operates on NUL-terminated
 *       buffers unless a _mem variant is used. Edge cases (empty input, zero length,
 *       no match, n == 0) are no-ops unless documented otherwise.
 */
JSTR_FUNC
jstr_ret_ty
jstr_repeat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(n <= 1))
		return JSTR_RET_SUCC;
	if (*sz != 0 && *sz > (SIZE_MAX - 1) / n)
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * n + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_repeat_len_unsafe_p(*s, *sz, n), *s);
	return JSTR_RET_SUCC;
}
#	else
;
#	endif

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_REPLACE_H */
