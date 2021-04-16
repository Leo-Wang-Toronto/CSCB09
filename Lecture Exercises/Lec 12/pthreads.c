#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 12
#define ARRAY_SIZE 100000000

int *array_ints;
long *sum;

typedef struct _thread_data_t {
    int tid;
    double stuff;
} thread_data_t;

void *thr_func(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    printf("inside thr_func: %d\n", data->tid);
    sum[data->tid] = 0;
    int thread_part = ARRAY_SIZE/NUM_THREADS;
    for (int i = data->tid * thread_part, j = 0; j < thread_part; i++, j++)
    {
        sum[data->tid] += array_ints[i];
    }
    printf("Sum calcualted by thread %d is %ld\n", data->tid, sum[data->tid]);
    pthread_exit(NULL);

}

// compile using gcc -pthread pthreads.c

int main(int argc, char**argv) {
    pthread_t thr[NUM_THREADS];
    int i, rc;

    // initalize thread data
    thread_data_t thr_data[NUM_THREADS];
    array_ints = malloc(ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array_ints[i] = rand() % 100;
    }
    sum = malloc(NUM_THREADS * sizeof(long));

    long total_sum = 0;

    // create threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thr_data[i].tid = i;
        if (rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i])) {
            fprintf(stderr, "error: pthread_Create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    // wait for threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(thr[i], NULL);
    }

    // sum thread outputs
    for (int i = 0; i < NUM_THREADS; i++)
    {
        total_sum += sum[i];
    }
    
    printf("Total sum is: %ld\n", total_sum);
    
    return EXIT_SUCCESS;
}