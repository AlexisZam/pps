#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* rand() */

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
    int key;
    struct ll_node *next;
} ll_node_t;

struct linked_list {
    ll_node_t *head;
};

typedef struct window {
    ll_node_t *prev;
    ll_node_t *curr;
} window_t;

static inline ll_node_t *get_next(ll_node_t *node, int *marked) {
    ll_node_t *temp = node->next;
    *marked = (uintptr_t)temp & 1;
    return (ll_node_t *)((uintptr_t)temp >> 1);
}

static inline ll_node_t *get_next_next(ll_node_t *node) {
    return (ll_node_t *)((uintptr_t)node->next >> 1);
}

static inline void set_next(ll_node_t *node, ll_node_t *next) {
    node->next = (ll_node_t *)((uintptr_t)next << 1);
}

static inline int get_next_marked(ll_node_t *node) {
    return (uintptr_t)node->next & 1;
}

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key) {
    ll_node_t *ret;

    XMALLOC(ret, 1);
    ret->key = key;
    ret->next = NULL;
    set_next(ret, ret->next);

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
        curr = get_next_next(prev);
        for (;;) {
            printf("%p\n", (uintptr_t)curr >> 1);
            printf("%p\n", curr);
            next = get_next(curr, &marked);
            while (marked) {
                if (!__sync_bool_compare_and_swap(&prev->next, (uintptr_t)curr << 1, (uintptr_t)next << 1))
                    goto retry;
                curr = next;
                next = get_next(curr, &marked);
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
        marked = get_next_marked(curr);
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
        set_next(node, curr);
        if (__sync_bool_compare_and_swap(&prev->next, (uintptr_t)curr << 1, node))
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
        next = get_next_next(curr);
        if (__sync_bool_compare_and_swap(&curr->next, ((uintptr_t)next << 1) | get_next_marked(curr), ((uintptr_t)curr->next << 1) | 1)) {
            __sync_bool_compare_and_swap(&prev->next, (uintptr_t)curr << 1, (uintptr_t)next << 1);
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
