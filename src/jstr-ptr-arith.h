/* See LICENSE file for copyright and license details. */

#ifndef JSTR_PTR_ARITH_H
#define JSTR_PTR_ARITH_H 1

#define JSTR_PTR_DIFF(p1, p2) \
	((uintptr_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
#define JSTR_PTR_IS_ALIGNED(base, size) \
	(!(((uintptr_t)(base)) & ((size)-1)))
#define JSTR_ALIGN_DOWN(base, size) \
	((base) & -((uintptr_t)(size)))
#define JSTR_ALIGN_UP(base, size) \
	JSTR_ALIGN_DOWN((base) + (size)-1, (size))
#define JSTR_PTR_ALIGN_DOWN(base, size) \
	JSTR_ALIGN_DOWN((uintptr_t)(base), (size))
#define JSTR_PTR_ALIGN_UP(base, size) \
	JSTR_ALIGN_UP((uintptr_t)(base), (size))

#endif /* JSTR_PTR_ARITH_H */
