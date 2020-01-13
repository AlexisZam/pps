#include <pthread.h>

#include "../common/alloc.h"
#include "lock.h"

struct lock_struct {
    pthread_spinlock_t lock;
};

lock_t *lock_init(int nthreads) {
    lock_t *lock;

    XMALLOC(lock, 1);
    pthread_spin_init(&lock->lock, PTHREAD_PROCESS_PRIVATE);
    return lock;
}

void lock_free(lock_t *lock) {
    pthread_spin_destroy(&lock->lock);
    XFREE(lock);
}

void lock_acquire(lock_t *lock) {
    pthread_spin_lock(&lock->lock);
}

void lock_release(lock_t *lock) {
    pthread_spin_unlock(&lock->lock);
}
