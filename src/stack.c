//
//  stack.c
//  yklib
//
//  Created by Yu Kuo on 2026/7/17.
//

#include <yklib/stack.h>
#include <stdlib.h>
#include <assert.h>



/* ===========================================
 *                   Private
 * =========================================== */
struct yk_stack_node{
    void *data;
    struct yk_stack_node *next;
};

static struct yk_stack_node *stack_create_node(void *data){
    assert(data != NULL);
    
    struct yk_stack_node *node = malloc(sizeof(*node));
    if (node==NULL){
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    return node;
}
/* ===========================================
*                   Public
* =========================================== */


/* ===========
* Life cycle
* ========== */
// init
void yk_stack_init(struct yk_stack *stack){
    assert(stack!=NULL);
    
    stack->top = NULL;
    stack->size = 0;
}
void yk_stack_clear(struct yk_stack *stack, yk_destroy_fn *destroy_fn){
    // destroy_fn may be NULL if node->data does not need or cannot be freed
    // If destroy_fn is NULL, payloads are left untouched.
    assert(stack!=NULL);
    
    struct yk_stack_node *node = stack->top;
    while (node){
        struct yk_stack_node *next = node->next;
        // free data if needed
        if (destroy_fn){
            destroy_fn(node->data);
        }
        free(node);
        node = next;
    }
    
    // restore empty stack invariant
    stack->top = NULL;
    stack->size = 0;
}

// allocate then init
struct yk_stack *yk_stack_create(void){
    struct yk_stack *stack = malloc(sizeof(*stack));
    if (stack==NULL){
        return NULL;
    }
    yk_stack_init(stack);
    return stack;
}


void yk_stack_destroy(struct yk_stack *stack, yk_destroy_fn *destroy_fn){
    if (stack==NULL){
        return;
    }
    
    yk_stack_clear(stack, destroy_fn);
    free(stack);
}

/* ===========
* Capacity
* ========== */

size_t yk_stack_size(const struct yk_stack *stack){
    assert(stack!=NULL);
    
    return stack->size;
}

bool yk_stack_is_empty(const struct yk_stack *stack){
    assert(stack!=NULL);
    
    return stack->size==0;
}

/* ===========
* Lookup
* ========== */
void *yk_stack_peek(const struct yk_stack *stack){
    assert(stack!=NULL);
    
    if (stack->top==NULL){
        return NULL;
    }
    return stack->top->data;
}


/* ===========
* Modification
* ========== */
// push data into stack
bool yk_stack_push(struct yk_stack *stack, void *data){
    assert(stack!=NULL);
    assert(data!=NULL);  // data cannot be NULL
    
    struct yk_stack_node *node = stack_create_node(data);
    if (node==NULL){
        return false;
    }
    
    node->next = stack->top;  // works even when stack is empty
    stack->top = node;
    stack->size++;
    return true;
}

void *yk_stack_pop(struct yk_stack *stack){
    assert(stack!=NULL);
    
    struct yk_stack_node *node = stack->top;
    
    if (node==NULL){
        return NULL;  // empty stack, nothing to pop
    }
    
    void *data = node->data;
    stack->top = node->next;
    stack->size--;
    // free at the end. 先完成 stack state 的更新，再做 resource cleanup
    free(node);
    
    return data;
}

/* ===========
* Traversal
* ========== */


