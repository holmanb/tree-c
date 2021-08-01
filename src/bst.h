#ifndef _TREE
#define _TREE

#define ENOVAL 1
#ifndef __DEBUG_STACK
#define __DEBUG_STACK 0
#endif
#define bfree(p) if(!__DEBUG_STACK) free(p)

struct bst {
	struct bst *left;
	struct bst *right;
	int val;
};


struct bst *bst_init(void);
int bst_insert(struct bst **, int);
int bst_search(struct bst *, int);
int bst_delete(struct bst **, int);
int bst_compare(struct bst *, struct bst *);
void bst_free(struct bst *);

#endif /* _TREE */
