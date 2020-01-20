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

static inline int compare_and_swap(ll_node_t *curr, ll_node_t *oldval_next, ll_node_t *newval_next, uintptr_t oldval_marked, uintptr_t newval_marked) {
    ll_node_t oldval, newval;

    oldval.next = oldval_next;
    newval.next = newval_next;
    if (oldval_marked)
        oldval.marked |= 1;
    else
        oldval.marked &= ~1;
    if (newval_marked)
        newval.marked |= 1;
    else
        newval.marked &= ~1;

    return __sync_bool_compare_and_swap(&curr->marked, oldval.marked, newval.marked);
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
        next = get_next(curr);
        ll_node_free(curr);
        curr = next;
    }
    XFREE(ll);
}

window_t find(ll_t *ll, int key) {
    ll_node_t *prev, *curr, *next;
    ll_node_t node;
    int marked;

retry:
    for (;;) {
        prev = ll->head;
        curr = get_next(prev);
        for (;;) {
            node = *curr;
            next = get_next(&node);
            marked = get_marked(&node);
            while (marked) {
                if (!compare_and_swap(prev, curr, next, 0, 0))
                    goto retry;
                curr = next;
                node = *curr;
                next = get_next(&node);
                marked = get_marked(&node);
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
    int ret = 0;

    while (curr->key < key)
        curr = get_next(curr);

    ret = (key == curr->key && !get_marked(curr));
    return ret;
}

int ll_add(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *prev, *curr;
    ll_node_t *new_node;
    window_t window;

    for (;;) {
        window = find(ll, key);
        prev = window.prev;
        curr = window.curr;

        if (key != curr->key) {
            new_node = ll_node_new(key);
            new_node->next = curr;
            new_node->marked &= ~1;
            if (!compare_and_swap(prev, curr, new_node, 0, 0))
                continue;
            ret = 1;
        }

        return ret;
    }
}

int ll_remove(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *prev, *curr, *next;
    window_t window;

    for (;;) {
        window = find(ll, key);
        prev = window.prev;
        curr = window.curr;

        if (key == curr->key) {
            next = get_next(curr);
            if (!compare_and_swap(curr, next, next, 0, 1))
                continue;
            ret = 1;
            compare_and_swap(prev, curr, next, 0, 0);
        }

        return ret;
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
    unsigned long long ret = 0;

    curr = get_next(curr);

    while (curr->key != INT_MAX) {
        ret++;
        curr = get_next(curr);
    }

    return ret;
}

unsigned long long ll_sum_of_keys(ll_t *ll) {
    ll_node_t *curr = ll->head;
    unsigned long long ret = 0;

    curr = get_next(curr);

    while (curr->key != INT_MAX) {
        ret += curr->key;
        curr = get_next(curr);
    }

    return ret;
}
