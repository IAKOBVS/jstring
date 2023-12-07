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

#ifndef JTRIE_H
#define JTRIE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
PJSTR_END_DECLS

#define R JSTR_RESTRICT

#define JTRIE_ASCII_SIZE     256
#define jtrie_init()         (jtrie_node_ty *)calloc(1, sizeof(jtrie_node_ty))
#define jtrie_child(node, i) ((node)->child[(unsigned char)i])

PJSTR_BEGIN_DECLS

typedef enum {
	JTRIE_RET_SUCC = 0,
	JTRIE_RET_ERR = -1
} jtrie_ret_ty;

typedef struct jtrie_node_ty {
	struct jtrie_node_ty *child[JTRIE_ASCII_SIZE];
	int EOW;
} jtrie_node_ty;

JSTR_FUNC_VOID_MAY_NULL
static void
pjtrie_free_recur(jtrie_node_ty *R node)
JSTR_NOEXCEPT
{
	if (node == NULL)
		return;
	for (int i = 0; i < JTRIE_ASCII_SIZE; ++i)
		pjtrie_free_recur(jtrie_child(node, i));
	free(node);
}

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jtrie_free(jtrie_node_ty *R *R node)
JSTR_NOEXCEPT
{
	if (*node == NULL)
		return;
	pjtrie_free_recur(*node);
	*node = NULL;
}

JSTR_FUNC
static jtrie_ret_ty
jtrie_add(jtrie_node_ty *R root,
          const char *R word)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*word == '\0'))
		return JTRIE_RET_SUCC;
	const unsigned char *w = (unsigned char *)word;
	jtrie_node_ty *curr = root;
	for (int c; (c = *w++);) {
		if (jtrie_child(curr, c) == NULL)
			jtrie_child(curr, c) = jtrie_init();
		curr = jtrie_child(curr, c);
		if (jstr_unlikely(curr == NULL))
			goto err;
	}
	curr->EOW = 1;
	return JTRIE_RET_SUCC;
err:
	jtrie_free(&root);
	JSTR_RETURN_ERR(JTRIE_RET_ERR);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjtrie_remove(jtrie_node_ty *R root,
              const char *R word,
              const int remove_prefixes)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*word == '\0'))
		return;
	const unsigned char *w = (unsigned char *)word;
	jtrie_node_ty *curr = jtrie_child(root, *w);
	if (jstr_unlikely(curr == NULL))
		return;
	for (int c; (c = *w++) && jtrie_child(curr, c);) {
		if (remove_prefixes)
			curr->EOW = 0;
		curr = jtrie_child(curr, c);
	}
	curr->EOW = 0;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jtrie_remove(jtrie_node_ty *R root,
             const char *R word)
JSTR_NOEXCEPT
{
	pjtrie_remove(root, word, 0);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jtrie_removeprefixes(jtrie_node_ty *R root,
                     const char *R word)
JSTR_NOEXCEPT
{
	pjtrie_remove(root, word, 1);
}

/* Return value:
   Pointer to node with last letter of WORD.
   NULL if not found. */
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
	const jtrie_node_ty *curr = jtrie_child(root, *w);
	if (jstr_unlikely(curr == NULL))
		return NULL;
	for (int c; (c = *w++) && (curr = jtrie_child(curr, c));) {}
	return (jtrie_node_ty *)curr;
}

/* Return value:
   1 if matches;
   0 otherwise. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jtrie_match(const jtrie_node_ty *R root,
            const char *R word)
JSTR_NOEXCEPT
{
	root = jtrie_starts(root, word);
	return root ? root->EOW : 0;
}

PJSTR_END_DECLS

#undef R

#endif /* JTRIE_H */
