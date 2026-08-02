//
//  queue.h
//  yklib
//
//  Created by Yu Kuo on 2026/7/15.
//
//
// Generic, non-intrusive
/*
 The queue owns its nodes. Node structure is not exposed
 The queue does not inherently own node data
 data cannot be NULL
 */
// Caller-owned: init, clear
// Library-owned: create, destroy

#ifndef YKLIB_QUEUE_H
#define YKLIB_QUEUE_H

#include <yklib/core/callback.h>
#include <stddef.h>  // size_t
#include <stdbool.h>

/*
 * Objects
 */
// queue
struct yk_queue{
    struct yk_queue_node *head;
    struct yk_queue_node *tail;
    size_t size;
};

/* ===========
* Life cycle
* ========== */
/* Caller-owned queue, stack object: caller prepare, then init, clear */
// caller prepare queue obj and pass in to init
void yk_queue_init(struct yk_queue *queue);
// destroy_fn: could be NULL, must not modify the queue
void yk_queue_clear(struct yk_queue *queue, yk_destroy_fn *destroy_fn);

/* Library-owned queue, heap object: create and destroy */
// create = allocate and init
struct yk_queue *yk_queue_create(void);

/*
 * Destroys a queue created by yk_queue_create().
 * If queue is NULL, this function has no effect.
 * If destroy_fn is non-NULL, it is called once for each payload.
 */
void yk_queue_destroy(struct yk_queue *queue, yk_destroy_fn *destroy_fn); //

/* ===========
* Capacity
* ========== */
bool yk_queue_is_empty(const struct yk_queue *queue);
size_t yk_queue_size(const struct yk_queue *queue);


/* ===========
* Lookup
* ========== */
// return NULL if empty queue (data cannot be NULL)
void *yk_queue_front(const struct yk_queue *queue);  // return node->data
void *yk_queue_back(const struct yk_queue *queue);


/* ===========
* Modification
* ========== */
// true if succeed; data cannot be NULL
bool yk_queue_enqueue(struct yk_queue *queue, void *data);
// return NULL if empty queue (data cannot be NULL)
void *yk_queue_dequeue(struct yk_queue *queue);


/* ===========
* Traversal
* ========== */
// not implemented


#endif // !YKLIB_QUEUE_H
