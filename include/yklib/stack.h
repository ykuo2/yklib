//
//  stack.h
//  yklib
//
//  Created by Yu Kuo on 2026/7/17.
//
// NB: data cannot be NULL
/*
 * Stack invariant
 * size == number of reachable nodes from top
 * size == 0  <=> top == NULL
 * top always points to the most recently pushed node.
 * Every node is singly linked toward the bottom.
 */

#ifndef YKLIB_STACK_H
#define YKLIB_STACK_H

#include <yklib/core/callback.h>
#include <stdbool.h>
#include <stddef.h>

/*
 * Public for caller-owned allocation.
 * Members are implementation details and must not be modified directly.
 */
struct yk_stack{
    struct yk_stack_node *top;
    size_t size;
};

/* ===========
* Life cycle
* ========== */
// Caller owned stack
void yk_stack_init(struct yk_stack *stack);
void yk_stack_clear(struct yk_stack *stack, yk_destroy_fn *destroy_fn);
// Library owned stack
struct yk_stack *yk_stack_create(void);
void yk_stack_destroy(struct yk_stack *stack, yk_destroy_fn *destroy_fn);


/* ===========
* Capacity
* ========== */
size_t yk_stack_size(const struct yk_stack *stack);
bool yk_stack_is_empty(const struct yk_stack *stack);

/* ===========
* Lookup
* ========== */
void *yk_stack_peek(const struct yk_stack *stack);

/* ===========
* Modification
* ========== */
// push data into stack
bool yk_stack_push(struct yk_stack *stack, void *data);
// return data*
void *yk_stack_pop(struct yk_stack *stack);
// NB: NULL if empty stack; node->data cannot be NULL

/* ===========
* Traversal
* ========== */

#endif // !YKLIB_STACK_H

