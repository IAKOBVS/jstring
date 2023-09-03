#ifndef JSTR_DEF_H
#define JSTR_DEF_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdlib.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "jstr_macros.h"

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
	unsigned int EOW;
} jtrie_node_ty;

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static jtrie_node_ty *
jtrie_init(void) JSTR_NOEXCEPT
{
	return (jtrie_node_ty *)calloc(1, sizeof(jtrie_node_ty));
}

JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
pjtrie_free_recur(jtrie_node_ty *JSTR_RST _node) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_node == NULL))
		return;
	for (int i = 0; i != JTRIE_ASCII_SIZE - 1; ++i)
		pjtrie_free_recur(_node->child[i]);
	free(_node);
	_node = NULL;
}

JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jtrie_free(jtrie_node_ty **JSTR_RST _node) JSTR_NOEXCEPT
{
	pjtrie_free_recur(*_node);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static jtrie_errcode_ty
jtrie_insert(jtrie_node_ty *JSTR_RST const _root,
	     const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_word == '\0'))
		return JTRIE_RET_NOERROR;
	const unsigned char *w = (unsigned char *)_word;
	jtrie_node_ty *curr = _root;
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

typedef enum {
	PJTRIE_FLAG_REMOVE_NOT_PREFIXES = 0,
	PJTRIE_FLAG_REMOVE_PREFIXES = 1,
} pjtrie_flag_rm_prefixes_ty;

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
pjtrie_remove(pjtrie_flag_rm_prefixes_ty _flag,
	      jtrie_node_ty *JSTR_RST const _root,
	      const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_word == '\0'))
		return;
	const unsigned char *w = (unsigned char *)_word;
	jtrie_node_ty *curr = _root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return;
	while (*++w && curr->child[*w]) {
		if (_flag & PJTRIE_FLAG_REMOVE_PREFIXES)
			curr->EOW = 0;
		curr = curr->child[*w];
	}
	curr->EOW = 0;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
jtrie_remove(jtrie_node_ty *JSTR_RST const _root,
	     const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	return pjtrie_remove(PJTRIE_FLAG_REMOVE_NOT_PREFIXES, _root, _word);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
jtrie_removeprefixes(jtrie_node_ty *JSTR_RST const _root,
		     const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	return pjtrie_remove(PJTRIE_FLAG_REMOVE_PREFIXES, _root, _word);
}

/*
   Return value:
   Pointer to _node with last letter of WORD.
   NULL if not found.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static jtrie_node_ty *
jtrie_starts_with(const jtrie_node_ty *JSTR_RST const _root,
		  const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_word == '\0'))
		return NULL;
	const unsigned char *w = (unsigned char *)_word;
	const jtrie_node_ty *curr = _root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return NULL;
	while (*++w && curr->child[*w])
		curr = curr->child[*w];
	return (jtrie_node_ty *)curr;
}

/*
   Return value:
   1 if matches;
   0 otherwise.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jtrie_match(const jtrie_node_ty *JSTR_RST const _root,
	    const char *JSTR_RST const _word) JSTR_NOEXCEPT
{
	return jtrie_starts_with(_root, _word)->EOW;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JSTR_DEF_H */
