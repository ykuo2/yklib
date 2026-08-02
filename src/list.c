//
//  list.c
//  implementation of linked list
//  Created by Yu Kuo on 2026/7/9.
//

#include <yklib/list.h>

#include <stddef.h>  // size_t
#include <stdlib.h>
#include <assert.h>

/* ===========================================
 *                   Private
 * =========================================== */
 static struct yk_list_node *list_create_node(void *data){
    assert(data!=NULL);  // data cannot be NULL
     
    // 抽象化 create_Node，給 add_head/tail/insert_after 使用
    // 若 Node 有增加欄位，可以一併在此處理
    struct yk_list_node *node = malloc(sizeof(*node));
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
// init linked list
void yk_list_init(struct yk_list *list){
    assert(list!=NULL);
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// for stack obj: clear
void yk_list_clear(struct yk_list *list, yk_destroy_fn *destroy_fn){
    // 若 node->data 不需要 destroy，destroy_fn 可以傳入 NULL
    assert(list!=NULL);
    
    struct yk_list_node *node = list->head;
    // 放在第一次要用，scope 最小的地方，所以放在 while 迴圈內較佳
    // struct yk_list_node *next_node = list->head;
    
    // free node and apply destroy_fn to each node->data
    while (node){
        // 保存 next node 位置
        struct yk_list_node *next_node = node->next;
        // 若 node->data 不需要 destroy，可以傳入 NULL
        if (destroy_fn){
            destroy_fn(node->data);
        }
        free(node);
        node = next_node;  // move node to next
    }
    
    // clear list
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// allocate then init linked list
struct yk_list *yk_list_create(void){
    struct yk_list *list = malloc(sizeof(*list));
    if (list==NULL){
        return NULL;
    }
    yk_list_init(list);
    return list;
}
// for heap obj: destroy and free
void yk_list_destroy(struct yk_list *list, yk_destroy_fn *destroy_fn){
    if (list==NULL){
        return;
    }
    
    // clear then free
    yk_list_clear(list, destroy_fn);
    free(list);
}


/* ===========
* Capacity
* ========== */
size_t yk_list_size(const struct yk_list *list){
    assert(list!=NULL);
    return list->size;
}

/* ===========
* Lookup
* ========== */
struct yk_list_node *yk_list_find(const struct yk_list *list,
                                  yk_compare_fn *compare_fn,
                                  const void *key){
    // input: linked list, compare function, key
    // output: node *, NULL
    assert(list!=NULL);
    assert(compare_fn!=NULL);
    assert(key!=NULL); // since data cannot be NULL, so as key
    
    struct yk_list_node *current = list->head;
    while (current){
        if (compare_fn(current->data, key)==0){
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/* ===========
* Modification
* ========== */

// Generic add data to head node
struct yk_list_node *yk_list_add_head(struct yk_list *list, void *data){
    assert(list!=NULL);
    assert(data!=NULL);  // data cannot be NULL

    struct yk_list_node *node = list_create_node(data);   // create head node
    if (node==NULL){
        return NULL;
    }
    
    if (list->head==NULL){  // if empty list
        list->head = node;  // 填入 head and tail
        list->tail = node;
    } else{
        node->next = list->head; // 取代原本 head
        list->head = node;
    }
    list->size++;
    return node;
}

// Generic add data to tail node
struct yk_list_node *yk_list_add_tail(struct yk_list *list, void *data){
    assert(list!=NULL);
    assert(data!=NULL);  // data cannot be NULL
    
    struct yk_list_node *node = list_create_node(data); // create tail node
    if (node==NULL){
        return NULL;
    }
    
    if (list->tail==NULL){  // if empty list
        list->head = node;  // 填入 head and tail
        list->tail = node;
    }else{
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
    return node;
}

// Generic inster new node after given node
struct yk_list_node *yk_list_insert_after(struct yk_list *list,
                          struct yk_list_node *node, void *data){
    assert(list!=NULL);
    assert(node!=NULL);
    assert(data!=NULL);  // data cannot be NULL
    
    struct yk_list_node *new_node = list_create_node(data);
    if(new_node==NULL){
        return NULL;
    }
    
    // insert node
    new_node->next = node->next;
    node->next = new_node;
    
    // special case: node==tail node
    if (node==list->tail){
        list->tail = new_node;
    }
    list->size++;
    return new_node;
}


// unlink node (not free the node)
struct yk_list_node *yk_list_unlink(struct yk_list *list, struct yk_list_node *node){
    assert(list!=NULL);
    assert(node!=NULL);
    
    struct yk_list_node *current = list->head;  // create traversal pointer
    
    // special case: if node==head
    if(list->head==node){
        list->head = node->next;
        // if node==tail
        if (list->tail==node){
            list->tail = NULL;
        }
        node->next = NULL; // clear node
        list->size--;
        return node;
    }
    
    while (current){
        // 判斷 pointer 是否一致
        if (current->next==node){
            // special case: if node==tail
            if (current->next==list->tail){
                list->tail = current;
            }
            current->next = node->next;
            node->next = NULL;
            list->size--;
            return node;
        }
        current = current->next;  // update
    }
    // if node not belong to this list
    return NULL;
}

/* ===========
* Traversal
* ========== */
// Visit all nodes in the list, applying visit_fn, eg. display
void yk_list_visit(const struct yk_list *list, yk_visit_fn *visit_fn){
    assert(list!=NULL);
    assert(visit_fn!=NULL);
    
    const struct yk_list_node *current_node = list->head;
    while (current_node){
        visit_fn(current_node->data);
        current_node = current_node->next;
    }
}



