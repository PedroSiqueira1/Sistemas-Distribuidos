#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MSGLEN 64

using namespace std;

// Function to generate random number within given range
int isPrime(long n) {
	int i; 
	for (i = 2; i <= n / 2; ++i) {
		if (n % i == 0){
            return false;
        }
    }
	return true;
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

    // Check if the number of arguments is correct
    if (argc < 2) {
        cout << "Not enough arguments" << endl;
        cout << "Usage: " << argv[0] << " <port>" << endl;
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
        bool consumer = true;
        int received_number, prime_flag;
        
        // Keep reading messages from the socket until the end message is received
        while(consumer) {

            // Clear the message buffer
            memset(message, 0, sizeof(message));

            // Read the message from the socket
            n = read(new_socket_fd, message, sizeof(message));
            if (n < 0)
                error("Error: Failed reading from socket");

            // Convert the message to an integer
            received_number = atoi(message);

            // Check if the message is the end message
            if(received_number == 0)
                consumer = false;

            else {
                // Check if the number is prime
                prime_flag = isPrime(received_number);
            
                // Send the result back to the producer
                sprintf (message, "%d", prime_flag);
                n = write(new_socket_fd, message, sizeof(message));
                if (n < 0)
                    error("Error: Failed writing to socket");
            }
        }
    }

    // Close the sockets

    close(new_socket_fd);
    close(socket_fd);
    return 0; 
}
