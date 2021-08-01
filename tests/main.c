#define _GNU_SOURCE


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "../src/bst.h"


struct bst_test_helper {
	struct bst bst;
	int val;
};

struct test {
	char desc[512];			/* description			*/
	int ret;			/* return value			*/
	int val;			/* function arg			*/
	int ptr_ptr_arg;		/* for selecting function, default no*/
	union {
		int (*others)(struct bst *, int);	/* test function */
		int (*ptr_ptr)(struct bst **, int);/* test function */
	} func;
	int arg_init;			/* arg initialized flag		*/
	struct bst arg;			/* initialized tree input	*/
	struct bst tree_after;		/* expected tree output		*/
};

/*
 * This test harness was an experiment to attempt a data-driven test harness
 * for C applications. This unfortunately doesn't work well for this project, as
 * compound literals are stack allocated and the functions under test expect
 * to allocate and free data that is allocated here. Currently it works because
 * of the macro that disables freeing memory when compiled with these tests.
 * That, unfortunately, makes memory access tools like Valgrind and asan useless
 * for use with these tests.
 */

struct test tests[] =  {
	{
		.ret = 0,
		.ptr_ptr_arg = 1,
		.func.ptr_ptr = &bst_insert,
		.val = 1,
		.arg_init = 0,
		.tree_after = {
			.val = 1,
			.left = NULL,
			.right = NULL,
		},
		.desc = "Insert one value",
	}, {
		.ret = 0,
		.func.others = &bst_search,
		.val = 1,
		.arg_init = 1,
		.arg = {
			.val = 1,
			.left = NULL,
			.right = NULL,
		},
		.tree_after = {
			.val = 1,
			.left = NULL,
			.right = NULL,
		},
		.desc = "Search for value, found",
	}, {
		.ret = ENOVAL,
		.func.others = &bst_search,
		.val = 1,
		.arg_init = 1,
		.arg = {
			.val = 0,
			.left = NULL,
			.right = NULL,
		},
		.tree_after = {
			.val = 0,
			.left = NULL,
			.right = NULL,
		},
		.desc = "Search for value, not found",
	}, {
		.ret = ENOVAL,
		.ptr_ptr_arg = 1,
		.func.ptr_ptr = &bst_delete,
		.val = 1,
		.arg_init = 1,
		.arg = {
			.val = 0,
			.left = NULL,
			.right = NULL,
		},
		.tree_after = {
			.val = 0,
			.left = NULL,
			.right = NULL,
		},
		.desc = "Delete one value, not found",
	}, {
		.ret = ENOVAL,
		.ptr_ptr_arg = 1,
		.func.ptr_ptr = &bst_delete,
		.val = 1,
		.arg_init = 1,
		.arg = {0},
		.tree_after = {0},
		.desc = "Delete one value, zero size",
	}, {
		.ret = 0,
		.ptr_ptr_arg = 1,
		.func.ptr_ptr = &bst_delete,
		.val = 1,
		.arg_init = 1,
		.arg = {
			.val = 1,
			.left = &(struct bst){
				.val = 0,
			},
			.right = &(struct bst){
				.val = 2,
			},
		},
		.tree_after = {
			.val = 2,
			.left = &(struct bst){
				.val = 0,
			},
			.right = NULL,
		},
		.desc = "Delete, tree size two",
	}, {
		.ret = 0,
		.ptr_ptr_arg = 1,
		.func.ptr_ptr = &bst_delete,
		.val = 1,
		.arg_init = 1,
		.arg = {
			.val = 1,
			.left = NULL,
			.right = NULL,
		},
		.tree_after = {0},
		.desc = "Delete one value, found",
	},
};

static void test_harness(struct test *t){
	int ret;
	struct bst *b = NULL;
	printf("TESTING [%s]\n", t->desc);
	fflush(stdout);

	if (t->arg_init){
		b = &t->arg;
	}
	if (t->ptr_ptr_arg)
		ret = t->func.ptr_ptr(&b, t->val);
	else
		ret = t->func.others(b, t->val);

	assert(b);
	assert(ret == t->ret);
	assert(bst_compare(&t->tree_after, b));
	//assert(!memcmp(&t->tree_after, b, sizeof(struct bst)));
	bst_free(b);
}

int main(){
	unsigned long i;
	struct test *t = tests;
	for(i=0; i<sizeof(tests) / sizeof(tests[0]); i++)
		test_harness(t++);
	puts("\n -- Tests succeeded --\n");
	return 0;
}
