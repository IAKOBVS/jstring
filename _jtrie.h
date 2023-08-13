#ifndef JSTR_DEF_H
#define JSTR_DEF_H

#include "_jstr_macros.h"
#include <stdlib.h>

#define JTRIE_ASCII_SIZE 256

typedef enum {
	JTRIE_RET_NOERROR = 0,
	JTRIE_RET_MALLOC_ERROR = 1,
} Jtrie_errcode;

struct Jtrie_node {
	struct Jtrie_node *child[JTRIE_ASCII_SIZE];
	unsigned char EOW;
};

JSTR_INLINE
JSTR_WARN_UNUSED
static struct Jtrie_node *
jtrie_create(void) JSTR_NOEXCEPT
{
	return (struct Jtrie_node *)calloc(1, sizeof(struct Jtrie_node));
}

JSTR_MAYBE_UNUSED
static void
jtrie_destruct(struct Jtrie_node *JSTR_RST const node) JSTR_NOEXCEPT
{
	if (jstr_unlikely(!node))
		return;
	for (int i = 0; i != JTRIE_ASCII_SIZE - 1; ++i)
		jtrie_destruct(node->child[i]);
	free(node);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static Jtrie_errcode
jtrie_insert(struct Jtrie_node *JSTR_RST const root,
	     const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	struct Jtrie_node *curr = root;
	for (const unsigned char *w = (unsigned char *)word;
	     *w;
	     ++w) {
		if (!curr->child[*w]) {
			curr->child[*w] = jtrie_create();
			if (jstr_unlikely(!curr->child[*w]))
				return JTRIE_RET_MALLOC_ERROR;
		}
		curr = curr->child[*w];
	}
	curr->EOW = 1;
	return JTRIE_RET_NOERROR;
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void
jtrie_remove(struct Jtrie_node *JSTR_RST const root,
	     const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	struct Jtrie_node *curr = root;
	for (const unsigned char *w = (unsigned char *)word;
	     *w && curr->child[*w];)
		curr = curr->child[*w++];
	curr->EOW = 0;
}

/*
   Return value:
   1 if matches;
   0 otherwise.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static int
jtrie_match(const struct Jtrie_node *JSTR_RST const root,
	    const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const struct Jtrie_node *curr = root;
	for (const unsigned char *w = (unsigned char *)word;
	     *w;
	     ++w) {
		if (!curr->child[*w])
			return 0;
		curr = curr->child[*w];
	}
	return curr->EOW;
}

/*
   Return value:
   Pointer to node with last letter of WORD.
   NULL if not found.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static struct Jtrie_node *
jtrie_starts_with(const struct Jtrie_node *JSTR_RST const root,
		  const char *JSTR_RST const word) JSTR_NOEXCEPT
{
	const struct Jtrie_node *curr = root;
	for (const unsigned char *w = (unsigned char *)word;
	     *w;
	     ++w) {
		if (!curr->child[*w])
			return NULL;
		curr = curr->child[*w];
	}
	return (struct Jtrie_node *)curr;
}

#endif /* JSTR_DEF_H */
