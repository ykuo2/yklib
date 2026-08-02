//
//  bst.h
//  yklib
//
//  Created by Yu Kuo on 2026/7/19.
//
//  Data cannot be NULL
//  Compare function need not consider NULL data

#ifndef YKLIB_BST_H
#define YKLIB_BST_H

#include <yklib/core/callback.h>
#include <stddef.h>
#include <stdbool.h>

/*
yk_bst_remove()

*/
 
struct yk_bst{
    struct yk_bst_node *root;
    size_t size;
    yk_compare_fn *compare;
};


/* ===========
* Life cycle
* ========== */
// for stack object
void yk_bst_init(struct yk_bst *bst, yk_compare_fn *compare_fn);  // must supply compare function for BST to work
void yk_bst_clear(struct yk_bst *bst, yk_destroy_fn *destroy_fn);
// for heap object
// allocate + init
struct yk_bst *yk_bst_create(yk_compare_fn *compare_fn);
// free node->data (if possible), nodes, and bst
void yk_bst_destroy(struct yk_bst *bst, yk_destroy_fn *destroy_fn);


/* ===========
* Capacity
* ========== */
size_t yk_bst_size(const struct yk_bst *bst);
bool yk_bst_is_empty(const struct yk_bst *bst);



/* ===========
* Lookup
* ========== */
// key must not be NULL, return *data
void *yk_bst_find(const struct yk_bst *bst, const void *key);  // compare_fn 已經在 BST，不需要提供
bool yk_bst_contains(const struct yk_bst *bst, const void *key);

/* ===========
* Modification
* ========== */
// data must not be NULL
bool yk_bst_insert(struct yk_bst *bst, void *data);
// key must not be NULL, return *data
void *yk_bst_remove(struct yk_bst *bst, const void *key);


/* ===========
* Traversal
* ========== */
void yk_bst_visit_inorder(const struct yk_bst *bst, yk_visit_fn *visit_fn);
void yk_bst_visit_preorder(const struct yk_bst *bst, yk_visit_fn *visit_fn);
void yk_bst_visit_postorder(const struct yk_bst *bst, yk_visit_fn *visit_fn);


#endif // !YKLIB_BST_H
