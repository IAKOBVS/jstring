/* See LICENSE file for copyright and license details. */

#ifndef JTRIE_H
#define JTRIE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
PJSTR_END_DECLS

#define JTRIE_ASCII_SIZE 256

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

typedef enum {
	JTRIE_SUCC = 0,
	JTRIE_ERR = 1
} jtrie_ret_ty;

typedef struct jtrie_node_ty {
	struct jtrie_node_ty *child[JTRIE_ASCII_SIZE];
	unsigned int EOW;
} jtrie_node_ty;

JSTR_FUNC
JSTR_ATTR_INLINE
static jtrie_node_ty *
jtrie_init(void)
JSTR_NOEXCEPT
{
	return (jtrie_node_ty *)calloc(1, sizeof(jtrie_node_ty));
}

JSTR_FUNC_VOID_MAY_NULL
static void
pjtrie_free_recur(jtrie_node_ty *R node)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(node == NULL))
		return;
	for (int i = 0; i != JTRIE_ASCII_SIZE - 1; ++i)
		pjtrie_free_recur(node->child[i]);
	free(node);
	node = NULL;
}

JSTR_ATTR_INLINE
static void
jtrie_free(jtrie_node_ty *R *R node)
JSTR_NOEXCEPT
{
	pjtrie_free_recur(*node);
}

JSTR_FUNC
static jtrie_ret_ty
jtrie_add(jtrie_node_ty *R root,
          const char *R word)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*word == '\0'))
		return JTRIE_SUCC;
	const unsigned char *w = (unsigned char *)word;
	jtrie_node_ty *curr = root;
	for (; *w; ++w) {
		if (curr->child[*w] == NULL)
			curr->child[*w] = jtrie_init();
		curr = curr->child[*w];
		if (jstr_unlikely(curr == NULL))
			return JTRIE_ERR;
	}
	curr->EOW = 1;
	return JTRIE_SUCC;
}

typedef enum {
	PJTRIE_FLAG_REMOVE_NOT_PREFIXES = 0,
	PJTRIE_FLAG_REMOVE_PREFIXES = 1
} pjtrie_flag_remove_prefixes_ty;

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjtrie_remove(pjtrie_flag_remove_prefixes_ty flag,
              jtrie_node_ty *R root,
              const char *R word)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*word == '\0'))
		return;
	const unsigned char *w = (unsigned char *)word;
	jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return;
	while (*++w && curr->child[*w]) {
		if (flag & PJTRIE_FLAG_REMOVE_PREFIXES)
			curr->EOW = 0;
		curr = curr->child[*w];
	}
	curr->EOW = 0;
}

JSTR_FUNC_VOID
static void
jtrie_remove(jtrie_node_ty *R root,
             const char *R word)
JSTR_NOEXCEPT
{
	return pjtrie_remove(PJTRIE_FLAG_REMOVE_NOT_PREFIXES, root, word);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jtrie_removeprefixes(jtrie_node_ty *R root,
                     const char *R word)
JSTR_NOEXCEPT
{
	return pjtrie_remove(PJTRIE_FLAG_REMOVE_PREFIXES, root, word);
}

/*
   Return value:
   Pointer to node with last letter of WORD.
   NULL if not found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jtrie_node_ty *
jtrie_starts(const jtrie_node_ty *R root,
             const char *R word)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*word == '\0'))
		return NULL;
	const unsigned char *w = (unsigned char *)word;
	const jtrie_node_ty *curr = root->child[*w];
	if (jstr_unlikely(curr == NULL))
		return NULL;
	for (; *++w && curr->child[*w]; curr = curr->child[*w])
		;
	return (jtrie_node_ty *)curr;
}

/*
   Return value:
   1 if matches;
   0 otherwise.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jtrie_match(const jtrie_node_ty *R root,
            const char *R word)
JSTR_NOEXCEPT
{
	return jtrie_starts(root, word)->EOW;
}

PJSTR_END_DECLS

#undef R

#endif /* JTRIE_H */
