// Producter-Consumer problem with semaphores with multithreading   
#include <iostream>
#include <pthread.h>         
#include <time.h>
#include <semaphore.h>
#include <vector>


int *shared_memory;
int memorySize;
int numbers_to_consume = 10000;
int consumed_numbers = 0; 
int produced_numbers = 0;
int keep_producing = 1;
int keep_consuming = 1;
std::vector<pthread_t> threads;
sem_t mutex;
sem_t empty_zap, full;
sem_t threads_producer, threads_consumer;



// generate random number between 1 and 10⁷
int random_number() {
    return rand() % 10000000 + 1;
}


// Get empty position in shared memory
int getEmptyPosition() {
    for (int i = 0; i < memorySize; i++) {
        if (shared_memory[i] == 0) {
            return i;
        }
    }
    return -1;
}

// Get full position in shared memory
int getFullPosition() {
    for (int i = 0; i < memorySize; i++) {
        if (shared_memory[i] != 0) {
            return i;
        }
    }
    return -1;
}

// check if number is prime

int isPrime(long n) {
	int i; 
	for (i = 2; i <= n / 2; ++i) {
		if (n % i == 0){
            return false;
        }
    }
	return true;
}

// add new resource in the buffer
void add_resource(int item) {
    int emptyPosition = getEmptyPosition();
    if (emptyPosition != -1) {
        shared_memory[emptyPosition] = item;
    }
    else {
        std::cout << "Memory is full" << std::endl;
    }
}

// remove resource from the buffer
int remove_resource() {
    int fullPosition = getFullPosition();
    if (fullPosition != -1) {
        int item = shared_memory[fullPosition];
        shared_memory[fullPosition] = 0;
        return item;
    }
    else {
        std::cout << "Memory is empty" << std::endl;
        return -1;
    }
}



// producer thread
void *producer(void* arg) {
    
    while(keep_producing) {
        int number = random_number();
        sem_wait(&empty_zap);
        sem_wait(&mutex);
        if (consumed_numbers < numbers_to_consume) {
            add_resource(number);
            produced_numbers++;
        }
        else {
            keep_producing = 0;
        }
        sem_post(&mutex);
        sem_post(&full);
    }
    
    return NULL;
}

// consumer thread
void* consumer(void* arg) {
    
    while(keep_consuming){  
        sem_wait(&full);
        sem_wait(&mutex);
        if (consumed_numbers < numbers_to_consume) {
            int number = remove_resource();
            consumed_numbers++;
            if (isPrime(number)) {
                std::cout << "Prime number: " << number << std::endl;
            }
            else {
                std::cout << "Not prime number: " << number << std::endl;
        }
        }
        else {
            keep_consuming = 0;
        }
        sem_post(&mutex);
        sem_post(&empty_zap);

        

    }
    return NULL;
    
}

// Create producers and consumers threads
void create_threads(int producer_threads, int consumer_threads) {

    threads.reserve(producer_threads + consumer_threads);

    for (int i = 0; i < producer_threads; i++) {
        threads.emplace_back();
        pthread_create(&threads.back(), NULL, &producer, NULL);
    }
    for (int i = 0; i < consumer_threads; i++) {
        threads.emplace_back();
        pthread_create(&threads.back(), NULL, &consumer, NULL);
    }
}

// Join all threads
void join_threads(int producer_threads, int consumer_threads) {
    
  
    for (int i = 0; i < producer_threads + consumer_threads; i++) {
        // Check if thread is joinable
        if (pthread_join(threads[i], NULL) != 0) {
            std::cout << "Error joining thread " << i << std::endl;
        }
        pthread_join(threads[i], NULL);

    }
}




int main(int argc, char* argv[]) {

    // Check if the number of arguments is correct
    if (argc < 4) {
        std::cout << "Not enough arguments" << std::endl;
        std::cout << "Usage: " << argv[0] << " <memory_size> <number_of_producer_threads> number_of_consumer_threads> " << std::endl;
        exit(1);        consumed_numbers++;

    }

    // Set random seed
    srand(time(NULL));

    // Get arguments
    memorySize = atoi(argv[1]);
    int producers_threads = atoi(argv[2]);
    int consumers_threads = atoi(argv[3]);

    clock_t start, end;
    double total_time;
    
    // Create shared memory
    shared_memory = (int*) malloc(memorySize * sizeof(int)); 

    // Fill shared memory with zeros
    for (int i = 0; i < memorySize; i++) {
        shared_memory[i] = 0;
    }


    // Create semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty_zap, 0, memorySize);
    sem_init(&full, 0, 0);

    // Start clock
    start = clock();

    // Create threads
    create_threads(producers_threads, consumers_threads);

    // Join threads
    join_threads(producers_threads, consumers_threads);

    // End clock
    end = clock();
    total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty_zap);
    sem_destroy(&full);

    // Free shared memory
    std::free(shared_memory);

    // Print results
    std::cout << "Total time: " << total_time << std::endl;

    return 0;
}