#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sum_total = 0; // Accumulator for the sum
char *values; // Array to store the generated values

struct lock {
    int held;
};

struct thread_args {
    int start_range;
    struct lock lock;
    int end_range;
};

// Acquire the lock
void acquire(struct lock lock) {
    while (__sync_lock_test_and_set(&lock.held, 1))
        ;
}

// Release the lock
void release(struct lock *lock) {
    lock->held = 0;
}

// Generate random values between -100 and 100 and add them to the array
void generate_values(int N) {
    int i;
    for (i = 0; i < N; i++) {
        values[i] = (char)(rand() % 200 - 100);
    }
}

//Entry function for the threads
void *sum(void *arg) {
    struct thread_args *args = arg;
    int partial_sum = 0;

    // Perform the sum within the assigned range for this thread
    for (int i = args->start_range; i < args->end_range; i++) {
        partial_sum += (int)values[i];
    }

    // Acquire the lock to update the accumulator
    acquire(args->lock);
    sum_total += partial_sum;
    release(&args->lock);

    return NULL;
}

// Function to create threads and assign them a range to perform the sum
void create_threads(int N, int K) {
    pthread_t *th = (pthread_t *)malloc(sizeof(pthread_t) * K);
    struct lock lock;
    lock.held = 0;
    int i;
    struct thread_args *args = (struct thread_args *)malloc(sizeof(struct thread_args) * K);
    
    int chunk_size = N / K;  // Interval size for each thread
    int remainder = N % K;  // Remainder of division
    int start_range = 0;
    
    // Create threads and distribute work until the penultimate one
    for (i = 0; i < K - 1; i++) {
        args[i].start_range = start_range;
        args[i].lock = lock;
        args[i].end_range = start_range + chunk_size;
        start_range += chunk_size;
        
        pthread_create(&th[i], NULL, &sum, (void *)&args[i]);
    }
    
    // Last thread receives the remainder
    args[K - 1].start_range = start_range;
    args[K - 1].lock = lock;
    args[K - 1].end_range = start_range + chunk_size + remainder;
    pthread_create(&th[K - 1], NULL, &sum, (void *)&args[K - 1]);
    
    //Wait for all threads to finish
    for (i = 0; i < K; i++) {
        pthread_join(th[i], NULL);
    }
    
    free(args);
    free(th);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid arguments\n");
        return 0;
    }
    srand(time(NULL));

    int N = atoi(argv[1]); // Number of values
    int K = atoi(argv[2]); // Number of threads
    clock_t start_timer, finish_timer;
    double execution_time = 0;

    // Run 10 times to get the average time
    for (int i = 0; i < 10; i++) {
        sum_total = 0;
        long check_sum = 0;
        values = (char *)malloc(sizeof(char) * N);

        generate_values(N);
        start_timer = clock(); //Start execution time
        create_threads(N, K);
        finish_timer = clock(); //Finish execution time
        execution_time += (double)(finish_timer - start_timer) / CLOCKS_PER_SEC;
        
        //Check if the sum is correct using main thread
        for (int j=0; j < N; j++) {
            check_sum += (int)values[j];
        }

        if (check_sum != sum_total) {
            printf("Error: Wrong sum value\n");
            return 0;
        }
        else {
            printf("Sum correct\n");
        }

        free(values);
    }

    printf("%.6f\n", execution_time / 10);

    return 0;
}