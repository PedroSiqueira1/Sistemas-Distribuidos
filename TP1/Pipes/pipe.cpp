#include <sys/wait.h> 
#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>   
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Define buffer size
#define buffer_size 20

using namespace std;

int pipe_params[2];

// Function to check if a number is prime
int isPrime(long n) {
	int i; 
	for (i = 2; i <= n / 2; ++i) {
		if (n % i == 0){
            return false;
        }
    }
	return true;
}

int main(int argc, char *argv[]){
    int N = 1; // First number to be produced
    int delta; // Random number to be added to N
    int numbers_to_produce = atoi(argv[1]); // Number of numbers to be produced, read from command line
    char buffer[buffer_size]; // Buffer to read and write to the pipe

    // Seed the random number generator with the current time
    srand(time(0));


    // If pipe fails, exit
    if (pipe(pipe_params) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid_t pid = fork();

    // If fork fails, exit
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    // If producer process
    if (pid > 0 ) {

        // Close the read end of the pipe
        close(pipe_params[0]);

        // Write to the pipe
        for (int i = 0; i < numbers_to_produce; i++) {
            delta = rand() % 100 + 1;
            N += delta;
            sprintf(buffer, "%d", N);
            write(pipe_params[1], buffer, buffer_size);
            
        }

        // Write 0 in pipe to signal the end of the stream
        sprintf(buffer, "0");
        write(pipe_params[1], buffer, buffer_size);
        

        // Close the write end of the pipe
        close(pipe_params[1]);

        // Wait for the child process to finish
        wait(NULL);
        exit(0);
    }

    if(pid == 0) {
        // Close the write end of the pipe
        close(pipe_params[1]);

        // Read from the pipe
        while (read(pipe_params[0], buffer, buffer_size) > 0) {

            // If the read value is 0, exit
            if (strcmp(buffer, "0") == 0) {
                exit(0);
            }

            // Check if the number is prime
            if (isPrime(atoi(buffer))) {
                printf("%s is prime\n", buffer);
            } else {
                printf("%s is not prime\n", buffer);
            }

        }

        // Close the read end of the pipe
        close(pipe_params[0]);
    }

}