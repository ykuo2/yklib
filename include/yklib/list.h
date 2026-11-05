//
//  list.h
//
//  Created by Yu Kuo on 2026/7/9.
//
// Generic, non-intrusive list
// Users must not modify structure members directly.
/*
 The list owns its nodes
 - free(node) is done by yk_list_destroy or yk_list_clear
 
 The list does not inherently own node data
 - free(node->data) and free(node->data->...) are handled by callback destroy_fn
 
 Data cannot be NULL
 */

// Caller-owned: init, clear
// Library-owned: create, destroy



#ifndef YKLIB_LINKED_LIST_H
#define YKLIB_LINKED_LIST_H

#include <yklib/core/callback.h>
#include <stddef.h>  // size_t


// Generic Pointer based Node
struct yk_list_node{
    void *data;
    struct yk_list_node *next;
};

struct yk_list{
    struct yk_list_node *head;
    struct yk_list_node *tail;
    size_t size;
};

/* ===========
* Life cycle
* ========== */
/* Caller-owned, stack object: caller prepare, then init, clear */
void yk_list_init(struct yk_list *list);
// yk_destroy_fn: could be NULL, must not modify the list
void yk_list_clear(struct yk_list *list, yk_destroy_fn *destroy_fn);

/* Library-owned, heap object: create and destroy */
struct yk_list *yk_list_create(void);
/*
 * Destroys a list created by yk_list_create().
 * If list is NULL, this function has no effect.
 * If destroy_fn is non-NULL, it is called once for each payload.
 */
void yk_list_destroy(struct yk_list *list, yk_destroy_fn *destroy_fn);



/* ===========
* Capacity
* ========== */
size_t yk_list_size(const struct yk_list *list);

/* ===========
* Lookup
* ========== */
// key and compare_fn must not be NULL
struct yk_list_node *yk_list_find(const struct yk_list *list,
                                  yk_compare_fn *compare_fn,
                                  const void *key);

/* ===========
* Modification
* ========== */
// data must not be NULL
struct yk_list_node *yk_list_add_head(struct yk_list *list, void *data);
struct yk_list_node *yk_list_add_tail(struct yk_list *list, void *data);
struct yk_list_node *yk_list_insert_after(struct yk_list *list,
                          struct yk_list_node *node, void *data);
// return Returns NULL if node does not belong to list.
struct yk_list_node *yk_list_unlink(struct yk_list *list, struct yk_list_node *node);

/* ===========
* Traversal
* ========== */
// Visit all nodes in the list, applying visit_fn
void yk_list_visit(const struct yk_list *list, yk_visit_fn *visit_fn);

#endif // YKLIB_LINKED_LIST_H
