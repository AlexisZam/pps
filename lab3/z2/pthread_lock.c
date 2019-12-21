#include "../common/alloc.h"
#include "lock.h"

struct lock_struct {
    /* Delete this in your implementation, just a placeholder. */
    int dummy;
};

lock_t *lock_init(int nthreads) {
    lock_t *lock;

    XMALLOC(lock, 1);
    /* other initializations here. */
    return lock;
}

void lock_free(lock_t *lock) {
    XFREE(lock);
}

void lock_acquire(lock_t *lock) {
}

void lock_release(lock_t *lock) {
}
