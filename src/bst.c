//
//  bst.c
//  yklib
//
//  Created by Yu Kuo on 2026/7/19.
//
// Binary Search Tree


#include <yklib/bst.h>

#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


/* ===========================================
 *                   Private
 * =========================================== */
struct yk_bst_node{
    void *data;
    struct yk_bst_node *left;
    struct yk_bst_node *right;
    struct yk_bst_node *parent;  // convienet for remove and rotation
};

static void bst_clear_subtree(struct yk_bst_node *node, yk_destroy_fn *destroy_fn){
    if (node==NULL){
        return;
    }
    // as post-order traversal
    bst_clear_subtree(node->left, destroy_fn);  // 先處理完所有 children node
    bst_clear_subtree(node->right, destroy_fn);
    // 再處理 root
    if (destroy_fn){  // if needed
        destroy_fn(node->data);
    }
    free(node);
}

static struct yk_bst_node *bst_create_node(void *data){
    assert(data!=NULL);  // data cannot be NULL
    
    struct yk_bst_node *node = malloc(sizeof(*node));
    if (node==NULL){
        return NULL;
    }
    node->data = data;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void bst_visit_inorder_inferior(const struct yk_bst_node *node, yk_visit_fn *visit_fn){
    // helper function for visit_inorder
    // 較差的 implementation
    // 一直往左邊，直到 left 沒有
    // 判斷有 subtree 才 call
    if (node->left){
        bst_visit_inorder_inferior(node->left, visit_fn);
    }
    visit_fn(node->data);  // print
    if (node->right){
        bst_visit_inorder_inferior(node->right, visit_fn);
    }
}

static void bst_visit_inorder(const struct yk_bst_node *node, yk_visit_fn *visit_fn){
    // helper function for visit_inorder
    // this is an empty subtree, back to parent
    // 直接 call，沒有的話就沒做事，返回
    if (node==NULL){
        return;
    }
    bst_visit_inorder(node->left, visit_fn);
    visit_fn(node->data);
    bst_visit_inorder(node->right, visit_fn);
    
}


static void bst_visit_preorder(const struct yk_bst_node *node, yk_visit_fn *visit_fn){
    if (node==NULL){
        return;
    }
    visit_fn(node->data);
    bst_visit_preorder(node->left, visit_fn);
    bst_visit_preorder(node->right, visit_fn);
}

static void bst_visit_postorder(const struct yk_bst_node *node, yk_visit_fn *visit_fn){
    if (node==NULL){
        return;
    }
    
    bst_visit_postorder(node->left, visit_fn);
    bst_visit_postorder(node->right, visit_fn);
    visit_fn(node->data);
}

/* ===========================================
 *                   Public
 * =========================================== */

/* ===========
 * Life cycle
 * ========== */

// for stack object
void yk_bst_init(struct yk_bst *bst, yk_compare_fn *compare_fn){
    assert(bst!=NULL);
    assert(compare_fn!=NULL);
    
    bst->root = NULL;
    bst->size = 0;
    bst->compare = compare_fn;
}

void yk_bst_clear(struct yk_bst *bst, yk_destroy_fn *destroy_fn){
    // destroy_fn could be NULL
    assert(bst!=NULL);
    
    bst_clear_subtree(bst->root, destroy_fn);
    // maintain invariant
    bst->root = NULL;
    bst->size = 0;
}

// for heap object
struct yk_bst *yk_bst_create(yk_compare_fn *compare_fn){
    assert(compare_fn!=NULL);
    
    struct yk_bst *bst = malloc(sizeof(*bst));
    if (bst==NULL){
        return NULL;
    }
    
    yk_bst_init(bst, compare_fn);
    return bst;
}

void yk_bst_destroy(struct yk_bst *bst, yk_destroy_fn *destroy_fn){
    assert(bst!=NULL);
    
    yk_bst_clear(bst, destroy_fn);
    free(bst);
}

/* ===========
* Capacity
* ========== */


size_t yk_bst_size(const struct yk_bst *bst){
    assert(bst!=NULL);
    
    return bst->size;
}


bool yk_bst_is_empty(const struct yk_bst *bst){
    assert (bst!=NULL);
    
    return bst->size==0;
}

/* ===========
* Lookup
* ========== */


void *yk_bst_find(const struct yk_bst *bst, const void *key){
    // compare_fn 已經在 BST，不需要提供
    // 搜尋時，key 只負責提供 compare_fn 需要的欄位即可
    // 找到之後，find() 回傳的是樹裡真正儲存的 node->data
    // since data cannot be NULL, so as key
    assert(bst!=NULL);
    assert(bst->compare!=NULL);
    assert(key!=NULL);
    
    struct yk_bst_node *node = bst->root;
    while(node){
        int cmp = bst->compare(key, node->data);
        if (cmp==0){
            // found
            return node->data;
        }
        // if key is smaller, then go left
        // if key is larger, then go right
        node = cmp<0 ? node->left : node->right;
    }
    // no found result
    return NULL;
}

bool yk_bst_contains(const struct yk_bst *bst, const void *key){
    assert(bst!=NULL);
    assert(key!=NULL);
    
    return yk_bst_find(bst, key)!=NULL;
}


/* ===========
* Modification
* ========== */

bool yk_bst_insert(struct yk_bst *bst, void *data){
    // 相等值固定放左邊
    //
    assert(bst!=NULL);
    assert(bst->compare!=NULL);
    assert(data!=NULL);  // data cannot be NULL
    
    struct yk_bst_node *new_node = bst_create_node(data);
    if (new_node==NULL){
        return false;
    }
    
    // if empty tree
    if (bst->root==NULL){
        bst->root = new_node;
        bst->size++;
        return true;
    }
    
    struct yk_bst_node *current = bst->root;
    for(;;){
        int cmp = bst->compare(data, current->data);
        // new <= current  負數往左、正數往右
        if (cmp<=0){
            // place new node to the left side
            if (current->left!=NULL){
                // if occupied, go deeper
                current = current->left;
                continue;
            }
            // if not occupied
            current->left = new_node;
            new_node->parent = current;
            bst->size++;
            return true;
        } else {
            // place new node to the right side
            if (current->right!=NULL){
                // if occupied, go deeper
                current = current->right;
                continue;
            }
            // if not occupied
            current->right = new_node;
            new_node->parent = current;
            bst->size++;
            return true;
        }
    }
    // won't reach here normally
}


static void *yk_bst_remove_original(struct yk_bst *bst, const void *key){
    // Remove/free the matching node and return its data.
    // The caller becomes responsible for the returned data.
    // return NULL if not found
    assert(bst!=NULL);
    assert(bst->compare!=NULL);
    assert(key!=NULL);  // data/key cannot be NULL
    
    // 每次修改樹的連線（detach/attach 時），都必須注意和維護相關 invariant
    // - 維護 parent <-> node 的關係
    // - 維護 node <-> child 的關係
    // - 處理 BST root
    // - 處理 BST size
    // - 不形成 cycle (指向自己，descendant 指回 ancestor)
    
    struct yk_bst_node *victim = bst->root;
    // find node
    for(;;){
        if (victim==NULL){  // not found or empty tree
            return NULL;
        }
        int cmp = bst->compare(key, victim->data);
        if (cmp>0){  // key is larger, go right
            victim = victim->right;
            continue;
        }
        if (cmp<0){   // key is smaller, go left
            victim = victim->left;
            continue;
        }
        // matched
        // condition 1: victim is leaf node -> 直接 remove victim 即可
        if (victim->left==NULL && victim->right==NULL){
            // 維護 parent <-> node
            // parent
            if (victim==bst->root){  // victim is root
                bst->root = NULL;
            } else if (victim->parent->left==victim){  // victim is at parent's left
                victim->parent->left = NULL;
            } else {  // victim at parent's right
                assert(victim->parent->right==victim);  // in case invariant is broken
                victim->parent->right = NULL;
            }
            // node
            victim->parent = NULL;
            // 維護 node <-> children
            // no need，bc no children
            
            void *data = victim->data;
            free(victim);
            bst->size--;
            return data;
        } else if ((victim->left==NULL && victim->right!=NULL) ||
                  (victim->right==NULL && victim->left!=NULL)){
        // condition 2: victim has only one child (another is NULL)
        // replace victim with its child; rmb to update child's parent
        
            // get the only child
            struct yk_bst_node *child = victim->left ? victim->left : victim->right;
            // parent <-> node and node <-> child
            if (victim==bst->root){  // victim is root, let the child be the new root
                bst->root = child;  // cannot use victim->parent->... bc victim->parent is NULL
                child->parent = NULL;
            } else if (victim->parent->left==victim){  // victim is at parent's left
                victim->parent->left = child; // replace with the only child
                child->parent = victim->parent;
            } else{
                assert(victim->parent->right==victim);  // check invariant
                victim->parent->right = child;
                child->parent = victim->parent;
            }
            victim->parent = NULL;
            victim->right = NULL;
            victim->left = NULL;
            void *data = victim->data;
            free(victim);
            bst->size--;
            return data;
        }
        // condition 3: victim has two children
        // either get the largest node on the left side (predecessor) or
        //   get the smallest node on the right side (successor)
        //   to replace the victim。（predecessor/successor 移除，然後到移動到原本 victim 的位置）
        //   predecessor or successor must have <=1 child
        //   bc 他們三個在 inorder travasal 中，一定是緊密排列: value(... predecessor <= victim < successor ...)
        //      如果 "predecessor" 有 right child，那他就不是該 subtree 中最大的那個了
        //      如果 "successor" 有 left child，那他就不是該 subtree 中最小的那個了
        struct yk_bst_node *predecessor = victim->left;
        for(;;){
            if (predecessor->right!=NULL){
                // until predecessor is the rightmost node in the left subtree
                predecessor = predecessor->right;
                continue;
            } else{
                break;
            }
        }
        // 首先判斷 predecessor 是否就是 victim 的 immediate left child
        // 如果是，需要動到的地方是
        // - 將 victim 和他的 parent, children 脫離
        // - 將 predecessor 接上 parent 和 victim 本來的 right child (predecessor 是 left child)
        if (predecessor->parent==victim){
            // 處理 parent <-> predecessor
            if (victim==bst->root){  // 若 victim 是 root
                bst->root = predecessor;
                predecessor->parent = NULL;
            } else{
                if (victim->parent->left==victim){
                    victim->parent->left = predecessor;
                } else{
                    assert(victim->parent->right==victim);
                    victim->parent->right = predecessor;
                }
                predecessor->parent = victim->parent;
            }
            // 處理 predecessor <-> children
            // predecessor left children 不改變
            // predecessor right children 會改變，接上 vicitm->right
            // 因為是 2 children，right children 一定存在，不必檢查
            predecessor->right = victim->right;
            victim->right->parent = predecessor;
        } else{
            // 如果 predecessor 不是 immediate left child of vicitim
            // 要將 predecessor 從原本的地方 detach，然後取代原本的 victim
            
            // 移除 predecessor 的情況如同 condition 1 and 2
            //  - 要把 predecessor 用 NULL (if leaf node)，或是 the only child of predessor 取代
            //  - 因此可以用 "transplant" 的概念去抽象化這個將 old_subtree 用 new_subtree 取代
            
            // note: victim has 2 child, predecessor has no right child
            // predecessor is not immediate children of victim
            
            // condition 1: predecessor has no child
            if (predecessor->left==NULL && predecessor->right==NULL){
                // predecessor <-> old parent detach
                predecessor->parent->right = NULL;  // predecessor 一定是 old parent->right
                predecessor->parent = NULL;  // may ignore, for symmetry
                // predecessor <-> old children
                // no need change because no children
                
                // predecessor <-> new parent
                if (victim==bst->root){  // if victim is root
                    bst->root = predecessor;
                } else if(victim->parent->left==victim){
                    victim->parent->left = predecessor;
                } else{
                    assert(victim->parent->right==victim);
                    victim->parent->right = predecessor;
                }
                predecessor->parent = victim->parent;
                // predecessor <-> new child
                predecessor->left = victim->left;
                predecessor->right = victim->right;
                victim->left->parent = predecessor;
                victim->right->parent = predecessor;
            } else {
                
                // condition 2: predecessor has 1 child (只可能是 left child)
                // old parent <-> predecessor <-> old child
                // - 移除 predecessor，將 old parent/child 互相接上
                predecessor->parent->right = predecessor->left;
                predecessor->left->parent = predecessor->parent;
                
                // new parent <-> predecessor <-> new child
                // - 用 predecessor 取代 victim
                if (victim==bst->root){  // if victim is root
                    bst->root = predecessor;
                } else if (victim->parent->left==victim){
                    victim->parent->left = predecessor;
                } else{
                    assert(victim->parent->right==victim);
                    victim->parent->right = predecessor;
                }
                predecessor->parent = victim->parent;
                
                // predecessor 兩邊 child 接上 victim 原本的 children
                predecessor->left = victim->left;
                predecessor->right = victim->right;
                victim->left->parent = predecessor;
                victim->right->parent = predecessor;
            }
        }
        
        victim->parent = NULL;
        victim->left = NULL;
        victim->right = NULL;
        void *data = victim->data;
        free(victim);
        bst->size--;
        return data;
    }
}



static void bst_transplant(struct yk_bst *bst,
                            struct yk_bst_node *old_node,
                            struct yk_bst_node *new_node){
    // Replace subtree rooted at old_node with
    //         subtree rooted at new_node
    // 將 old_node 原本所在的位置改由以 new_node 為根的 subtree 取代
    
    // 建立新的 parent <-> child 關係：
    //      建立 old_node 的 parent 與 new_node 之間的相互連結
    //      - old_node 的 parent 中的 left or right 指向 new_node
    //      - new_node 中的 parent 指向 old_node 的 parent
    // 舊的依然藕斷絲連：
    //      未修改 old_node 本身的 members（parent、left、right、data）
    //      未修改 new_node 原本的 parent 中指向 new_node 的 child pointer
    //              （呼叫者需視情況先將 new_node 從原位置 detach）
    //      未修改 new_node 原本的 children 中指向 new_node 的 parent pointer
    //              （若呼叫者重新指定 new_node 的 children，需同步更新 child->parent）
    

    assert(bst!=NULL);
    assert(old_node!=NULL);
    // new node could be NULL
    
    // update old_node->parent-> side
    if (old_node==bst->root){
        bst->root = new_node;
    } else{
        if (old_node->parent->left==old_node){
            old_node->parent->left = new_node;
        } else{
            assert(old_node->parent->right==old_node);
            old_node->parent->right = new_node;
        }
    }
    // update parent side of new_node
    if (new_node!=NULL){
        new_node->parent = old_node->parent;  // 若 old_node 為 root，也會寫入 NULL
    }
}

void *yk_bst_remove(struct yk_bst *bst, const void *key){
    // with helper function bst_transplant
    // Remove/free the matching node and return its data.
    // The caller becomes responsible for the returned data.
    // return NULL if not found
    assert(bst!=NULL);
    assert(bst->compare!=NULL);
    assert(key!=NULL);  // data/key cannot be NULL
    
    struct yk_bst_node *victim = bst->root;
    // find node match with key
    for(;;){
        if (victim==NULL){  // not found or empty tree
            return NULL;
        }
        int cmp = bst->compare(key, victim->data);
        if (cmp>0){  // key is larger, go right
            victim = victim->right;
            continue;
        }
        if (cmp<0){   // key is smaller, go left
            victim = victim->left;
            continue;
        }
        // matched
        // condition 1, 2: victim has 0 or 1 child
        if (victim->left==NULL){  // 這包含了 victim has 0 or only right child
            bst_transplant(bst, victim, victim->right);
        } else if (victim->right==NULL){  // 這包含了 victim has only left child
            bst_transplant(bst, victim, victim->left);
        } else{
            // condition 3: victim has 2 children
            // 經過剛剛篩選，剩下 victim has 2 children
            
            // 找尋 predecessor or successor 用來替代 victim
            struct yk_bst_node *predecessor = victim->left;
            for(;;){
                if (predecessor->right!=NULL){
                    // until predecessor is the rightmost node in the left subtree
                    predecessor = predecessor->right;
                } else{
                    break;
                }
            }
            
            // Predecessor 是 victim 的 immediate left child
            // - 用 predecessor 取代 victim 後，要將 victim->right 接上 predecessor->right
            if (predecessor->parent==victim){
                // 用 predecessor 將 victim 取代
                bst_transplant(bst, victim, predecessor);
                // 將 victim 原先剩下的 right child 接上 predecessor
                predecessor->right = victim->right;
                victim->right->parent = predecessor;
                // 左邊就不用動了，維持原本 subtree，否則會變成自我循環
                // predecessor->left = victim->left;
                // victim->left->parent = predecessor;
            } else{
            // Predecessor 不是 immediate left child of vicitim
                // 要先將 predecessor 從原本的地方 detach，然後取代原本的 victim
                // 也就是用 predecessor->left 取代 predecessor 本來的位置
                bst_transplant(bst, predecessor, predecessor->left);
                // 接著用 predecessor 將 victim 取代
                bst_transplant(bst, victim, predecessor);
                // predecessor 兩邊 child 接上 victim 原本的 children
                predecessor->left = victim->left;
                predecessor->right = victim->right;
                victim->left->parent = predecessor;
                victim->right->parent = predecessor;
            }

        }
        void *data = victim->data;
        free(victim);
        bst->size--;
        return data;
    }
}


/* ===========
* Traversal
* ========== */

void yk_bst_visit_inorder(const struct yk_bst *bst, yk_visit_fn *visit_fn){
    assert(bst!=NULL);
    assert(visit_fn!=NULL);
    
    // call helper function
    bst_visit_inorder(bst->root, visit_fn);
}


void yk_bst_visit_preorder(const struct yk_bst *bst, yk_visit_fn *visit_fn){
    assert(bst!=NULL);
    assert(visit_fn!=NULL);
    
    bst_visit_preorder(bst->root, visit_fn);
}


void yk_bst_visit_postorder(const struct yk_bst *bst, yk_visit_fn *visit_fn){
    assert(bst!=NULL);
    assert(visit_fn!=NULL);
    
    bst_visit_postorder(bst->root, visit_fn);
}









