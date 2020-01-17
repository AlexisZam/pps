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

typedef struct {
    ll_node_t *prev;
    ll_node_t *curr;
} window_t;

static inline ll_node_t *get_next(ll_node_t *curr) {
    ll_node_t node = *curr;
    node.marked &= ~1;
    return node.next;
}

static inline int get_marked(ll_node_t *curr) {
    return curr->marked & 1;
}

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key) {
    ll_node_t *ret;

    XMALLOC(ret, 1);
    ret->key = key;
    ret->next = NULL;
    ret->marked &= ~1;

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

int ll_contains(ll_t *ll, int key) {
    ll_node_t *curr = ll->head;
    int ret = 0;

    while (curr->key < key)
        curr = get_next(curr);

    ret = (key == curr->key && !get_marked(curr));
    return ret;
}

int ll_add(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *curr, *next;
    ll_node_t *new_node;

    curr = ll->head;
    next = curr->next;

    while (next->key < key) {
        curr = next;
        next = curr->next;
    }

    if (key != next->key) {
        ret = 1;
        new_node = ll_node_new(key);
        new_node->next = next;
        curr->next = new_node;
    }

    return ret;
}

int ll_remove(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *curr, *next;

    curr = ll->head;
    next = curr->next;

    while (next->key < key) {
        curr = next;
        next = curr->next;
    }

    if (key == next->key) {
        ret = 1;
        curr->next = next->next;
        ll_node_free(next);
    }

    return ret;
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

    while (curr->key != INT_MAX) {
        if (curr->key >= next->key)
            return 0;
        curr = next;
        next = get_next(curr);
    }

    return 1;
}

unsigned long long ll_length(ll_t *ll) {
    ll_node_t *curr = ll->head;
    int ret = 0;

    curr = get_next(curr);

    while (curr->key != INT_MAX) {
        ret++;
        curr = get_next(curr);
    }

    return ret;
}

unsigned long long ll_sum_of_keys(ll_t *ll) {
    ll_node_t *curr = ll->head;
    int ret = 0;

    curr = get_next(curr);

    while (curr->key != INT_MAX) {
        ret += curr->key;
        curr = get_next(curr);
    }

    return ret;
}
