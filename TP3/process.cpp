#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    // Create a socket for communication with the coordinator
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating client socket" << std::endl;
        return 1;
    }

    // Set up the server address
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);  // Adjust the port number as needed
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    // Connect to the server (coordinator)
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return 1;
    }

    int r = 3;  // Adjust the number of repetitions as needed

    for (int i = 0; i < r; ++i) {
        // Send request to the coordinator | process ID
        std::string requestMessage = "1|" + std::to_string(getpid());
        if (send(clientSocket, requestMessage.c_str(), requestMessage.size(), 0) == -1) {
            std::cerr << "Error sending request to coordinator" << std::endl;
            close(clientSocket);
            return 1;
        }

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // Receive response from the coordinator
        while (true) {
            if (recv(clientSocket, buffer, BUFFER_SIZE - 1, 0) == -1) {
                std::cerr << "Error receiving response from coordinator" << std::endl;
                close(clientSocket);
                return 1;
            }
            std::string response(buffer);
            std::cout << "Received response: " << response << std::endl;
            if (response.substr(0, 1) == "2") {
                // Received access message from coordinator
                // Proceed with critical region access

                std::cout << "Received access message. Proceeding with critical region access." << std::endl;

                // Open "resultado.txt" file
                std::ofstream file("resultado.txt", std::ios::app);
                if (!file) {
                    std::cerr << "Error opening file: resultado.txt" << std::endl;
                    close(clientSocket);
                    return 1;
                }

                // Write process ID and current time to the file
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                file << getpid() << " " << std::ctime(&now_c);

                // Close the file
                file.close();

                // Wait for k seconds
                int k = 7;  // Adjust the wait time as needed
                sleep(k);

                break;
            }

            // Clear the buffer for the next read
            memset(buffer, 0, sizeof(buffer));
        }

        // After completing the critical region, release access by sending "3" | process id to the coordinator
        std::string releaseMessage = "3|" + std::to_string(getpid());
        if (send(clientSocket, releaseMessage.c_str(), releaseMessage.size(), 0) == -1) {
            std::cerr << "Error sending release message to coordinator" << std::endl;
            close(clientSocket);
            return 1;
        }
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}