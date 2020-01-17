#include "../common/alloc.h"
#include "lock.h"

#define FALSE 0
#define TRUE 1

typedef struct {
    char locked; /* FALSE or TRUE. */
    char padding[63];
} array_node_t;

struct lock_struct {
    int tail;
    array_node_t *flag;
    int size;
};

/**
 * These are GCC's magic. Per thread variables.
 * They are initialized to 0 ( = NULL).
 **/
__thread int mySlotIndex;

lock_t *lock_init(int nthreads) {
    lock_t *lock;
    array_node_t *flag;

    XMALLOC(lock, 1);
    XMALLOC(flag, nthreads);

    lock->size = nthreads;
    lock->tail = 0;
    lock->flag = flag;
    lock->flag[0].locked = TRUE;

    return lock;
}

void lock_free(lock_t *lock) {
    lock_t *l = lock;
    XFREE(l);
}

void lock_acquire(lock_t *lock) {
    lock_t *l = lock;

    mySlotIndex = __sync_fetch_and_add(&l->tail, 1) % l->size;

    while (l->flag[mySlotIndex].locked == FALSE)
        /* do nothing */;
}

void lock_release(lock_t *lock) {
    lock_t *l = lock;

    l->flag[mySlotIndex].locked = FALSE;
    l->flag[(mySlotIndex + 1) % l->size].locked = TRUE;
}
