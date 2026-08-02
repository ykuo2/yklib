//
//  queue.c
//  implementation of queue
//  Created by Yu Kuo on 2026/7/15.
//
// NB: data cannot be NULL
//
/*
 * Queue invariants:
 *
 * - An empty queue has head == tail == NULL and size == 0.
 * - A non-empty queue has valid head and tail, and tail->next == NULL.
 */


#include <yklib/queue.h>
#include <stdlib.h>
#include <stdbool.h>  // bool
#include <assert.h>

// TODO

/* ===========================================
 *                   Private
 * =========================================== */

/* Private, object */
struct yk_queue_node{
    void *data;
    struct yk_queue_node *next;
};

/* Private, static */
// create queue node, provided data pointer
static struct yk_queue_node *queue_create_node(void *data){
    assert(data != NULL);  // data cannot be NULL
    
    struct yk_queue_node *node = malloc(sizeof(*node));
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

void yk_queue_init(struct yk_queue *queue){
    assert(queue!=NULL);
    
    queue->head = NULL;  // next leaving node
    queue->tail = NULL;  // just added node
    queue->size = 0;
}

// destroy_fn must not modify the queue being cleared.
void yk_queue_clear(struct yk_queue *queue, yk_destroy_fn *destroy_fn){
    // 若 node->data 不需要 destroy，destroy_fn 可以傳入 NULL
    assert(queue!=NULL);

    struct yk_queue_node *node = queue->head;
    while (node){
        struct yk_queue_node *next = node->next;
        // destroy node->data
        if (destroy_fn){
            destroy_fn(node->data);
        }
        // destroy node
        free(node);
        node = next;
    }
    
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

// allocate then init
struct yk_queue *yk_queue_create(void){
    struct yk_queue *queue = malloc(sizeof(*queue));
    if (queue==NULL){
        return NULL;
    }
    // init
    yk_queue_init(queue);
    return queue;
}

void yk_queue_destroy(struct yk_queue *queue, yk_destroy_fn *destroy_fn){
    // yk_queue_destroy(NULL,...) is acceptable
    if (queue==NULL){
        return;
    }
    
    yk_queue_clear(queue, destroy_fn);
    free(queue);
}


/* ===========
* Capacity
* ========== */

bool yk_queue_is_empty(const struct yk_queue *queue){
    assert(queue!=NULL);
    
    return queue->size == 0;
}


size_t yk_queue_size(const struct yk_queue *queue){
    assert(queue!=NULL);
    
    return queue->size;
}

/* ===========
* Lookup
* ========== */

// return node->data
// return NULL if empty queue (data cannot be NULL)
void *yk_queue_front(const struct yk_queue *queue){
    assert(queue!=NULL);
    
    if (queue->head==NULL){  // empty queue
        return NULL;
    }
    return queue->head->data;
}

// return node->data
// return NULL if empty queue (data cannot be NULL)
void *yk_queue_back(const struct yk_queue *queue){
    assert(queue!=NULL);
    
    if (queue->tail==NULL){  // empty queue
        return NULL;
    }
    return queue->tail->data;
}

/* ===========
* Modification
* ========== */

// enqueue data and return if succeed
bool yk_queue_enqueue(struct yk_queue *queue, void *data){
    assert(queue!=NULL);
    assert(data!=NULL);  // data cannot be NULL
    
    // create new node
    struct yk_queue_node *node = queue_create_node(data);
    if (node==NULL){
        return false;
    }
    
    // add to tail
    if (queue->head==NULL){
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    // increment size
    queue->size++;
    
    return true;
}

// dequeue and return node->data
// return NULL if empty queue (data cannot be NULL)
void *yk_queue_dequeue(struct yk_queue *queue){
    assert(queue!=NULL);
    
    if (yk_queue_is_empty(queue)){
        return NULL;
    }
    struct yk_queue_node *node = queue->head;
    
    // update queue and node
    queue->head = node->next; // update queue head
    queue->size--;  // update queue size
    
    // if emptied after dequeue
    if (queue->size == 0){
        queue->tail = NULL;
        // head 不需要設置，因為 queue->head = node->next; head 就已經會是 NULL 了
    }

    void *data = node->data;
    free(node);
    return data;
}


/* ===========
* Traversal
* ========== */
// not implemented
