#include <sys/wait.h> 
#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>   
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iostream>


#define buffer_size 20

using namespace std;

int pipe_params[2];

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
    int N = 1;
    int delta;
    int numbers_to_produce = atoi(argv[1]);
    char buffer[buffer_size];
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
        // Close the read end of the pipe= 0) {
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
        sprintf(buffer, "%d", 0);
        write(pipe_params[1], buffer, buffer_size);
        

        // Close the write end of the pipe
        close(pipe_params[1]);
    }

    if(pid == 0) {
        // Close the write end of the pipe
        close(pipe_params[1]);

        // Read from the pipe
        while (read(pipe_params[0], buffer, buffer_size) > 0) {

            // If the read value is 0, break
            if (strcmp(buffer, "0") == 0) {
                break;
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