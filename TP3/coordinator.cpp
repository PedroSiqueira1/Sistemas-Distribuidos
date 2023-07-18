// Multithreaded coordinator

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <string.h>
#include <fcntl.h>
#include <cstdlib>


using namespace std;
#define BUFFER_SIZE 10
#define PORT 8888

std::mutex mtx;
std::mutex logMutex;
std::queue<int> requestQueue;
int coordinatorId; // Global variable for coordinator ID
std::map<int, int> processExecutionCount; // Global variable for process execution count



// Terminal Thread that keeps waiting for commands
void terminal_thread() {
    string command;
    while (true) {
        cin >> command;
        if (command == "1") {
            // Print actual queue of requests
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Queue of Requests:" << std::endl;

            // Cria uma cópia da fila de solicitações
            std::queue<int> requestQueueCopy = requestQueue;

            while (!requestQueueCopy.empty()) {
                std::cout << requestQueueCopy.front() << " ";
                requestQueueCopy.pop();
            }
            std::cout << std::endl;
        }

        if (command == "2") {
            // Print how many times each process was executed
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Process Execution Counts:" << std::endl;
            for (const auto& pair : processExecutionCount) {
                std::cout << "Process ID: " << pair.first << ", Execution Count: " << pair.second << std::endl;
            }
        }
        
        if (command == "3") {
            // Stop coordinator 
            std::cout << "Coordinator stopped" << std::endl;
            std::system("python3 cleanlog.py");            
            exit(0);
        }
    }
}

// Create resultado.txt and log.txt files
void createFiles() {
    // Create resultado.txt file
    std::ofstream resultadoFile("resultado.txt");
    if (resultadoFile.is_open()) {
        resultadoFile.close();
        std::cout << "Blank file created successfully: resultado.txt" << std::endl;
    } else {
        std::cout << "Failed to create blank file: resultado.txt" << std::endl;
    }

    // Create log.txt file
    std::ofstream logFile("log.txt");
    if (logFile.is_open()) {
        logFile.close();
        std::cout << "Blank file created successfully: log.txt" << std::endl;
    } else {
        std::cout << "Failed to create blank file: log.txt" << std::endl;
    }
}

void handleRequest(int socket) {
    char buffer[BUFFER_SIZE];
    int clientId = -1; // Variable for client ID, initialized to -1
    bool clientSet = false; // Flag to check if client ID is already set
    // Set socket to non-blocking mode
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);


    while (true) {
        memset(buffer, 0, sizeof(buffer));
        // Cout thread ID and client ID
        int bytesRead = read(socket, buffer, BUFFER_SIZE);
    
        std::string received(buffer);



        if (!clientSet) {
            // Set client ID if not already set
            if (received.substr(0, 1) == "1") {
                clientId = stoi(received.substr(2));
                clientSet = true;
            }
        }

        // Log the received message
            {
                std::lock_guard<std::mutex> lock(logMutex);
                std::ofstream logFile("log.txt", std::ios_base::app);
                if (logFile.is_open()) {
                    if (received.empty() == false){
                        cout << "Received: " << received << endl;
                        logFile << "Received: " << received << std::endl;                        
                    }
                    logFile.close();
                } else {
                    std::cerr << "Failed to open log file: log.txt" << std::endl;
                }
            }

        
        if (received.substr(0, 1) == "1" && received.size() == 10) {
            // Received request message from client
            // Add client to the request queue
            {
                std::lock_guard<std::mutex> lock(mtx);    
                requestQueue.push(clientId);
            }

        
        } 
        
        else if (received.substr(0, 1) == "3" && received.size() == 10) {
            // Received release message from client
            // Process the release or take necessary actions

            // Remove client from the request queue and promote the next client to the front
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (!requestQueue.empty() && requestQueue.front() == clientId) {
                    requestQueue.pop();
                }
            }
            // Add to process execution count
            {
                std::lock_guard<std::mutex> lock(mtx);
                processExecutionCount[clientId]++;
            }
        }

        // Check if this thread is at the front of the queue
        // If yes, send "2" to the client to indicate access to the critical region
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (requestQueue.front() == clientId) {
                
    

                // Send 2|coordinatorId to client
                std::string accessMessage = "2|" + std::to_string(coordinatorId) + "|";
                if (accessMessage.size() < BUFFER_SIZE) {
                    accessMessage.append(BUFFER_SIZE - accessMessage.size(), '0');
                }

                 // Log the sent message
                {
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::ofstream logFile("log.txt", std::ios_base::app);
                    if (logFile.is_open()) {
                        logFile << "Sent: " << accessMessage << std::endl;
                        logFile.close();
                    } else {
                        std::cerr << "Failed to open log file: log.txt" << std::endl;
                    }
                }
                send(socket, accessMessage.c_str(), BUFFER_SIZE, 0);
            }
        }
    }
}

void manager_thread() {
    int server_socket, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    std::vector<std::thread> threads;

    // Define coordinator ID
    coordinatorId = getpid();

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a specific address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Accept a new connection
        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Create a new thread for each client connection
        std::thread t(handleRequest, new_socket);
        threads.push_back(std::move(t));
    }

    // Join all the threads
    for (auto& t : threads) {
        t.join();
    }

    // Close the server socket
    close(server_socket);

    std::system("python3 cleanlog.py");

}

int main() {
    // Create resultado.txt and log.txt files
    createFiles();

    // Create terminal thread
    std::thread terminal(terminal_thread);    
    std::thread manager(manager_thread);
    manager.join();


    return 0;
}