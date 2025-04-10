#include <limits.h>
#include <pthread.h> /* for pthread_spinlock_t */
#include <stdio.h>

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
    int key;
    struct ll_node *next;
    pthread_spinlock_t lock;
} ll_node_t;

struct linked_list {
    ll_node_t *head;
};

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key) {
    ll_node_t *ret;

    XMALLOC(ret, 1);
    ret->key = key;
    ret->next = NULL;
    pthread_spin_init(&ret->lock, PTHREAD_PROCESS_PRIVATE);

    return ret;
}

/**
 * Free a linked list node.
 **/
static void ll_node_free(ll_node_t *ll_node) {
    pthread_spin_destroy(&ll_node->lock);
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
    int ret = 0;
    ll_node_t *curr, *next;

    pthread_spin_lock(&ll->head->lock);
    curr = ll->head;
    next = curr->next;
    pthread_spin_lock(&next->lock);

    while (next->key < key) {
        pthread_spin_unlock(&curr->lock);
        curr = next;
        next = curr->next;
        pthread_spin_lock(&next->lock);
    }

    if (key == next->key)
        ret = 1;
    pthread_spin_unlock(&next->lock);
    pthread_spin_unlock(&curr->lock);

    return ret;
}

int ll_add(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *curr, *next;
    ll_node_t *new_node;

    pthread_spin_lock(&ll->head->lock);
    curr = ll->head;
    next = curr->next;
    pthread_spin_lock(&next->lock);

    while (next->key < key) {
        pthread_spin_unlock(&curr->lock);
        curr = next;
        next = curr->next;
        pthread_spin_lock(&next->lock);
    }

    if (key != next->key) {
        ret = 1;
        new_node = ll_node_new(key);
        new_node->next = next;
        curr->next = new_node;
    }
    pthread_spin_unlock(&next->lock);
    pthread_spin_unlock(&curr->lock);

    return ret;
}

int ll_remove(ll_t *ll, int key) {
    int ret = 0;
    ll_node_t *curr, *next;

    pthread_spin_lock(&ll->head->lock);
    curr = ll->head;
    next = curr->next;
    pthread_spin_lock(&next->lock);

    while (next->key < key) {
        pthread_spin_unlock(&curr->lock);
        curr = next;
        next = curr->next;
        pthread_spin_lock(&next->lock);
    }

    if (key == next->key) {
        ret = 1;
        curr->next = next->next;
        ll_node_free(next);
    }
    pthread_spin_unlock(&next->lock);
    pthread_spin_unlock(&curr->lock);

    return ret;
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

int ll_is_sorted(ll_t *ll) {
    ll_node_t *curr, *next;

    curr = ll->head;
    next = curr->next;

    while (curr->key != INT_MAX) {
        if (curr->key >= next->key)
            return 0;
        curr = next;
        next = curr->next;
    }

    return 1;
}

unsigned long long ll_length(ll_t *ll) {
    ll_node_t *curr = ll->head;
    unsigned long long ret = 0;

    curr = curr->next;

    while (curr->key != INT_MAX) {
        ret++;
        curr = curr->next;
    }

    return ret;
}

unsigned long long ll_sum_of_keys(ll_t *ll) {
    ll_node_t *curr = ll->head;
    unsigned long long ret = 0;

    curr = curr->next;

    while (curr->key != INT_MAX) {
        ret += curr->key;
        curr = curr->next;
    }

    return ret;
}
