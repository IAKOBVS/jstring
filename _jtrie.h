#ifndef JSTR_DEF_H
#define JSTR_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdlib.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "_jstr_macros.h"

#define JTRIE_ASCII_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	JTRIE_RET_NOERROR = 0,
	JTRIE_RET_MALLOC_ERROR = 1,
} jtrie_errcode_ty;

typedef struct jtrie_node_ty {
	struct jtrie_node_ty *child[JTRIE_ASCII_SIZE];
	unsigned char EOW;
} jtrie_node_ty;

JSTR_INLINE
JSTR_WARN_UNUSED
static jtrie_node_ty *
jtrie_init(void) JSTR_NOEXCEPT
{
	return (jtrie_node_ty *)calloc(1, sizeof(jtrie_node_ty));
}

JSTR_MAYBE_UNUSED
static void
priv_jtrie_free_recur(jtrie_node_ty *JSTR_RST node) JSTR_NOEXCEPT
{
	if (jstr_unlikely(node == NULL))
		return;
	for (int i = 0; i != JTRIE_ASCII_SIZE - 1; ++i)
		priv_jtrie_free_recur(node->child[i]);
	free(node);
	node = NULL;
}

JSTR_MAYBE_UNUSED
static void
jtrie_free(jtrie_node_ty **JSTR_RST node) JSTR_NOEXCEPT
{
	priv_jtrie_free_recur(*node);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jtrie_errcode_ty
jtrie_insert(jtrie_node_ty *JSTR_RST const root,
	     const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return JTRIE_RET_NOERROR;
	jtrie_node_ty *curr = root;
	for (; *w; ++w) {
		if (curr->child[*w] == NULL)
			curr->child[*w] = jtrie_init();
		curr = curr->child[*w];
		if (jstr_unlikely(curr == NULL))
			return JTRIE_RET_MALLOC_ERROR;
	}
	curr->EOW = 1;
	return JTRIE_RET_NOERROR;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jtrie_errcode_ty
jtrie_insertprefix(jtrie_node_ty *JSTR_RST const root,
		   const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return JTRIE_RET_NOERROR;
	jtrie_node_ty *curr = root;
	for (; *w; ++w) {
		if (curr->child[*w] == NULL)
			curr->child[*w] = jtrie_init();
		curr = curr->child[*w];
		if (jstr_unlikely(curr == NULL))
			return JTRIE_RET_MALLOC_ERROR;
		curr->EOW = 1;
	}
	curr->EOW = 1;
	return JTRIE_RET_NOERROR;
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void
jtrie_remove(jtrie_node_ty *JSTR_RST const root,
	     const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return;
	jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return;
	while (*++w && curr->child[*w])
		curr = curr->child[*w];
	curr->EOW = 0;
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void
jtrie_removeprefix(jtrie_node_ty *JSTR_RST const root,
		   const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return;
	jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return;
	while (*++w && curr->child[*w]) {
		curr->EOW = 0;
		curr = curr->child[*w];
	}
	curr->EOW = 0;
}

/*
   Return value:
   1 if matches;
   0 otherwise.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static int
jtrie_match(const jtrie_node_ty *JSTR_RST const root,
	    const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return 0;
	const jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return 0;
	while (*++w && curr->child[*w])
		curr = curr->child[*w];
	return curr->EOW;
}

/*
   Return value:
   Pointer to node with last letter of WORD.
   NULL if not found.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jtrie_node_ty *
jtrie_starts_with(const jtrie_node_ty *JSTR_RST const root,
		  const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const unsigned char *w = (unsigned char *)word;
	if (jstr_unlikely(*w == '\0'))
		return NULL;
	const jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return NULL;
	while (*++w && curr->child[*w])
		curr = curr->child[*w];
	return (jtrie_node_ty *)curr;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JSTR_DEF_H */
