#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
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
    int socket_fd, new_socket_fd, port_number;
    socklen_t client_address_length;
    struct sockaddr_in server_address, client_address;
    int n;

    srand(time(NULL));

    // Check if the number of arguments is correct
    if (argc < 3) {
        cout << "Not enough arguments" << endl;
        cout << "Usage: " << argv[0] << " <port> <random numbers>" << endl;
        exit(1);
    }

    // Socket for communication
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
        error("Error: Failed opening socket");

    // Initialize the server address
    memset((char *) &server_address, 0, sizeof(server_address));
    port_number = atoi(argv[1]); // Get the port number from the command line arguments
    server_address.sin_family = AF_INET; // Set the address family to IPv4
    server_address.sin_addr.s_addr = INADDR_ANY; // Set the IP address to any available interface of the host
    server_address.sin_port = htons(port_number); // Set the port number to the specified port number

    // Bind the socket to the server address
    if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
        error("Error: Failed binding");

    // Listen for connections
    listen(socket_fd, 5);

    // Accept a connection from the client
    client_address_length = sizeof(client_address);
    new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_address_length);
    if (new_socket_fd < 0) 
        error("Error: Failed accepting connection");
    else {
        char message[MSGLEN];
        bool producer = true;
        int count = atoi(argv[2]) + 1;
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

            // Send the next number in the sequence to the client
            n = write(new_socket_fd, message, sizeof(message));
            if (n < 0)
                error("Error: Failed writing to socket");

            // Receive the result from the client
            n = read(new_socket_fd, message, sizeof(message));
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

    // Close the sockets
    close(new_socket_fd);
    close(socket_fd);
    return 0; 
}
