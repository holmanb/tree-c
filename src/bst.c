#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include "bst.h"

#define DEBUGGING 1
#ifdef DEBUGGING

#define DEBUG(_format, ...)							\
	fprintf (stderr, "%s:%d" _format, __FILE__, __LINE__, __VA_ARGS__);	\
	fflush(stderr)

#define DEBUGS(...)							\
	DEBUG("%s", __VA_ARGS__)

#endif

static inline void bst_print_node(struct bst *b)
{
	DEBUG("{.left=%p, .right=%p, .val=%d,}\n", b->left, b->right, b->val);
}

static inline void bst_print(struct bst *b)
{
	if(!b){
		DEBUGS("bst is null\n");
		return;
	}
	if(b->left){
		bst_print(b->left);
	} else if (b->right) {
		bst_print(b->right);
	}
	bst_print_node(b);
}

static inline struct bst *new_node(int val)
{
	struct bst *new = calloc(1, sizeof(struct bst));
	if (new)
		new->val = val;

	return new;
}


inline int bst_insert(struct bst **bp, int val)
{
	struct bst *b = *bp;
	int ret = 0;
	struct bst *new;
	if(!b){
		new = new_node(val);
		if(!new)
			return -ENOMEM;
		*bp = new;
	} else {
		if(val < b->val)
			ret = bst_insert(&b->left, val);
		else
			ret = bst_insert(&b->right, val);
	}
	return ret;
}

inline int bst_search(struct bst *b, int val)
{
	int ret;
	if(!b){
		ret = ENOVAL;
	} else if (val == b->val) {
		ret = 0;
	} else {
		if(val < b->val)
			ret = bst_search(b->left, val);
		else
			ret = bst_search(b->right, val);
	}
	return ret;
}

static inline struct bst *bst_min(struct bst *b)
{
	while (b && b->left)
		b = b->left;
	return b;
}


static struct bst *_bst_delete(struct bst **bst, int val, int *deleted)
{
	struct bst *ret = *bst;
	struct bst *b = *bst;
	if (!b){
		ret = NULL;
	} else if (val < b->val) {
		b->left = _bst_delete(&b->left, val, deleted);
	} else if (val > b->val){
		b->right = _bst_delete(&b->right, val, deleted);
	} else {
		*deleted = 0;
		if (!b->left && !b->right){
			return NULL;
		} else if (b->left && b->right) {
			struct bst *tmp = bst_min(b->right);
			b->val = tmp->val;
			b->right = _bst_delete(&b->right, tmp->val, deleted);
		} else if (b->right){
			ret = b->right;
			bfree(b);
		} else if (b->left){
			ret = b->left;
			bfree(b);
		}
	}
	return ret;
}

inline int bst_delete(struct bst **b, int val)
{
	int deleted = ENOVAL;
	_bst_delete(b, val, &deleted);
	return deleted;
}

inline void bst_free(struct bst *b)
{
	if(!b)
		return;
	if(b->left){
		bst_free(b->left);
		bfree(b->left);
	}
	if (b->right){
		bst_free(b->right);
		bfree(b->right);
	}

	return;
}

#define boolean_eq(b1, b2)	\
	(b1 && b2) || (!b1 && !b2)

/* zero if match */
int bst_node_compare(struct bst *b1, struct bst *b2)
{
	return !(
		(!b1 && !b2) ||
			(
			boolean_eq(b1->left, b2->left) &&
			boolean_eq(b2->left, b2->left) &&
			b1->val == b2->val
			)
		);
}

/* shallow compare */
int bst_compare(struct bst *b1, struct bst *b2)
{
	int ret = 0;
	if(bst_node_compare(b1, b2))
		return 1;
	if (b1->left && b2->left){
		if(bst_compare(b1->left, b2->left))
			return 1;
	}
	if (b1->right && b2->right) {
		if(bst_compare(b1->left, b2->left))
			return 1;
	}

	return 1;
}
