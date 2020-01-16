#include <limits.h>
#include <stdint.h>
#include <stdio.h>

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

static inline int compare_and_set(ll_node_t *node, ll_node_t *expected_next, ll_node_t *new_next, uintptr_t expected_marked, uintptr_t new_marked) {
    uintptr_t expected, new;

    expected = expected_marked ? (uintptr_t)expected_next | 1 : (uintptr_t)expected_next & ~1;
    new = new_marked ? (uintptr_t)new_next | 1 : (uintptr_t)new_next & ~1;
    return __sync_bool_compare_and_swap(&node->marked, expected, new);
}

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key) {
    ll_node_t *ret;

    XMALLOC(ret, 1);
    ret->key = key;
    ret->next = NULL;

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
        ll_node_t temp;
        int marked;

        prev = head;
        curr = get_next(prev);
        for (;;) {
            temp = *curr;
            next = get_next(&temp);
            marked = get_marked(&temp);
            while (marked) {
                if (!compare_and_set(prev, curr, next, 0, 0))
                    goto retry;
                curr = next;
                temp = *curr;
                next = get_next(&temp);
                marked = get_marked(&temp);
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

    while (curr->key < key)
        curr = get_next(curr);

    return key == curr->key && !get_marked(curr);
}

int ll_add(ll_t *ll, int key) {
    ll_node_t *prev, *curr, *node;
    window_t window;

    for (;;) {
        window = find(ll->head, key);
        prev = window.prev;
        curr = window.curr;

        if (curr->key == key)
            return 0;
        node = ll_node_new(key);
        node->next = curr;
        node->marked &= ~1;
        if (compare_and_set(prev, curr, node, 0, 0))
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
        if (compare_and_set(curr, next, next, 0, 1)) {
            compare_and_set(prev, curr, next, 0, 0);
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
        if (!get_marked(curr)) {
            if (curr->key == INT_MAX)
                printf(" -> MAX");
            else
                printf(" -> %d", curr->key);
            curr = get_next(curr);
        }
    }
    printf(" ]\n");
}

int ll_is_sorted(ll_t *ll) {
    ll_node_t *curr, *next;

    curr = ll->head;
    next = get_next(curr);

    while (next->key != INT_MAX) {
        if (!get_marked(curr))
            if (curr->key >= next->key)
                return 0;
        curr = next;
        next = get_next(curr);
    }

    return 1;
}

unsigned long long ll_length(ll_t *ll) {
    int length = 0;

    for (ll_node_t *curr = ll->head; curr; curr = get_next(curr))
        if (!get_marked(curr))
            length++;

    return length - 2;
}

unsigned long long ll_key_sum(ll_t *ll) {
    unsigned long long key_sum = 0;
    ll_node_t *curr = ll->head;

    while (curr->key != INT_MAX) {
        if (!get_marked(curr))
            key_sum += curr->key;
        curr = get_next(curr);
    }

    return key_sum + 1;
}