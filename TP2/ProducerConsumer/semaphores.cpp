#include <iostream>
#include <pthread.h>         
#include <time.h>
#include <semaphore.h>
#include <vector>
#include <list>

// Define variables
int *shared_memory;
int memorySize;
int numbers_to_consume = 100000;
int consumed_numbers = 0; 
int produced_numbers = 0;
std::vector<pthread_t> threads;
std::vector<int> buffer_tracker;
std::list<int> freeList;
std::list<int> occupiedList;
sem_t mutex;
sem_t empty, full;
sem_t threads_producer, threads_consumer;

// Function to write vector to file with total time
void writeVectorToFile(int vector[], int size, const char* filename, double total_time) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s.\n", filename);
        return;
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%i\n", vector[i]);
    }

    // Write total time
    fprintf(file, "%f\n", total_time);

    fclose(file);
}

// Fill free list with all positions in shared memory
void fillFreeList() {
    for (int i = 0; i < memorySize; i++) {
        freeList.push_back(i);
    }
}

// Generate random number between 1 and 10⁷
int random_number() {
    return rand() % 10000000 + 1;
}


// Get empty position in shared memory
int getEmptyPosition() {
    if (freeList.empty()) {
        return -1; // No empty positions available
    }

    int position = freeList.front();
    freeList.pop_front(); // Remove the position from the free list
    occupiedList.push_back(position); // Add the position to the occupied list
    return position;

}


// Get full position in shared memory
int getFullPosition() {
    if (occupiedList.empty()) {
        return -1; // No full positions available
    }

    int position = occupiedList.front();
    occupiedList.pop_front(); // Remove the position from the occupied list
    freeList.push_back(position); // Add the position to the free list
    return position;
    
}

// Check if number is prime
int isPrime(long n) {
	int i; 
	for (i = 2; i * i <= n; ++i) {
		if (n % i == 0){
            return false;
        }
    }
	return true;
}

// Add new resource in the buffer
void add_resource(int item) {
    int emptyPosition = getEmptyPosition();
    if (emptyPosition != -1) {
        shared_memory[emptyPosition] = item;
    }
    else {
        std::cout << "Memory is full" << std::endl;
    }
}

// Remove resource from the buffer
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



// Producer thread
void *producer(void* arg) {
    while(consumed_numbers < numbers_to_consume) {

        
        sem_wait(&empty);
        sem_wait(&mutex);

        // Produce resource
        int number = random_number();
        add_resource(number);
        produced_numbers++;

        // Add to buffer tracker
        buffer_tracker.push_back(buffer_tracker.back() + 1);
    
        sem_post(&mutex);
        sem_post(&full);
    }

    // Signal consumers to stop
    sem_post(&empty);
    sem_post(&full);

    return NULL;
}

// Consumer thread
void* consumer(void* arg) {
    
    while(consumed_numbers < numbers_to_consume){  
        
        sem_wait(&full);
        sem_wait(&mutex);

        // Consume resource
        int number = remove_resource();
        consumed_numbers++;

        // Add to buffer tracker
        buffer_tracker.push_back(buffer_tracker.back() - 1);

        sem_post(&mutex);
        sem_post(&empty);  

        // Check if number is prime
        if (isPrime(number)) {
           // std::cout << "Prime number: " << number << std::endl;
        }
        else {
          //  std::cout << "Not prime number: " << number << std::endl;
        }
        

    }

    // Signal producers to stop
    sem_post(&empty);
    sem_post(&full);
    
    return NULL;
}

// Create producers and consumers threads
void create_threads(int producer_threads, int consumer_threads) {

    // Reserve space for threads
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

    // Initialize buffer tracker with 0
    buffer_tracker.push_back(0);

    // Set random seed
    srand(time(NULL));

    // Get arguments
    memorySize = atoi(argv[1]);
    int producers_threads = atoi(argv[2]);
    int consumers_threads = atoi(argv[3]);
    
    // Create shared memory
    shared_memory = (int*) malloc(memorySize * sizeof(int)); 

    // Fill free list
    fillFreeList();

    // Create semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, memorySize);
    sem_init(&full, 0, 0);

    // Start clock
    struct timespec start, end;
    double total_time;
    clock_gettime(CLOCK_MONOTONIC, &start); //Start execution time

    // Create threads
    create_threads(producers_threads, consumers_threads);

    // Join threads
    join_threads(producers_threads, consumers_threads);

    // End clock
    clock_gettime(CLOCK_MONOTONIC, &end); //Start execution time
    total_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    // Free shared memory
    std::free(shared_memory);

    // Print results
    std::cout << "Total time: " << total_time << std::endl;
    std::cout << "Produced numbers: " << produced_numbers << std::endl;
    std::cout << "Consumed numbers: " << consumed_numbers << std::endl;

    // Write buffer tracker to file
    const char* filename = "buffer_tracker.txt";
    writeVectorToFile(buffer_tracker.data(), buffer_tracker.size(), filename, total_time);
    std::cout << "Buffer tracker written to file " << filename << std::endl;

    return 0;
}