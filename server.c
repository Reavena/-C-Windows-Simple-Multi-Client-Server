// 2212170


#include <stdio.h>
#include <winsock2.h>

#define PORT 4444

// Function prototype for the client handler thread
DWORD WINAPI ClientHandler(LPVOID lpParam);

int main() {
    WSADATA wsa;
    SOCKET sockfd, newSocket; //socket file descriptor
    struct sockaddr_in serverAddr, newAddr; //structure that hols info about the sockets

    int addr_size;
    char buffer[1024];

    // Initialize Winsock v 2.2.
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-]Failed to initialize Winsock.\n");
        return 1;
    }

    // Create a socket   IPv4           TCP            default
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("[-]Error in connection.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("[+]Server Socket is created.\n");

    // Set up server address structure
    memset(&serverAddr, 0, sizeof(serverAddr)); //initialize all serverAddr bytes to 0
    serverAddr.sin_family = AF_INET;                          //set to IPv4
    serverAddr.sin_port = htons(PORT);              // set port
    serverAddr.sin_addr.s_addr = INADDR_ANY;                //server will bind to any available network interface on the machine, not specific

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { //(struct sockaddr*)&serverAddr: A pointer to the server's address structure, cast to a generic socket address structure
        printf("[-]Error in binding.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("[+]Bind to port %d\n", PORT);

    // Listen for incoming connections
    if (listen(sockfd, 10) == 0) { //max queue == 10 pending connections
        printf("[+]Listening....\n");
    } else {
        printf("[-]Error in listening.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Main server loop
    while (1) {
        addr_size = sizeof(newAddr);
        // Accept a new connection
        newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);//accept incoming connection, return new socket descriptor
        if (newSocket == INVALID_SOCKET) {
            printf("[-]Error in accepting connection.\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port)); //inet_toa converts the binary Pv4 to string format, ntohs -  is a function that converts a 16-bit (short) integer from network byte order to host byte order.

        // Create a new thread to handle the client
        HANDLE childThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)newSocket, 0, NULL);
                                            //NULL - default security attributes
                                            // 0 - default stack size for new thread
                                            //(LPTHREAD_START_ROUTINE)ClientHandler - pointer to thread function
                                            //(LPVOID)newSocket - passed as variable to ClientHandler
                                            //0 - will run immediately after creatiom
                                            //NULL - No thread identifier

        if (childThread == NULL) {
            printf("[-]Error creating thread for client.\n");
            closesocket(newSocket);
        }
    }

    // Close the server socket and clean up
    closesocket(sockfd);
    WSACleanup();

    return 0;
}

// Function definition for the client handler thread
DWORD WINAPI ClientHandler(LPVOID lpParam) { //Returns double word - 32 bit unsigned integer, takes long pointer to void parameter
    SOCKET newSocket = (SOCKET)lpParam;  // Cast the parameter back to a socket descriptor, lpParam is the socket descriptor passed as an argument when the thread is created.
    char buffer[1024]; // for storing received data

    // Client handling loop
    while (1) {
        // Receive data from the client
        int bytesReceived = recv(newSocket, buffer, sizeof(buffer) - 1, 0); //receives data from the client
        if (bytesReceived <= 0) {
            // Handle disconnection or error
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytesReceived] = '\0';  // Null-terminate the received data to be a string

        printf("Client: %s\n", buffer);// Process the received data

        send(newSocket, buffer, bytesReceived, 0);   // Echo the data back to the client
    }

    // Close the socket and clean up
    closesocket(newSocket);
    return 0;
}
