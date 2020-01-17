#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../common/aff.h"
#include "../common/timer.h"
#include "ll.h"

#define MAX_THREADS 64
#define RUNTIME 10

#define print_error_and_exit(format...) \
    do {                                \
        fprintf(stderr, format);        \
        exit(EXIT_FAILURE);             \
    } while (0)

/**
 * Global data.
**/
ll_t *ll;
unsigned int list_size;
pthread_barrier_t start_barrier;
int time_to_leave;
short contains_pct, add_pct, remove_pct;

/**
 * The struct that is passed as an argument to each thread.
**/
typedef struct {
    int tid,
        cpu;
    unsigned long long net_adds,
        sum_of_keys;
    char padding[64 - 2 * sizeof(int) - 2 * sizeof(unsigned long long)];
} tdata_t;

void *thread_fn(void *targ);

int main(int argc, char **argv) {
    pthread_t threads[MAX_THREADS];
    tdata_t threads_data[MAX_THREADS];
    unsigned int nthreads = 0, *cpus;
    unsigned int i;

    //> Initializations.
    if (argc != 5)
        print_error_and_exit("usage: %s <list_size> <contains_pct> <add_pct> "
                             "<remove_pct>\n",
                             argv[0]);
    list_size = atoi(argv[1]);
    contains_pct = atoi(argv[2]);
    add_pct = atoi(argv[3]);
    remove_pct = atoi(argv[4]);
    if (contains_pct + add_pct + remove_pct != 100)
        print_error_and_exit("The total percentage of operations is not 100%%!\n");

    get_mtconf_options(&nthreads, &cpus);
    mt_conf_print(nthreads, cpus);

    if (pthread_barrier_init(&start_barrier, NULL, nthreads + 1))
        print_error_and_exit("Failed to initialize start_barrier.\n");

    //> Initialize the linked list.
    ll = ll_new();
    for (i = list_size / 2; i > 0; i--)
        ll_add(ll, i);

    //> Spawn threads.
    for (i = 0; i < nthreads; i++) {
        threads_data[i].tid = i;
        threads_data[i].cpu = cpus[i];
        threads_data[i].net_adds = 0;
        threads_data[i].sum_of_keys = 0;
        if (pthread_create(&threads[i], NULL, thread_fn, &threads_data[i]))
            print_error_and_exit("Error creating thread %d.\n", i);
    }

    //> Signal threads to start computation.
    pthread_barrier_wait(&start_barrier);

    sleep(RUNTIME);
    time_to_leave = 1;

    //> Wait for threads to complete their execution.
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], NULL))
            print_error_and_exit("Failure on pthread_join for thread %d.\n", i);
    }

    //> How many operations have been performed by all threads?
    unsigned long long total_net_adds = 0, total_sum_of_keys = 0;
    for (i = 0; i < nthreads; i++) {
        total_net_adds += threads_data[i].net_adds;
        total_sum_of_keys += threads_data[i].sum_of_keys;
    }

    //> Print results.
    if (ll_is_sorted(ll))
        printf("Is sorted: CORRECT\n");
    else
        printf("Is sorted: ERROR\n");

    unsigned long long expected = list_size / 2 + total_net_adds;
    unsigned long long result = ll_length(ll);
    if (result == expected)
        printf("Length: CORRECT\n");
    else
        printf("Length: ERROR (expected = %llu, result = %llu)\n", expected, result);

    expected = (list_size / 2) * (list_size / 2 + 1) / 2 + total_sum_of_keys;
    result = ll_sum_of_keys(ll);
    if (result == expected)
        printf("Sum of keys: CORRECT\n");
    else
        printf("Sum of keys: ERROR (expected = %llu, result = %llu)\n", expected, result);

    // ll_print(ll);
    ll_free(ll);
    return EXIT_SUCCESS;
}

/**
 * This is the function that is executed by each spawned thread.
 **/
void *thread_fn(void *targ) {
    tdata_t *mydata = targ;
    int i;
    struct drand48_data drand_buffer;
    long int drand_res;

    //> Initialize the thread-safe (and scalable) random number generator.
    srand48_r(rand() * mydata->tid, &drand_buffer);

    //> Pin thread to the specified cpu.
    setaffinity_oncpu(mydata->cpu);

    //> Wait until master gives the green light!
    pthread_barrier_wait(&start_barrier);

    while (!time_to_leave) {
        //> Get a random number.
        lrand48_r(&drand_buffer, &drand_res);
        int key = drand_res % (list_size + 1);

        //> Get a random operation.
        lrand48_r(&drand_buffer, &drand_res);
        int op = drand_res % 100;

        if (op < contains_pct)
            ll_contains(ll, key);
        else if (op < contains_pct + add_pct) {
            if (ll_add(ll, key)) {
                mydata->net_adds++;
                mydata->sum_of_keys += key;
            }
        } else if (ll_remove(ll, key)) {
            mydata->net_adds--;
            mydata->sum_of_keys -= key;
        }

        for (i = 0; i < 200; i++)
            /* do nothing */;
    }

    return NULL;
}
