#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>


#define MSGLEN 64

using namespace std;

// Function to generate random number within given range
int randomDelta(int i, int f){
    int bigger = f;
    int smaller = i;
    return rand()%(bigger-smaller+1) + smaller;
}

void error(const char *msg)
{
    perror(msg);
    exit(2);
}

int main(int argc, char *argv[])
{
    int socket_fd, port_number, n;
    struct sockaddr_in server_address;
    struct hostent *server;


    srand(time(NULL));


    // Check if the number of arguments is correct
    if (argc < 4)
    {
        cout << "Not enough arguments" << endl;
        cout << "Usage: " << argv[0] << " <server> <port> <random numbers> " << endl;
        exit(1);
    }

    // Get the port number from the command line arguments
    port_number = atoi(argv[2]);

    // Socket for communication
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        error("Error: Failed opening socket");

    // Get the server from the command line arguments
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        error("Error: No such host");
    }

    // Initialize the server address
    memset((char *)&server_address, 0, sizeof(server_address)); // Clear the server address
    server_address.sin_family = AF_INET;
    memcpy((char *)&server_address.sin_addr.s_addr, (char *)server->h_addr, server->h_length); // Copy the server address
    server_address.sin_port = htons(port_number);

    // Connect to the server socket
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        error("Error: Connection failed");

    else
    {
        char message[MSGLEN];
        bool producer = true;
        int count = atoi(argv[3]);
        int r_number = 3; // Start from 3 since we know 2 is the only even prime number
        int prime_flag;

        while (producer) {
            count--;
            
            memset(message, 0, sizeof(message));

            int delta = randomDelta(1, 100); // Generate a random number within range [1, 100]

            r_number += delta + 1; // Generate the next number

            if (count <= 0) {
                producer = false;
            }

            sprintf (message, "%d", r_number);

            // Send the next number in the sequence to the server
            n = write(socket_fd, message, sizeof(message));
            if (n < 0)
                error("Error: Failed writing to socket");

            // Receive the result from the server
            n = read(socket_fd, message, sizeof(message));
            if (n < 0)
                error("Error: Failed reading from socket");

            // Check if the number is prime
            prime_flag = atoi(message);
            if (prime_flag == 1)
                cout << "Prime number: " << r_number << endl;
            if (prime_flag == 0)
                cout << "Not a Prime number: " << r_number << endl;
        }
    }

    close(socket_fd);
    return 0;
}