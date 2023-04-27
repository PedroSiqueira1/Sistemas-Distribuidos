#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MSGLEN 64

using namespace std;

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
    int socket_fd, port_number, n;
    struct sockaddr_in server_address;
    struct hostent *server;

    // Check if the number of arguments is correct
    if (argc < 3) {
       cout << "Not enough arguments" << endl;
       cout << "Usage: " << argv[0] << " <server> <port>" << endl;
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
    if (server == NULL) {
        error("Error: No such host");
    }

    // Initialize the server address
    memset((char *) &server_address, 0, sizeof(server_address)); // Clear the server address
    server_address.sin_family = AF_INET;
    memcpy((char *)&server_address.sin_addr.s_addr, (char *)server->h_addr, server->h_length); // Copy the server address
    server_address.sin_port = htons(port_number);


    // Connect to the server socket 
    if (connect(socket_fd,(struct sockaddr *) &server_address,sizeof(server_address)) < 0) 
        error("Error: Connection failed");

    else {
        char message[MSGLEN];
        bool consumer = true;
        int received_number, prime_flag;
        
        // Keep reading messages from the server until the end message is received
        while(consumer) {

            // Clear the message buffer
            memset(message, 0, sizeof(message));

            // Read the message from the server
            n = read(socket_fd, message, sizeof(message));
            if (n < 0)
                error("Error: Failed reading from socket");

            received_number = atoi(message);

            // Check if the message is the end message
            if(received_number == 0)
                consumer = false;

            else {
                // Check if the number is prime
                prime_flag = isPrime(received_number);
            
                // Send the result back to the server
                sprintf (message, "%d", prime_flag);
                n = write(socket_fd, message, sizeof(message));
                if (n < 0)
                    error("Error: Failed writing to socket");
            }
        }
    }
    
    close(socket_fd);
    return 0;
}