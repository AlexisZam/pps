#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
    int key;
    union {
        struct ll_node *next;
        uintptr_t marked;
    };
} ll_node_t;

struct linked_list {
    ll_node_t *head;
};

typedef struct window {
    ll_node_t *prev;
    ll_node_t *curr;
} window_t;

static inline ll_node_t *get_next(ll_node_t *node) {
    ll_node_t temp = *node;
    temp.marked &= ~1;
    return temp.next;
}

static inline int get_marked(ll_node_t *node) {
    return node->marked & 1;
}

static inline int compare_and_set(ll_node_t *node, ll_node_t *expected_reference, ll_node_t *new_reference, int expected_mark, int new_mark) {
    ll_node_t expected_node, new_node;

    expected_node.next = expected_reference;
    expected_node.marked &= expected_mark;
    new_node.next = new_reference;
    new_node.marked &= new_mark;

    return __sync_bool_compare_and_swap(&node->marked, expected_node.marked, new_node.marked);
}

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key) {
    ll_node_t *ret;

    XMALLOC(ret, 1);
    ret->key = key;
    ret->next = NULL;
    // ret->marked &= ~1;

    return ret;
}

/**
 * Free a linked list node.
 **/
static void ll_node_free(ll_node_t *ll_node) {
    XFREE(ll_node);
}

/**
 * Create a new empty linked list.
 **/
ll_t *ll_new() {
    ll_t *ret;

    XMALLOC(ret, 1);
    ret->head = ll_node_new(-1);
    ret->head->next = ll_node_new(INT_MAX);
    ret->head->next->next = NULL;

    return ret;
}

/**
 * Free a linked list and all its contained nodes.
 **/
void ll_free(ll_t *ll) {
    ll_node_t *next, *curr = ll->head;
    while (curr) {
        next = curr->next;
        ll_node_free(curr);
        curr = next;
    }
    XFREE(ll);
}

window_t find(ll_node_t *head, int key) {
retry:
    for (;;) {
        ll_node_t *prev, *curr, *next;
        int marked;

        prev = head;
        curr = get_next(prev);
        for (;;) {
            next = get_next(curr);
            marked = get_marked(curr);
            while (marked) {
                if (!compare_and_set(prev->next, curr, next, ~1, ~1))
                    goto retry;
                curr = next;
                next = get_next(curr);
                marked = get_marked(curr);
            }
            if (curr->key >= key)
                return (window_t){prev, curr};
            prev = curr;
            curr = next;
        }
    }
}

int ll_contains(ll_t *ll, int key) {
    ll_node_t *curr = ll->head;
    int marked;

    while (curr->key < key) {
        curr = curr->next;
        marked = get_marked(curr);
    }
    return key == curr->key && !marked;
}

int ll_add(ll_t *ll, int key) {
    ll_node_t *prev, *curr, *node;
    window_t window;

    for (;;) {
        window = find(ll->head, key);
        prev = window.prev;
        curr = window.curr;

        if (curr->key != key)
            return 0;
        node = ll_node_new(key);
        node->next = curr;
        node->marked &= 0;
        if (compare_and_set(prev->next, curr, node, ~1, ~1))
            return 1;
    }
}

int ll_remove(ll_t *ll, int key) {
    ll_node_t *prev, *curr, *next;
    window_t window;

    for (;;) {
        window = find(ll->head, key);
        prev = window.prev;
        curr = window.curr;

        if (curr->key != key)
            return 0;
        next = get_next(curr);
        if (compare_and_set(curr->next, next, next, get_marked(curr->next), 1)) {
            compare_and_set(prev->next, curr, next, ~1, ~1);
            return 1;
        }
    }
}

/**
 * Print a linked list.
 **/
void ll_print(ll_t *ll) {
    ll_node_t *curr = ll->head;
    printf("LIST [");
    while (curr) {
        if (curr->key == INT_MAX)
            printf(" -> MAX");
        else
            printf(" -> %d", curr->key);
        curr = curr->next;
    }
    printf(" ]\n");
}
